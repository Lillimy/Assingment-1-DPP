#include "queue.h"
#include <stdlib.h>

Queue *queue_create(size_t capacity) {
  if (capacity == 0) {
    return NULL;
  }
  Queue *queue = malloc(sizeof(Queue));
  if (queue == NULL) {
    return NULL;
  }

  queue->buffer = malloc(capacity * sizeof(uint64_t));
  if (queue->buffer == NULL) {
    free(queue);
    return NULL;
  }

  queue->capacity = capacity;
  queue->head = 0;
  queue->tail = 0;
  queue->count = 0;

  if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
    free(queue->buffer);
    free(queue);
    return NULL;
  }

  if (pthread_cond_init(&queue->not_empty, NULL) != 0) {
    pthread_mutex_destroy(&queue->mutex);
    free(queue->buffer);
    free(queue);
    return NULL;
  }

  if (pthread_cond_init(&queue->not_full, NULL) != 0) {
    pthread_cond_destroy(&queue->not_empty);
    pthread_mutex_destroy(&queue->mutex);
    free(queue->buffer);
    free(queue);
    return NULL;
  }

  return queue;
}

void queue_push(Queue *queue, uint64_t number) {
  pthread_mutex_lock(&queue->mutex);

  while (queue->count == queue->capacity) {
    pthread_cond_wait(&queue->not_full, &queue->mutex);
  }

  queue->buffer[queue->tail] = number;
  queue->tail = (queue->tail + 1) % queue->capacity;
  queue->count++;

  pthread_cond_signal(&queue->not_empty);

  pthread_mutex_unlock(&queue->mutex);
}

uint64_t queue_pop(Queue *queue) {
  pthread_mutex_lock(&queue->mutex);

  while (queue->count == 0) {
    pthread_cond_wait(&queue->not_empty, &queue->mutex);
  }

  const uint64_t number = queue->buffer[queue->head];

  queue->head = (queue->head + 1) % queue->capacity;
  queue->count--;

  pthread_cond_signal(&queue->not_full);
  pthread_mutex_unlock(&queue->mutex);

  return number;

}

void queue_destroy(Queue *queue) {
  if (queue == NULL) {
    return;
  }

  pthread_cond_destroy(&queue->not_empty);
  pthread_cond_destroy(&queue->not_full);
  pthread_mutex_destroy(&queue->mutex);

  free(queue->buffer);
  free(queue);
}

