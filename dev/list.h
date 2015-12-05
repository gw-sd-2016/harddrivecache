
#include <stdlib.h>
#include <stdio.h>
//struct linked list to hold data about file_stats
struct linkedList{
    struct file_stat* head;
};

struct file_stat{
    char path[100];
    
    int lastread, lastwrite;
    int numreads, numwrites;
    
    struct file_stat* next;
};

struct linkedList* ll_create(void);
struct file_stat* file_stat_create(char* path, int lr, int lw, int nr, int nw);
void ll_destroy(struct linkedList *ll);
void ll_add(struct linkedList *ll, struct file_stat *value);
int ll_length(struct linkedList *ll);
struct file_stat* ll_remove_first(struct linkedList *ll);
struct file_stat* ll_find(struct linkedList *ll, char* path);
void ll_print(struct linkedList *ll);

struct linkedList* ll_create(void){
    struct linkedList* newList = malloc(sizeof(struct linkedList*));
    newList->head = NULL;
    return newList;
}

struct file_stat* file_stat_create(char* path, int lr, int lw, int nr, int nw){
    struct file_stat* newfile = malloc(sizeof(struct file_stat));
    
    strcpy(newfile->path, path);
    newfile->lastread = lr;
    newfile->lastwrite = lw;
    newfile->numreads = nr;
    newfile->numwrites = nw;
    newfile->next = NULL;
    
    return newfile;
}
//destroys a linked list and frees all the memory
void ll_destroy(struct linkedList *ll){
    while(ll_length(ll)>0){
        ll_remove_first(ll);
    }
    free(ll);
}
//adds a file_stat to the linked list.
void ll_add(struct linkedList *ll, struct file_stat* nf){

    if(ll->head==NULL){
        ll->head = nf;
    } else {
        nf->next = ll->head;
        ll->head = nf;
    }
}
//returns the length of a ll.  Iterates through and counts.
int ll_length(struct linkedList *ll){
    if(ll->head==NULL) return 0;
    else{
        struct file_stat* move;
        move = ll->head;
        int count = 1;
        
        while(move->next!=NULL){
            move = move->next;
            count++;
        }
        return count;
    }
}
//removes first file_stat from the linked list
struct file_stat* ll_remove_first(struct linkedList *ll){
    
    if(ll->head==NULL) return NULL;
    else{
        struct file_stat* first = ll->head;
        ll->head = first->next;
        first->next = NULL;
        return first;
    }
}
//finds a file_stat in linked list.  Searches by name attribute
struct file_stat* ll_find(struct linkedList *ll, char* path){
    if(ll->head==NULL) return NULL;
    else{
        struct file_stat* move;
        move = ll->head;
        
        while(move!=NULL){
            if(strcmp(path, move->path)==0) return move;
	    move = move->next;
        }
        return NULL;
    }
}
//prints out linked list by iterating through nodes.
void ll_print(struct linkedList *ll)
{
	struct file_stat* move = ll->head;
	printf("List Contents:\n");
	while(move != NULL)
	{
		printf("Path: %s: %d, %d, %d, %d, %d, %d\n", move->path, 
		    move->lastread, move->lastwrite, move->numreads, move->numwrites);
		move = move->next;
	}
}
