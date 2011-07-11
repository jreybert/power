
#include <stdio.h>
#include <stdlib.h>

#define TIME_DIFF(t1, t2) \
	((t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec))

int *a;
int size;

void pouletaccess() {
  srand(0);
  int i=0;
  long long tmp = 0;
  for (i=0; i< size; i++) {
    tmp += a[rand() % size];
  }
  printf("%lld\n", tmp);
}

int main(int argc, char ** argv){
	int i,j;
	struct timeval s,e;

  size = atoi(argv[1]);
  a = malloc(sizeof(int) * size);
  for(j=0;j<size;j++){			
    a[j] = 1;
  }


	pouletaccess();
//  srand48(0);
  


	return 0;
}
