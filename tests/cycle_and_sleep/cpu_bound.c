#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define SIZE 128

typedef int test_t;
int sleep_in_us;

test_t array[SIZE];

void sqrt_on_one() {
  test_t n = rand();
  long long t = 0;
  int i = 0;
  while(1) {
    t = sqrt(n);
    if (++i%100000 == 0)
      usleep(sleep_in_us);
  }
  printf("%lld\n", t);
}

void sqrt_on_array() {
  int i = 0;
  long long t = 0;
  while(1) {
    for (i=0; i<SIZE; i++) {
      t = sqrt(array[i]);
      if (++i%100000 == 0)
        usleep(sleep_in_us);
    }
  }
  printf("%lld\n", t);
}

void sqrt_on_rand() {
  int i = 0;
  long long t = 0;
  while(1) {
    t = sqrt(rand());

    if (++i%100000 == 0)
      usleep(sleep_in_us);
  }
  printf("%lld\n", t);
}


int main(int argc, char ** argv) {
  int i, opt;

  if ( argc != 3 ) {
    fprintf(stderr, "Usage: %s option sleep_in_us\n\t1 : sqrt on one variable\n\t2 : sqrt on variable in array\n\t3 : sqrt on rand\n", argv[0]);
    return 1;
  }


  for (i=0; i<SIZE; i++)
    array[i] = rand();

  opt = atoi(argv[1]);
  sleep_in_us = atoi(argv[2]);

  switch (opt) {
    case 1:
      sqrt_on_one();
      break;
    case 2:
      sqrt_on_array();
      break;
    case 3:
      sqrt_on_rand();
      break;
    default:
      fprintf(stderr, "Error\n");
      return 1;
  }

}
