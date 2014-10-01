#ifndef SERVER_H
#define SERVER_H

#include <semaphore.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "food_data.h"
#include "file_parser.h"
#include "int_queue.h"
#include "thread_pool.h"
#include "networking.h"

#define DEFAULT_PORT "12345"
#define BACKLOG 10

typedef struct {
  int used_fds[THREAD_POOL_SIZE];
  food_t** db;
  int db_size;
  thread_pool_t* tp;
  int_queue_t* queue;
  const char* db_filename;
  const char* port;

  pthread_mutex_t read_mutex;
  pthread_mutex_t write_mutex;
  sem_t read_sem;

  int server_fd;
  struct addrinfo hints;

  pthread_t listener;

  bool running;
} server_t;


void server_worker(int id, server_t* s) {
  if(s != NULL && s->running) {
    int client_fd;
    int_queue_pop(s->queue, &client_fd);

  }
}

bool server_create(server_t** const sp, const char* const db_filename,
                   const char* const port) {
  if(sp != NULL && db_filename != NULL) {
    *sp = malloc(sizeof(server_t));
    server_t* s = *sp;

    s->running = false;
    s->db_filename = db_filename;
    if(port != NULL) {
      s->port = port;
    } else {
      s->port = DEFAULT_PORT;
    }

    if(file_to_food_array(db_filename, &s->db, &s->db_size)) {
      if(thread_pool_create(&s->tp)) {
        if(int_queue_create(&s->queue)) {
          pthread_mutex_init(&s->read_mutex, NULL);
          pthread_mutex_init(&s->write_mutex, NULL);
          sem_init(&s->read_sem, 0, 0);
        }
      }
    }
  }
  return false;
}

bool server_start(server_t* const s) {
  if(s != NULL && s->running == false) {
    s->running = true;

    thread_pool_start(s->tp, (void (*)(int, void*))server_worker, s);

    return true;
  }
  return false;
}

void server_listen(server_t* const s) {
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
      printf("server: got connection from %s\n", their_addr_hum);

      int_queue_push_back(s->queue, new_fd);
      thread_pool_notify(s->tp);
    }
  }
}

bool server_init(server_t* const s) {
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

  printf("server: waiting for connections...\n");

  pthread_create(&s->listener, NULL, (void* (*)(void*))server_listen, s);

  return true;
}


#endif
