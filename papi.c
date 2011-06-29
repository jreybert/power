#include <papi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "power.h"

char events_name[NB_EVENTS][PAPI_MAX_STR_LEN] = {"UNHALTED_CORE_CYCLES", "INSTRUCTION_RETIRED", "MEM_LOAD_RETIRED:L1D_HIT", "MEM_LOAD_RETIRED:L2_HIT", "LLC_REFERENCES"};

static void check_events(int *events) {
  int i;
  if ( NB_EVENTS > PAPI_num_counters() ) {
    fprintf(stderr, "Too many counters: %d\nMaximum available %d\n", NB_EVENTS, PAPI_num_counters());
    exit(1);
  }
  for (i=0; i < NB_EVENTS; i++) {
    if (PAPI_query_event(events[i]) != PAPI_OK) {
      PAPI_event_info_t info;
      PAPI_get_event_info(events[i], &info);
      fprintf(stderr,"No instruction counter for \"%s\" event\n", info.short_descr);
      exit(1);
    }
  }
}


static void name_to_code(int *events_code) {
  int i;
  for (i=0; i < NB_EVENTS; i++) {
    PAPI_event_name_to_code(events_name[i], &events_code[i]);
  }
}


void init_papi(pid_t pid, int *EventSet) {

  unsigned int native_events[NB_EVENTS];

  if (PAPI_thread_init(pthread_self) != PAPI_OK) {
    fprintf(stderr, "PAPI thread init error!\n");
    exit(1);
  }
  // Traduction des string en code
  name_to_code(native_events);
  // Vérification des évènements
  check_events(native_events);
  
  if (PAPI_create_eventset(EventSet) != PAPI_OK) {    
    fprintf(stderr, "PAPI EventSet init error!\n");
    exit(1);
  }

  if (PAPI_add_events(*EventSet, native_events, NB_EVENTS) != PAPI_OK) {
    fprintf(stderr, "PAPI add events error!\n");
    exit(1);
  }

  if (PAPI_attach(*EventSet, pid) != PAPI_OK) {
    fprintf(stderr, "PAPI attach error!\n");
    exit(1);
  }
}

