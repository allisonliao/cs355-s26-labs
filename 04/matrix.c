#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct thread_data {
  int idx;
  int start;
  int end;
  int* first;
  int* second;
  int* result;
};

void* add_mat(void* arg) {
  struct thread_data* data = (struct thread_data*)arg;
  printf("Starting thread %lu (idx = %d)\n", pthread_self(), data->idx);
  for (int i = data->start; i < data->end; i++) {
    data->result[i] = data->first[i] + data->second[i];
  }
  return NULL;
}

int* read_mat(FILE* f, int tot) {
  int* m = (int*)malloc(tot * sizeof(int));
  fread(m, sizeof(int), tot, f);
  return m;
}

int main(int argc, char** argv) {
  if (argc < 4) {
    printf("usage: %s <M> <mat1> <mat2>\n", argv[0]);
    exit(1);
  }

  int M = atoi(argv[1]);
  if (M < 1) {
    printf("M must be >= 1\n");
    exit(1);
  }

  FILE* mat1 = fopen(argv[2], "rb");
  if (!mat1) {
    printf("File 1 cannot be opened\n");
    exit(1);
  }
  FILE* mat2 = fopen(argv[3], "rb");
  if (!mat2) {
    printf("File 2 cannot be opened\n");
    exit(1);
  }

  int h1, w1, h2, w2;
  fread(&h1, sizeof(int), 1, mat1);
  fread(&w1, sizeof(int), 1, mat1);
  fread(&h2, sizeof(int), 1, mat2);
  fread(&w2, sizeof(int), 1, mat2);

  if (h1 != h2 || w1 != w2) {
    printf("Dimensions do not match!\n");
    exit(1);
  }

  int total = h1 * w1;
  int* a = read_mat(mat1, total);
  int* b = read_mat(mat2, total);
  int* c = (int*)malloc(total * sizeof(int));
  fclose(mat1);
  fclose(mat2);

  if (M > total) M = total;

  pthread_t* threads = (pthread_t*)malloc(M * sizeof(pthread_t));
  struct thread_data* tds =
      (struct thread_data*)malloc(M * sizeof(struct thread_data));

  int chunk = total / M;
  int remainder = total % M;

  struct timespec ts, te;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  for (int i = 0; i < M; i++) {
    tds[i].idx = i;
    tds[i].start = i * chunk + (i < remainder ? i : remainder);
    tds[i].end = tds[i].start + chunk + (i < remainder ? 1 : 0);
    tds[i].first = a;
    tds[i].second = b;
    tds[i].result = c;
    pthread_create(&threads[i], NULL, add_mat, &tds[i]);
  }

  for (int i = 0; i < M; i++) {
    pthread_join(threads[i], NULL);
  }

  clock_gettime(CLOCK_MONOTONIC, &te);
  float dt = (te.tv_sec - ts.tv_sec) + (te.tv_nsec - ts.tv_nsec) / 1e9;
  printf("duration: %.2f\n", dt);

  char filename[256];
  snprintf(filename, sizeof(filename), "%s_%s", argv[2], argv[3]);

  FILE* outfile = fopen(filename, "wb");
  if (!outfile) {
    printf("Output file could not be created\n");
    exit(1);
  }

  fwrite(&h1, sizeof(int), 1, outfile);
  fwrite(&w1, sizeof(int), 1, outfile);
  fwrite(c, sizeof(int), total, outfile);
  fclose(outfile);

  free(a);
  free(b);
  free(c);
  free(threads);
  free(tds);
  return 0;
}