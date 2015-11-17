#ifndef LOG
#define LOG
#include <stdio.h>
#include <time.h>
#include <string.h>

#define LOG_FILE "/home/timstamler/harddrivecache/log/cache_fs.log"

static inline void log_msg(char* msg)
{
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  char* time_str = asctime(timeinfo);
  time_str[strlen(time_str)-1] = '\0';

  FILE* log_file = fopen(LOG_FILE, "a");
  fprintf (log_file, "%s --- %s\n", time_str, msg );
  fclose(log_file);
}	
#endif
