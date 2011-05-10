#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#define TDIFF(te, ts) (te.tv_sec - ts.tv_sec + (te.tv_usec - ts.tv_usec) * 1e-6)

#include "power.h"

static volatile int tracing;

pthread_t tid;
FILE *trace;
struct timeval start_time;


infos_t *infos;

static void * energy_tracing(void *arg) {
  int cpu_id, state;
  struct timeval curr_time;
  refresh_cstates_trace(infos);
  refresh_freqs_trace(infos);
  while(tracing) {
    refresh_cstates_trace(infos);
    refresh_freqs_trace(infos);
//    unsigned long long tmp;
//
//    for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
//    printf("cpu %d\n", cpu_id);
//    printf("cstates\n");
//    for (state = 0; state < infos->nb_states; state++) {
//      tmp = infos->cstate_trace_end[cpu_id*infos->nb_states+state].time - infos->cstate_trace_beg[cpu_id*infos->nb_states+state].time;
//      printf("%lld - ", tmp);
//    }
//    printf("\nfreqs\n");
//    for (state = 0; state < infos->nb_freqs; state++) {
//      tmp = infos->time_in_freq_trace_end[cpu_id][state].time - infos->time_in_freq_trace_beg[cpu_id][state].time;
//      printf("%lld - ", tmp);        
//    }
//    printf("\n");
//    }

    gettimeofday(&curr_time, NULL);
    fprintf(trace, "%f ", TDIFF(curr_time, start_time));
    for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
      double total_joules = 0;
      double total_time_cstate = 0;
      for (state = 0; state < infos->nb_states; state++) {
        total_time_cstate += infos->cstate_trace_end[cpu_id*infos->nb_states+state].time - infos->cstate_trace_beg[cpu_id*infos->nb_states+state].time;
      }
      // time in idle
      total_joules += CSTATE_IN_MS(total_time_cstate) * ST_10_IDLE / 1000;
      // time at low freq but not idle
      double rest = PSTATE_IN_MS(infos->time_in_freq_trace_end[cpu_id][2].time - infos->time_in_freq_trace_beg[cpu_id][2].time) - CSTATE_IN_MS(total_time_cstate);
      if (rest > 0) {
        total_joules += rest * ST_10_RUN / 1000;
      }
      total_joules += PSTATE_IN_MS(infos->time_in_freq_trace_end[cpu_id][1].time - infos->time_in_freq_trace_beg[cpu_id][1].time) * ST_13_RUN / 1000;
      total_joules += PSTATE_IN_MS(infos->time_in_freq_trace_end[cpu_id][0].time - infos->time_in_freq_trace_beg[cpu_id][0].time) * ST_18_RUN / 1000;
      fprintf(trace, "%f ", total_joules);
    }
    fprintf(trace, "\n");

    usleep(100000);
  }
}

int start_tracing(infos_t *_infos) {
  gettimeofday(&start_time, NULL);  
  infos = _infos;
  tracing = 1;
  trace = fopen("trace_perf", "w");
  pthread_create(&tid, NULL, energy_tracing, NULL);
}

int stop_tracing() {
  tracing = 0;
  pthread_join(tid, NULL);
  fclose(trace);
}
