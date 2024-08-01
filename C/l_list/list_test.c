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
#define EX_14 1

/*****************************************************************************
*  --------------------------------- main ---------------------------------  *
*****************************************************************************/
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


void length_test(){
	struct node *my_list = build_one_two_three();
	int len = length(my_list);
	printf("length=%d\n",len);
}

/* section 2.1 */
void push_test(){
  //This works since head is init to NULL. Legnth comes as 0
  struct node *head=NULL;
#if(NULL_LIST_LENGTH_TEST)
	//Not initiliazing the head to NULL gives segmentation fault.
	//struct node *head;
#endif
  /* test length of an empty string */
  printf("length=%d\n",length(head));
  head = build_one_two_three();

  int *data;
  data = malloc(sizeof(int));
  *data = 10;
  push(&head, (void *)10);
  *data = 13;
  push(&head, (void *)13);

  printf("length=%d\n",length(head));
  print_list(head);
}

void push_test_dummy_node(void){
  struct node *head=NULL;
  /* test length of an empty string */
  printf("length=%d\n",length(head));
  head = build_with_dummy_node();

  printf("length=%d\n",length(head));
  print_list(head);
}

void test_append_node(void){
	struct node **headRef=NULL;
	AppendNode(&headRef, 5);
	AppendNode(&headRef, 4);
	AppendNode(&headRef, 3);
	AppendNode(&headRef, 2);

	printf("length=%d\n",length(headRef));
	print_list(headRef);
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

/** Section 2.2
 * push_test2
 * push with empty head node */
void push_test2(void){
	struct node *head = NULL; //make an empty list. Not having NULL, segmentation fault

	push(&head, (void *)1);
	push(&head, (void *)2);
	push(&head, (void *)3);

	printf("length=%d\n",length(head));
	print_list(head);
}

/** section 2.4
 * change_caller()
 * make list head ptr to NULL */
void change_caller(){
	struct node *head1;
	struct node *head2;

	change_to_null(&head1);
	change_to_null(&head2);
	//head1 and head2 are NULL at this point
}

/** section 2.5
 * add_at_head()
 * Push as in pushing in a stack. 1,2,3 pushed give 3,2,1. */
struct node *add_at_head(){
	struct node *head = NULL;
	int i;

	for(i=1;i<6;i++){
		push(&head,(void *)i);
	}
	return (head);
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


/**
 * build_with_special_case()
 * build list naturally. 1,2,3... gives or builds 1,2,3 in the list */
struct node *build_with_special_case(){
  struct node *head = NULL;
  struct node *tail;
  int i;

  /* first push needs to be on head ptr */
  push(&head,(void *)1);
  tail=head;

  /* start with 2 since we are pushing in 1 first then 2....*/
  for(i=2; i<6;i++){
	  push(&(tail->nxt),(void *)i);
	  tail=tail->nxt;
  }

  return head;
}


struct node *build_with_dummy_node(void){
  struct node dummy;
  struct node *tail = &dummy;

  int i;

  dummy.nxt = NULL;

  for(i=1;i<6;i++){
	  push(&(tail->nxt),(void *)i);
	  tail=tail->nxt;
  }

  return(dummy.nxt);
}

struct node *build_with_local_ref(void){
  struct node *head = NULL;
  struct node **last_ptr_ref= &head;
  int i;

  for(i=1;i<6;i++){
	  push(last_ptr_ref, i);
	  last_ptr_ref= &((*last_ptr_ref)->nxt);
  }

  return head;
}



/*****************************************************************************
*  ------------------------------ print_list ------------------------------  *
*****************************************************************************/
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

