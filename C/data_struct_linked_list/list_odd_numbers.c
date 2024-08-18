/**
 * Was a question asked in Fitbit interview
 * Input list 1->2->3->4->5
 * Output list 1->3->5 and 2->4
 *
 */
#include <stdio.h>
#include <stdlib.h>

struct list{
	int d;
	struct list *next;
};

typedef struct list list_t;

list_t *push_t(list_t *h, list_t *n){
	list_t *t = h;

	while(t != NULL){
		t = t->next;
	}
	t->next = n;
	n->next = NULL;

	return h;
}

int print(list_t *h){
	while(h != NULL){
		printf("%d\n",h->d);
		h = h->next;
	}
	return 0;
}

int main(){
	int i;
	list_t *h;

	for(i = 0; i < 5; i++){
		list_t *n = malloc(sizeof(list_t));
		n->d = i;
		n->next = NULL;
		h = push_t(h, n);
	}

	print(h);
	return 0;
}
