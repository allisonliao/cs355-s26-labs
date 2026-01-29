#include <stdio.h>
#include <string.h>

int main(int argc, char** argv, char** envp) {
  while (*envp) {
    char path[] = "PATH=";
    char str[6];
    strncpy(str, *envp, 5);
    str[5] = '\0';

    if (strcmp(str, path) == 0) {
      printf("%s\n", *envp);
    }
    envp++;
  }
}
