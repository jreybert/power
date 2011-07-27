#include <papi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <pwd.h>

#include "power.h"

#define INPUT_FILE ".power/papi_counters"
char **events_name;
float *events_weight;

char events_name_defaults[NB_EVENTS][PAPI_MAX_STR_LEN] = {"UNHALTED_CORE_CYCLES", "INSTRUCTION_RETIRED", "MEM_LOAD_RETIRED:L1D_HIT", "L2_DATA_RQSTS:ANY", "LLC_REFERENCES"};

static void check_events(int *events) {
  int i, retval;
  if ( NB_EVENTS > PAPI_num_counters() ) {
    fprintf(stderr, "Too many counters: %d\nMaximum available %d\n", NB_EVENTS, PAPI_num_counters());
  }
  for (i=0; i < NB_EVENTS; i++) {
    if ( (retval = PAPI_query_event(events[i])) != PAPI_OK) {
      PAPI_event_info_t info;
      PAPI_get_event_info(events[i], &info);
      fprintf(stderr,"No instruction counter for \"%s\" event\n%s\n", info.short_descr, PAPI_strerror(retval));
    }
  }
}

static void name_to_code(int *events_code) {
  int i, retval;
  for (i=0; i < NB_EVENTS; i++) {
    if ( (retval = PAPI_event_name_to_code(events_name[i], &events_code[i]) ) != PAPI_OK ) {
      fprintf(stderr,"Instruction counter \"%s\" does not exist\n%s\n", events_name[i], PAPI_strerror(retval));
    }
  }
}

void papi_init_counters_name_non_thread_safe() {

  events_name = malloc( sizeof(char*) * NB_EVENTS );
  events_weight = malloc( sizeof(float) * NB_EVENTS );
  int i;
  for (i = 0; i < NB_EVENTS; i++) {
    events_name[i] = malloc( sizeof(char) * PAPI_MAX_STR_LEN );
  }

  char *user = getenv("USER");
  struct passwd *p = getpwnam(user);
  char filename[256];
  sprintf(filename, "%s/%s", p->pw_dir, INPUT_FILE);

  FILE *file = fopen ( filename, "r" );
  if ( file != NULL ) {
    printf("Use \'%s\' file for PAPI counters\n", filename);
    char line [ PAPI_MAX_STR_LEN ]; 

    for (i = 0; i < NB_EVENTS; i++) {
//      fgets (line, sizeof(line), file );
//      int cch = strlen(line);
//      if (cch > 1 && line[cch-1] == '\n')
//        line[cch-1] = '\0';
      
      fscanf(file, "%s %f\n", events_name[i], &events_weight[i]);
//      strcpy(events_name[i], line);
    }
    fclose ( file );
  }
  else {
    printf("Can't find \'%s\' file, use default PAPI counters\n", filename);
    for (i = 0; i < NB_EVENTS; i++) {
      strcpy(events_name[i], events_name_defaults[i]);
    }
  }
}


void papi_init_eventset(pid_t pid, int *EventSet) {

  unsigned int native_events[NB_EVENTS];
  int retval;
  name_to_code(native_events);
  check_events(native_events);
  
  if ( (retval = PAPI_create_eventset(EventSet) ) != PAPI_OK) {    
    fprintf(stderr, "PAPI EventSet init error!\n%s\n", PAPI_strerror(retval));
  }

  if ( (retval = PAPI_add_events(*EventSet, native_events, NB_EVENTS) ) != PAPI_OK) {
    fprintf(stderr, "PAPI add events error!\n%s\n", PAPI_strerror(retval));
  }

  if ( (retval = PAPI_attach(*EventSet, pid) ) != PAPI_OK) {
    fprintf(stderr, "PAPI attach error!\n%s\n", PAPI_strerror(retval));
  }
}

void papi_global_init() {
  int retval;
  if ( (retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error!\n%s\n", PAPI_strerror(retval));
  }
  papi_init_counters_name_non_thread_safe();
}

void papi_thread_init() {
  int retval;
  if ( (retval = PAPI_thread_init(pthread_self)) != PAPI_OK) {
    fprintf(stderr, "PAPI thread init error!\n");
  }
}


