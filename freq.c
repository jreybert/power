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

  infos->time_in_freq_trace_beg = malloc( infos->nb_cpus * sizeof(time_in_freq_t*) );
  infos->time_in_freq_trace_end = malloc( infos->nb_cpus * sizeof(time_in_freq_t*) );
  int cpu_id ;
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    infos->time_in_freq_trace_beg[cpu_id] = malloc( infos->nb_freqs * sizeof(time_in_freq_t) );
    infos->time_in_freq_trace_end[cpu_id] = malloc( infos->nb_freqs * sizeof(time_in_freq_t) );
  }
}

void refresh_freqs_trace(infos_t *infos) {
  time_in_freq_t **tmp = infos->time_in_freq_trace_beg;
  infos->time_in_freq_trace_beg = infos->time_in_freq_trace_end;
  infos->time_in_freq_trace_end = tmp;
  int cpu_id;
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    get_freq_stats(cpu_id, infos->time_in_freq_trace_end[cpu_id]);
  }
}

