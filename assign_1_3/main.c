
#include "sieve.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s  <number_of_primes>\n", argv[0]);
    return EXIT_FAILURE;
  }

  errno = 0;
  char *end = NULL;

  unsigned long long requested =
      strtoull(argv[1], &end, 10);

  if (errno != 0 ||
      *end != '\0' ||
      requested == 0 ||
      requested > SIZE_MAX ||
      argv[1][0] == '-') {
    fprintf(stderr, "Invalid number of primes\n");
    return EXIT_FAILURE;
      }

  return run_sieve((size_t)requested);
}

