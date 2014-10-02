#include <sys/signalfd.h>
#include <signal.h>
#include <unistd.h>

#include "server/server.h"

int main(int argc, char const* argv[]) {
  server_t* s;
  sigset_t mask;
  siginfo_t info;
  struct sigaction action;

  server_create(&s, "../calories.csv", NULL);

  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);
  sigprocmask(SIG_BLOCK, &mask, NULL);

  sigemptyset(&action.sa_mask);
  action.sa_flags = SA_SIGINFO;
  sigaction(SIGCHLD, &action, NULL);

  server_start(s);

  while(true) {
    // signal() is NOT thread safe!
    // And since the server doesn't block signal handling is "not" needed ...
    if(sigwaitinfo(&mask, &info) == -1) {
      perror("sigwaitinfo() failed");
      exit(1);
    }

    switch(info.si_signo) {
    case SIGINT:
      server_destroy(s);
      return 0;
    }
  }
}
