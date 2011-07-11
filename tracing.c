#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pwd.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>

#include <errno.h>

#include <papi.h>


#include "power.h"


static volatile int tracing_energy, tracing_process;

pthread_t tid[2];
FILE *trace;
struct timeval start_time;

infos_t *infos;

pid_t parent_pid;

struct timeval glob_tod_time;
double glob_curr_time;

typedef struct {
  pid_t watched_pid;
  char proc_path[128];
  FILE *proc_file;
  char output_path[128];
  FILE *output_file;
  int is_thread;
  int event_set;
  
} watched_param_t;

int nb_proc_watched=0;
watched_param_t watched_processes[MAX_PROC_WATCHED];

int get_ipmi_power() {
  int power_value;
  FILE *f_cmd = popen("sudo ipmi-oem Dell get-instantaneous-power-consumption-data 0 | head -n 1 | cut -f4 -d' '", "r");
  fscanf(f_cmd, "%d", &power_value);
  pclose(f_cmd);
  return power_value;
}

static FILE* init_energy_stat_file() {
  FILE *freq_stat_file = fopen(FREQ_STAT_PATH, "w");

  if (freq_stat_file == NULL) {
//    fprintf(stderr, "Cannot open /tmp/trace/ipmi_freq\n");
    perror("Cannot open "FREQ_STAT_PATH"\n");
  }
  fprintf(freq_stat_file, "# time nb_cycle_freq1,nb_cycle_freq2 nb_cycle_freq1,nb_cycle_freq2 ...\n");
  return freq_stat_file;
}

static void refresh_energy_stat(infos_t *infos, int *ipmi_watt) {
  refresh_cstates_trace(infos);
  refresh_freqs_trace(infos);
  *ipmi_watt = get_ipmi_power();
}

static void get_energy_stat(infos_t *infos, int ipmi_watt, double curr_time, FILE *freq_stat_file) {
  int cpu_id, state;
  unsigned long long tmp;
  fprintf(freq_stat_file, "%f %d ", curr_time, ipmi_watt);
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    for (state = 0; state < infos->nb_freqs; state++) {
      tmp = infos->time_in_freq_trace_end[cpu_id][state].time - infos->time_in_freq_trace_beg[cpu_id][state].time;
      fprintf(freq_stat_file, "%lld", tmp);
      if (state != infos->nb_freqs -1)
      fprintf(freq_stat_file, ",");
    }
    fprintf(freq_stat_file, " ");

  }

  fprintf(freq_stat_file, "\n");
  if ( fflush(freq_stat_file) != 0 )
  perror("ipmi trace flush error");

}

static void refresh_children_list() {
  char parent_proc_dir[128], tmp_proc_path[128];
  sprintf(parent_proc_dir, "/proc/%d/task", parent_pid);

  int ppid, proc_id;
  int j;
  struct dirent *ent;          /* dirent handle */
  DIR *dir;
  dir = opendir("/proc");

  // List all processes in /proc, and search for matching ppid
  while(( ent = readdir(dir) )){
    int curr_pid = atoi(ent->d_name);
    if(*ent->d_name<'0' || *ent->d_name>'9') continue;
    snprintf(tmp_proc_path, 32, "/proc/%d/stat", curr_pid);
    if(!stat2proc(curr_pid, &ppid, &proc_id, tmp_proc_path, 0)) continue;
    //      if(want_one_command){
    //        if(strcmp(want_one_command,P_cmd)) continue;
    //      }else{
    //        if(!select_notty && P_tty_num==NO_TTY_VALUE) continue;
    //        if(!select_all && P_euid!=ouruid) continue;
    //      }

    if (parent_pid == ppid) {
      for (j=0; j < nb_proc_watched; j++) {
        if (curr_pid == watched_processes[j].watched_pid)
        break;
      }
      // if not found, add a new watched process
      if (j == nb_proc_watched) {
        sprintf(watched_processes[j].proc_path, "/proc/%d/stat", curr_pid);
        watched_processes[j].watched_pid = curr_pid;
        watched_processes[j].is_thread = 0;
        sprintf(watched_processes[j].output_path, "/tmp/trace/%d.proc", curr_pid);
        watched_processes[j].output_file = fopen(watched_processes[j].output_path, "w");
        fprintf(watched_processes[j].output_file, "# time unh_core_cycle inst_retired L1_misses L2_misses LLC_misses core_used\n");
        watched_processes[j].event_set = PAPI_NULL;
        papi_init_eventset(curr_pid, &watched_processes[j].event_set);
        nb_proc_watched++;
      }
    }
  }
  closedir(dir);

  // List all threads in parent process
  dir = opendir(parent_proc_dir);
  while(( ent = readdir(dir) )){
    int curr_tid = atoi(ent->d_name);
    if(*ent->d_name<'0' || *ent->d_name>'9') continue;

    for (j=0; j < nb_proc_watched; j++) {
      if (curr_tid == watched_processes[j].watched_pid)
      break;
    }
    // if not found, add a new watched thread
    if (j == nb_proc_watched) {
      sprintf(watched_processes[j].proc_path, "/proc/%d/task/%d/stat", parent_pid, curr_tid);
      watched_processes[j].watched_pid = curr_tid;
      watched_processes[j].is_thread = 1;
      sprintf(watched_processes[j].output_path, "/tmp/trace/%d.proc", curr_tid);
      watched_processes[j].output_file = fopen(watched_processes[j].output_path, "w");
      fprintf(watched_processes[j].output_file, "# time unh_core_cycle inst_retired L1_misses L2_misses LLC_misses core_used\n");
      watched_processes[j].event_set = PAPI_NULL;
      papi_init_eventset(curr_tid, &watched_processes[j].event_set);
      nb_proc_watched++;
    }
  }
}

