#ifndef __POWER_H
#define __POWER_H

#include <sys/time.h>
#include <sys/resource.h>

#define PATH_TO_CPU "/sys/devices/system/cpu/"
#define MAX_LINE_LEN 255
#define SYSFS_PATH_MAX 255
#define MAX_CSTATE_NAME 255

#define PSTATE_IN_MS(x) ( (x)*10)
#define CSTATE_IN_MS(x) ( (x)/1000)


#define ST_18_RUN  31
#define ST_18_IDLE 15
#define ST_13_RUN  25
#define ST_13_IDLE 10
#define ST_10_RUN  13.1
#define ST_10_IDLE 4


#define MAX_FREQ_NB 8

typedef struct {
  char name[MAX_CSTATE_NAME];
  unsigned long long time;
  unsigned long usage;
} cstate_stat_t;

// time is the time passed in this state
// usage is the number of time CPU went through this state

typedef struct {
  unsigned long long freq;
  unsigned long long time;
} time_in_freq_t;


typedef struct {
  int waitstatus;
  // general system
  int nb_cpus;
  struct rusage ru;
  struct timeval time_start, time_elapsed; /* Wallclock time of process.  */
  // cstates
  int nb_states;
  unsigned long long *cstates_total;
  cstate_stat_t *cstate_stat_beg, *cstate_stat_end;
  cstate_stat_t *cstate_trace_beg, *cstate_trace_end;
  
  int nb_freqs;
  unsigned long long *freqs_total;
  time_in_freq_t **time_in_freq_beg, **time_in_freq_end;
  time_in_freq_t **time_in_freq_trace_beg, **time_in_freq_trace_end;
} infos_t;

// Trace functions
int start_tracing(infos_t *infos);
int stop_tracing();

// cstates functions
void init_cstates(infos_t *infos);
void refresh_cstates_trace(infos_t *infos);
void start_cstates(infos_t *infos);
void finish_cstates(infos_t *infos);
void print_cstates(infos_t *infos);


// freqs functions
void init_freqs(infos_t *infos);
void refresh_freqs_trace(infos_t *infos);
void start_freqs(infos_t *infos);
void finish_freqs(infos_t *infos);
void print_freqs(infos_t *infos);


#endif
