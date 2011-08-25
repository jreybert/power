#include <papi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#include "power.h"



void check_events(int *events, int nb_events) {
  int i, retval;
  if ( nb_events > PAPI_num_counters() ) {
    fprintf(stderr, "Too many counters: %d\nMaximum available %d\n", nb_events, PAPI_num_counters());
  }
  for (i=0; i < nb_events; i++) {
    if ( (retval = PAPI_query_event(events[i])) != PAPI_OK) {
      PAPI_event_info_t info;
      PAPI_get_event_info(events[i], &info);
      fprintf(stderr,"No instruction counter for \"%s\" event\n%s\n", info.short_descr, PAPI_strerror(retval));
    }
  }
}

void name_to_code(int *events_code, char **events_name, int nb_events ) {
  int i, retval;
  for (i=0; i < nb_events; i++) {
    if ( (retval = PAPI_event_name_to_code(events_name[i], &events_code[i]) ) != PAPI_OK ) {
      fprintf(stderr,"Instruction counter \"%s\" does not exist\n%s\n", events_name[i], PAPI_strerror(retval));
    }
  }
}

void papi_init_counters_name_non_thread_safe() {


//  char *user = getenv("USER");
//  struct passwd *p = getpwnam(user);
//  char filename[256];
//  sprintf(filename, "%s/%s", p->pw_dir, CONFIG_FILE_PATH);
//  int i;
//
//  FILE *file = fopen ( filename, "r" );
//
//  if ( file != NULL ) {
//    printf("Use \'%s\' file for PAPI counters\n", filename);
//    char line [ PAPI_MAX_STR_LEN ]; 
//
//    for (i = 0; i < global_config.nb_papi_events; i++) {
//      fscanf(file, "%s %f\n", global_config.events_name[i], &global_config.events_weight[i]);
////      strcpy(global_config.events_name[i], line);
//    }
//    fclose ( file );
//  }
//  else {
//    printf("Can't find \'%s\' file, use default PAPI counters\n", filename);
//    for (i = 0; i < global_config.nb_papi_events; i++) {
//      strcpy(global_config.events_name[i], events_name_defaults[i]);
//    }
//  }
}


void papi_init_eventset(pid_t pid, int *EventSet) {

  int retval;
  if ( (retval = PAPI_create_eventset(EventSet) ) != PAPI_OK) {    
    fprintf(stderr, "PAPI EventSet init error!\n%s\n", PAPI_strerror(retval));
  }

  if ( (retval = PAPI_add_events(*EventSet, global_config.native_events, global_config.nb_papi_events) ) != PAPI_OK) {
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
  //papi_init_counters_name_non_thread_safe();
}


void papi_thread_init() {
  int retval;
  if ( (retval = PAPI_thread_init(pthread_self)) != PAPI_OK) {
    fprintf(stderr, "PAPI thread init error!\n");
  }
  
}


