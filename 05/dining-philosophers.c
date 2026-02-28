#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define N 5
#define ROUNDS 100

pthread_mutex_t mutex[N];  // one per fork
struct thread_data {
  int id;
};

void* Philosophize(void* args) {
  struct thread_data* data = (struct thread_data*)args;

  printf("Starting thread %d\n", data->id);
  for (int i = 0; i < ROUNDS; i++) {
    float duration = 0.01;

    int decision = rand() % 2;
    if (decision == 0) {
      printf("%d is thinking\n", data->id);
      sleep(duration);
    } else {
      printf("%d is hungry\n", data->id);

      int leftFork = data->id;
      int rightFork = (data->id + 1) % N;

      int firstFork = (leftFork < rightFork) ? leftFork : rightFork;
      int secondFork = (leftFork < rightFork) ? rightFork : leftFork;

      // pthread_mutex_lock(&mutex[leftFork]);
      // pthread_mutex_lock(&mutex[rightFork]);
      pthread_mutex_lock(&mutex[firstFork]); 
      pthread_mutex_lock(&mutex[secondFork]);
      printf("%d is eating\n", data->id);
      sleep(duration);
      // pthread_mutex_unlock(&mutex[leftFork]);
      // pthread_mutex_unlock(&mutex[rightFork]);
      pthread_mutex_unlock(&mutex[firstFork]); 
      pthread_mutex_unlock(&mutex[secondFork]);
    }
  }
  return NULL;
}

int main() {
  struct thread_data data[N];
  for (int i = 0; i < N; i++) {
    pthread_mutex_init(&(mutex[i]), NULL);
    data[i].id = i;
  }

  pthread_t threads[N];

  for (int i = 0; i < N; i++) {
    pthread_create(&threads[i], NULL, Philosophize, &(data[i]));
  }

  for (int i = 0; i < N; i++) {
    pthread_join(threads[i], NULL);
  }

  for (int i = 0; i < N; i++) {
    pthread_mutex_destroy(&(mutex[i]));
  }
  return 0;
}
