#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#include <papi.h>

#define TDIFF(te, ts) (te.tv_sec - ts.tv_sec + (te.tv_usec - ts.tv_usec) * 1e-6)

#include "power.h"

double BASE_CONS = 480;

double WATT_FREQ[10] = {  7, 7, 7, 7, 7, 7, 7, 7, 7, 4 };

static volatile int tracing_energy, tracing_process;

pthread_t tid[2];
FILE *trace;
struct timeval start_time;

// ugly, temp variable, but don't want to malloc each time
double *time_in_freq;

infos_t *infos;

pid_t parent_pid;

#define MAX_PROC_WATCHED 128
#define TRACE_DIR "/tmp/trace"

int get_ipmi_power() {
  int power_value;
  FILE *f_cmd = popen("sudo ipmi-oem Dell get-instantaneous-power-consumption-data 0 | head -n 1 | cut -f4 -d' '", "r");
  fscanf(f_cmd, "%d", &power_value);
  pclose(f_cmd);
  return power_value;
}

#ifdef __linux__
/* return 1 if it works, or 0 for failure */
static int stat2proc(int pid, int *ppid, int *processor_id, char* proc_file) {
    char buf[800], c_null[4]; /* about 40 fields, 64-bit decimal is about 20 chars */
    int num;
    int fd;
    unsigned long null_int;
    char* tmp;
    //struct stat sb; /* stat() used to get EUID */
    //snprintf(buf, 32, "/proc/%d/stat", pid);
    if ( (fd = open(proc_file, O_RDONLY, 0) ) == -1 ) return 0;
    num = read(fd, buf, sizeof buf - 1);
    //fstat(fd, &sb);
    //P_euid = sb.st_uid;
    close(fd);
    if(num<80) return 0;
    buf[num] = '\0';
    tmp = strrchr(buf, ')');      /* split into "PID (cmd" and "<rest>" */
    *tmp = '\0';                  /* replace trailing ')' with NUL */
    /* parse these two strings separately, skipping the leading "(". */
//    memset(P_cmd, 0, sizeof P_cmd);          /* clear */
//    sscanf(buf, "%d (%15c", &P_pid, P_cmd);  /* comm[16] in kernel */
    num = sscanf(tmp + 2,                    /* skip space after ')' too */
       "%c "
       "%d %d %d %d %d "
       "%lu %lu %lu %lu %lu %lu %lu "
       "%ld %ld %ld %ld %ld %ld "
       "%lu %lu "
       "%ld "
       "%lu %lu %lu %lu %lu %lu "
       "%u %u %u %u " /* no use for RT signals */
       "%lu %lu %lu "
       "%d %d",
       c_null,
       ppid, &null_int, &null_int, &null_int, &null_int,
       &null_int, &null_int, &null_int, &null_int, &null_int, &null_int, &null_int,
       &null_int, &null_int, &null_int, &null_int, &null_int, &null_int,
       &null_int, &null_int,
       &null_int,
       &null_int, &null_int, &null_int, &null_int, &null_int, &null_int,
       &null_int, &null_int, &null_int, &null_int,
       &null_int, &null_int, &null_int, //&P_cnswap
       &null_int, processor_id
    );

    if(num < 30) return 0;
    //if(P_pid != pid) return 0;
    return 1;
}
#endif

pthread_cond_t      cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;
struct timeval glob_tod_time;
double glob_curr_time;

typedef struct {
  pid_t watched_pid;
  char proc_path[128];
  int is_thread;
} watched_param_t;
  
