
#include <stdio.h>
#include <stdlib.h>

#include <papi.h>
#include "../perfcount/perfcount.h" 

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
  counter_t *counter1, *counter2;

  size = atoi(argv[1]);
  a = malloc(sizeof(int) * size);
  for(j=0;j<size;j++){			
    a[j] = 1;
  }


  init_counter_env();
  register_thread(PERF_COUNTERS);
  counter1 = create_counter(PERF_COUNTERS,"MASTER");
  counter2 = create_counter(PERF_COUNTERS,"MASTER");
  start_counter(counter1);
	pouletaccess();
  stop_counter(counter1); 
  print_counter(counter1);
//  srand48(0);
  


	return 0;
}
