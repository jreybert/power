
// time is the time passed in this state
// usage is the number of time CPU went through this state
typedef struct {
  char name[MAX_CSTATE_NAME];
  unsigned long long time;
  unsigned long usage;
} cstate_stat_t;


typedef struct {
  unsigned long long freq;
  unsigned long long time;
} time_in_freq_t;

typedef struct {
  int waitstatus;
  struct rusage ru;
  struct timeval time_start, time_elapsed; /* Wallclock time of process.  */
  cstate_stat_t cstate_start, cstate_end;
  time_in_freq_t freq_st
} infos;
