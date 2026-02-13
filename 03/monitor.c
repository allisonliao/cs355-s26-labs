#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t child_pid = 0;

void sigchld_handler(int sig) {
  int status;
  pid_t pid;
  (void)sig;

  while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
    if (WIFEXITED(status)) {
      // child exit
      printf("\n%d exited with status %d\n", pid, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
      // child terminated
      printf("\n%d received signal %s\n", pid, strsignal(WTERMSIG(status)));
    } else if (WIFSTOPPED(status)) {
      // child killed
      printf("\n%d stopped.\n", pid);
    } else if (WIFCONTINUED(status)) {
      // child continued
      printf("\n%d continued.\n", pid);
    }
  }
}

int main() {
  struct sigaction sa;
  char input[100];

  // sigchild handler
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;  // restart system calls ?

  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  child_pid = fork();

  if (child_pid == -1) {
    perror("fork");
    exit(1);
  }

  if (child_pid == 0) {
    // child, new pgroup
    if (setpgid(0, 0) == -1) {
      perror("setpgid");
      exit(1);
    }

    // wait infinite
    while (1) {
      sleep(1);
    }
    exit(0);
  } else {
    printf("Monitoring child process %d.\n", child_pid);

    // read input
    while (1) {
      if (fgets(input, sizeof(input), stdin) == NULL) {
        // error
        break;
      }

      input[strcspn(input, "\n")] = 0;

      // quit
      if (strcmp(input, "quit") == 0) {
        break;
      }
    }

    // sigterm
    printf("\n");
    if (kill(child_pid, SIGTERM) == -1) {
      perror("kill");
    }
    sleep(1);

    exit(0);
  }

  return 0;
}