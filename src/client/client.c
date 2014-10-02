#include "client/client.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "networking.h"
#include "food_data.h"

#define STRGIFY(x) STRGIFY2(x)
#define STRGIFY2(x) #x

#define CLIENT_DEFAULT_IP "127.0.0.1"
#define CLIENT_DEFAULT_PORT "12345"


void client_create(client_t** const c, const char* const ip,
                   const char* const port) {
  *c = malloc(sizeof(client_t));
  (*c)->running = false;

  if(ip == NULL) {
    (*c)->server_ip = CLIENT_DEFAULT_IP;
  } else {
    (*c)->server_ip = NULL;
  }

  if(port == NULL) {
    (*c)->server_port = CLIENT_DEFAULT_PORT;
  } else {
    (*c)->server_port = NULL;
  }
}

void client_destroy(client_t* const c) {
  shutdown(c->server_fd, 2);  // Block read and write
  free(c);
}

bool client_connect(client_t* const c) {
  struct addrinfo hints;
  struct addrinfo* p;
  struct addrinfo* servinfo;
  int rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if((rv = getaddrinfo(c->server_ip, c->server_port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    freeaddrinfo(servinfo);
    return false;
  }

  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if((c->server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
       == -1) {
      perror("client: socket");
      continue;
    }

    if(connect(c->server_fd, p->ai_addr, p->ai_addrlen) == -1) {
      close(c->server_fd);
      perror("client: connect");
      continue;
    }

    break;
  }

  if(p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    freeaddrinfo(servinfo);
    return false;
  }

  c->running = true;

  freeaddrinfo(servinfo);  // all done with this structure
  return true;
}

bool client_ui(client_t* const c) {
  int intent;

  if(recv_number(c->server_fd, &intent) && intent == START) {
    char oe[3];
    oe[1] = '\0';

    while(c->running) {
      printf("\nWhat do you want to do?\n> ");
      scanf(" %2[^\n]%*[^\n]", oe);

      if(oe[1] != '\0') {
        oe[1] = '\0';
        continue;
      }

      switch(oe[0]) {
      default:
        // TODO HELP
        break;
      case 'a':
        c->running = client_ui_add(c);
        if(!c->running) {
          printf("Server error\n");
        }
        break;
      case 'q':
        send_number(c->server_fd, QUIT);
        c->running = false;
        break;
      case 's':
        c->running = client_ui_search(c);
        if(!c->running) {
          printf("Server error\n");
        }
        break;
      }
    }
    return true;
  }
  return false;
}

bool client_ui_add(client_t* const c) {
  bool suc = false;
  food_t* f;
  food_create(&f);

  client_ui_get_cstring(
      "Please enter the name of the food: [" STRGIFY(FOOD_DATA_TEXT_LENGTH) "]",
      &f->name);
  client_ui_get_cstring("Please enter the measure of the food data: [" STRGIFY(
                            FOOD_DATA_TEXT_LENGTH) "]",
                        &f->measure);
  client_ui_get_ufloat("Please enter the weight of the food data: [g]",
                       &f->weight);
  client_ui_get_uint("Please enter the kilo calorie of the food data:",
                     &f->k_cal);
  client_ui_get_uint("Please enter the fat of the food data: [g]", &f->fat);
  client_ui_get_uint("Please enter the carbohydrate of the food data: [g]",
                     &f->carbo);
  client_ui_get_uint("Please enter the protein of the food data: [g]",
                     &f->protein);
  if(send_number(c->server_fd, ADD)) {
    int rec;
    char buf[sizeof(char) * 2 * FOOD_DATA_TEXT_LENGTH + 6 * FOOD_DATA_INT_LENGTH
             + FOOD_DATA_FLOAT_LENGTH];

    food_serialize(f, buf);

    if(send_cstr(c->server_fd, buf) && recv_number(c->server_fd, &rec)
       && rec == SUCCESS) {
      suc = true;
    }
  }
  food_destroy(f);

  return suc;
}


bool client_ui_search(client_t* const c) {
  char* s;
  char** res;
  int res_len;
  bool suc = false;

  client_ui_get_cstring(
      "Please enter the search term: [" STRGIFY(FOOD_DATA_TEXT_LENGTH) "]", &s);

  printf("Sending request\n> ...");

  if(send_number(c->server_fd, SEARCH) && send_cstr(c->server_fd, s)) {
    if(recv_cstr_arr(c->server_fd, &res, &res_len)) {
      if(res_len > 0) {
        printf("\r> %d results\n", res_len);
      } else {
        printf("\r> No food item found.\nPlease check your spelling and try "
               "again.\n");
      }

      for(int i = 0; i < res_len; ++i) {
        food_t* f;
        food_deserialize(res[i], &f);

        printf("\nFood: %s\nMeasure: %s\nWeight: %.3f g\nkCal: %d\nFat: %d "
               "g\nCarbo: %d g\nProtein: %dg\n",
               f->name, f->measure, f->weight, f->k_cal, f->fat, f->carbo,
               f->protein);

        food_destroy(f);
        free(res[i]);
      }
      free(res);
      suc = true;
    }
  }
  free(s);

  return suc;
}

void client_ui_get_uint(const char* const text, int* const n) {
  bool good = false;

  while(!good) {
    printf("%s\n> ", text);

    if(scanf(" %d", n) != 0 && *n >= 0) {
      good = true;
    } else {
      scanf(" %*[^\n]");  // clean stdin
    }
  }
}

void client_ui_get_ufloat(const char* const text, float* const n) {
  bool good = false;

  while(!good) {
    printf("%s\n> ", text);

    if(scanf(" %f", n) != 0 && *n >= 0) {
      good = true;
    } else {
      scanf(" %*[^\n]");  // clean stdin
    }
  }
}

void client_ui_get_cstring(const char* const text, char** const s) {
  *s = malloc(sizeof(char) * FOOD_DATA_TEXT_LENGTH);
  do {
    printf("%s\n> ", text);
  } while(scanf(" %[^\n]", *s) == 0 || strlen(*s) < 1);
}

#undef STRGIFY
#undef STRGIFY2
