#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "power.h"

static unsigned int sysfs_read_file(unsigned int cpu, const char *fname, char *buf, size_t buflen) {
	char path[SYSFS_PATH_MAX];
	int fd;
	size_t numread;
	snprintf(path, sizeof(path), PATH_TO_CPU "cpu%u/cpufreq/%s", cpu, fname);
	if ( ( fd = open(path, O_RDONLY) ) == -1 )
		return 0;
	numread = read(fd, buf, buflen - 1);
	if ( numread < 1 ) {
		close(fd);
		return 0;
	}
	buf[numread] = '\0';
	close(fd);
	return numread;
}

static int get_nb_freqs() {
	char path[SYSFS_PATH_MAX];
	char linebuf[MAX_LINE_LEN];  
  int nb_lines = 0;
	snprintf(path, sizeof(path), PATH_TO_CPU "cpu0/cpufreq/stats/time_in_state");
  FILE *f = fopen (path , "r");
  while ( fgets (linebuf, MAX_LINE_LEN, f) ) {
    nb_lines++;
  }
  fclose(f);
  return nb_lines;
}

static void get_freq_stats(unsigned int cpu, time_in_freq_t *tis) {
//	char filebuf[MAX_LINE_LEN];
//  char *linebuf;
//	unsigned int pos;
//	unsigned int len;
//
//	if ( ( len = sysfs_read_file(cpu, "stats/time_in_state", filebuf, sizeof(filebuf))) == 0 ) {
//    perror("Could not open time_in_freq");
//		return;
//  }
//
//	pos = 0;
//
  /* 
   * - time_in_state
   * This gives the amount of time spent in each of the frequencies supported by
   * this CPU. The cat output will have "<frequency> <time>" pair in each line, which
   * will mean this CPU spent <time> usertime units of time at <frequency>. Output
   * will have one line for each of the supported frequencies. usertime units here 
   * is 10mS (similar to other time exported in /proc).
   */
//  linebuf = (char*) strtok (filebuf, "\n");
//  while (linebuf != NULL) {
//    sscanf(linebuf, "%llu %llu", &tis[pos].freq, &tis[pos].time);
//    linebuf = (char*)strtok (NULL, "\n");
//    pos++;
//  }

  char path[SYSFS_PATH_MAX];
	char linebuf[MAX_LINE_LEN];  
  int pos = 0;
	snprintf(path, sizeof(path), PATH_TO_CPU "cpu%d/cpufreq/stats/time_in_state", cpu);
  FILE *f = fopen (path , "r");
  while ( fgets (linebuf, MAX_LINE_LEN, f) ) {
    sscanf(linebuf, "%llu %llu", &tis[pos].freq, &tis[pos].time);
//    linebuf = strtok (NULL, "\n");
    pos++;
  }
  fclose(f);
}

void init_freqs(infos_t *infos) {
  infos->nb_freqs = get_nb_freqs() ;

  infos->freqs_total = malloc( infos->nb_cpus * sizeof(unsigned long long));
  memset(infos->freqs_total, 0, infos->nb_cpus * sizeof(unsigned long long) );
  infos->time_in_freq_beg = malloc( infos->nb_cpus * sizeof(time_in_freq_t*) );
  infos->time_in_freq_end = malloc( infos->nb_cpus * sizeof(time_in_freq_t*) );
  int cpu_id ;
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    infos->time_in_freq_beg[cpu_id] = malloc( infos->nb_freqs * sizeof(time_in_freq_t) );
    infos->time_in_freq_end[cpu_id] = malloc( infos->nb_freqs * sizeof(time_in_freq_t) );
  }
  //memset(infos->time_in_freq_beg, 0, infos->nb_cpus * infos->nb_freqs  * sizeof(time_in_freq_t) );
  //memset(infos->time_in_freq_end, 0, infos->nb_cpus * infos->nb_freqs  * sizeof(time_in_freq_t) );
}

void start_freqs(infos_t *infos) {
  int cpu_id;
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    get_freq_stats(cpu_id, infos->time_in_freq_beg[cpu_id]);
  }
}

void finish_freqs(infos_t *infos) {
  int cpu_id;
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    get_freq_stats(cpu_id, infos->time_in_freq_end[cpu_id]);
  }
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    int state;
    for (state = 0; state < infos->nb_freqs; state++) {
      infos->time_in_freq_beg[cpu_id][state].time = infos->time_in_freq_end[cpu_id][state].time - infos->time_in_freq_beg[cpu_id][state].time;
      infos->freqs_total[cpu_id] += infos->time_in_freq_beg[cpu_id][state].time;
    }
  }
}

void print_freqs(infos_t *infos) {
  printf("** Frequencies: **\n");
  int cpu_id;
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    printf("   CPU %d: total: %lld ms\n", cpu_id, PSTATE_IN_MS(infos->freqs_total[cpu_id]) );
    int state;
    for (state = 0; state < infos->nb_freqs; state++) {
      printf("     %lld: %.2f%% - %lld ms\n", infos->time_in_freq_beg[cpu_id][state].freq, (100.0 * infos->time_in_freq_beg[cpu_id][state].time) / infos->freqs_total[cpu_id], PSTATE_IN_MS(infos->time_in_freq_beg[cpu_id][state].time) );
    }
  }
}
