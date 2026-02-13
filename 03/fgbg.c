#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t child_pid = -1;

void sigchld_handler(int sig) {
  int status;
  if (waitpid(child_pid, &status, WNOHANG | WUNTRACED | WCONTINUED) > 0) {
    if (WIFEXITED(status)) {
      printf("child exited\n");
      child_pid = -1;
    } else if (WIFSIGNALED(status)) {
      printf("child received signal Terminated\n");
      child_pid = -1;
    } else if (WIFSTOPPED(status)) {
      printf("child stopped.\n");
    } else if (WIFCONTINUED(status)) {
      printf("child continued.\n");
    }
  }
}

int main() {
  char cmd[100];

  signal(SIGCHLD, sigchld_handler);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);

  child_pid = fork();

  if (child_pid == 0) {
    // child
    setpgid(0, 0);
    execlp("./infinite", "./infinite", (char*)NULL);
    exit(1);
  }

  // parent
  setpgid(child_pid, child_pid);
  printf("[1] Created process %d in the background\n", child_pid);

  while (1) {
    printf("$ ");
    fflush(stdout);

    if (!fgets(cmd, sizeof(cmd), stdin)) break;
    cmd[strcspn(cmd, "\n")] = 0;

    if (strcmp(cmd, "fg") == 0) {
      tcsetpgrp(0, child_pid);
      kill(child_pid, SIGCONT);
      waitpid(child_pid, NULL, WUNTRACED);
      tcsetpgrp(0, getpgrp());
    } else if (strcmp(cmd, "bg") == 0) {
      kill(child_pid, SIGCONT);
    } else if (strcmp(cmd, "quit") == 0) {
      if (child_pid > 0) {
        kill(child_pid, SIGTERM);
        usleep(50000);
      }
      break;
    }
  }

  return 0;
}