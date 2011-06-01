#include <stdio.h>
#include <stdlib.h>

#include <papi.h>
#include "../perfcount/perfcount.h" 

#define N 1024

#define TIME_DIFF(t1, t2) \
	((t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec))

double a[N][N];
double b[N][N];
double c[N][N];

void mulMat() {
	int i,j,k;
	for(i=0;i<N;i++) {
//			c[i][j]=0;
			for(k=0;k<N;k++) {
		for(j=0;j<N;j++) {
				c[i][j]+=a[i][k]*b[k][j];			
      }
		}
	}
}


int main(int argc, char ** argv){
	int i,j;
	struct timeval s,e;
  counter_t *counter1, *counter2;

  srand48(0);
  
	for(i=0;i<N;i++)
		for(j=0;j<N;j++){			
			a[i][j] = drand48();
			b[i][j] = drand48();
			c[i][j] = 0;
		}
  init_counter_env();
  register_thread(PERF_COUNTERS);
  counter1 = create_counter(PERF_COUNTERS,"MASTER");
  counter2 = create_counter(PERF_COUNTERS,"MASTER");
  start_counter(counter1);
	mulMat();
  stop_counter(counter1); 
  print_counter(counter1);


	return 0;
}
