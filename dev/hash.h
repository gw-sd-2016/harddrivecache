#ifndef HASH_H_
#define HASH_H_

#include <list.h>
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
int update_file_stat(struct hashTable* table, char* oldpath, char* newpath);
#endif
