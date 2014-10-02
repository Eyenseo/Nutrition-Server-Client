#include "server/server.h"

#include <unistd.h>
#include <arpa/inet.h>

#include "networking.h"

void server_worker(int id, server_t* s) {
  if(s != NULL && s->running) {
    int client_fd;
    if(int_queue_pop(s->queue, &client_fd)) {
      bool client = true;
      s->used_fds[id] = client_fd;

      send_number(client_fd, START);

      while(s->running && client) {
        network_intent_t ni;

        if((client = recv_number(client_fd, (int*)&ni)) && s->running) {
          switch(ni) {
          default:
            printf("Received bad intent '%d' from client %d\n", ni, client_fd);
            break;
          case ADD:
            client = server_worker_add(s, client_fd);
            if(!client) {
              printf("Client %d disconnected ungracefully\n", client_fd);
            }
            break;
          case QUIT:
            printf("Client %d disconnected gracefully\n", client_fd);
            client = false;
            break;
          case SEARCH:
            client = server_worker_search(s, client_fd);
            if(!client) {
              printf("Client %d disconnected ungracefully\n", client_fd);
            }
          }
        } else {
          if(!client) {
            printf("Client %d disconnected ungracefully\n", client_fd);
          }
        }
      }

      shutdown(client_fd, 3);  // Block read and write
    }
  }
}

bool server_worker_add(server_t* const s, int client_fd) {
  char* buf;

  if(recv_cstr(client_fd, &buf)) {
    food_t* f;
    if(food_deserialize(buf, &f)) {
      server_write_start(s);

      food_t** db_new = malloc(sizeof(food_t*) * (s->db_size + 1));
      bool once = true;
      for(int i = 0, j = 0; i < s->db_size; ++i, ++j) {
        if(once && strncasecmp(f->name, s->db[i]->name, strlen(f->name)) < 0) {
          db_new[j] = f;
          ++j;
          once = false;
        }
        db_new[j] = s->db[i];
      }
      if(once) {
        db_new[s->db_size] = f;
      }
      ++s->db_size;
      free(s->db);
      s->db = db_new;
      server_write_end(s);

      send_number(client_fd, SUCCESS);
      free(buf);
      return true;
    }
    free(buf);
    send_number(client_fd, FAILIOR);
  }

  return false;
}

bool server_worker_search(server_t* const s, int client_fd) {
  bool result = false;
  char* buf;

  if(recv_cstr(client_fd, &buf)) {
    int len = strlen(buf);
    int q_len = 0;
    int_queue_t* q;
    int_queue_create(&q);

    server_read_start(s);

    for(int i = 0; i < s->db_size; ++i) {
      const char* const n = s->db[i]->name;

      if(strncasecmp(buf, n, len) == 0) {
        const char c = n[len];
        if(c == '\0' || c == ' ' || c == '.' || c == ',' || c == '-' || c == '+'
           || c == '_' || c == '~' || c == '/' || c == '&' || c == '|'
           || c == ':' || c == ';' || c == '(' || c == ')' || c == '['
           || c == ']' || c == '{' || c == '}') {
          int_queue_push_back(q, i);
          ++q_len;
        }
      }
    }

    char** arr = malloc(sizeof(char*) * q_len);
    for(int i = 0; i < q_len; ++i) {
      int n;
      int_queue_pop(q, &n);

      arr[i] = calloc(sizeof(char), FOOD_DATA_SERIALIZED_LENGTH);

      food_serialize(s->db[n], arr[i]);
    }
    server_read_end(s);

    if(send_cstr_arr(client_fd, arr, q_len)) {
      result = true;
    }

    int_queue_destroy(q);

    for(int i = 0; i < q_len; ++i) {
      free(arr[i]);
    }
    free(arr);
    free(buf);
  }
  return result;
}

void server_destroy(server_t* const s) {
  if(s != NULL) {
    if(s->running) {
      server_stop(s);

      pthread_mutex_destroy(&s->read_mutex);
      pthread_mutex_destroy(&s->write_mutex);
      pthread_cond_destroy(&s->write_cond);

      file_parser_food_array_to_file(s->db_filename, s->db, s->db_size);

      for(int i = 0; i < s->db_size; ++i) {
        food_destroy(s->db[i]);
      }
      free(s->db);
      free(s);
    }
  }
}

