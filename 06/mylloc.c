#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <list>

using namespace std;

struct chunk {
  int size;
  int padding;
};
list<chunk*> flist;
list<chunk*> ulist;  // todo: keep track of user data here

void* myalloc(size_t size) {
  if (size <= 0) return NULL;

  for (chunk* ptr : flist) {
    if (ptr->size >= size) {
      flist.remove(ptr);
      return (void*)(ptr + 1);
    }
  }

  void* memory = sbrk(size + sizeof(struct chunk));
  if (memory == (void*)-1) {
    return NULL;
  } else {
    struct chunk* cnk = (struct chunk*)memory;
    cnk->size = size;
    ulist.push_front(cnk);
    return (void*)(cnk + 1);
  }
}

void myfree(void* memory) {
  if (memory != NULL) {
    struct chunk* cnk = (struct chunk*)((struct chunk*)memory - 1);
    flist.push_front(cnk);
  }
}

void mem_dump() {
  for (struct chunk* cnk : ulist) {
    printf("size: %d, address: 0x%x\n", cnk->size, &cnk);
    myfree(cnk);
  }
}

int main() {
  void* init = sbrk(0);
  printf("Base address: %p\n", init);

  double* b1 = (double*)myalloc(sizeof(double) * 100);
  for (int i = 0; i < 100; i++) b1[i] = i;

  int* b2 = (int*)myalloc(sizeof(int) * 100);
  for (int i = 0; i < 100; i++) b2[i] = i;

  printf("1. Memory dump:\n");
  mem_dump();

  myfree(b1);
  char* b3 = (char*)myalloc(32);
  strncpy(b3, "Monster Truck", 32);

  printf("2. Memory dump:\n");
  mem_dump();

  return 0;
}

// QUESTION: There are three allocations. Why are there only two chunks?
// ANSWER: There are only two chunks because the last allocation is not yet
// added to the ulist, it gets added when the next block of memory is allocated
// (functionality is it returns the address which the user can use without
// needing it to be in the list).

// QUESTION: Below, the base address is 0x56418de96000. The first chunk listed
// has address 0x56418de96328. Why?
// ANSWER: Because the address is updated to be the end of the block of memory
// and it is 0x328 off which is 808 decimal, which is 800 for the memory block
// and 8 for the struct.
