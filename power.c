#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include <signal.h>
typedef void (*sighandler_t)(int);

#include <errno.h>

#include "power.h"

// options
int opt_is_verbose = 0;
int opt_is_extended = 0;


int get_options(int argc, char **argv) {
  int flags, opt;
  flags = 0;
  while ((opt = getopt(argc, argv, "v-")) != -1) {
    switch (opt) {
      case 'v':
        opt_is_verbose = 1;
        break;
      case '-':
        return opt;
      default: /* '?' */
        fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n",
              argv[0]);
        exit(EXIT_FAILURE);
    }
  }
  return opt;
}

void init_counters(infos_t *infos) {
  infos->nb_cpus = sysconf( _SC_NPROCESSORS_ONLN );
  init_cstates(infos);
  init_freqs(infos);
}

int start_counters(infos_t *infos) {
  init_counters(infos);
  start_cstates(infos);
  start_freqs(infos);
  start_tracing(infos);
  gettimeofday (&infos->time_start, (struct timezone *) 0);  
}

int end_counters(pid_t pid, infos_t *infos) {
  pid_t caught;
  int status;  
  /* Ignore signals, but don't ignore the children.  When wait3
     returns the child process, set the time the command finished. */
  while ((caught = wait3 (&status, 0, &infos->ru)) != pid) {
    if (caught == -1)
    return 0;
  }
  finish_cstates(infos);
  finish_freqs(infos);
  stop_tracing();
  gettimeofday (&infos->time_elapsed, (struct timezone *) 0);
  infos->time_elapsed.tv_sec -= infos->time_start.tv_sec;
  if (infos->time_elapsed.tv_usec < infos->time_start.tv_usec)
    {
      /* Manually carry a one from the seconds field.  */
      infos->time_elapsed.tv_usec += 1000000;
      --infos->time_elapsed.tv_sec;
    }
  infos->time_elapsed.tv_usec -= infos->time_start.tv_usec;
  infos->waitstatus = status;
  return 1;
}

static void run_command (char *const *cmd, infos_t *infos) {
  pid_t pid;			/* Pid of child.  */
  sighandler_t interrupt_signal, quit_signal;

  start_counters(infos);

  pid = fork ();		/* Run CMD as child process.  */
  if (pid < 0)
  error (1, errno, "cannot fork");
  else if (pid == 0) {				/* If child.  */
    /* Don't cast execvp arguments; that causes errors on some systems,
       versus merely warnings if the cast is left off.  */
    execvp (cmd[0], cmd);
    error (0, errno, "cannot run %s", cmd[0]);
    _exit (errno == ENOENT ? 127 : 126);
  }

  /* Have signals kill the child but not self (if possible).  */
  interrupt_signal = signal (SIGINT, SIG_IGN);
  quit_signal = signal (SIGQUIT, SIG_IGN);

  if (end_counters(pid, infos) == 0)
  error (1, errno, "error waiting for child process");

  /* Re-enable signals.  */
  signal (SIGINT, interrupt_signal);
  signal (SIGQUIT, quit_signal);
}

void print_total_energy(infos_t *infos) {
  int cpu_id;
  printf("Total energy consumption: \n");
  for (cpu_id = 0; cpu_id < infos->nb_cpus; cpu_id++) {
    long total_joules = 0;
    // time in idle
    total_joules += CSTATE_IN_MS(infos->cstates_total[cpu_id]) * ST_10_IDLE / 1000;
    // time at low freq but not idle
    long rest = PSTATE_IN_MS(infos->time_in_freq_beg[cpu_id][2].time) - CSTATE_IN_MS(infos->cstates_total[cpu_id]);
    if (rest > 0) {
      total_joules += rest * ST_10_RUN / 1000;
    }
    total_joules += PSTATE_IN_MS(infos->time_in_freq_beg[cpu_id][1].time) * ST_13_RUN / 1000;
    total_joules += PSTATE_IN_MS(infos->time_in_freq_beg[cpu_id][0].time) * ST_18_RUN / 1000;
    printf ("  CPU %d: %ld joules\n", cpu_id, total_joules);
  }

}


