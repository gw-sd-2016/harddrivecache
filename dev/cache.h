#include <string.h>
#include <stdlib.h>

int cache_exists(const char* path);
int cache_add(const char* path);
int cache_remove(const char* path);
void gen_priority(struct file_stat* file);
int get_data(struct file_stat* file);
int update_data(struct file_stat* file);
int insert_data(struct file_stat* file);
void cache_init();
