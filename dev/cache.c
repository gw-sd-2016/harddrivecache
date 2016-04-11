#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sqlite3.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

#include <heap.h>
#include <hash.h>
#include <list.h>
#include <cache.h>
#include <disk_info.h>
#include <log.h>

#ifndef MAX_CACHE
#define MAX_CACHE 536870912
#endif

#define BUF_SIZE 4096

#define P_CHAR '+'

#define NAME_BIAS 0
#define SIZE_BIAS 1000
#define READ_BIAS 1000
#define WRITE_BIAS 0
#define TIME_BIAS 1000

struct hashTable* cache_table;
struct heap* priority_heap;
int current_size;

int time_to_index(int time){
	if(time < 1000) return 0;
	if(time < 10000) return 1;
	if(time < 100000) return 2;
	if(time < 1000000) return 3;
	return 4;
}

int pow(int b, int e){
	if(e == 0) return 1;
	if(e == 1) return b;
	return b * pow(b, e-1);
}

int cache_exists(const char* path){
	struct file_stat* file = table_search(cache_table, path);
	char * new_path = get_ssd_path(path);
	if(access( new_path, R_OK ) != -1){
		//log_msg("cache hit");
		//log_hit();
		if(!file){
			file = file_stat_create(path, (int) time(0), 0, 0, 0, 0);
			get_data(file);
			
			struct stat st;
			stat(new_path, &st);
			file->size = st.st_size;

			/*char print[256];
    		sprintf(print, "size: %d read/time:%d/%d", file->size, file->numreads, file->lastread);
    		log_msg(print);*/	

			current_size += file->size;
            table_add(&cache_table, file);
			heap_insert(priority_heap, file);
			//log_msg("inserted into ds");
		}
		file->numreads++;
		int diff = file->lastread - (int) time(0);
		file->freq_hist[time_to_index(diff)]++;
		file->lastread = (int) time(0);
		//log_msg("generating priority");
		gen_priority(file);
		//log_msg("updating db");
	
		update_data(file);
		while(current_size > MAX_CACHE){
			//char msg[256];
			//sprintf(msg, "total size: %d, removing from heap of size %d", current_size, priority_heap->n);
			//log_msg(msg);
			struct file_stat* deleted = heap_delete(priority_heap, 0);
			//log_msg(deleted->path);
			cache_remove(deleted->path);
		}
        //while(current_size > MAX_CACHE) cache_remove(heap_delete(priority_heap, 0)->path);
		free(new_path);
		return 1;
	}
	else{
		//log_msg("cache miss");
		//log_miss();
		free(new_path);
		return 0;
	}
}
void make_dirs(const char* path){
	char fill_path[256];
	strcpy(fill_path, SSD_PATH);

	char* next, *curr;
	char new_path[256];
	strcpy(new_path, path);
	curr = strtok(new_path, "/");
	int ret;
	
	while(curr){
		sprintf(fill_path, "%s/%s", fill_path, curr);
		next = strtok(NULL, "/");
		if(!next) break;
		//log_msg(fill_path);
		mkdir(fill_path, S_IRWXU); 
		curr = next;
	}
}

void copy_to_cache(const char* path){
	/*pid_t pid = fork();

	if(pid == 0){
		execv("/bin/cp", (char* []){ "/bin/cp", hd_path, ssd_path, NULL});
		_exit(1);
	}
		
	while((ch = fgetc(hd_file))!=EOF) fputc(ch, ssd_file);
	*/
	char buf[BUF_SIZE];
	int numRead = 0, numWrite = 0;
	char* hd_path = get_hd_path(path);
	char* ssd_path = get_ssd_path(path);

	//log_msg("copying");
	//pthread_detach(pthread_self());

	int hd_file = open(hd_path, O_RDONLY);
	int ssd_file = open(ssd_path, O_WRONLY | O_CREAT, 777);

	while((numRead = read(hd_file, buf, BUF_SIZE)) > 0)
	{
		numWrite = write(ssd_file, buf, numRead);
		if(numRead != numWrite)
		{
			char err[256];
			sprintf(err, "error num read: %d, num write: %d", numRead, numWrite);
			log_msg(err);
			break;	
		}
	}
	//log_msg("done copying");
	close(hd_file);
	close(ssd_file);
	free(hd_path);
	free(ssd_path);
	//pthread_exit(0);
}