void print_infos(infos_t *infos) {
  unsigned long r;		/* Elapsed real milliseconds.  */
  unsigned long v;		/* Elapsed virtual (CPU) milliseconds.  */
  FILE *fp = stdout;

  if (WIFSTOPPED (infos->waitstatus))
  fprintf (fp, "Command stopped by signal %d\n",
        WSTOPSIG (infos->waitstatus));
  else if (WIFSIGNALED (infos->waitstatus))
  fprintf (fp, "Command terminated by signal %d\n",
        WTERMSIG (infos->waitstatus));
  else if (WIFEXITED (infos->waitstatus) && WEXITSTATUS (infos->waitstatus))
  fprintf (fp, "Command exited with non-zero status %d\n",
        WEXITSTATUS (infos->waitstatus));

  /* Convert all times to milliseconds.  Occasionally, one of these values
     comes out as zero.  Dividing by zero causes problems, so we first
     check the time value.  If it is zero, then we take `evasive action'
     instead of calculating a value.  */

  r = infos->time_elapsed.tv_sec * 1000 + infos->time_elapsed.tv_usec / 1000;

  v = infos->ru.ru_utime.tv_sec * 1000 + infos->ru.ru_utime.tv_usec +
   infos->ru.ru_stime.tv_sec * 1000 + infos->ru.ru_stime.tv_usec;

  printf("Time Elapsed: ");
  if (infos->time_elapsed.tv_sec >= 3600)	/* One hour -> h:m:s.  */
  fprintf (fp, "%ld:%02ld:%02ld",
        infos->time_elapsed.tv_sec / 3600,
        (infos->time_elapsed.tv_sec % 3600) / 60,
        infos->time_elapsed.tv_sec % 60);
  else
  fprintf (fp, "%ld:%02ld.%02ld",	/* -> m:s.  */
        infos->time_elapsed.tv_sec / 60,
        infos->time_elapsed.tv_sec % 60,
        infos->time_elapsed.tv_usec / 10000);
  printf("\n");

  printf("\nMore info\n");
  /* Percent of CPU this job got.  */
  /* % cpu is (total cpu time)/(time_elapsed time).  */
  printf("CPU usage: ");
  if (r > 0)
  fprintf (fp, "%lu%%", (v * 100 / r));
  else
  fprintf (fp, "?%%");
  printf("\n");

  printf("System time: ");
  /* System time.  */
  fprintf (fp, "%ld.%02ld",
        infos->ru.ru_stime.tv_sec,
        infos->ru.ru_stime.tv_usec / 1000);
  printf("\n");
  printf("User time: ");
  /* User time.  */
  fprintf (fp, "%ld.%02ld",
        infos->ru.ru_utime.tv_sec,
        infos->ru.ru_utime.tv_usec / 1000);
  printf("\n");
  printf("Elapsed time: ");
  /* time_elapsed real time in seconds.  */
  fprintf (fp, "%ld.%02ld",
        infos->time_elapsed.tv_sec,
        infos->time_elapsed.tv_usec / 10000);
  printf("\n");
  
  print_total_energy(infos);
  print_cstates(infos);
  print_freqs(infos);

}

void main (int argc, char **argv) {
  const char **command_line;

  infos_t infos;

//  int nb_args = get_options (argc, argv);
  run_command (&argv[1], &infos);
  //  summarize (outfp, output_format, command_line, &res);
  //  fflush (outfp);
  // print_infos(&infos);

  if (WIFSTOPPED (infos.waitstatus))
  exit (WSTOPSIG (infos.waitstatus) + 128);
  else if (WIFSIGNALED (infos.waitstatus))
  exit (WTERMSIG (infos.waitstatus) + 128);
  else if (WIFEXITED (infos.waitstatus))
  exit (WEXITSTATUS (infos.waitstatus));
}

