#ifndef NETWORKING_H
#define NETWORKING_H

#include <netdb.h>

enum Network_Type { INIT, ADD, SEARCH, START, QUIT };

void* get_ip4_or_ip6(struct sockaddr* sa);

#endif
