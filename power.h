#ifndef __POWER_H
#define __POWER_H

#include <sys/time.h>
#include <sys/resource.h>

#define PATH_TO_CPU "/sys/devices/system/cpu/"
#define MAX_LINE_LEN 255
#define SYSFS_PATH_MAX 255
#define MAX_CSTATE_NAME 255

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
  
  unsigned long long *freqs_total;
  time_in_freq_t *time_in_freq_beg, *time_in_freq_end;
} infos_t;

// cstates functions
void init_cstates(infos_t *infos);
void start_cstates(infos_t *infos);
void finish_cstates(infos_t *infos);
void print_cstates(infos_t *infos);

// freqs functions
void init_freqs(infos_t *infos);
void start_freqs(infos_t *infos);
void finish_freqs(infos_t *infos);
void print_freqs(infos_t *infos);


#endif