int cache_add(const char* path){
	char* hd_path = get_hd_path(path);
	char* ssd_path = get_ssd_path(path);
	char buf[BUF_SIZE];
	char ch;

	//log_msg("adding to cache");
	//log_msg(path);
	//log_msg(hd_path);
	//log_msg(ssd_path);
		
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
	//FILE* hd_file, *ssd_file;
	//hd_file = fopen(hd_path, "r");
	//if(hd_file == NULL) return -1;
	
	make_dirs(path);
	
	//ssd_file = fopen(ssd_path, "w");
	//if(ssd_file == NULL) return -1;
	
	struct stat st;
	stat(hd_path, &st);
	
	if(!file){
		file = file_stat_create(path, (int) time(0), (int) time(0), 1, 1, st.st_size);
		get_data(file);
		table_add(&cache_table, file);
		heap_insert(priority_heap, file);
		gen_priority(file);
		//insert_data(file);
	}
	else{
		file->size = st.st_size;
		update_data(file);
	}	
	current_size += file->size;	
 
	if(current_size > MAX_CACHE)
	{
		if(strcmp(priority_heap->elements[0]->path, path)==0){
			heap_delete(priority_heap, 0);
			table_rem(cache_table, path);
			current_size -= file->size;	
			free(hd_path);
			free(ssd_path);
			return 0;
		}
		else{
			while(current_size > MAX_CACHE){
				//char msg[256];
				//sprintf(msg, "total size: %d, removing from heap of size %d", current_size, priority_heap->n);
				//log_msg(msg);
				struct file_stat* deleted = heap_delete(priority_heap, 0);
				//log_msg(deleted->path);
				cache_remove(deleted->path);
			}
		}
	}

	//pthread_t copier;
	//pthread_create(&copier, NULL, copy_to_cache, path);
	copy_to_cache(path);	
	
	free(hd_path);
	free(ssd_path);
	return 0;
}
int cache_remove(const char* path){
	char* ssd_path = get_ssd_path(path);
	//log_msg("removing from cache");
	int ret = unlink(ssd_path);
	free(ssd_path);	
	//log_msg("removing from table");
	struct file_stat* file = table_rem(cache_table, path);
    if(file) current_size -= file->size;
	return ret;
}

int name_priority(char* path){
	if(path == NULL) return 0;
	char c = path[0];
	int i=0, count=0;
	while(c != '\0'){
		if(c == P_CHAR) count++;
		c = path[++i];
	}
	return count;
}

void gen_priority(struct file_stat* file){
    int size;
	if(file->size > 0) size =  (MAX_CACHE / file->size) / SIZE_BIAS;
	else size = 0;
    int curr_time = (int) time(0);
    int read_time = (curr_time-file->lastread)/(60000*TIME_BIAS);
    int write_time = (curr_time-file->lastwrite)/(60000*TIME_BIAS);
        
    int name = name_priority(file->path)*NAME_BIAS;
	int read = file->numreads*read_time*READ_BIAS;
    int write = file->numwrites*write_time*WRITE_BIAS;
    int hist = 0, i = 0;

	for(i = 0; i<5; i++)
		hist += file->freq_hist[i] * pow(10, 5 - i);

	int priority = size + read + write + hist + name;

	if(file->size > MAX_CACHE) file->p = 0;
    else file->p = priority;
      
	//log_msg("fixing heap"); 
	heap_delete(priority_heap, file->heap_pos);
    heap_insert(priority_heap, file);
	/*
    char print[256];
    sprintf(print, "size:%d read/time:%d/%d write/time:%d/%d priority:%d", size, read, read_time, write, write_time, priority);
    log_msg(print);*/
}

int callback(struct file_stat* file, int num_col, char** results, char** col_names){
	if(results[0] == NULL){
		file->lastread = 0;
		file->lastwrite = 0;
		file->numreads = 0;
		file->numwrites = 0;
		insert_data(file);	
	}
	else{
		file->lastread = atoi(results[1]);
		file->lastwrite = atoi(results[2]);
		file->numreads = atoi(results[3]);
		file->numwrites = atoi(results[4]);
	}
	return 0;
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
		//log_msg("SQL error\n");
		//log_msg(err);
		sqlite3_free(err);
	} 
	sqlite3_close(db);
	return ret;
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
		//log_msg("SQL error\n");
		//log_msg(err);
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

	sprintf(sql, "insert into file_stats values ('%s', %d, %d, %d, %d);", 
		file->path, file->lastread, file->lastwrite, file->numreads, file->numwrites);

	if(ret){
		log_msg("Can't open database!\n");
		sqlite3_close(db);
		return -1;
	}

	//log_msg(sql);
	ret = sqlite3_exec(db, sql, callback, file, &err);

	if(ret != SQLITE_OK){
		//log_msg("SQL error\n");
		//log_msg(err);
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

