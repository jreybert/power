#include <stdio.h>
#include <stdlib.h>

//#include <papi.h>
//#include "../perfcount/perfcount.h" 

#define TIME_DIFF(t1, t2) \
	((t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec))

int **a;
int **b;
int **c;
long size;

void mulMat() {
  int i,j,k;
  for(i=0;i<size;i++) {
    for(j=0;j<size;j++) {
      //			c[i][j]=0;
      for(k=0;k<size;k++) {
        c[i][j]+=a[i][k]*b[k][j];
      }
    }
  }
}


int main(int argc, char ** argv){
	int i,j;
	struct timeval s,e;
  //counter_t *counter1, *counter2;

  size = atol(argv[1]);
  a = malloc(sizeof(int*) * size);
  b = malloc(sizeof(int*) * size);
  c = malloc(sizeof(int*) * size);
  for ( i = 0; i < size; i++) {
    a[i] = malloc(sizeof(int) * size);
    b[i] = malloc(sizeof(int) * size);
    c[i] = malloc(sizeof(int) * size);
		for(j=0;j<size;j++){			
			a[i][j] = 1;
			b[i][j] = 2;
			c[i][j] = 0;
    }
  }



//  init_counter_env();
//  register_thread(PERF_COUNTERS);
//  counter1 = create_counter(PERF_COUNTERS,"MASTER");
//  start_counter(counter1);
	mulMat();
//  stop_counter(counter1); 
//  print_counter(counter1);

	return 0;
}
