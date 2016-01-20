#include <stdlib.h>
#include <list.h>

struct heap{
	struct file_stat** elements;
	int n, max;
};

struct heap* heap_create(int max);
int heap_insert(struct heap* h, struct file_stat* ins);
struct file_stat* heap_delete(struct heap* h, int pos);

