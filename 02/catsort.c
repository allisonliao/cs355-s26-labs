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
  int p[2];
  pid_t firChild, secChild;
  int status = pipe(p);
  if (pipe(p) < 0) {
    perror("pipe error\n");
    exit(EXIT_FAILURE);
  }

  int fd = open("list.txt", O_RDONLY);
  if (fd < 0) {
    perror("file open error\n");
    exit(EXIT_FAILURE);
  }
  if (dup2(fd, 0) < 0) {
    perror("dup2 error\n");
    exit(EXIT_FAILURE);
  }

  firChild = fork();
  if (firChild < 0) {
    perror("cat fork error\n");
    exit(EXIT_FAILURE);
  }
  if (firChild == 0) {
    // cat child
    int fd = open("list.txt", O_RDONLY);
    dup2(fd, STDIN_FILENO);
    close(p[0]);                // close read
    dup2(p[1], STDOUT_FILENO);  // change output stream to pipe
    close(p[1]);                // close write
    execlp("cat", "cat", (char*)NULL);
    perror("cat didn't work :(");
  } else {  // parent spawns new child
    secChild = fork();
    if (secChild < 0) {
      perror("sort fork error\n");
      exit(EXIT_FAILURE);
    }
    if (secChild == 0) {
      // sort child
      close(p[1]);               // close write
      dup2(p[0], STDIN_FILENO);  // change input stream to pipe
      close(p[0]);               // close read
      execlp("sort", "sort", (char*)NULL);
      perror("sort didn't work D:");
    }
  }

  close(p[0]);
  close(p[1]);

  waitpid(firChild, NULL, 0);
  printf("cat successful\n");
  waitpid(secChild, NULL, 0);
  printf("sort successful\n");
  close(fd);
  exit(EXIT_SUCCESS);
}
