#include <stdio.h>

int main() {
  for (int i = 0; i < 10; i++) {
    if (i % 2 == 0) {
      // cout << i;
      fprintf(stdout, "%d\n", i);
    } else {
      // cerr << i;
      fprintf(stderr, "%d\n", i);
    }
  }
}
