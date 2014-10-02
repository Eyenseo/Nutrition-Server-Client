#include "networking.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* get_ip4_or_ip6(const struct sockaddr* const sa) {
  if(sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool send_cstr(int const fd, const char* const s) {
  int len = strlen(s);

  if(send_number(fd, len)) {
    return save_send(fd, s, len);
  }
  return false;
}

bool recv_cstr(int const fd, char** const s) {
  int len;
  int b_rec;

  if(recv_number(fd, &len)) {
    *s = malloc(sizeof(char) * len + 1);

    if((b_rec = recv(fd, *s, len, 0)) == -1) {
      perror("recv");
      return false;
    }
    (*s)[b_rec] = '\0';
  }

  return false;
}

bool send_number(int const fd, int n) {
  char buf[NETWORKING_INT_LENGTH];

  sprintf(buf, "%d", n);
  return save_send(fd, buf, strlen(buf));
}

bool recv_number(int const fd, int* const n) {
  char buf[NETWORKING_INT_LENGTH + 1];
  int b_rec;

  if((b_rec = recv(fd, buf, NETWORKING_INT_LENGTH, 0)) == -1) {
    perror("recv");
    return false;
  }
  buf[b_rec] = '\0';

  sscanf(buf, "%d", n);
  return true;
}

bool send_cstr_arr(int const fd, const char** const r, int const len) {
  if(send_number(fd, len)) {
    for(int i = 0; i < len; ++i) {
      if(!send_cstr(fd, r[i])) {
        return false;
      }
    }
    return true;
  }
  return false;
}

bool recv_cstr_arr(int const fd, char*** const r, int* const len) {
  if(recv_number(fd, len)) {
    *r = malloc(sizeof(char*) * *len);

    for(int i = 0; i < *len; ++i) {
      if(!recv_cstr(fd, &(*r)[i])) {
        return false;
      }
    }
    return true;
  }
  return false;
}

bool save_send(int fd, const char* const m, int const len) {
  int total = 0;
  int bytesleft = len;
  int n;

  while(total < len) {
    n = send(fd, m + total, bytesleft, 0);
    if(n == -1) {
      return false;
    }
    total += n;
    bytesleft -= n;
  }

  return true;
}
