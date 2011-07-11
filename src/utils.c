#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <string.h>

#include <errno.h>

#include <dirent.h>

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
