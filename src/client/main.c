#include "client/client.h"

#include <stdlib.h>
#include <stdio.h>

#include "networking.h"

static const char* const CLIENT_MAIN_HELP
    = "Usage:\n"
      "  ./client [IP-ADDRESS] [PORT]\n"
      "Parameter:\n"
      "  IP-ADDRESS: 127.0.0.1 (default)\n"
      "  PORT:       12345     (default)\n"
      "Example:\n"
      "  ./client 192.168.0.4 54321\n\n";


int main(int argc, char const* argv[]) {
  client_t* c;

  if(argc == 1) {
    client_create(&c, NULL, NULL);
  } else if(argc == 2) {
    client_create(&c, argv[1], NULL);
  } else if(argc == 3) {
    client_create(&c, argv[1], argv[2]);
  } else {
    printf("%s\n", CLIENT_MAIN_HELP);
    exit(1);
  }

  if(client_connect(c)) {
    client_ui(c);
  }
  client_destroy(c);


  return 0;
}
