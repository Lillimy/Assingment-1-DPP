/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>

#include "simulate.h"
#include <pthread.h>


/* Add any global variables you may need. */

typedef struct {
    int start;
    int end;
    double *old_array;
    double *current_array;
    double *next_array;
} wave_data_t;

/* Add any functions you may need (like a worker) here. */

void *wave_worker(void *arg) {
  wave_data_t *data = (wave_data_t *)arg;

  // inner loop for the wave simulation
  for(int i = data->start; i < data->end; i++) {
    data->next_array[i] = 2 * data->current_array[i] - data->old_array[i] +
     0.15 * (data->current_array[i-1] - 2 * data->current_array[i] + 
      data->current_array[i+1]);
  }

  return NULL;
}

/*
 * Executes the entire simulation.
 *
 * Implement your code here.
 *
 * i_max: how many data points are on a single wave
 * t_max: how many iterations the simulation should run
 * num_threads: how many threads to use (excluding the main threads)
 * old_array: array of size i_max filled with data for t-1
 * current_array: array of size i_max filled with data for t
 * next_array: array of size i_max. You should fill this with t+1
 */
double *simulate(const int i_max, const int t_max, const int num_threads,
        double *old_array, double *current_array, double *next_array) {

    pthread_t threads[num_threads];
    wave_data_t data[num_threads];

    // Divide points between workers
    int total_points = i_max - 2;
    int base = total_points / num_threads;
    int remainder = total_points % num_threads;


    for (int t = 0; t < t_max; t++) {

      // arguments for each threa
      for (int k = 0; k < num_threads; k++) {
        int start = 1 + k * base + (k < remainder ? k : remainder);
        int count = base + (k < remainder ? 1 : 0);
        data[k].start = start;
        data[k].end = start + count;
        data[k].old_array = old_array;
        data[k].current_array = current_array;
        data[k].next_array = next_array;
      }

      // create all worker threads
      for (int k = 0; k < num_threads; k++) {
        pthread_create(&threads[k], NULL, wave_worker, &data[k]);
      }


      // join all worker threads
      for (int k = 0; k < num_threads; k++) {
        pthread_join(threads[k], NULL);
      }

   /*
     * After each timestep, you should swap the buffers around. Watch out none
     * of the threads actually use the buffers at that time.
     */

      double *temp = old_array;
      old_array = current_array;
      current_array = next_array;
      next_array = temp;

      
    }

    /* You should return a pointer to the array with the final results. */
    return current_array;
}
