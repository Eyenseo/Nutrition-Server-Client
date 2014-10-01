#ifndef NETWORKING_H
#define NETWORKING_H

#include <netdb.h>

enum Network_Type { INIT, ADD, SEARCH, START, QUIT };

void* get_ip4_or_ip6(struct sockaddr* sa) {
  if(sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


#endif