static void * forked_process_watching(void *arg) {
//  pid_t watch_pid = *((pid_t*)arg);
  watched_param_t *params = (watched_param_t*)arg;
  
  int ppid, proc_id;
  double curr_time = 0;
  FILE *fork_file;
  char filename[128];
  sprintf(filename, "/tmp/trace/%d.proc", params->watched_pid);
  fork_file = fopen(filename, "w");

  int EventSet = PAPI_NULL;
  long long values[NB_EVENTS];

  init_papi(params->watched_pid, &EventSet);


  fprintf(fork_file, "# time unh_core_cycle inst_retired L1_misses L2_misses LLC_misses core_used\n");

  while(tracing_energy) {
    /* Start counting */
    if (PAPI_start(EventSet) != PAPI_OK) {
      fprintf(stderr, "PAPI start error!\n");
      exit(1);
    }

    int           rc;
    rc = pthread_mutex_lock(&mutex);
    do {
      rc = pthread_cond_wait(&cond, &mutex);
      curr_time = glob_curr_time;
    } while ( (curr_time != glob_curr_time) && (tracing_energy != 0) );
    rc = pthread_mutex_unlock(&mutex);
//    usleep(1000000);

    if (PAPI_stop(EventSet, values) != PAPI_OK) {
      fprintf(stderr, "PAPI stop error!\n");
      exit(1);
    }
    
    //print_values(events_name, values, n_events);
    stat2proc(params->watched_pid, &ppid, &proc_id, params->proc_path);
    fprintf(fork_file, "%f %lld %lld %d %d %d %d\n", curr_time, values[0], values[1], values[2], values[3], values[4], proc_id);
    //fflush(fork_file);
  }
  fclose(fork_file);
}


static void init_trace_dir() {
  struct dirent *ent;          /* dirent handle */
  int err;
  DIR *dir;
  dir = opendir(TRACE_DIR);

  if (dir == NULL) {
    mkdir(TRACE_DIR, S_IRWXU);
  }
  else {

    while(( ent = readdir(dir) )){
      char file_to_del[NAME_MAX];
      snprintf(file_to_del, sizeof(file_to_del), TRACE_DIR"/%s", ent->d_name);
      //printf("defl %s\n", file_to_del);
      if ( (err = unlink(file_to_del) ) != 0) {
        printf("eer %d %s\n", errno, strerror(errno));
      }
    }
  }

}

