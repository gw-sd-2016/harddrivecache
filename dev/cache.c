#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <hash.h>
#include <list.h>
#include <cache.h>
#include <disk_info.h>
#include <log.h>

struct hashTable* cache_table;

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
void cache_init(){
	cache_table = table_create(10);
}

