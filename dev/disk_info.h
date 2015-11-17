#ifndef DISK_INFO
#define DISK_INFO
#include <string.h>
#include <stdlib.h>

#define HD_PATH "/home/timstamler/harddrivecache/mnt/hd"
#define SSD_PATH "/home/timstamler/harddrivecache/mnt/ssd"


static inline char* get_hd_path(const char* path){
        char* new_path = malloc(strlen(path) + strlen(HD_PATH));
        sprintf(new_path, "%s%s", HD_PATH, path);
        return new_path;
}

static inline char* get_ssd_path(const char* path){
        char* new_path = malloc(strlen(path) + strlen(SSD_PATH));
        sprintf(new_path, "%s%s", SSD_PATH, path);
        return new_path;
}

#endif
