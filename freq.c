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

static void get_freq_stats(unsigned int cpu, time_in_freq_t *tis) {
	char filebuf[MAX_LINE_LEN];
  char *linebuf;
	unsigned int pos;
	unsigned int len;

	if ( ( len = sysfs_read_file(cpu, "stats/time_in_state", filebuf, sizeof(filebuf))) == 0 ) {
    perror("Could not open time_in_freq");
		return;
  }

	pos = 0;

  linebuf = (char*) strtok (filebuf, "\n");
  while (linebuf != NULL) {
    sscanf(linebuf, "%llu %llu", &tis[pos].freq, &tis[pos].time);
    linebuf = (char*)strtok (NULL, "\n");
    pos++;
  }

}

void init_freqs(infos_t *infos) {
  infos->freqs_total = malloc( infos->nb_cpus * sizeof(unsigned long long));
  memset(infos->freqs_total, 0, infos->nb_cpus * sizeof(unsigned long long) );
  infos->time_in_freq_beg = malloc( infos->nb_cpus * MAX_FREQ_NB * sizeof(time_in_freq_t) );
  infos->time_in_freq_end = malloc( infos->nb_cpus * MAX_FREQ_NB * sizeof(time_in_freq_t) );
  memset(infos->time_in_freq_beg, 0, infos->nb_cpus * MAX_FREQ_NB * sizeof(time_in_freq_t) );
  memset(infos->time_in_freq_end, 0, infos->nb_cpus * MAX_FREQ_NB * sizeof(time_in_freq_t) );
}

void start_freqs(infos_t *infos) {
  int cpu_id;
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    get_freq_stats(cpu_id, &infos->time_in_freq_beg[cpu_id*MAX_FREQ_NB]);
  }
}

void finish_freqs(infos_t *infos) {
  int cpu_id;
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    get_freq_stats(cpu_id, &infos->time_in_freq_end[cpu_id*MAX_FREQ_NB]);
  }
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    int state;
    for (state = 0; state < MAX_FREQ_NB; state++) {
      infos->time_in_freq_beg[cpu_id*MAX_FREQ_NB+state].time = infos->time_in_freq_end[cpu_id*MAX_FREQ_NB+state].time - infos->time_in_freq_beg[cpu_id*MAX_FREQ_NB+state].time;
      infos->freqs_total[cpu_id] += infos->time_in_freq_beg[cpu_id*MAX_FREQ_NB+state].time;
    }
  }
}

void print_freqs(infos_t *infos) {
  printf("** Frequencies: **\n");
  int cpu_id;
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    printf("   CPU %d: total: %lld ms\n", cpu_id, infos->freqs_total[cpu_id]);
    int pos = 0;
    while ( infos->time_in_freq_beg[cpu_id*MAX_FREQ_NB+pos].freq != 0 ) {
      printf("     %lld: %.2f%% - %lld\n", infos->time_in_freq_beg[cpu_id*MAX_FREQ_NB+pos].freq, (100.0 * infos->time_in_freq_beg[cpu_id*MAX_FREQ_NB+pos].time) / infos->freqs_total[cpu_id], infos->time_in_freq_beg[cpu_id*MAX_FREQ_NB+pos].time);
      pos++;
    }
  }
}
