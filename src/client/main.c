#include "client/client.h"

#include <stdlib.h>
#include <stdio.h>

#include "networking.h"

int main(int argc, char const* argv[]) {
  client_t* c;
  client_create(&c, NULL, NULL);
  if(client_connect(c)) {
    client_ui(c);
  }
  client_destroy(c);


  return 0;
}
