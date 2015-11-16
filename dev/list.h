#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct linked_list{
	struct list_node* head;
	struct list_node* tail;
};

struct list_node{
	void *value;
	struct list_node* next;
	struct list_node* prev;
};

struct linked_list* ll_create(void);
int ll_length(struct linked_list *ll);
int ll_contains(struct linked_list *ll, void *value);
void* ll_remove_first(struct linked_list *ll);
void ll_destroy(struct linked_list *ll);
void ll_add(struct linked_list *ll, void *value);
void print(struct linked_list *ll);

struct linked_list* ll_create(void){
	struct linked_list* ll = malloc(sizeof(struct linked_list));//creates a linked list
	ll->head = NULL;//sets the value of the head to null
	ll->tail = NULL;
	return ll;
}

void ll_remove(struct linked_list *ll, struct list_node* rem){
     struct list_node* temp = ll->head;
     while(temp != rem){
         if(temp == ll->tail) return;
         temp = temp->next;
     }

     if(ll->head == rem && ll->tail == rem){
         ll->head = NULL;
         ll->tail = NULL;
         return;
     }

     if(ll->head == rem) ll->head = rem->next;
     if(ll->tail == rem) ll->tail = rem->prev;
     
     rem->prev->next = rem->next;
     rem->next->prev = rem->prev;
}


void ll_add(struct linked_list *ll, void *value){
	struct list_node* newnode = malloc(sizeof(struct list_node));//creates a temp node
	newnode->next = ll->head;
	newnode->prev = ll->tail;
	if (ll->head != NULL){//if there is a  node in the current list, it will set the next node to be the current first node
		ll->head->prev = newnode;//sets the old first node's prev to the new node
		ll->tail->next = newnode;//sets the tails next to the new node
	}else{
		ll->head = newnode;//sets the head to the new node
	}
	ll->tail = newnode;//sets the new node to the free node
	newnode->value = value;//sets the value of the new node to value
}

/*
* this will remove the first node (only if empty though)
 */
void* ll_remove_first(struct linked_list *ll){
	if(ll->head==NULL) return NULL;
	else{
		void* value = ll->head->value;

		if(ll->head==ll->tail){
			free(ll->head);
			ll->head = NULL;
			ll->tail = NULL;
		}
		else
		{
			free(ll->head);
			ll->head = ll->head->next;
			ll->tail->next = ll->head;
			ll->head->prev = ll->tail;
		}
		return value;
	}

}
/*
* destroys the list by removing all the first nodes
*/
void ll_destroy(struct linked_list *ll){
	while(!ll_remove_first(ll));{
		ll_remove_first(ll);
	}
	free(ll);
}

/*
* determines if the list contains a specific value
*/
int ll_contains(struct linked_list *ll, void *value){
	struct list_node* search = malloc(sizeof(struct list_node));
	search = ll->head;//sets search to the head of the list
	int counter = 1;

	if(ll->head == NULL){//this means the list is empty
		printf("This list is empty\n");
		return 0;
	}
	while(search->value != value && search->next != ll->head){//this traverses the list looking for the value
		counter++;
		search = search->next;
	}
	if(search->value != value) return 0;
	return counter;

}

/*
* determines the length of the list
*/
int ll_length(struct linked_list *ll){
	struct list_node* search = malloc(sizeof(struct list_node));
	search = ll->head;//sets this node to the first one
	int counter = 1;

	if(ll->head == NULL){//this means the list is empty
		printf("This list is empty\n");
		return 0;
	}
	while(search->next != ll->head){//goes through the whole list and increments a counter
		counter++;
		search = search->next;
	}
	return counter;
}

/*
* prints the list
*/
void ll_print(struct linked_list *ll){
	struct list_node* search = ll->head;
	printf("List: \n");
	while(search != NULL){
        	printf("%d\n", search->value);
        	if(search == ll->tail) break;
        	search = search->next;
     	}
	printf("END\n");

}

