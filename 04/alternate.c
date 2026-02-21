// #define _XOPEN_SOURCE 600  // or 500
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <ucontext.h>
#include <unistd.h>
#define STACKSIZE 4096

static ucontext_t uceven, ucodd, ucmain;

void even() {
  struct timespec delay = {1, 0};
  for (unsigned int i = 0;; i += 2) {
    printf("even: %d\n", i);
    nanosleep(&delay, 0);
    swapcontext(&uceven, &ucodd);
  }
}

void odd() {
  struct timespec delay = {1, 0};
  for (unsigned int i = 1;; i += 2) {
    printf("odd: %d\n", i);
    nanosleep(&delay, 0);
    swapcontext(&ucodd, &uceven);
  }
}

int main() {
  setvbuf(stdout, 0, _IOLBF, 0);

  getcontext(&uceven);
  getcontext(&ucodd);

  // setup stack and signal handling
  void* stack = malloc(STACKSIZE);
  uceven.uc_stack.ss_sp = stack;
  uceven.uc_stack.ss_size = STACKSIZE;
  uceven.uc_stack.ss_flags = SS_DISABLE;
  sigemptyset(&(uceven.uc_sigmask));

  void* stack2 = malloc(STACKSIZE);
  ucodd.uc_stack.ss_sp = stack2;
  ucodd.uc_stack.ss_size = STACKSIZE;
  ucodd.uc_stack.ss_flags = SS_DISABLE;
  sigemptyset(&(ucodd.uc_sigmask));

  makecontext(&uceven, even, 0);
  makecontext(&ucodd, odd, 0);

  swapcontext(&ucmain, &uceven);
  swapcontext(&uceven, &ucodd);
}
