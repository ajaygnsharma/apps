/*
 * list_test.c
 *
 *  Created on: Jan 21, 2013
 *      Author: sharmaj
 *      section 2.1
 */
#include "list.h"
#include "list_test.h"

#define EX_9 0
#define EX_13 0
#define EX_14 0
#define EX_2 1


int main(int argc, char **argv) {
#if EX_1
	List               list;
	ListElmt           *element;

	int                *data, i;

	/* Initialize the linked list. */
	list_init(&list, free);

	/* Perform some linked list operations. */
	element = list_head(&list);

	for (i = 10; i > 0; i--) {
		if ((data = (int *)malloc(sizeof(int))) == NULL)
			return 1;

		*data = i;
		if (list_ins_next(&list, NULL, data) != 0)
			return 1;
	}

	print_list(&list);
	element = list_head(&list);

	for (i = 0; i < 7; i++)
		element = list_next(element);

	data = list_data(element);
	fprintf(stdout, "Removing an element after the one containing %03d\n", *data);

	if (list_rem_next(&list, element, (void **)&data) != 0)
		return 1;

	print_list(&list);

	fprintf(stdout, "Inserting 011 at the tail of the list\n");

	*data = 11;
	if (list_ins_next(&list, list_tail(&list), data) != 0)
		return 1;

	print_list(&list);

	fprintf(stdout, "Removing an element after the first element\n");

	element = list_head(&list);
	if (list_rem_next(&list, element, (void **)&data) != 0)
		return 1;

	print_list(&list);

	fprintf(stdout, "Inserting 012 at the head of the list\n");

	*data = 12;
	if (list_ins_next(&list, NULL, data) != 0)
		return 1;

	print_list(&list);

	fprintf(stdout, "Iterating and removing the fourth element\n");

	element = list_head(&list);
	element = list_next(element);
	element = list_next(element);

	if (list_rem_next(&list, element, (void **)&data) != 0)
		return 1;

	print_list(&list);

	fprintf(stdout, "Inserting 013 after the first element\n");

	*data = 13;
	if (list_ins_next(&list, list_head(&list), data) != 0)
		return 1;

	print_list(&list);

	i = list_is_head(&list, list_head(&list));
	fprintf(stdout, "Testing list_is_head...Value=%d (1=OK)\n", i);
	i = list_is_head(&list, list_tail(&list));
	fprintf(stdout, "Testing list_is_head...Value=%d (0=OK)\n", i);
	i = list_is_tail(list_tail(&list));
	fprintf(stdout, "Testing list_is_tail...Value=%d (1=OK)\n", i);
	i = list_is_tail(list_head(&list));
	fprintf(stdout, "Testing list_is_tail...Value=%d (0=OK)\n", i);

	/* Destroy the linked list. */
	fprintf(stdout, "Destroying the list\n");
	list_destroy(&list);
#endif

#if EX_2
	length_test();
#endif

#if EX_3
	push_test();
#endif

#if EX_4
	push_test2();
#endif

#if EX_5
	push_test_dummy_node();
#endif

#if EX_6
	test_append_node();
#endif

#if(EX_7)
	test_push_append_node();
#endif

#if(EX_8)
	test_copy_list();
#endif

#if(EX_9)
	test_copy_list_recr();
#endif

#if(EX_10)
	test_copy_list_lref();
#endif

#if(EX_11)
	test_copy_list_push();
#endif

#if(EX_12)
	test_copy_list_dummy_node();
#endif

#if(EX_13)
	test_ring_buf_one_two_three();
#endif

#if(EX_14)
	test_ring_buf_append();
#endif

	return 0;
}


void test_push_append_node(void){
	struct node **headRef=NULL;
	append_node_w_push(&headRef, 5);
	append_node_w_push(&headRef, 4);
	append_node_w_push(&headRef, 3);
	append_node_w_push(&headRef, 2);

	printf("length=%d\n",length(headRef));
	print_list(headRef);
}

void test_copy_list(){
	struct node **headRef=NULL;
	append_node_w_push(&headRef, 5);
	append_node_w_push(&headRef, 4);
	append_node_w_push(&headRef, 3);
	append_node_w_push(&headRef, 2);

	printf("length=%d\n",length(headRef));
	print_list(headRef);

	struct node *new_ref =NULL;
	new_ref = copy_list(headRef);

	printf("new length=%d\n",length(new_ref));
	print_list(new_ref);
}

void test_copy_list_dummy_node(){
	struct node **headRef=NULL;
	append_node_w_push(&headRef, 5678);
	append_node_w_push(&headRef, 4567);
	append_node_w_push(&headRef, 3456);
	append_node_w_push(&headRef, 2345);

	printf("length=%d\n",length(headRef));
	print_list(headRef);

	struct node *new_ref =NULL;
	new_ref = copy_list_dummy_node(headRef);

	printf("new length=%d\n",length(new_ref));
	print_list(new_ref);
}

void test_copy_list_push(){
	struct node **headRef=NULL;
	append_node_w_push(&headRef, 345);
	append_node_w_push(&headRef, 234);
	append_node_w_push(&headRef, 123);
	append_node_w_push(&headRef, 12);

	printf("length=%d\n",length(headRef));
	print_list(headRef);

	struct node *new_ref =NULL;
	new_ref = copy_list_push(headRef);

	printf("new length=%d\n",length(new_ref));
	print_list(new_ref);
}

void test_copy_list_lref(){
	struct node **headRef=NULL;
	append_node_w_push(&headRef, 54);
	append_node_w_push(&headRef, 43);
	append_node_w_push(&headRef, 32);
	append_node_w_push(&headRef, 21);

	printf("length=%d\n",length(headRef));
	print_list(headRef);

	struct node *new_ref =NULL;
	new_ref = copy_list_lref(headRef);

	printf("new length=%d\n",length(new_ref));
	print_list(new_ref);
}

void test_copy_list_recr(){
	struct node **headRef=NULL;
	append_node_w_push(&headRef, 500);
	append_node_w_push(&headRef, 400);
	append_node_w_push(&headRef, 300);
	append_node_w_push(&headRef, 200);

	printf("length=%d\n",length(headRef));
	print_list(headRef);

	struct node *new_ref =NULL;
	new_ref = copy_list_recr(headRef);

	printf("new length=%d\n",length(new_ref));
	print_list(new_ref);
}

void test_ring_buf_one_two_three(){
	struct node *headRef = NULL;
	headRef = ring_buf_build_one_two_three();
	print_ring_buf(headRef);
}

void test_ring_buf_append(){
	struct node **headRef = NULL;

	ring_buf_append_node(headRef, 1);
	ring_buf_append_node(headRef, 2);
	ring_buf_append_node(headRef, 3);

	print_ring_buf(*headRef);
}




#if 0
int                *data, i;

/* Initialize the linked list. */
list_init(&list, free);

/* Perform some linked list operations. */
element = list_head(&list);

for (i = 10; i > 0; i--) {
	if ((data = (int *)malloc(sizeof(int))) == NULL)
		return 1;

	*data = i;
	if (list_ins_next(&list, NULL, data) != 0)
		return 1;
}
#endif









#if EX_1
static void print_list(const List *list) {
	ListElmt           *element;
	int                *data, i;

	/* Display the linked list. */
	fprintf(stdout, "List size is %d\n", list_size(list));

	i = 0;
	element = list_head(list);

	while (1) {
		data = list_data(element);
		fprintf(stdout, "list[%03d]=%03d\n", i, *data);

		i++;

		if (list_is_tail(element))
			break;
		else
			element = list_next(element);
	}

	return;
}
#endif

