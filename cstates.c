#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>


#include "power.h"
/*
 * desc : Small description about the idle state (string)
 * latency : Latency to exit out of this idle state (in microseconds)
 * name : Name of the idle state (string)
 * power : Power consumed while in this idle state (in milliwatts)
 * time : Total time spent in this idle state (in microseconds)
 * usage : Number of times this state was entered (count)
 */

static void get_cstate_stats(unsigned int cpu, int nb_states, cstate_stat_t *tis) {
  char path[SYSFS_PATH_MAX];
  FILE *file;
  int cstate_id;
  for (cstate_id = 0; cstate_id < nb_states; cstate_id++) {
    snprintf(path, sizeof(path), PATH_TO_CPU "cpu%u/cpuidle/state%d/name", cpu, cstate_id);
    file = fopen(path, "r");
    if (file != NULL) {
      fscanf(file, "%s", tis[cstate_id].name);
      fclose(file);
    }
    else
    perror("cstate_stat: couldn't open file name");
    // ==
    snprintf(path, sizeof(path), PATH_TO_CPU "cpu%u/cpuidle/state%d/time", cpu, cstate_id);
    file = fopen(path, "r");
    if (file != NULL) {
      fscanf(file, "%lld", &tis[cstate_id].time);
      fclose(file);
    }
    else
    perror("cstate_stat: couldn't open file time");
    // ==
    snprintf(path, sizeof(path), PATH_TO_CPU "cpu%u/cpuidle/state%d/usage", cpu, cstate_id);
    file = fopen(path, "r");
    if (file != NULL) {
      fscanf(file, "%ld", &tis[cstate_id].usage);
      fclose(file);
    }
    else
    perror("cstate_stat: couldn't open file usage");
//    printf("%s %lld %ld\n", tis[cstate_id].name, tis[cstate_id].time, tis[cstate_id].usage);
  }
}

static int get_nb_states() {
  char path[SYSFS_PATH_MAX] = PATH_TO_CPU "cpu0/cpuidle/";
  int file_count = 0;
  DIR * dirp;
  struct dirent * entry;

  dirp = opendir(path); 
  if (dirp != NULL) {
    while ((entry = readdir(dirp)) != NULL) {
      if (entry->d_type == DT_DIR) { 
        file_count++;
      }
    }
    closedir(dirp);
  }
  else
    perror ("cstate_stat: couldn't open the directory");
  // -2 because . and .. files
  return file_count - 2;
}

void init_cstates(infos_t *infos) {
  infos->nb_states = get_nb_states();
  infos->cstates_total = malloc( infos->nb_cpus * sizeof(unsigned long long));
  memset(infos->cstates_total, 0, infos->nb_cpus * sizeof(unsigned long long) );
  infos->cstate_stat_beg = malloc( infos->nb_cpus * sizeof(cstate_stat_t*) );
  infos->cstate_stat_end = malloc( infos->nb_cpus * sizeof(cstate_stat_t*) );
  infos->cstate_trace_beg = malloc( infos->nb_cpus * sizeof(cstate_stat_t*) );
  infos->cstate_trace_end = malloc( infos->nb_cpus * sizeof(cstate_stat_t*) );
  int cpu_id ;
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    infos->cstate_stat_beg[cpu_id] = malloc( infos->nb_states * sizeof(cstate_stat_t) );
    infos->cstate_stat_end[cpu_id] = malloc( infos->nb_states * sizeof(cstate_stat_t) );
    infos->cstate_trace_beg[cpu_id] = malloc( infos->nb_states * sizeof(cstate_stat_t) );
    infos->cstate_trace_end[cpu_id] = malloc( infos->nb_states * sizeof(cstate_stat_t) );
  }
}

void refresh_cstates_trace(infos_t *infos) {
  cstate_stat_t **tmp = infos->cstate_trace_beg;
  infos->cstate_trace_beg = infos->cstate_trace_end;
  infos->cstate_trace_end = tmp; 
  int cpu_id;
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    get_cstate_stats(cpu_id, infos->nb_states, infos->cstate_trace_end[cpu_id]);
  }
}

void start_cstates(infos_t *infos) {
  int cpu_id;
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    get_cstate_stats(cpu_id, infos->nb_states, infos->cstate_stat_beg[cpu_id]);
  }
}

void finish_cstates(infos_t *infos) {
  unsigned long long time;
  int cpu_id;
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    get_cstate_stats(cpu_id, infos->nb_states, infos->cstate_stat_end[cpu_id]);
  }

  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    int state;
    for (state = 0; state < infos->nb_states; state++) {
      infos->cstate_stat_beg[cpu_id][state].time = infos->cstate_stat_end[cpu_id][state].time - infos->cstate_stat_beg[cpu_id][state].time;
      infos->cstates_total[cpu_id] += infos->cstate_stat_beg[cpu_id][state].time;
      infos->cstate_stat_beg[cpu_id][state].usage = infos->cstate_stat_end[cpu_id][state].usage - infos->cstate_stat_beg[cpu_id][state].usage;
    }
  }
}

void print_cstates(infos_t *infos) {
  printf("** C-States: **\n");
  int cpu_id;
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    printf("   CPU %d: total %lld ms - %.2f%% in idle\n", cpu_id, CSTATE_IN_MS(infos->cstates_total[cpu_id]), (100.0 * CSTATE_IN_MS(infos->cstates_total[cpu_id]) / PSTATE_IN_MS(infos->freqs_total[cpu_id])));
    int pos;
    for (pos = 0; pos < infos->nb_states; pos++) {
      //printf("     %5s: %.2f%% - %lld ms - %ld\n", infos->cstate_stat_beg[cpu_id*infos->nb_states+pos].name, (100.0 * infos->cstate_stat_beg[cpu_id*infos->nb_states+pos].time) / infos->cstates_total[cpu_id], infos->cstate_stat_beg[cpu_id*infos->nb_states+pos].time / CSTATE_IN_MS, infos->cstate_stat_beg[cpu_id*infos->nb_states+pos].usage);
      printf("     %5s: %.2f%% - %lld ms\n", infos->cstate_stat_beg[cpu_id][pos].name, (100.0 * infos->cstate_stat_beg[cpu_id][pos].time) / infos->cstates_total[cpu_id], CSTATE_IN_MS(infos->cstate_stat_beg[cpu_id][pos].time) );
    }
  }
}

