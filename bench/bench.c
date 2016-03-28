#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

#define SMALL 4096
#define MED 1048576
#define LARGE 16777216
#define HUGE 1073741824 

#define N_THREADS 50
#define N_ACCESSES 100
#define P_READS 95

#define TEST_DIR "/home/timstamler/harddrivecache/mnt/ssd"

int num_small, num_med, num_large, num_huge;
float p_small, p_med, p_large, p_huge;

int bytes_read, bytes_write;
double read_time, write_time;

void get_numbers(){
	FILE* nfile = fopen("./numbers.txt", "r");
	fscanf(nfile, "%f\t%d\n", &p_small, &num_small);
	fscanf(nfile, "%f\t%d\n", &p_med, &num_med);
	fscanf(nfile, "%f\t%d\n", &p_large, &num_large);
	fscanf(nfile, "%f\t%d\n", &p_huge, &num_huge);
	fclose(nfile);
}

int gen_file(char* file){
	int size = rand()%10 + 1;
	if(size <= p_small*10){
		int file_num = rand()%num_small;
		sprintf(file, "%s/small/small%d", TEST_DIR, file_num);
		return SMALL;
	}
	if(size > p_small*10 && size <= (p_small+p_med)*10){
		int file_num = rand()%num_med;
		sprintf(file, "%s/med/med%d", TEST_DIR, file_num);
		return MED;
	}
	if(size > (p_small+p_med)*10 && size <= (p_small+p_med+p_large)*10){
		int file_num = rand()%num_large;
		sprintf(file, "%s/large/large%d", TEST_DIR, file_num);
		return LARGE;
	}
	if(size > (p_small+p_med+p_large)*10){
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
		printf("got file %s\n", file);
		clock_t start, stop;
		if(rw < P_READS){ //read
			int fd = open(file, O_RDONLY);
			if(fd == -1) printf("couldn't open\n");
			int totalRead = 0;
			start = clock();
			int numRead = read(fd, buf, 4096);
			totalRead += numRead;
			while(numRead > 0){
				numRead = read(fd, buf, 4096);
				totalRead += numRead;
			}
			stop = clock();
			double time_e = (double)(stop-start);
			printf("read %d of %d in %lf\n", totalRead, fsize, time_e);
			bytes_read+=fsize;	
			read_time +=time_e;
			close(fd);
		}
		else{ //write
			int fd = open(file, O_WRONLY);
			if(fd == -1) printf("couldn't open\n");
			start = clock();
			int numWrite = fsize;
			while(numWrite > 0){
				numWrite -= write(fd, buf, 4096);
			}
			stop = clock();
			double time_e = (double)(stop-start);
			printf("wrote %d in %lf\n", fsize, time_e);
			bytes_write+=fsize;
			write_time+=time_e;
			close(fd);
		}
	}
}
int main(){
	srand(time(0));
	get_numbers();
	access_file();
	printf("num read: %d in %lf\nnum write: %d in %lf\n", bytes_read, read_time/CLOCKS_PER_SEC, bytes_write, write_time/CLOCKS_PER_SEC);
}
