#define _XOPEN_SOURCE 600  // or 500
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <ucontext.h>
#include <unistd.h>
#define STACKSIZE 4096

struct timeval tstart;
int which;  // 0 for even, 1 for odd
static ucontext_t uceven, ucodd, ucmain;

int checkerror(int v, const char* msg) {
  if (v < 0) {
    perror(msg);
    exit(0);
  }
  return v;
}

void sig_alarm(int signum) {
  if (which == 0) {
    which = 1;
    swapcontext(&uceven, &ucodd);
  } else {
    which = 0;
    swapcontext(&ucodd, &uceven);
  }
}

void nextEven() {
  for (unsigned int i = 0;; i += 2) {
    printf("even: %d\n", i);
    sleep(1);
    // swapcontext(&uceven, &ucodd);
  }
}

void nextOdd() {
  for (unsigned int i = 1;; i += 2) {
    printf("odd: %d\n", i);
    sleep(1);
    // swapcontext(&ucodd, &uceven);
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

  makecontext(&uceven, nextEven, 0);
  makecontext(&ucodd, nextOdd, 0);

  struct itimerval v;
  v.it_interval.tv_sec = 2;
  v.it_interval.tv_usec = 0;
  v.it_value.tv_sec = 2;
  v.it_value.tv_usec = 0;

  gettimeofday(&tstart, NULL);
  which = 0;
  signal(SIGALRM, sig_alarm);
  // setitimer(ITIMER_REAL, &v, NULL);
  checkerror(setitimer(ITIMER_REAL, &v, NULL), "settimer");

  swapcontext(&ucmain, &uceven);

  while (1) pause();
  return 0;
}
