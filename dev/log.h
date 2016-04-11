#ifndef LOG
#define LOG
#include <stdio.h>
#include <time.h>
#include <string.h>

#define LOG_FILE "/home/timstamler/harddrivecache/log/hdc_fs.log"
#define HIT_FILE "/home/timstamler/harddrivecache/log/cache.log"

static inline void log_hit()
{
	FILE* hit_file = fopen(HIT_FILE, "rw");
	int hits, misses;
	fscanf(hit_file, "%d\t%d", &hits, &misses);
	hits++;
	fprintf(hit_file, "%d\t%d", hits, misses);
	fclose(hit_file);	
}
static inline void log_miss()
{
	FILE* hit_file = fopen(HIT_FILE, "rw");
	int hits, misses;
	fscanf("%d\t%d", &hits, &misses);
	misses++;
	fprintf("%d\t%d", hits, misses);
	fclose(hit_file);	
}
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
