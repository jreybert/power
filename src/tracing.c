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

#define NB_WATCHING_FUNC 3

extern char **events_name;
extern float *events_weight;

static volatile int tracing_hw;
static volatile int tracing_process;

pthread_t tid[NB_WATCHING_FUNC];
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


static FILE* init_hardware_file() {
  FILE *hw_file = fopen(HW_FILE_PATH, "w");

  if (hw_file == NULL) {
//    fprintf(stderr, "Cannot open /tmp/trace/ipmi_freq\n");
    perror("Cannot open "HW_FILE_PATH"\n");
  }
  fprintf(hw_file, "# time temp_core_1 temp_core_2 ... \n");
  return hw_file;
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

static void refresh_energy_stat(infos_t *infos, int *ipmi_power) {
  refresh_cstates_trace(infos);
  refresh_freqs_trace(infos);
  *ipmi_power = get_ipmi_power();
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
//        fprintf(watched_processes[j].output_file, "# time unh_core_cycle inst_retired L1_misses L2_misses LLC_misses core_used\n");
        fprintf(watched_processes[j].output_file, "# time %s %s %s %s %s core_used\n", events_name[0], events_name[1], events_name[2], events_name[3], events_name[4]); 
        fprintf(watched_processes[j].output_file, "# weight %f %f %f %f %f\n", events_weight[0], events_weight[1], events_weight[2], events_weight[3], events_weight[4]); 
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
      //fprintf(watched_processes[j].output_file, "# time unh_core_cycle inst_retired L1_misses L2_misses LLC_misses core_used\n");
      fprintf(watched_processes[j].output_file, "# time %s %s %s %s %s core_used\n", events_name[0], events_name[1], events_name[2], events_name[3], events_name[4]); 
      fprintf(watched_processes[j].output_file, "# weight %f %f %f %f %f\n", events_weight[0], events_weight[1], events_weight[2], events_weight[3], events_weight[4]); 
      watched_processes[j].event_set = PAPI_NULL;
      papi_init_eventset(curr_tid, &watched_processes[j].event_set);
      nb_proc_watched++;
    }
  }
}

static void * watch_hardware(void * arg) {
  
  int ipmi_watt;
  int sleep_time = 1000000;
  double local_curr_time, tracing_time;
  
  struct timeval tracing_tod_beg;
  struct timeval tracing_tod_end;
  struct timeval local_tod_time;

  gettimeofday(&tracing_tod_beg, NULL);


  FILE* hw_file = init_hardware_file();

  ipmi_watt = get_ipmi_power();

  gettimeofday(&local_tod_time, NULL);
  local_curr_time = TDIFF(local_tod_time, start_time);
  print_temp_values(hw_file, ipmi_watt, local_curr_time);    
  
  gettimeofday(&tracing_tod_end, NULL);

  while (tracing_hw) {
    gettimeofday(&tracing_tod_end, NULL);
    tracing_time = TDIFF(tracing_tod_end, tracing_tod_beg);
    
    usleep(sleep_time - tracing_time * 1000000);

    gettimeofday(&tracing_tod_beg, NULL);
    gettimeofday(&local_tod_time, NULL);
    local_curr_time = TDIFF(local_tod_time, start_time);

    ipmi_watt = get_ipmi_power();
    print_temp_values(hw_file, ipmi_watt, local_curr_time);    

  }
  if ( fclose(hw_file) != 0 )
    perror("temp trace close error");

}

static void * check_new_children(void *arg) {
  while(tracing_process) {
    refresh_children_list();
    usleep(1000000);
  }
}

static void * main_process_watching(void *arg) {
  long long values[NB_EVENTS];
  int ppid, proc_id;
  int i, err;
  int ipmi_watt;

  int sleep_time = 1000000;
  double tracing_time = 0;
  struct timeval tracing_tod_beg;
  struct timeval tracing_tod_end;

  papi_thread_init();

  gettimeofday(&tracing_tod_beg, NULL);

  FILE* freq_stat_file = init_energy_stat_file();
  refresh_energy_stat(infos, &ipmi_watt);

  refresh_children_list();

  while(tracing_process) {

    int local_nb_proc_watched = nb_proc_watched;

// Start the counters
    for (i = 0; i < local_nb_proc_watched; i++) {
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

    //print_temp_values(hw_file, glob_curr_time);

// Stop all the counters and get values
    for (i = 0; i < local_nb_proc_watched; i++) {
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

int init_global_tracing(infos_t *_infos) {
  infos = _infos;
  papi_global_init();
  init_trace_dir();
  my_init_sensors();
  gettimeofday(&start_time, NULL);  
}


int stop_global_tracing() {
  pthread_join(tid[1], NULL);
  pthread_join(tid[2], NULL);
  pthread_join(tid[0], NULL);
}

int init_hw_tracing() {
  tracing_hw = 1;
  pthread_create(&tid[0], NULL, watch_hardware, NULL);
}

int stop_hw_tracing() {
  tracing_hw = 0;
}

int start_proc_tracing(pid_t _parent_pid) {
  parent_pid = _parent_pid;
  tracing_process = 1;
  pthread_create(&tid[1], NULL, main_process_watching, NULL);
  pthread_create(&tid[2], NULL, check_new_children, NULL);
}

int stop_proc_tracing() {
  tracing_process = 0;
  gettimeofday(&glob_tod_time, NULL);
  //glob_curr_time = TDIFF(glob_tod_time, start_time);
}