static void * main_process_watching(void *arg) {

  int nb_proc_watched=0;
  watched_param_t watched_processes[MAX_PROC_WATCHED];
  pthread_t tids[MAX_PROC_WATCHED];

  char parent_proc_dir[128], tmp_proc_path[128];
  sprintf(parent_proc_dir, "/proc/%d/task", parent_pid);


  int err;
  /* Initialize the library */
  if ( (err = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error! %d\n", err);
    exit(1);
  }


  int sleep_time = 1000000;
  int i;
  for (i = 0; i < 10; i++) {
    
  }
  while(tracing_energy) {

    
    struct dirent *ent;          /* dirent handle */
    DIR *dir;
    dir = opendir("/proc");
    while(( ent = readdir(dir) )){
      int ppid, proc_id;
      int curr_pid = atoi(ent->d_name);
      if(*ent->d_name<'0' || *ent->d_name>'9') continue;
      snprintf(tmp_proc_path, 32, "/proc/%d/stat", curr_pid);
      if(!stat2proc(curr_pid, &ppid, &proc_id, tmp_proc_path)) continue;
//      if(want_one_command){
//        if(strcmp(want_one_command,P_cmd)) continue;
//      }else{
//        if(!select_notty && P_tty_num==NO_TTY_VALUE) continue;
//        if(!select_all && P_euid!=ouruid) continue;
//      }
      
      if (parent_pid == ppid) {
        int j;
        for (j=0; j < nb_proc_watched; j++) {
          if (curr_pid == watched_processes[j].watched_pid)
            break;
        }
        // if not found
        if (j == nb_proc_watched) {
          sprintf(watched_processes[j].proc_path, "/proc/%d", curr_pid);
          watched_processes[j].watched_pid = curr_pid;
          watched_processes[j].is_thread = 0;
          pthread_create(&tids[j], NULL, forked_process_watching, &watched_processes[j]);
          nb_proc_watched++;
          
          // launch thread
        }
      }
    }
    closedir(dir);
    

    dir = opendir(parent_proc_dir);
    while(( ent = readdir(dir) )){
      int curr_tid = atoi(ent->d_name);
      if(*ent->d_name<'0' || *ent->d_name>'9') continue;

      int j;
      for (j=0; j < nb_proc_watched; j++) {
        if (curr_tid == watched_processes[j].watched_pid)
        break;
      }
      // if not found
      if (j == nb_proc_watched) {
        sprintf(watched_processes[j].proc_path, "/proc/%d/task/%d", parent_pid, curr_tid);
        watched_processes[j].watched_pid = curr_tid;
        watched_processes[j].is_thread = 1;
        pthread_create(&tids[j], NULL, forked_process_watching, &watched_processes[j]);
        nb_proc_watched++;

        // launch thread
      }



    }
    

    // wake waiting thread, to give them the hour
    pthread_mutex_lock(&mutex);
    gettimeofday(&glob_tod_time, NULL);
    glob_curr_time = TDIFF(glob_tod_time, start_time);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    //printf("\n");
    usleep(sleep_time);
  }
  int j;
  for (j=0; j < nb_proc_watched; j++) {
    pthread_join(tids[j], NULL);
  }
}
static void * energy_tracing(void *arg) {
  
  int cpu_id, state, ipmi_watt;
  double curr_time = 0;

  FILE *ipmi_freq_file = fopen("/tmp/trace/ipmi_freq", "w");

  if (ipmi_freq_file == NULL) {
    fprintf(stderr, "Cannot open /tmp/trace/ipmi_freq\n");
  }

  fprintf(ipmi_freq_file, "# time nb_cycle_freq1,nb_cycle_freq2 nb_cycle_freq1,nb_cycle_freq2 ...\n");


  refresh_cstates_trace(infos);
  refresh_freqs_trace(infos);
  ipmi_watt = get_ipmi_power();
  while(tracing_energy) {

    int rc;
    rc = pthread_mutex_lock(&mutex);
    do {
      rc = pthread_cond_wait(&cond, &mutex);
      curr_time = glob_curr_time;
    } while ( (curr_time != glob_curr_time) && (tracing_energy != 0) );
    rc = pthread_mutex_unlock(&mutex);

    ipmi_watt = get_ipmi_power();
    fprintf(ipmi_freq_file, "%f %d ", curr_time, ipmi_watt);
    refresh_cstates_trace(infos);
    refresh_freqs_trace(infos);
    unsigned long long tmp;

    for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
      for (state = 0; state < infos->nb_freqs; state++) {
        tmp = infos->time_in_freq_trace_end[cpu_id][state].time - infos->time_in_freq_trace_beg[cpu_id][state].time;
        fprintf(ipmi_freq_file, "%lld", tmp);
        if (state != infos->nb_freqs -1)
          fprintf(ipmi_freq_file, ",");
      }
      fprintf(ipmi_freq_file, " ");

    }

    fprintf(ipmi_freq_file, "\n");
    if ( fflush(ipmi_freq_file) != 0 )
      fprintf(stderr, "ipmi trace flush error");


//    usleep(100000);
  }
  if ( fclose(ipmi_freq_file) != 0 )
    fprintf(stderr, "ipmi trace close error");
}

int start_tracing(infos_t *_infos, pid_t _parent_pid) {
  init_trace_dir();
  time_in_freq = malloc(_infos->nb_freqs * sizeof(double));
  parent_pid = _parent_pid;
  gettimeofday(&start_time, NULL);  
  infos = _infos;
  tracing_energy = 1;
  tracing_process = 1;
  trace = fopen("trace_perf", "w");
  pthread_create(&tid[0], NULL, energy_tracing, NULL);
  pthread_create(&tid[1], NULL, main_process_watching, NULL);
}

int stop_tracing() {
  tracing_energy = 0;
  pthread_mutex_lock(&mutex);
  gettimeofday(&glob_tod_time, NULL);
  glob_curr_time = TDIFF(glob_tod_time, start_time);
  pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&mutex);
  tracing_process = 0;
  pthread_join(tid[0], NULL);
  pthread_join(tid[1], NULL);
  fclose(trace);
}
