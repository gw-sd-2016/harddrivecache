#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define SMALL 16384
#define MED 1048576
#define LARGE 16777216
#define HUGE 1073741824 

#define N_THREADS 50
#define N_ACCESSES 100000
#define P_READS 100 

#define TEST_DIR "/home/timstamler/harddrivecache/fs"

int num_small, num_med, num_large, num_huge;
int total;

float p_small, p_med, p_large, p_huge;

int bytes_read, bytes_write;
double read_time, write_time;

int hits, misses, writes;

void get_numbers(){
	FILE* nfile = fopen("/home/timstamler/harddrivecache/bench/numbers.txt", "r");
	fscanf(nfile, "%f\t%d\n", &p_small, &num_small);
	fscanf(nfile, "%f\t%d\n", &p_med, &num_med);
	fscanf(nfile, "%f\t%d\n", &p_large, &num_large);
	fscanf(nfile, "%f\t%d\n", &p_huge, &num_huge);
	total = num_small + num_med + num_large + num_huge;
	fclose(nfile);
}

int gen_file(char* file){
	int size = rand()%total + 1;
	if(size <= num_small){
		int file_num = rand()%num_small;
		sprintf(file, "%s/small/small%d", TEST_DIR, file_num);
		return SMALL;
	}
	if(size > num_small && size <= (num_small+num_med)){
		int file_num = rand()%num_med;
		sprintf(file, "%s/med/med%d", TEST_DIR, file_num);
		return MED;
	}
	if(size > (num_small+num_med) && size <= (num_small+num_med+num_large)){
		int file_num = rand()%num_large;
		sprintf(file, "%s/large/large%d", TEST_DIR, file_num);
		return LARGE;
	}
	if(size > (num_small+num_med+num_large)){
		int file_num = rand()%num_huge;
		sprintf(file, "%s/huge/huge%d", TEST_DIR, file_num);
		return HUGE;
	}
}
void access_file() {
	int i;
	bytes_read = 0;
	bytes_write = 0;
	read_time = 0;
	write_time = 0;
	char buf[4096];
	for(i = 0; i<N_ACCESSES; i++){
		int rw = rand()%100 + 1;
		char file[256];
		int fsize = gen_file(file);
		//printf("got file %s\n", file);
		clock_t start, stop;
		if(rw <= P_READS){ //read
			start = clock();
			int fd = open(file, O_RDONLY);
			if(fd == -1){
				printf("%d couldn't open %s\n", i, file);
				perror("error ");
				break;
			}
			int totalRead = 0;
			int numRead = read(fd, buf, 4096);
			if(numRead == 1) hits++;
			if(numRead == 0) misses++;
			if(numRead == -1) printf("read error\n");
			//totalRead += numRead;
			//while(numRead > 0){
			//	numRead = read(fd, buf, 4096);
			//	totalRead += numRead;
			//}
			stop = clock();
			double time_e = (double)(stop-start);
			//printf("read %d of %d in %lf\n", totalRead, fsize, time_e);
			bytes_read+=fsize;	
			read_time +=time_e;
			close(fd);
		}
		else{ //write
			start = clock();
			int fd = open(file, O_WRONLY);
			if(fd == -1){
				printf("couldn't open\n");
				perror("error ");
			}
			int numWrite = fsize;
			//while(numWrite > 0){
				numWrite -= write(fd, buf, 4096);
			//}
			stop = clock();
			double time_e = (double)(stop-start);
			//printf("wrote %d in %lf\n", fsize, time_e);
			//bytes_write+=fsize;
			writes++;
			write_time+=time_e;
			close(fd);
		}
	}
}
int main(){
	srand(time(0));
	get_numbers();
	hits = 0;
	misses = 0;
	writes = 0;
	access_file();
	printf("hits: %d, misses: %d, writes: %d\n", hits, misses, writes);
	//printf("num read: %d in %lf\nnum write: %d in %lf\n", bytes_read, read_time/CLOCKS_PER_SEC, bytes_write, write_time/CLOCKS_PER_SEC);
	//double mbps_r = (bytes_read/read_time)*CLOCKS_PER_SEC/(1000000);
	//double mbps_w = (bytes_write/write_time)*CLOCKS_PER_SEC/1000000;
	//printf("Read MB/s: %lf\tWrite MB/s: %lf\n", mbps_r, mbps_w);
}
