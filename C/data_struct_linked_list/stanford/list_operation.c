#include "list_operation.h"

/**
 * Page 6
 * Exercise.
 * TODO: Section 1.2 in 7 steps build_one_two_three instead of 9.
 */
void print_list(node_t *head){
  node_t *cur = head;
  //int *p;

  while(cur!=NULL){
	  //p = (int *)&cur->data;
	  //printf("%d\n",*p);
	  printf("%d ",cur->data);
	  cur=cur->link;
  }
  printf("\n");
}

/**
 * Page 7
 * section 1.3, 1.4, 1.5, 2.3. <--Not sure how these come here.
 * Example to return the number of nodes in a list.
 */
int length_list(node_t *head){
	node_t *cur = head;
	int cnt = 0;

	while(cur!=NULL){
		cnt++;
		cur = cur->link;
	}
    /* Setting head to NULL will not make the caller's head to NULL
	since this head is not a reference pointer. It is pointing to
	a copy of the head. */
#if(HEAD_NULL_TEST)
	head=NULL;
#endif
	return cnt;
}

int length_list_using_for_loop(node_t *head){
	node_t *cur;
	int i = 0;

	for(cur = head; cur != NULL; cur = cur->link)
		i++;

	return i;
}

/* Search for an element and swap it with the next element
 * h - head of the list
 * f - Matching node with this data to be searched
 * return - head of the list with elements swapped */
node_t *swap_2(node_t *h, int f){
	node_t *c = h;
	node_t *n = c->link;
	node_t *p = h;

	while(c->data != f){
	  p = c;
	  c = c->link;
	  n = c->link;
	}

	if(n == NULL){ // next element is NULL
	  // since last node, no swapping after that return the same list
	  return h;
	}

	if(p->data == c->data){
	  h = n; // Change head only if swapping the  first element.
	}else{
	  p->link = n;
	}
	c->link = n->link;
	n->link = c;

	return h;
}

static int raise_power_10(int n){
	int i;
	int power = 1;
	for(i=0;i<n;i++){
		power*=10;
	}
	return power;
}

/* This function adds two nodes and returns the sum of
 * their values in a new linked list.
 */
node_t *sum_nodes(node_t *h1, node_t *h2){
	node_t *t1 = h1;
	node_t *t2 = h2;
	int l1 = 0, l2 = 0;
	int sum = 0;

	while(t1->link != NULL){
		t1 = t1->link;
		l1++;
	}

	while(t2->link != NULL){
		t2 = t2->link;
		l2++;
	}


	t2 = h2; t1 = h1;
	int i = 0;

	if(l2 > l1){
		for(i = l2; i > 0; i--){
			t2->data = ((t2->data + t1->data) * raise_power_10(i));
			t2 = t2->link;
			t1 = t1->link;
		}
		return h2;
	}else{
		for(i = l1; i > 0; i--){
			if(i>l2)
				t1->data = ((t1->data + 0) *\
						raise_power_10(i));
			else{
				t1->data = ((t1->data + t2->data) *\
									raise_power_10(i));
				t2 = t2->link;
			}
			sum += t1->data;
			t1 = t1->link;
		}
		sum += (int)t1->data;
		printf("sum=%d\n",sum);
		return h1;
	}
}


node_t *odd_list(node_t *h){
	node_t *odd = NULL, *c = h, *odd_h = NULL;

	while(c != NULL){
		if((int)c->data % 2){
			if(odd==NULL){
				odd = c;
				odd_h = odd;
			}else{
				odd->link = c;
				odd = odd->link;
			}
		}
		c = c->link;
	}
	odd->link = NULL;

	return odd_h;
}

/**
 * Does a reverse using a link adjustment technique.
 * Interestingly, this is very fast since iterations are not used.
 * Takes a list, reverses it and returns the new one.
 */
node_t *reverse_using_link_adjust(node_t *h){
	node_t *c = h;
	node_t *n = c;
	node_t *p = NULL;

	while(c!=NULL){
		n = c->link;
		c->link = p;
		p = c;
		c = n;
		if( p == NULL ){
			p->link = NULL;
		}
	}
	h = p;
	return h;
}
