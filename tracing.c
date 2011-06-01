#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define TDIFF(te, ts) (te.tv_sec - ts.tv_sec + (te.tv_usec - ts.tv_usec) * 1e-6)

#include "power.h"

double BASE_CONS = 480;

double WATT_FREQ[10] = {  7,
                          7,
                          7,
                          7,
                          7,
                          7,
                          7,
                          7,
                          7,
                          4
};

static volatile int tracing;

pthread_t tid;
FILE *trace;
struct timeval start_time;

// ugly, temp variable, but don't want to malloc each time
double *time_in_freq;

infos_t *infos;


int get_ipmi_power() {
  int power_value;
  FILE *f_cmd = popen("sudo ipmi-oem Dell get-instantaneous-power-consumption-data 0 | head -n 1 | cut -f4 -d' '", "r");
  fscanf(f_cmd, "%d", &power_value);
  pclose(f_cmd);
  return power_value;
}

static void * energy_tracing(void *arg) {
  int cpu_id, state, ipmi_watt;
  struct timeval curr_time;
  double second_period = 0;
  refresh_cstates_trace(infos);
  refresh_freqs_trace(infos);
  ipmi_watt = get_ipmi_power();
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
    fprintf(trace, "%f %d ", TDIFF(curr_time, start_time), ipmi_watt);
    second_period += TDIFF(curr_time, start_time);
    if (second_period >= 1) {
      ipmi_watt = get_ipmi_power();
    }
    for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
      float total_watt = 0;
      long long int total_time_cstate = 0;
      long long int total_time = 0;
      for (state = 0; state < infos->nb_states; state++) {
        total_time_cstate += infos->cstate_trace_end[cpu_id][state].time - infos->cstate_trace_beg[cpu_id][state].time;
      }
      total_time_cstate = CSTATE_IN_MS(total_time_cstate);

      for (state = 0; state < infos->nb_freqs; state++) {
        time_in_freq[state] = PSTATE_IN_MS(infos->time_in_freq_trace_end[cpu_id][state].time - infos->time_in_freq_trace_beg[cpu_id][state].time);
        total_time += time_in_freq[state];
      }
      double tmp_cstate = total_time_cstate;
      state = infos->nb_freqs - 1;
/*    DEBUG
      int titi;
      printf("cpuid %d\n", cpu_id);
      printf("total idle %d\n", total_time_cstate);
      if (total_time_cstate > 100) {
        printf("states\n");
        for (titi = 0; titi < infos->nb_states; titi++) {
          printf(">>> %lld - %lld = %lld\n", (infos->cstate_trace_end[cpu_id][titi]).time, infos->cstate_trace_beg[cpu_id][titi].time, (infos->cstate_trace_end[cpu_id][titi]).time - infos->cstate_trace_beg[cpu_id][titi].time);
        }
        printf("freq\n");
        for (titi = 0; titi < infos->nb_freqs; titi++) {
          printf(">>> %lld - %lld = %lld\n", (infos->time_in_freq_trace_end[cpu_id][titi]).time, infos->time_in_freq_trace_beg[cpu_id][titi].time, (infos->time_in_freq_trace_end[cpu_id][titi]).time - infos->time_in_freq_trace_beg[cpu_id][titi].time);
        }
      }
      else {
        printf("states\n");
        for (titi = 0; titi < infos->nb_states; titi++) {
          printf("%lld - %lld = %lld\n", (infos->cstate_trace_end[cpu_id][titi]).time, infos->cstate_trace_beg[cpu_id][titi].time, (infos->cstate_trace_end[cpu_id][titi]).time - infos->cstate_trace_beg[cpu_id][titi].time);
        }
        printf("freq\n");
        for (titi = 0; titi < infos->nb_freqs; titi++) {
          printf("%lld - %lld = %lld\n", (infos->time_in_freq_trace_end[cpu_id][titi]).time, infos->time_in_freq_trace_beg[cpu_id][titi].time, (infos->time_in_freq_trace_end[cpu_id][titi]).time - infos->time_in_freq_trace_beg[cpu_id][titi].time);
        }
      }
      */

      while ( tmp_cstate > 0 ) {
        if (state < 0) {
//          fprintf(stderr, "Problem while subing cstate time from freqs time, %s %d\ntotal_time_cstate %f - tmp_cstate %f\n", __FILE__, __LINE__ , total_time_cstate, tmp_cstate );
          break;
        }
        double tmp_time = time_in_freq[state];
        time_in_freq[state] = time_in_freq[state] - tmp_cstate;
        if (time_in_freq[state] < 0)
          time_in_freq[state] = 0;
        tmp_cstate = tmp_cstate - tmp_time;
        if (tmp_cstate < 0)
          tmp_cstate = 0;
        state--;
      }
      
      //total_watt = total_time_cstate / total_time * WATT_IDLE;

      for (state = 0; state < infos->nb_freqs; state++) {
        total_watt += time_in_freq[state] / total_time * WATT_FREQ[state];
//        fprintf(trace, "(%f/%f*%f) ", time_in_freq[state], total_time, WATT_FREQ[state]);
      }

      fprintf(trace, "%f ", total_watt);
    }
    fprintf(trace, "\n");

    usleep(100000);
  }
}

int start_tracing(infos_t *_infos) {
  time_in_freq = malloc(_infos->nb_freqs * sizeof(double));
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
