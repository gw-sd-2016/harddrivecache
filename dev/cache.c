#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <heap.h>
#include <hash.h>
#include <list.h>
#include <cache.h>
#include <disk_info.h>
#include <log.h>

#define MAX_CACHE 256000

#define SIZE_BIAS 10
#define READ_BIAS 10
#define WRITE_BIAS 0
#define TIME_BIAS 10

struct hashTable* cache_table;
struct heap* priority_heap;
int current_size;

int cache_exists(const char* path){
	struct file_stat* file = table_search(cache_table, path);
	char * new_path = get_ssd_path(path);
	table_print(cache_table);
	if(access( new_path, R_OK ) != -1){
		log_msg("cache hit");
		if(file){
			file->numreads++;
			file->lastread = (int) time(0);
		}
		else{
			struct stat st;
			stat(new_path, &st);
			struct file_stat* file = file_stat_create(path, (int) time(0), (int) time(0), 1, 1, st.st_size);
			current_size += st.st_size;
                        table_add(&cache_table, file);
		}
		free(new_path);
		return 1;
	}
	else{
		log_msg("cache miss");
		free(new_path);
		return 0;
	}
}
void make_dirs(const char* path){
	char fill_path[256];
	strcpy(fill_path, SSD_PATH);

	char* next, *curr = strtok(path, "/");
	int ret;
	
	while(curr){
		sprintf(fill_path, "%s/%s", fill_path, curr);
		next = strtok(NULL, "/");
		if(!next) break;
		log_msg(fill_path);
		mkdir(fill_path, S_IRWXU); 
		curr = next;
	}
}

int cache_add(const char* path){
	char* hd_path = get_hd_path(path);
	char* ssd_path = get_ssd_path(path);
	char ch;

	log_msg("adding to cache");
	log_msg(ssd_path);
		
	struct file_stat* file = table_search(cache_table, path);

	if(file){
		file->numwrites++;
		file->lastwrite = (int) time(0);
	}
	else{
		struct stat st;
		stat(hd_path, &st);
		struct file_stat* file = file_stat_create(path, (int) time(0), (int) time(0), 1, 1, st.st_size);
		current_size += st.st_size;
                table_add(&cache_table, file);
	}
	FILE* hd_file, *ssd_file;
	hd_file = fopen(hd_path, "r");
	if(hd_file == NULL) return -1;
	
	make_dirs(path);
	ssd_file = fopen(ssd_path, "w");
	if(ssd_file == NULL) return -1;

	while((ch = fgetc(hd_file))!=EOF) fputc(ch, ssd_file);
	
	fclose(hd_file);
	fclose(ssd_file);
	free(hd_path);
	free(ssd_path);
	return 0;
}
int cache_remove(const char* path){
	char* ssd_path = get_ssd_path(path);
	unlink(ssd_path);
	free(ssd_path);	
	table_rem(cache_table, path);
}
int gen_priority(const char* path){
        struct file_stat* file = table_search(cache_table, path);
            
        int size = SIZE_BIAS * MAX_CACHE / file->size;
        int curr_time = (int) time(0);
        int read_time = (curr_time-file->lastread)/(60000*TIME_BIAS);
        int write_time = (curr_time-file->lastwrite)/(60000*TIME_BIAS);
        
        int read = file->numreads*read_time*READ_BIAS;
        int write = file->numwrites*write_time*WRITE_BIAS;
        int priority = size + read + write;

        file->p = priority;
        heap_delete(priority_heap, file->heap_pos);
        heap_insert(priority_heap, file);

        char print[256];
        sprintf(print, "size:%d read/time:%d/%d write/time:%d/%d priority:%d", size, read, read_time, write, write_time, priority);
        log_msg(print);
}
void cache_init(){
        current_size = 0;
	cache_table = table_create(10);
        priority_heap = heap_create(10);
}

