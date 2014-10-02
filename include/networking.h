#ifndef NETWORKING_H
#define NETWORKING_H

#include <netdb.h>
#include <stdbool.h>

#define NETWORKING_INT_LENGTH 10

typedef enum { INIT, ADD, SEARCH, RESULT, START, QUIT } network_type_t;

void* get_ip4_or_ip6(const struct sockaddr* const sa);

bool send_cstr(int const fd, const char* const s);
bool recv_cstr(int const fd, char** const s);

bool send_number(int const fd, int n);
bool recv_number(int const fd, int* const n);

bool send_cstr_arr(int const fd, const char** const r, int const len);
bool recv_cstr_arr(int const fd, char*** const r, int* const len);

bool save_send(int s, const char* const buf, int const len);

#endif
