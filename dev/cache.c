#include <stdio.h>
#include <unistd.h>

#include <cache.h>
#include <disk_info.h>
#include <log.h>

int cache_exists(const char* path){
	char * new_path = get_ssd_path(path);
	int ret;
	if( access( new_path, R_OK ) != -1 ) ret = 1;
	else ret = 0;
	free(new_path);
	return ret;
}
int cache_add(const char* path){
	char* hd_path = get_hd_path(path);
	char* ssd_path = get_ssd_path(path);
	char ch;

	log_msg(hd_path);
	log_msg(ssd_path);
	
	FILE* hd_file, *ssd_file;
	hd_file = fopen(hd_path, "r");
	if(hd_file == NULL) return -1;
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
	//TODO
}
