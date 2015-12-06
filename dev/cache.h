#include <string.h>
#include <stdlib.h>

int cache_exists(const char* path);
int cache_add(const char* path);
int cache_remove(const char* path);
void cache_init();
