#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <list>
#include <string.h>

using namespace std;

struct chunk {
  int size;
  int padding;
};
list<chunk *> flist;
list<chunk *> ulist; // todo: keep track of user data here

void *myalloc (size_t size) {
  if (size <= 0) return NULL;

  for (chunk* ptr : flist) {
    if (ptr->size >= size) {
      flist.remove(ptr);
      return (void*) (ptr + 1);
    }
  }

  void *memory = sbrk(size + sizeof(struct chunk));
  if (memory == (void *) -1) {
    return NULL;
  } 
  else {
    struct chunk *cnk = (struct chunk*) memory;
    cnk->size = size;
    return (void*) (cnk + 1);
  }
}

void myfree(void *memory) {
  if (memory != NULL) {
    struct chunk *cnk = (struct chunk*) ((struct chunk*) memory -1);
    flist.push_front(cnk);
  }
}

void mem_dump() {
  // todo: print user/free data chunks
}

int main() {

  void *init = sbrk(0);
  printf("Base address: %p\n", init);

  double* b1 = (double*) myalloc(sizeof(double)*100);
  for (int i = 0; i < 100; i++) b1[i] = i;

  int* b2 = (int*) myalloc(sizeof(int)*100);
  for (int i = 0; i < 100; i++) b2[i] = i;

  printf("1. Memory dump:\n");
  mem_dump();

  myfree(b1);
  char* b3 = (char*) myalloc(32);
  strncpy(b3, "Monster Truck", 32);

  printf("2. Memory dump:\n");
  mem_dump();

  return 0;
}

