#include "queue.h"
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

void *producer(void *arg);
void *consumer(void *arg);

int main() {
  Queue *queue = queue_create(4);

  if (queue == NULL) {
    printf("Queue creation failed\n");
    return 1;
  }

  pthread_t producer_thread;
  pthread_t consumer_thread;

  if (pthread_create(&producer_thread, NULL, producer, queue) != 0) {
    printf("Producer thread creation failed\n");
    queue_destroy(queue);
    return EXIT_FAILURE;
  }

  if (pthread_create(&consumer_thread, NULL, consumer, queue) != 0) {
    printf("Consumer thread creation failed\n");
    exit(EXIT_FAILURE);
  }

  pthread_join(producer_thread, NULL);
  pthread_join(consumer_thread, NULL);

  queue_destroy(queue);

  return 0;
}

void *producer(void *arg) {
  Queue *queue = arg;

  for (int i = 2; i < 22; i++) {
    queue_push(queue, i);
  }

  return NULL;
}

void *consumer(void *arg) {
  Queue *queue = arg;

  for (int i = 2; i < 22; i++) {
    uint64_t number = queue_pop(queue);
    printf("%" PRIu64 "\n", number);
  }

  return NULL;
}