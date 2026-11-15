#include <stdio.h>
#include "queue.h"
#include <inttypes.h>
#include <stdlib.h>

void *generator(void *arg);
void *filter(void *arg);

typedef struct {
  size_t  target_primes,
          printed_primes;
  pthread_mutex_t mutex;
  pthread_attr_t filter_attr;
  pthread_cond_t finished_condition;
} SieveState;

typedef struct {
  Queue* input_queue;
  SieveState *sieve;
} FilterArgs;

int run_sieve(size_t nprimes) {
  if (nprimes == 0) {
        return EXIT_FAILURE;
    }

    SieveState state;

    state.target_primes = nprimes;
    state.printed_primes = 0;

    if (pthread_mutex_init(&state.mutex, NULL) != 0) {
        fprintf(stderr, "Mutex initialization failed\n");
        return EXIT_FAILURE;
    }

    if (pthread_cond_init(&state.finished_condition, NULL) != 0) {
        fprintf(stderr, "Condition initialization failed\n");
        pthread_mutex_destroy(&state.mutex);
        return EXIT_FAILURE;
    }

    if (pthread_attr_init(&state.filter_attr) != 0) {
        fprintf(stderr, "Thread attributes failed\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_attr_setdetachstate(
            &state.filter_attr, PTHREAD_CREATE_DETACHED) != 0) {
        fprintf(stderr, "Detached thread configuration failed\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_attr_setstacksize(
            &state.filter_attr, 64 * 1024) != 0) {
        fprintf(stderr, "Thread stack configuration failed\n");
        exit(EXIT_FAILURE);
    }

    Queue *first_queue = queue_create(4);

    if (first_queue == NULL) {
        fprintf(stderr, "Initial queue creation failed\n");
        exit(EXIT_FAILURE);
    }

    FilterArgs *first_args = malloc(sizeof(*first_args));

    if (first_args == NULL) {
        fprintf(stderr, "Allocation failed\n");
        exit(EXIT_FAILURE);
    }

    first_args->input_queue = first_queue;
    first_args->sieve = &state;

    pthread_t first_filter;
    pthread_t generator_thread;

    if (pthread_create(&first_filter, &state.filter_attr,
                       filter, first_args) != 0) {
        free(first_args);
        fprintf(stderr, "First filter creation failed\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&generator_thread, &state.filter_attr,
                       generator, first_queue) != 0) {
        fprintf(stderr, "Generator creation failed\n");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&state.mutex);

    while (state.printed_primes < state.target_primes) {
        pthread_cond_wait(&state.finished_condition,
                          &state.mutex);
    }

    pthread_mutex_unlock(&state.mutex);

    fflush(stdout);

    exit(EXIT_SUCCESS);
}

void *generator(void *arg) {
  Queue *queue = arg;
  uint64_t number = 2;

  while (1) {
    queue_push(queue, number);
    number++;
  }

  return NULL;
}

void *filter(void *arg) {
  FilterArgs* args = arg;

  Queue* queue = args->input_queue;
  SieveState* sieve = args->sieve;

  free(args);

  uint64_t prime = queue_pop(queue);

  pthread_mutex_lock(&sieve->mutex);

  if (sieve->printed_primes < sieve->target_primes) {
    printf("%" PRIu64 "\n", prime);
    sieve->printed_primes++;

    if (sieve->printed_primes == sieve->target_primes) {
      pthread_cond_signal(&sieve->finished_condition);
    }
  }

  pthread_mutex_unlock(&sieve->mutex);

  Queue *output_queue = NULL;

  while (1) {
    uint64_t number = queue_pop(queue);

    if (number % prime == 0) {
      continue;
    }

    if (output_queue == NULL) {
      output_queue = queue_create(4);

      if (output_queue == NULL) {
        fprintf(stderr, "Queue creation failed\n");
        exit(EXIT_FAILURE);
      }

      FilterArgs *new_args = malloc(sizeof(FilterArgs));
      if (new_args == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
      }

      new_args->input_queue = output_queue;
      new_args->sieve = sieve;

      pthread_t filter_thread;

      if (pthread_create(&filter_thread, &sieve->filter_attr, filter, new_args) != 0) {
        fprintf(stderr, "Filter thread creation failed\n");
        free(new_args);
        exit(EXIT_FAILURE);
      }
    }
    queue_push(output_queue, number);
  }
  return NULL;
}