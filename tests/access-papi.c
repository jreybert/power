#include <stdio.h>
#include <stdlib.h>

#include <papi.h>
#define NUM_EVENTS  3

typedef int array_t;


int **a;
int **b;
int **c;
long size;

void mulMatUnopt() {
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

void mulMatOpt() {
  int i,j,k;
  for(i=0;i<size;i++) {
    for(k=0;k<size;k++) {
      for(j=0;j<size;j++) {
        c[i][j]+=a[i][k]*b[k][j];			
      }
    }
  }
}

void check_events(int *events, int nb_events) {
  int i;
  if ( nb_events > PAPI_num_counters() ) {
    fprintf(stderr, "Too many counters: %d\nMaximum available %d\n", nb_events, PAPI_num_counters());
    exit(1);
  }
  for (i=0; i < nb_events; i++) {
    if (PAPI_query_event(events[i]) != PAPI_OK) {
      PAPI_event_info_t info;
      PAPI_get_event_info(events[i], &info);
      fprintf(stderr,"No instruction counter for \"%s\" event\n", info.short_descr);
      exit(1);
    }
  }
}

void print_values(int *events, long long *values, int nb_events) {
  int i;
  for (i = 0; i < nb_events; i++) {
    PAPI_event_info_t info;
    PAPI_get_event_info(events[i], &info);
    printf("%20s : %lld\n", info.short_descr, values[i]);
  }
}

int main(int argc, char ** argv){
	int i,j;
  int events[NUM_EVENTS] = {PAPI_TOT_INS, PAPI_TOT_CYC, PAPI_L2_DCM};
  long long values[NUM_EVENTS];
  
  check_events(events, NUM_EVENTS);

  if ( argc != 2 ) {
    fprintf(stderr, "usage: %s matrix_size\n", argv[0]);
    exit(1);
  }
  size = atoi(argv[1]);
  
  // start the counters in the events array
  if (PAPI_start_counters(events, NUM_EVENTS) != PAPI_OK) {
    fprintf(stderr, "error starting counters\n");
    exit(1);
  }

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
 // a = malloc(sizeof(array_t) * size);
 // for(j=0;j<size;j++){			
 //   a[j] = 1;
 // }
  
  // first reading of the counters
  if (PAPI_read_counters(values, NUM_EVENTS) != PAPI_OK) {
    fprintf(stderr, "error read counters\n");
    exit(1);
  }
  printf("Matrix init\n");
  print_values(events, values, NUM_EVENTS);

	mulMatUnopt();  
//	array_access_unopt();

  // second reading of the counters
  if (PAPI_read_counters(values, NUM_EVENTS) != PAPI_OK) {
    fprintf(stderr, "error read counters\n");
    exit(1);
  }
  printf("Unoptimized matrix access\n");
  print_values(events, values, NUM_EVENTS);

	//array_access_opt();
	//mulMatOpt();
  
  // third and last reading
  if (PAPI_stop_counters(values, NUM_EVENTS) != PAPI_OK) {
    fprintf(stderr, "error stop counters\n");
    exit(1);
  }
  printf("Optimized matrix access\n");
  print_values(events, values, NUM_EVENTS);

	return 0;
}
