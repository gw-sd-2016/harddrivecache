#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sqlite3.h>

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
		if(!file){
			file = file_stat_create(path, 0, 0, 0, 0, 0);
			get_data(file);
			current_size += file->size;
            table_add(&cache_table, file);
			heap_insert(priority_heap, file);
		}
		file->numreads++;
		file->lastread = (int) time(0);
		gen_priority(file);
		update_data(file);

        while(current_size > MAX_CACHE) cache_remove(heap_delete(priority_heap, 0)->path);
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

	if(access( ssd_path, R_OK ) != -1){
		if(!file){
			file = file_stat_create(path, 0, 0, 0, 0, 0);
			get_data(file);
            table_add(&cache_table, file);
			heap_insert(priority_heap, file);
		}
		file->numwrites++;
		file->lastwrite = (int) time(0);
	}
	FILE* hd_file, *ssd_file;
	hd_file = fopen(hd_path, "r");
	if(hd_file == NULL) return -1;
	
	make_dirs(path);
	ssd_file = fopen(ssd_path, "w");
	if(ssd_file == NULL) return -1;
	
	struct stat st;
	stat(hd_path, &st);
	
	if(!file){
		file = file_stat_create(path, (int) time(0), (int) time(0), 1, 1, st.st_size);
		table_add(&cache_table, file);
		heap_insert(priority_heap, file);
		gen_priority(file);
		insert_data(file);
	}
	else{
		file->size = st.st_size;
		update_data(file);
	}

	current_size += file->size;
    while(current_size > MAX_CACHE) cache_remove(heap_delete(priority_heap, 0)->path);

	while((ch = fgetc(hd_file))!=EOF) fputc(ch, ssd_file);
	
	fclose(hd_file);
	fclose(ssd_file);
	free(hd_path);
	free(ssd_path);
	return 0;
}
int cache_remove(const char* path){
	char* ssd_path = get_ssd_path(path);
	int ret = unlink(ssd_path);
	if(!ret){
		char* data_path = get_data_path(path);
		unlink(data_path);
		free(data_path);
	}
	free(ssd_path);	
	struct file_stat* file = table_rem(cache_table, path);
    if(file) current_size -= file->size;
	return ret;
}
void gen_priority(struct file_stat* file){
    int size = SIZE_BIAS * MAX_CACHE / file->size;
    int curr_time = (int) time(0);
    int read_time = (curr_time-file->lastread)/(60000*TIME_BIAS);
    int write_time = (curr_time-file->lastwrite)/(60000*TIME_BIAS);
        
    int read = file->numreads*read_time*READ_BIAS;
    int write = file->numwrites*write_time*WRITE_BIAS;
    int priority = size + read + write;

	if(file->size > MAX_CACHE) file->p = 0;
    else file->p = priority;
       
	heap_delete(priority_heap, file->heap_pos);
    heap_insert(priority_heap, file);

    char print[256];
    sprintf(print, "size:%d read/time:%d/%d write/time:%d/%d priority:%d", size, read, read_time, write, write_time, priority);
    log_msg(print);
}

void callback(struct file_stat* file, int num_col, char** results, char** col_names){
	file->lastread = atoi(results[1]);
	file->lastwrite = atoi(results[2]);
	file->numreads = atoi(results[3]);
	file->numwrites = atoi(results[4]);
}
int get_data(struct file_stat* file){
	sqlite3 *db;
	char sql[256];
	char* err;

	int ret = sqlite3_open(DB_PATH, &db);

	sprintf(sql, "select * from file_stats where file='%s';", file->path);

	if(ret){
		log_msg("Can't open database!\n");
		sqlite3_close(db);
		return -1;
	}

	ret = sqlite3_exec(db, sql, callback, file, &err);

	if(ret != SQLITE_OK){
		log_msg("SQL error\n");
		log_msg(err);
		sqlite3_free(err);
	} 
	sqlite3_close(db);
	return 0;
}
int update_data(struct file_stat* file){
	sqlite3 *db;
	char sql[512];
	char* err;

	int ret = sqlite3_open(DB_PATH, &db);

	sprintf(sql, "update file_stats set lastread=%d,lastwrite=%d,numreads=%d,numwrites=%d where file='%s';", 
		file->lastread, file->lastwrite, file->numreads, file->numwrites, file->path);

	if(ret){
		log_msg("Can't open database!\n");
		sqlite3_close(db);
		return -1;
	}

	ret = sqlite3_exec(db, sql, callback, file, &err);

	if(ret != SQLITE_OK){
		log_msg("SQL error\n");
		log_msg(err);
		sqlite3_free(err);
	} 
	sqlite3_close(db);
	return 0;	
}
int insert_data(struct file_stat* file){
	sqlite3 *db;
	char sql[512];
	char* err;

	int ret = sqlite3_open(DB_PATH, &db);

	sprintf(sql, "insert into file_stats values (%s, %d, %d, %d, %d);", 
		file->path, file->lastread, file->lastwrite, file->numreads, file->numwrites);

	if(ret){
		log_msg("Can't open database!\n");
		sqlite3_close(db);
		return -1;
	}

	ret = sqlite3_exec(db, sql, callback, file, &err);

	if(ret != SQLITE_OK){
		log_msg("SQL error\n");
		log_msg(err);
		sqlite3_free(err);
	} 
	sqlite3_close(db);
	return 0;	
}
void cache_init(){
    current_size = 0;
	cache_table = table_create(10);
    priority_heap = heap_create(10);
}

