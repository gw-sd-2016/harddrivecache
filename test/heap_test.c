#include <stdio.h>
#include <stdlib.h>
#include <heap.h>
#include <list.h>

int main(){

	struct heap* test_heap = heap_create(2);

	struct file_stat* test1 = file_stat_create("a",0,0,0,0,0,1);
	struct file_stat* test2 = file_stat_create("b",0,0,0,0,0,2);
	struct file_stat* test3 = file_stat_create("c",0,0,0,0,0,3);
	struct file_stat* test4 = file_stat_create("d",0,0,0,0,0,4);
	struct file_stat* test5 = file_stat_create("e",0,0,0,0,0,5);
	struct file_stat* test6 = file_stat_create("f",0,0,0,0,0,6);
	struct file_stat* test7 = file_stat_create("g",0,0,0,0,0,7);

	printf("files created\n");

	heap_insert(test_heap, test1);
	heap_insert(test_heap, test7);
	heap_insert(test_heap, test6);
	heap_print(test_heap);
	heap_insert(test_heap, test2);
	heap_insert(test_heap, test3);
	heap_insert(test_heap, test5);
	heap_insert(test_heap, test4);
	heap_print(test_heap);
	printf("delete 1: %d\n", heap_delete(test_heap, 2)->p);
	heap_print(test_heap);
	printf("delete 2: %d\n", heap_delete(test_heap, 2)->p);
	heap_print(test_heap);
	printf("delete 3: %d\n", heap_delete(test_heap, 0)->p);
	heap_print(test_heap);
}

void heap_print(struct heap* h){
	int i;
	printf("[ ");
	for(i = 0; i<h->n; i++){
		printf("%d ", h->elements[i]->p);
	}
	for(i = h->n; i<h->max; i++){
		printf("x ");
	}
	printf("]\n");

}