bool server_stop(server_t* const s) {
  if(s != NULL && s->running) {
    s->running = false;

    for(int i = 0; i < THREAD_POOL_SIZE; ++i) {
      shutdown(s->used_fds[i], 2);  // Block read and write
    }
    shutdown(s->server_fd, 2);  // Block read and write

    pthread_join(s->listener, NULL);
    thread_pool_stop(s->tp);

    thread_pool_destroy(s->tp);
    int_queue_destroy(s->queue);

    return true;
  }
  return false;
}

bool server_create(server_t** const sp, const char* const db_filename,
                   const char* const port) {
  if(sp != NULL && db_filename != NULL) {
    *sp = malloc(sizeof(server_t));
    server_t* s = *sp;

    s->running = false;
    s->read_count = 0;
    s->db_filename = db_filename;
    if(port != NULL) {
      s->port = port;
    } else {
      s->port = DEFAULT_PORT;
    }

    if(file_parser_file_to_food_array(db_filename, &s->db, &s->db_size)) {
      pthread_mutex_init(&s->read_mutex, NULL);
      pthread_mutex_init(&s->write_mutex, NULL);
      pthread_cond_init(&s->write_cond, NULL);

      return true;
    }
    free(sp);
  }
  return false;
}

bool server_start(server_t* const s) {
  if(s != NULL && s->running == false) {
    if(thread_pool_create(&s->tp) && int_queue_create(&s->queue)) {

      s->running = true;

      thread_pool_start(s->tp, (void (*)(int, void*))server_worker, s);
      server_start_init(s);
      return true;
    }
  }
  return false;
}

void server_start_listen(server_t* const s) {
  char their_addr_hum[INET6_ADDRSTRLEN];
  socklen_t sin_size;
  struct sockaddr_storage their_addr;  // For fancy status
  int new_fd;

  while(s->running) {
    sin_size = sizeof their_addr;
    new_fd = accept(s->server_fd, (struct sockaddr*)&their_addr, &sin_size);

    if(s->running) {
      if(new_fd == -1) {
        perror("accept");
        continue;
      }

      inet_ntop(their_addr.ss_family,
                get_ip4_or_ip6((struct sockaddr*)&their_addr), their_addr_hum,
                sizeof their_addr_hum);
      printf("server: got connection from %s as client %d\n", their_addr_hum,
             new_fd);

      int_queue_push_back(s->queue, new_fd);
      thread_pool_notify(s->tp);
    }
  }
}

bool server_start_init(server_t* const s) {
  struct addrinfo hints;
  struct addrinfo* servinfo;
  struct addrinfo* p;
  int rv;
  int yes = 1;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;  // use my IP

  if((rv = getaddrinfo(NULL, s->port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if((s->server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
       == -1) {
      perror("server: socket");
      continue;
    }

    if(setsockopt(s->server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
       == -1) {
      perror("setsockopt");
      return false;
    }

    if(bind(s->server_fd, p->ai_addr, p->ai_addrlen) == -1) {
      close(s->server_fd);
      perror("server: bind");
      continue;
    }

    break;
  }

  if(p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    return 2;
  }

  freeaddrinfo(servinfo);  // all done with this structure

  if(listen(s->server_fd, BACKLOG) == -1) {
    perror("listen");
    return false;
  }

  pthread_create(&s->listener, NULL, (void* (*)(void*))server_start_listen, s);

  return true;
}

void server_read_start(server_t* const s) {
  pthread_mutex_lock(&s->write_mutex);
  pthread_mutex_lock(&s->read_mutex);
  ++s->read_count;
  pthread_mutex_unlock(&s->read_mutex);
  pthread_mutex_unlock(&s->write_mutex);
}

void server_read_end(server_t* const s) {
  pthread_mutex_lock(&s->read_mutex);
  --s->read_count;
  pthread_cond_signal(&s->write_cond);
  pthread_mutex_unlock(&s->read_mutex);
}

void server_write_start(server_t* const s) {
  pthread_mutex_lock(&s->write_mutex);
  pthread_mutex_lock(&s->read_mutex);

  while(s->read_count != 0) {
    pthread_cond_wait(&s->write_cond, &s->read_mutex);
  }

  pthread_mutex_unlock(&s->read_mutex);
}

void server_write_end(server_t* const s) {
  pthread_mutex_unlock(&s->write_mutex);
}
