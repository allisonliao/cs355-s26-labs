#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ALLOCSIZE 1000   // size of total available space

static char allocbuf[ALLOCSIZE]; // our stack for allocating memory
static char* allocp = allocbuf;  // a pointer to the next free block in memory

char* stalloc(int n) 
{
  // implement me
  return 0;
}

void stfree(char* p)
{
  // implement me
}

int main()
{
  char* b1 = stalloc(200);
  assert(b1 == allocbuf);
  assert(allocp == (allocbuf + 200));

  char* b2 = stalloc(300);
  assert(b2 == 0); // todo: update tests
  assert(allocp == 0);

  stfree(b2);
  assert(b1 == 0);
  assert(allocp == 0);

  char* b3 = stalloc(1000);
  assert(b3 == 0);
  assert(allocp == 0);

  char* b4 = stalloc(0);
  assert(b4 == allocp);

  return 0;
}

