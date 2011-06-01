#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIZE 1024

int array[SIZE];

void sqrt_on_one() {
  int n = rand();
  long long t = 0;
  while(1) {
    t = sqrt(n);
  }
  printf("%lld\n", t);
}

void sqrt_on_array() {
  int i;
  long long t = 0;
  while(1) {
    for (i=0; i<SIZE; i++) {
      t = sqrt(array[i]);
    }
  }
  printf("%lld\n", t);
}

void sqrt_on_rand() {
  int i;
  long long t = 0;
  while(1) {
    t = sqrt(rand());
  }
  printf("%lld\n", t);
}


int main(int argc, char ** argv) {
  int i, opt;

  if ( argc != 2 ) {
    fprintf(stderr, "Usage: %s option\n\t1 : sqrt on one variable\n\t2 : sqrt on variable in array\n\t3 : sqrt on rand\n", argv[0]);
    return 1;
  }


  for (i=0; i<SIZE; i++)
    array[i] = rand();

  opt = atoi(argv[1]);

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
