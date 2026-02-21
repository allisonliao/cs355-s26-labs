// #define _XOPEN_SOURCE 600  // or 500
#include <ucontext.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define STACKSIZE 4096

void f(){
  printf("Hello World\n");
}

int main(int argc, char * argv[]){

  ucontext_t uc, uc_main;
  getcontext(&uc); // initialize context

  // setup stack and signal handling
  void* stack = malloc(STACKSIZE);
  uc.uc_stack.ss_sp = stack;
  uc.uc_stack.ss_size = STACKSIZE;
  uc.uc_stack.ss_flags = SS_DISABLE;
  sigemptyset(&(uc.uc_sigmask));
  uc.uc_link = &uc_main;

  makecontext(&uc, f, 0); // associate context with f()
  // setcontext(&uc); // "calls" f(); doesn't return if successful
  swapcontext(&uc_main, &uc);
  // perror("setcontext"); //setcontext() doesn’t return on success
  return 0;
}

