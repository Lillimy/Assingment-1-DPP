//
// Created by Lillian Myrland on 14/11/2026.
//

#ifndef queue_h
#define queue_h

#include <stddef.h>
#include <pthread.h>
#include <stdint.h>

typedef struct {
  uint64_t *buffer;

  size_t  capacity,
          head,
          tail,
          count;

  pthread_mutex_t mutex;
  pthread_cond_t  not_empty;
  pthread_cond_t  not_full;
} Queue;

Queue *queue_create(size_t capacity);
void queue_push(Queue *queue, uint64_t number);
uint64_t queue_pop(Queue *queue);
void queue_destroy(Queue *queue);

#endif //queue_h
