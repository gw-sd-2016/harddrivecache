#include <stdio.h>
#include <stdlib.h>

#define SMALL 4096
#define MED 1048576
#define LARGE 16777216
#define HUGE 1073741824 

#define MAX_SIZE 1073741824 

//should add up to 1
#define N_SMALL 0.6
#define N_MED 0.3
#define N_LARGE 0.1
#define N_HUGE 0.0

int num_small = 0;
int num_med = 0;
int num_large = 0;
int num_huge = 0;

void create_file(char* name, int size){

	FILE* new_file = fopen(name, "w");
	char buf[4096];
	int rem = size;

	while(rem > 0){
		fwrite(buf, 4096, 1, new_file);
		rem -= 4096;
	}
	fclose(new_file);
	printf("created file %s of size %d\n", name, size);
}

int main(){

	num_small = (N_SMALL * MAX_SIZE) / SMALL;
	num_med = (N_MED * MAX_SIZE) / MED;
	num_large = (N_LARGE * MAX_SIZE) / LARGE;
	num_huge = (N_HUGE * MAX_SIZE) / HUGE;
	
	int i;

	FILE* nfile = fopen("./numbers.txt", "w");

	printf("creating %d small files\n", num_small);
	fprintf(nfile, "%f\t%d\n", N_SMALL, num_small);
	for(i = 0; i< num_small; i++){
		char path[256];
		sprintf(path, "/home/timstamler/harddrivecache/mnt/hd/small/small%d", i);
		create_file(path, SMALL);
	}
	printf("creating %d med files\n", num_med);
	fprintf(nfile, "%f\t%d\n", N_MED, num_med);
	for(i = 0; i< num_med; i++){
		char path[256];
		sprintf(path, "/home/timstamler/harddrivecache/mnt/hd/med/med%d", i);
		create_file(path, MED);
	}
	printf("creating %d large files\n", num_large);
	fprintf(nfile, "%f\t%d\n", N_LARGE, num_large);
	for(i = 0; i< num_large; i++){
		char path[256];
		sprintf(path, "/home/timstamler/harddrivecache/mnt/hd/large/large%d", i);
		create_file(path, LARGE);
	}
	printf("creating %d huge files\n", num_huge);
	fprintf(nfile, "%f\t%d\n", N_HUGE, num_huge);
	for(i = 0; i< num_huge; i++){
		char path[256];
		sprintf(path, "/home/timstamler/harddrivecache/mnt/hd/huge/huge%d", i);
		create_file(path, HUGE);
	}
	fclose(nfile);
}
