#ifndef __POWER_H
#define __POWER_H

#include <sys/time.h>
#include <sys/resource.h>
#include <papi.h>

#define TDIFF(te, ts) (te.tv_sec - ts.tv_sec + (te.tv_usec - ts.tv_usec) * 1e-6)

#define MAX_PROC_WATCHED 128
#define TRACE_DIR "/tmp/trace"
#define FREQ_STAT_PATH TRACE_DIR"/freq_stat"
#define TEMP_PATH TRACE_DIR"/temp_stat"

#define PATH_TO_CPU "/sys/devices/system/cpu/"
#define MAX_LINE_LEN 255
#define SYSFS_PATH_MAX 255
#define MAX_CSTATE_NAME 255

#define NB_EVENTS 5

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
  cstate_stat_t **cstate_trace_beg, **cstate_trace_end;
  
  int nb_freqs;
  time_in_freq_t **time_in_freq_trace_beg, **time_in_freq_trace_end;
} infos_t;

// Trace functions
int start_tracing(infos_t *infos, pid_t pid);
int stop_tracing();

// cstates functions
void init_cstates(infos_t *infos);
void refresh_cstates_trace(infos_t *infos);


// freqs functions
void init_freqs(infos_t *infos);
void refresh_freqs_trace(infos_t *infos);


#endif
