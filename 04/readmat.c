#include <stdio.h>
#include <stdlib.h>
#include <random>

int main(int argc, char** argv)
{
  if (argc == 2) {
    FILE* f = fopen(argv[1], "rb");
    if (!f) {
      printf("Could not open %s\n", argv[1]);
      exit(0);
    }

    int h, w;
    fread(&h, sizeof(int), 1, f);
    fread(&w, sizeof(int), 1, f);

    int size = w * h;
    int* M = new int[size];
    fread(M, sizeof(int), size, f);

    for (int i = 0; i < h; i++) {
      for (int j = 0; j < w; j++) {
        printf("%d ", M[i*w + j]);
      }
      printf("\n");
    }
    fclose(f);
  }
  else {
    printf("usage: %s <Filename>\n", argv[0]);
    exit(0);
  }

  return 0;
}

