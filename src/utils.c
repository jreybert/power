#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>           /* For O_* constants */
#include <string.h>

#include <errno.h>

#include <dirent.h>


#include <sys/types.h>
#include <pwd.h>

#include <glib.h>

#include "power.h"

// Function extracted from top
int stat2proc(int pid, int *ppid, int *processor_id, char* proc_file) {
    char buf[800], c_null[4]; /* about 40 fields, 64-bit decimal is about 20 chars */
    int num;
    int fd;
    unsigned long null_int;
    char* tmp;
    if ( (fd = open(proc_file, O_RDONLY, 0) ) == -1 ) { 
//      perror("Open error\n");
//      fprintf(stderr, "file: %s\n", proc_file);
      return 0;
    }
    num = read(fd, buf, sizeof buf - 1);
    close(fd);
    if(num<80) {
//      fprintf(stderr, "error a la con %s\n", proc_file);
      return 0;
    }
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

void init_trace_dir() {
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
        perror("Delete error\n");
      }
    }
  }

}

static void inline check_error(GError *error, char *message) {
  if (error) {
    printf("%s conf error\n", message);
    g_warning(error->message);
    g_error_free(error);
  }
}

void read_conf_file() {
  // PAPI must have been initialized first
  //papi_global_init() ;
#define nb_papi_events_defaults  5
  char events_name_defaults[nb_papi_events_defaults][PAPI_MAX_STR_LEN] = {"UNHALTED_CORE_CYCLES", "INSTRUCTION_RETIRED", "MEM_LOAD_RETIRED:L1D_HIT", "L2_DATA_RQSTS:ANY", "LLC_REFERENCES"};
  float events_weight_defaults[nb_papi_events_defaults] = {1000000000.0, 1000000000.0, 1000000000.0, 1000000000.0, 1000000000.0};

  char *user = getenv("USER");
  struct passwd *p = getpwnam(user);
  char filename[256];
  sprintf(filename, "%s/%s", p->pw_dir, CONFIG_FILE_PATH);

  //FILE *file = fopen ( filename, "r" );

  GKeyFile *keyfile;
  GKeyFileFlags flags = G_KEY_FILE_NONE;
  GError *error = NULL;
  keyfile = g_key_file_new ();
  if (!g_key_file_load_from_file (keyfile, filename, flags, &error)) {
    check_error(error, "load file");
  }
  
  global_config.events_name = g_key_file_get_keys (keyfile, "PAPI_EVENTS", &global_config.nb_papi_events, &error);
  if (error) 
    check_error(error, "get PAPI_EVENTS");
  else {
    if (global_config.nb_papi_events == 0 ) {
      printf("Using defaults PAPI events\n");
      global_config.events_name = malloc( sizeof(char*) * nb_papi_events_defaults );
      //global_config.events_weight = malloc( sizeof(float) * nb_papi_events_defaults );
      int i;
      for (i = 0; i < nb_papi_events_defaults; i++) {
        global_config.events_name[i] = malloc( sizeof(char) * PAPI_MAX_STR_LEN );
        strcpy(global_config.events_name[i], events_name_defaults[i]);
        global_config.events_weight[i] = events_weight_defaults[i];
      }
      global_config.nb_papi_events = nb_papi_events_defaults;
    }
    else {
      int i;
      for (i = 0; i < nb_papi_events_defaults; i++) {
        global_config.events_weight[i] = g_key_file_get_double ( keyfile, "PAPI_EVENTS", global_config.events_name[i], &error);
        check_error(error, "get weights");
      }
    }
  }

  printf("nb events %d\n", global_config.nb_papi_events);


  name_to_code(global_config.native_events, global_config.events_name, global_config.nb_papi_events);
  check_events(global_config.native_events, global_config.nb_papi_events);

  // Global configuration

  global_config.verbose = g_key_file_get_boolean ( keyfile, "GLOBAL", "VERBOSE", &error);
  check_error(error, "verbose");

  global_config.use_ipmi = g_key_file_get_boolean ( keyfile, "GLOBAL", "USE_IPMI", &error);
  check_error(error, "use_ipmi");
  global_config.use_sensors = g_key_file_get_boolean ( keyfile, "GLOBAL", "USE_SENSORS", &error);
  check_error(error, "use_sensors");

  global_config.nb_seconds_beg = g_key_file_get_integer ( keyfile, "GLOBAL", "NB_SECONDS_BEG", &error);
  check_error(error, "nb_seconds_beg");
  global_config.nb_seconds_end = g_key_file_get_integer ( keyfile, "GLOBAL", "NB_SECONDS_END", &error);
  check_error(error, "nb_seconds_end");
  global_config.refresh_rate = g_key_file_get_integer ( keyfile, "GLOBAL", "REFRESH_RATE", &error);
  check_error(error, "refresh_rate");
  if (global_config.refresh_rate == 0)
    global_config.refresh_rate = 1;



}
