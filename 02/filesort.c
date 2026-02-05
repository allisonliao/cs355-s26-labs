
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  pid_t pid = fork();
  int fd = open("list.txt", O_RDONLY);
  if (fd < 0) {
    perror("file open error\n");
    exit(EXIT_FAILURE);
  }
  if (dup2(fd, 0) < 0) {
    perror("dup2 error\n");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    // child process: replace with call to sort
    execlp("sort", "sort", (char*)NULL);
    perror("execlp failed\n");
    exit(EXIT_FAILURE);
  }
  close(fd);
  wait(&pid);
  printf("sort complete\n");
  return 0;
}
