#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ALLOCSIZE 1000   // size of total available space

static char allocbuf[ALLOCSIZE]; // our stack for allocating memory
static char* allocp = allocbuf;  // a pointer to the next free block in memory

char* stalloc(int n) 
{
  if (n > ALLOCSIZE) return 0;
  allocp = allocp + n;
  return allocp-n;
}

void stfree(char* p)
{
  for (int i = 0; (i + p) < allocp; i++) {
    char* temp = i+p;
    *temp = NULL;
  }
  allocp = p;
}

int main()
{
  char* b1 = stalloc(200);
  assert(b1 == allocbuf);
  assert(allocp == (allocbuf + 200));

  char* b2 = stalloc(300);
  assert(b2 == (allocbuf+200));
  assert(allocp == (allocbuf+500));

  stfree(b2);
  assert(b1 == allocbuf);
  assert(allocp == (allocbuf+200));

  char* b3 = stalloc(1000);
  assert(b3 == (allocbuf+200));
  assert(allocp == (allocbuf+1200));

  char* b4 = stalloc(0);
  assert(b4 == allocp);

  return 0;
}

