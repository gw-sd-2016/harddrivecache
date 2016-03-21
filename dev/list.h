#ifndef LIST_H_
#define LIST_H_
#include <log.h>
#include <stdlib.h>
#include <stdio.h>
//struct linked list to hold data about file_stats
struct linkedList{
    struct file_stat* head;
};

struct file_stat{
    char path[256];
    
    int freq_hist[5];
	int lastread, lastwrite;
    int numreads, numwrites;
    int size, p;
    int heap_pos;
        
    struct file_stat* next;
};

struct linkedList* ll_create(void);
struct file_stat* file_stat_create(char* path, int lr, int lw, int nr, int nw, int size);
void ll_destroy(struct linkedList *ll);
void ll_add(struct linkedList *ll, struct file_stat *value);
int ll_length(struct linkedList *ll);
struct file_stat* ll_remove_first(struct linkedList *ll);
struct file_stat* ll_find(struct linkedList *ll, char* path);
#endif
