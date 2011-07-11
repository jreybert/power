#include <papi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "power.h"

char events_name[NB_EVENTS][PAPI_MAX_STR_LEN] = {"UNHALTED_CORE_CYCLES", "INSTRUCTION_RETIRED", "MEM_LOAD_RETIRED:L1D_HIT", "L2_DATA_RQSTS:ANY", "LLC_REFERENCES"};

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

void papi_global_init() {
  int retval;
  if ( (retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error!\n%s\n", PAPI_strerror(retval));
  }
}

void papi_thread_init() {
  int retval;
  if ( (retval = PAPI_thread_init(pthread_self)) != PAPI_OK) {
    fprintf(stderr, "PAPI thread init error!\n");
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