static void * main_process_watching(void *arg) {


  long long values[NB_EVENTS];
  int ppid, proc_id;
  int ipmi_watt;
  int i, err;

  int sleep_time = 1000000;
  double tracing_time = 0;
  struct timeval tracing_tod_beg;
  struct timeval tracing_tod_end;

  gettimeofday(&tracing_tod_beg, NULL);

  papi_thread_init();
  
  FILE* freq_stat_file = init_energy_stat_file();
  refresh_energy_stat(infos, &ipmi_watt);
  
  while(tracing_energy) {

    refresh_children_list();

// Start the counters
    for (i = 0; i < nb_proc_watched; i++) {
      if (PAPI_start(watched_processes[i].event_set) != PAPI_OK) {
        fprintf(stderr, "PAPI start error!\n");
      }
    }

// Sleep a while
    
    gettimeofday(&tracing_tod_end, NULL);
    tracing_time = TDIFF(tracing_tod_end, tracing_tod_beg);
    
    usleep(sleep_time - tracing_time * 1000000);

    gettimeofday(&tracing_tod_beg, NULL);
    gettimeofday(&glob_tod_time, NULL);
    glob_curr_time = TDIFF(glob_tod_time, start_time);

    refresh_energy_stat(infos, &ipmi_watt);
    get_energy_stat(infos, ipmi_watt, glob_curr_time, freq_stat_file);

// Stop all the counters and get values
    for (i = 0; i < nb_proc_watched; i++) {
      if (PAPI_stop(watched_processes[i].event_set, values) != PAPI_OK) {
        fprintf(stderr, "PAPI stop error!\n");
      }
      proc_id = -1;
      stat2proc(watched_processes[i].watched_pid, &ppid, &proc_id, watched_processes[i].proc_path);
      fprintf(watched_processes[i].output_file, "%f %lld %lld %d %d %d %d\n", glob_curr_time, values[0], values[1], values[2], values[3], values[4], proc_id);
    }
  }

  for (i = 0; i < nb_proc_watched; i++) {
    fclose(watched_processes[i].output_file);
  }
  if ( fclose(freq_stat_file) != 0 )
    perror("ipmi trace close error");
}

int start_tracing(infos_t *_infos, pid_t _parent_pid) {
  papi_global_init();
  init_trace_dir();
  parent_pid = _parent_pid;
  gettimeofday(&start_time, NULL);  
  infos = _infos;
  tracing_energy = 1;
  tracing_process = 1;
  pthread_create(&tid[1], NULL, main_process_watching, NULL);
}

int stop_tracing() {
  tracing_energy = 0;
  gettimeofday(&glob_tod_time, NULL);
  glob_curr_time = TDIFF(glob_tod_time, start_time);
  tracing_process = 0;
  pthread_join(tid[1], NULL);
}
