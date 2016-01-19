#include <stdlib.h>
#include <list.h>
#include <heap.h>

struct heap* heap_create(int max){
	struct heap* new_heap = malloc(sizeof(struct heap));
	
	if(!new_heap) return NULL;

	new_heap->elements = malloc(sizeof(struct file_stat*)*max);
	if(!new_heap->elements) return NULL;

	new_heap->n = 0;
	new_heap->max = max;

	return new_heap;
}

int heap_insert(struct heap* h, struct file_stat* ins){
	if(h->n == h->max){
		int i;
		h->max*=2;
		struct file_stat** new_e = malloc(sizeof(struct file_stat*)*h->max);
		if(!new_e) return -1;

		for(i = 0; i<h->n; i++) new_e[i] = h->elements[i];
		free(h->elements);
		h->elements = new_e;
	}

	int pos = h->n;
	h->elements[pos] = ins;
	h->n++;

	while(pos>0){
		int parent = (pos-1)/2;

		if(h->elements[parent]->p < h->elements[pos]->p){
			struct file_stat* temp = h->elements[parent];
			h->elements[parent] = h->elements[pos];
			h->elements[parent]->heap_pos = parent;
			h->elements[pos] = temp;
			h->elements[pos]->heap_pos = pos;
			pos = parent;
		}
		else break;
	}
	return 0;
}

struct file_stat* heap_delete(struct heap* h, int pos){
	if(pos >= h->n) return NULL;

	struct file_stat* ret = h->elements[pos];
	h->n--;
	h->elements[pos] = h->elements[h->n];
	struct file_stat** e = h->elements;

	while(pos<h->n){
		int left = pos*2 + 1;
		int right = pos*2 + 2;

		if(left >= h->n && right >= h->n) break;

		if(left < h->n && right >= h->n && e[left]->p < e[pos]->p){
			struct file_stat* temp = e[left];
			e[left] = e[pos];
			e[left]->heap_pos = left;
			e[pos] = temp;
			e[pos]->heap_pos = pos;
			break;
		}
		else{
			if(e[left]->p < e[right]->p && e[left]->p < e[pos]->p){
				struct file_stat* temp = e[left];
				e[left] = e[pos];
				e[left]->heap_pos = left;
				e[pos] = temp;
				e[pos]->heap_pos = pos;
				pos = left;
			}
			else if(e[right]->p < e[pos]->p)
			{
				struct file_stat* temp = e[right];
				e[right] = e[pos];
				e[right]->heap_pos = right;
				e[pos] = temp;
				e[pos]->heap_pos = pos;
				pos = right;
			}
			else break;	
		}
	}
	return ret;
}
