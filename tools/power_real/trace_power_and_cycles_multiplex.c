#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
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

#include <curses.h>

#include <errno.h>


#include <papi.h>
#define NB_EVENTS 11


int volatile while_watch = 1;

void check_events(int *events, int nb_events) {
  int i;
  //if ( nb_events > PAPI_num_counters() ) {
  //  fprintf(stderr, "Too many counters: %d\nMaximum available %d\n", nb_events, PAPI_num_counters());
  //  exit(1);
  //}
  for (i=0; i < nb_events; i++) {
    if (PAPI_query_event(events[i]) != PAPI_OK) {
      PAPI_event_info_t info;
      PAPI_get_event_info(events[i], &info);
      fprintf(stderr,"No instruction counter for \"%s\" event\n", info.short_descr);
      exit(1);
    }
  }
}


void print_values(char events_name[NB_EVENTS][PAPI_MAX_STR_LEN], long long *values, int nb_events) {
  int i;
//  printf("\r");
  for (i = 0; i < nb_events; i++) {
    move(i*2,0);
    printw("%30s : %12lld ", events_name[i], values[i]);
    move(i*2+1,0);
    printw("%30s     °  °  °  °", " ", values[i]);
  }
  refresh();
//  fflush(stdout);
}

void name_to_code(char events_name[NB_EVENTS][PAPI_MAX_STR_LEN], int nb_events, int *events_code) {
  int i;
  for (i=0; i < nb_events; i++)
    PAPI_event_name_to_code(events_name[i], &events_code[i]);
}
void *watch_process(void* _pid) {


  pid_t pid = *((pid_t*)_pid);
  printf("%d\n", pid);
  int retval, EventSet = PAPI_NULL;

  //char events_name[NB_EVENTS][PAPI_MAX_STR_LEN] = {"UNHALTED_CORE_CYCLES", "INSTRUCTION_RETIRED", "MEM_LOAD_RETIRED:L1D_HIT", "MEM_LOAD_RETIRED:L2_HIT", "LLC_REFERENCES"};
  //char events_name[NB_EVENTS][PAPI_MAX_STR_LEN] = {"UNHALTED_CORE_CYCLES", "INSTRUCTION_RETIRED", "MEM_LOAD_RETIRED:L2_HIT", "L2_RQSTS:LOADS", "L2_DATA_RQSTS:ANY"};
  //char events_name[NB_EVENTS][PAPI_MAX_STR_LEN] = {"UNHALTED_CORE_CYCLES", "INSTRUCTION_RETIRED", "L1I:READS", "L1D_CACHE_LD:MESI", "L1D_CACHE_ST:MESI"};
  //char events_name[NB_EVENTS][PAPI_MAX_STR_LEN] = {"UNHALTED_CORE_CYCLES", "INSTRUCTION_RETIRED", "L1D_CACHE_LD:MESI", "L1D_CACHE_ST:MESI", "L2_DATA_RQSTS:ANY" };
  char events_name[NB_EVENTS][PAPI_MAX_STR_LEN] = {"UNHALTED_CORE_CYCLES", "INSTRUCTION_RETIRED", "L1I:READS", "L1D_CACHE_LD:MESI", "L1D_CACHE_ST:MESI", "MEM_LOAD_RETIRED:L2_HIT", "L2_RQSTS:LOADS", "L2_DATA_RQSTS:ANY", "BR_INST_RETIRED:ALL_BRANCHES", "BR_INST_EXEC:ANY", "BR_MISP_EXEC:ANY" };
  unsigned int native_events[NB_EVENTS];
  long long values[NB_EVENTS];


  int err;
  /* Initialize the library */
  if ( (err = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT) {
    fprintf(stderr, "PAPI library init error! %d\n", err);
    exit(1);
  }

  if ( (err = PAPI_multiplex_init()) != PAPI_OK) {
    fprintf(stderr, "PAPI multiplex init error! %d\n", err);
    exit(1);
  }

  
  if (PAPI_thread_init(pthread_self) != PAPI_OK) {
    fprintf(stderr, "PAPI thread init error! %d\n", err);
    exit(1);
  }


  //sched_setaffinity(0, 0);
  //if ((retval = PAPI_set_granularity(PAPI_GRN_MAX)) != PAPI_OK) {
  //  fprintf(stderr, "PAPI set granurality error! %d, %d, %d, %d, %d\n", retval, PAPI_EINVAL, PAPI_ENOEVST, PAPI_ENOCMP, PAPI_EISRUN);
  //  exit(1);
  //}

  // Traduction des string en code
  name_to_code(events_name, NB_EVENTS, native_events);
  // Vérification des évènements
  check_events(native_events, NB_EVENTS);
  
  if (PAPI_create_eventset(&EventSet) != PAPI_OK) {    
    fprintf(stderr, "PAPI EventSet init error!\n");
    exit(1);
  }

  if ( (err = PAPI_assign_eventset_component( EventSet, 0 ) ) != PAPI_OK ) {
    fprintf(stderr, "PAPI assign component error!\n%s\n", PAPI_strerror(err));
    exit(1);
  }

  if ( ( err = PAPI_set_multiplex(EventSet) ) != PAPI_OK) {    
    fprintf(stderr, "PAPI set multiplex error!\n%s\n", PAPI_strerror(err));
    exit(1);
  }

  if ( (retval = PAPI_add_events(EventSet, native_events, NB_EVENTS) ) != PAPI_OK) {
    fprintf(stderr, "PAPI add events error!\n%s\n", PAPI_strerror(retval) );
    exit(1);
  }

  if (PAPI_attach(EventSet, pid) != PAPI_OK)
    exit(1);

  while(while_watch) {
    /* Start counting */
    if (PAPI_start(EventSet) != PAPI_OK) {
      fprintf(stderr, "PAPI start error!\n");
      exit(1);
    }
    usleep(1000000);

    if (PAPI_stop(EventSet, values) != PAPI_OK) {
      fprintf(stderr, "PAPI stop error!\n");
      exit(1);
    }

    print_values(events_name, values, NB_EVENTS);


  }

	return 0;
}

static void run_command (char *const *cmd) {
  pid_t pid;			/* Pid of child.  */
  sighandler_t interrupt_signal, quit_signal;


  pid = fork ();		/* Run CMD as child process.  */
  if (pid < 0)
    error (1, errno, "cannot fork");
  else if (pid == 0) {				/* If child.  */
    /* Don't cast execvp arguments; that causes errors on some systems,
       versus merely warnings if the cast is left off.  */
    execvp (cmd[0], cmd);
    error (0, errno, "cannot run %s\n", cmd[0]);
    _exit (errno == ENOENT ? 127 : 126);
  }

 // usleep(300);

  pthread_t tid;
  if (pthread_create(&tid, NULL, watch_process, (void * ) &pid) != 0) {
    fprintf(stderr, "pthread_create error\n");
  }
  /* Have signals kill the child but not self (if possible).  */
  interrupt_signal = signal (SIGINT, SIG_IGN);
  quit_signal = signal (SIGQUIT, SIG_IGN);
  pid_t caught;
  int status;  
  /* Ignore signals, but don't ignore the children.  When wait3
     returns the child process, set the time the command finished. */
  while ((caught = wait3 (&status, 0, NULL)) != pid) {
    if (caught == -1)
    break;
  }
  while_watch=0;

  printf("\n");
  /* Re-enable signals.  */
  signal (SIGINT, interrupt_signal);
  signal (SIGQUIT, quit_signal);
}

static void finish_ncurses(int sig);

int init_ncurses() {
    int num = 0;

    /* initialize your non-curses data structures here */

    signal(SIGINT, finish_ncurses);      /* arrange interrupts to terminate */

    initscr();      /* initialize the curses library */
    keypad(stdscr, TRUE);  /* enable keyboard mapping */
    nonl();         /* tell curses not to do NL->CR/NL on output */
    cbreak();       /* take input chars one at a time, no wait for \n */
    noecho();         /* echo input - in color */

    if (has_colors())
    {
        start_color();
    }

}

static void finish_ncurses(int sig)
{
    endwin();

    /* do your non-curses wrapup here */

    //exit(0);
}

int main(int argc, char **argv) {
  init_ncurses();
  run_command (&argv[1]);
  finish_ncurses(0);
  //  summarize (outfp, output_format, command_line, &res);
  //  fflush (outfp);
  //print_infos(&infos);

 // if (WIFSTOPPED (infos.waitstatus))
 // exit (WSTOPSIG (infos.waitstatus) + 128);
 // else if (WIFSIGNALED (infos.waitstatus))
 // exit (WTERMSIG (infos.waitstatus) + 128);
 // else if (WIFEXITED (infos.waitstatus))
 // exit (WEXITSTATUS (infos.waitstatus));
  

}

