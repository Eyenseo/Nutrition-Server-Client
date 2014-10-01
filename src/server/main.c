#include "server/server.h"

int main(int argc, char const* argv[]) {
  server_t* s;

  server_create(&s, "../calories.csv", NULL);

  server_start(s);
  getchar();

  server_destroy(s);
  return 0;
}
