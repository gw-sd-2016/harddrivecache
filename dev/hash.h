#include "list.h"
#define MAX_LOAD_AVG 3

struct hashTable
{
    	int bucketn, size;
	struct linkedList** buckets;
};

struct hashTable* table_create();
int hashFunc(struct hashTable* table, char* path);
void table_add(struct hashTable **table, struct file_stat* nf);
struct file_stat* table_rem(struct hashTable *table, char* path);
struct file_stat* table_search(struct hashTable *table, char* path);
struct hashTable* table_resize(struct hashTable *table);
void table_print(struct hashTable *table);
void table_destroy(struct hashTable* table);
char** table_get_names(struct hashTable* table);
struct file_stat** table_get_file_stats(struct hashTable* table);
int update_file_stat(struct hashTable* table, char* oldpath, char* newpath);
//function to create a table
struct hashTable* table_create(int bucketn){
    struct hashTable* table = malloc(sizeof(struct hashTable));
    int i;
    table->bucketn = bucketn;
    table->size = 0;
    
    table->buckets = malloc(sizeof(struct linkedList*)*table->bucketn);
    for(i = 0; i<table->bucketn; i++)
    {
        table->buckets[i]=ll_create();
    }
    return table;
}
//hashes the entry 
int hashFunc(struct hashTable* table, char* path){
    char* s;
    int i, id=0;
    for(s = path; *s!='\0'; s++){
        id += *s;
    }
    return id%(table->bucketn);
}
//adds the file_stat to the table
void table_add(struct hashTable **table, struct file_stat* nf)
{
    int i = hashFunc(*table, nf->path);
    
    ll_add((*table)->buckets[i], nf);
    
    (*table)->size++;
    //increases table size if buckets are maxed out
    if((*table)->size/(*table)->bucketn > MAX_LOAD_AVG) *table = table_resize(*table);
}
//removes a file_stat from the table
struct file_stat* table_rem(struct hashTable *table, char* path)
{
    int i = hashFunc(table, path);
    
    struct linkedList* temp = ll_create();
    //creates new list with all but removed
    while(table->buckets[i]->head!=NULL && strcmp(table->buckets[i]->head->path, path)!=0){
        ll_add(temp, ll_remove_first(table->buckets[i]));
    }
    
    struct file_stat* ret = ll_remove_first(table->buckets[i]);
    
    while(temp->head!=NULL){
        ll_add(table->buckets[i], ll_remove_first(temp));
    }
    
    if(ret!=NULL) table->size--;
    free(temp);
    return ret;
}
//searches table and returns file_stat with name
struct file_stat* table_search(struct hashTable *table, char* path)
{
    int i = hashFunc(table, path);
    
    return ll_find(table->buckets[i], path);
}
//resizes table when it gets too big
struct hashTable* table_resize(struct hashTable *table){
    
    struct hashTable* newTable = table_create(table->bucketn*2);
    int i;
    
    for(i = 0; i<table->bucketn; i++){
        while(ll_length(table->buckets[i])>0){
            table_add(&newTable, ll_remove_first(table->buckets[i]));
        }
    }
    table_destroy(table);
    return newTable;
}
//prints out table
void table_print(struct hashTable *table){
    int i;
    for(i = 0; i<table->bucketn; i++)
    {
        printf("Bucket %d ", i);
        ll_print(table->buckets[i]);
    }
}
//destroys table freeing mem
void table_destroy(struct hashTable* table){
    int i;
    for(i = 0; i<table->bucketn; i++)
    {
        ll_destroy(table->buckets[i]);
    }
    free(table);
}
//gets all file_stats in a table.  Used for get attr
char** table_get_names(struct hashTable* table){
    int i, j=0;
    char** names = malloc(sizeof(char*)*table->size);
    
    for(i = 0; i<table->bucketn; i++)
    {
        struct file_stat* move = table->buckets[i]->head;
	    while(move != NULL)
	    {
		    names[j] = move->path;
		    j++;
		    move = move->next;
	    }
    }
    return names;
}
//update file
int update_file_stat(struct hashTable* table, char* oldpath, char* newpath){
    struct file_stat* temp = table_rem(table, oldpath);
   
    if(temp==NULL) return -1; 
    strcpy(temp->path, newpath);
    
    table_add(&table, temp);
    return 0;
}
