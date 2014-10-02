#ifndef CLIENT_H
#define CLIENT_H

#include <stdbool.h>

#define DEFAULT_PORT "12345"

typedef struct {
  int server_fd;
  bool running;
  const char* server_ip;
  const char* server_port;
} client_t;


void client_create(client_t** const c, const char* const ip,
                   const char* const port);
void client_destroy(client_t* const c);
bool client_connect(client_t* const c);
bool client_ui(client_t* const c);
bool client_ui_add(client_t* const c);
bool client_ui_search(client_t* const c);

void client_get_uint(const char* const text, int* const n);
void client_get_ufloat(const char* const text, float* const n);
void client_get_cstring(const char* const text, char** const s);


#endif
