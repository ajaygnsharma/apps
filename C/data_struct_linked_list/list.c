/*
 * list.c
 *
 *  Don't work on this. Its very confusing. Use the new repository that
 *  I created for Data structures and algorithm separately.
 *
 *  Created on: Nov 8, 2012
 *  Author: user1
 *
 *  This is a series of functions that do linked list operations.
 *  These are taken from the stanford website:
 *  http://cslibrary.stanford.edu/103/LinkedListBasics.pdf
 *  The sections that are covered here are:
 *  1.1,1.2,1.3,1.4,1.5. 2.1, 2.3, 2.4
 *
 */
/** list.c**/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "list.h"


/**
 * Page 6
 * Section 1.1:
 * I converted the names from XsomeThing to x_some_thing.
 * build_one_two_three()
 * build a link list with 3 elements in the heap and return the head ptr
 * */
struct node *build_one_two_three(){
  struct node *head = NULL;
  struct node *second = NULL;
  struct node *third = NULL;

  head = malloc(sizeof(struct node));
  second = malloc(sizeof(struct node));
  third = malloc(sizeof(struct node));

  head->data = (int *)1;
  head->nxt = second;

  second->data = (int *)2;
  second->nxt = third;

  third->data = (int *)3;
  third->nxt = NULL;

  return head;
}

/**
 * Page 6
 * Exercise.
 * TODO: Section 1.2 in 7 steps build_one_two_three instead of 9.
 */
void print_list(struct node *head){
  struct node *cur = head;
  //int *p;

  while(cur!=NULL){
	  //p = (int *)&cur->data;
	  //printf("%d\n",*p);
	  printf("%d\n",(int)cur->data);
	  cur=cur->nxt;
  }
}


/**
 * Page 7
 * section 1.3, 1.4, 1.5, 2.3. <--Not sure how these come here.
 * Example to return the number of nodes in a list.
 */
int length(struct node *head){
	struct node *cur = head;
	int cnt = 0;

	while(cur!=NULL){
		cnt++;
		cur = cur->nxt;
	}
    /* Setting head to NULL will not make the caller's head to NULL
	since this head is not a reference pointer. It is pointing to
	a copy of the head. */
#if(HEAD_NULL_TEST)
	head=NULL;
#endif
	return cnt;
}

/**
 * Page 8.
 * A function to test length() above.
 */
void length_test(){
	struct node *my_list = build_one_two_three();
	int len = length(my_list);
	printf("length=%d\n",len);
}

/**
 * Page 11
 * LinkTest() is not done here.
 *
 * Page 12
 * WrongPush(), WrongPushTest() is also not done here.
 */

/**
 * Page 14
 * This function does the correct push and below tests it.
 * section 2.1 */
void push(struct node **head_ref, void *data){
  struct node *new_node = malloc(sizeof(struct node));

  new_node->data = (void *)data;
  new_node->nxt = *head_ref;
  *head_ref = new_node;
}

/**
 * Page 14
 * push test. Its not the same as the function in the article and I have
 * added void pointers to try something here.
 * section 2.1 */
void push_test1(){
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

/**
 * Page 14. Section 2.2
 * push_test2
 * push with empty head node. Now this is the same as the text book */
void push_test2(void){
	struct node *head = NULL; //make an empty list. Not having NULL, segmentation fault

	push(&head, (void *)1);
	push(&head, (void *)2);
	push(&head, (void *)3);

	printf("length=%d\n",length(head));
	print_list(head);
}

/**
 * Page 17
 * Length function in a slightly different way.
 * This makes it easy to ensure that cur is properly initialized, tested for
 * end and moves forward. Lesser code.
 */
int length_for_loop(struct node *head){
	struct node *cur;
	int i = 0;

	for(cur = head; cur != NULL; cur = cur->nxt)
		i++;

	return i;
}


/**
 * Page 17, 18
 * section 2.4
 * Using a reference pointer(pointer to a pointer) to change the caller's head
 * pointer. Here its initialized to NULL.
 * */
void change_to_null(struct node **head_ref){
	*head_ref=NULL;
}

/**
 * Page 18
 * section 2.4
 * change_caller()
 * make list head ptr to NULL
 * */
void change_caller(){
	struct node *head1;
	struct node *head2;

	change_to_null(&head1);
	change_to_null(&head2);
	//head1 and head2 are NULL at this point
}

/**
 * Page 18.
 * add_at_head()
 *
 * Very easy way to build a list. by adding elements to the head.
 * This is kind of like building a stack. Push in data. Printing it will
 * result in reverse order. Push as in pushing in a stack.
 * 1,2,3 pushed give 3,2,1.
 * section 2.5
 * TODO: Memory drawing.
 * */
struct node *add_at_head(){
	struct node *head = NULL;
	int i;

	for(i=1;i<6;i++){
		push(&head,(void *)i);
	}
	return (head);
}

/**
 * Page 19:
 * build_with_special_case().
 * build list naturally. 1,2,3... gives or builds 1,2,3 in the list
 * First node is created normally and after that others are added to the
 * tail pointer.
 * TODO: Memory drawing.
 * */
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


/**
 * Page 20:
 * Instead of pushing a head node first using push and then using tail.
 * This method uses a dummy node which is allocated on the stack.
 * Everything is added to it as a tail.
 * TODO: Memory drawing.
 */
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

/**
 * This is MY unit test function to the test the build dummy code.
 */
void push_test_dummy_node(void){
	struct node *head=NULL;
	/* test length of an empty string */
	printf("length=%d\n",length(head));
	head = build_with_dummy_node();

	printf("length=%d\n",length(head));
	print_list(head);
}


/**
 * Page 20: This time no dummy node or extra node is created.
 * Using a special reference pointer.
 * TODO: Memory drawing.
 */
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
// DATE: 09/18/2014.








/**
 * Page 22: Similar to Push . Just that it adds to the tail pointer
 * rather than the head.
 * section 4.1 ordinary push but to tail.
 * This has cur->nxt = newNode. The other one had head = newNode.
 * So here the list is built normally.
 * TODO: Memory drawing.
 * */
struct node* AppendNode(struct node** headRef, int num) {
   struct node* current = *headRef;
   struct node* newNode;

   newNode = malloc(sizeof(struct node));
   newNode->data = (void *)num;
   newNode->nxt = NULL;

   // special case for length 0
   if (current == NULL) {
      *headRef = newNode;
   }else{
      // Locate the last node
      while (current->nxt != NULL) {
          current = current->nxt;
      }
      current->nxt = newNode;
   }
   return *headRef;
}

/**
 * my unit test function to test the normally linked list function.
 */
void test_append_node(void){
	struct node **headRef=NULL;
	AppendNode(&headRef, 5);
	AppendNode(&headRef, 4);
	AppendNode(&headRef, 3);
	AppendNode(&headRef, 2);

	printf("length=%d\n",length(headRef));
	print_list(headRef);
}


/* Page 22:
 * section 4.2 Append node with Push. Pushes to the tail of current(nxt).
 * Builds a  */
struct node *append_node_w_push(struct node **headRef, int num) {
	struct node *current = *headRef;
	// special case for the empty list
	if (current == NULL) {
		push(headRef, num);
	} else {
		// Locate the last node
		while (current->nxt != NULL) {
			current = current->nxt;
		}
		// Build the node after the last node
		push(&(current->nxt), num);
	}
	return current;
}

struct node* copy_list(struct node* head) {
	struct node* current = head;      // used to iterate over the original list
	struct node* newList = NULL;      // head of the new list
	struct node* tail = NULL; // kept pointing to the last node in the new list
	while (current != NULL) {
		if (newList == NULL) { // special case for the first new node
			newList = malloc(sizeof(struct node));
			newList->data = current->data;
			newList->nxt = NULL;
			tail = newList;
		}else{
			tail->nxt = malloc(sizeof(struct node));
			tail = tail->nxt;
			tail->data = current->data;
			tail->nxt = NULL;
		}
		current = current->nxt;
	}
	return(newList);
}

// Variant of CopyList() that uses Push()
struct node* copy_list_push(struct node* head) {
   struct node* current = head;      // used to iterate over the original list
   struct node* newList = NULL;      // head of the new list
   struct node* tail = NULL; // kept pointing to the last node in the new list
   while (current != NULL) {
      if (newList == NULL) { // special case for the first new node
         push(&newList, current->data);
         tail = newList;
      }
      else {
    	  push(&(tail->nxt), current->data);     // add each node at the tail
         tail = tail->nxt;    // advance the tail to the new last node
      }
      current = current->nxt;
   }
   return(newList);
}

// Dummy node variant
struct node *copy_list_dummy_node(struct node* head) {
   struct node* current = head;      // used to iterate over the original list
   struct node* tail;    // kept pointing to the last node in the new list
   struct node dummy;    // build the new list off this dummy node
   dummy.nxt = NULL;
   tail = &dummy;        // start the tail pointing at the dummy
   while (current != NULL) {
      push(&(tail->nxt), current->data); // add each node at the tail
      tail = tail->nxt;       // advance the tail to the new last node
      current = current->nxt;
   }
   return(dummy.nxt);
}

// Local reference variant
struct node* copy_list_lref(struct node* head) {
   struct node* current = head;      // used to iterate over the original list
   struct node* newList = NULL;
   struct node** lastPtr;
   lastPtr = &newList;         // start off pointing to the head itself
   while (current != NULL) {
      push(lastPtr, current->data);        // add each node at the lastPtr
      lastPtr = &((*lastPtr)->nxt);       // advance lastPtr
      current = current->nxt;
   }
   return(newList);
}

// Recursive variant
struct node* copy_list_recr(struct node* head) {
	struct node *current;
   if (head == NULL) return NULL;
   else {
      struct node* newList = malloc(sizeof(struct node));   // make the one node
      newList->data = current->data;
      newList->nxt = copy_list(current->nxt);  // recur for the rest
      return(newList);
   }
}


/* section 4.1 ordinary push */
struct node* ring_buf_append_node(struct node** headRef, int num) {
   struct node* current = *headRef;

   struct node* newNode;
   newNode       = malloc(sizeof(struct node));
   newNode->data = (void *)num;
   newNode->nxt  = NULL;

   // special case for length 0
   if (current == NULL) {
      *headRef = newNode;
   }else{
      // Locate the last node
      while (current->nxt != current) {
          current = current->nxt;
      }
      current->nxt = newNode;
   }
   return *headRef;
}

void print_ring_buf(struct node *head){
  struct node *cur = head;

  while(cur->nxt != head){
      printf("%d\n",(int)cur->data);
      cur = cur->nxt;
  }
  printf("%d\n",(int)cur->data);
}

struct node *ring_buf_build_one_two_three(){
  struct node *head = NULL;
  struct node *second = NULL;
  struct node *third = NULL;

  head = malloc(sizeof(struct node));
  second = malloc(sizeof(struct node));
  third = malloc(sizeof(struct node));

  head->data = (int *)1;
  head->nxt = second;

  second->data = (int *)2;
  second->nxt = third;

  third->data = (int *)3;
  third->nxt = head;

  return head;
}


/**
 * IGNORE.
 * This is from the Mastering Algorithm book and is not being used actively
 * here.
 */
#if 0
/*list_init() - Initialize the list.  */
void list_init(List *list, void (*destroy)(void *data)) {
  list->size = 0;
  list->destroy = destroy;
  list->head = NULL;
  list->tail = NULL;

  return;
}

/**************************************************************
list_destroy: Remove each element.
***************************************************************/
void list_destroy(List *list) {
  void               *data;

  while (list_size(list) > 0) {
    if (list_rem_next(list, NULL, (void **)&data) == 0 && list->destroy !=
      NULL) {
      /*Call a user-defined function to free dynamically allocated data.*/
      list->destroy(data);
    }
  }

  /*No operations are allowed now, but clear the structure as a
  precaution.*/
  memset(list, 0, sizeof(List));

  return;
}

/*******************************************************************
  list_ins_next: Allocate storage for the element.
  Insert the element into the list.
*******************************************************************/
int list_ins_next(List *list, ListElmt *element, const void *data) {
  ListElmt           *new_element;

  if ((new_element = (ListElmt *)malloc(sizeof(ListElmt))) == NULL)
   return -1;

  new_element->data = (void *)data;

  if (element == NULL) {
   /*Handle insertion at the head of the list.*/
   if (list_size(list) == 0)
      list->tail = new_element;

   new_element->next = list->head;
   list->head = new_element;
   }else {
   /*Handle insertion somewhere other than at the head. */
   if (element->next == NULL)
      list->tail = new_element;

   new_element->next = element->next;
   element->next = new_element;
  }

  /*Adjust the size of the list to account for the inserted element.*/
  list->size++;

  return 0;
}

/**************************************************************
*  list_rem_next
**************************************************************/
int list_rem_next(List *list, ListElmt *element, void **data) {
  ListElmt           *old_element;

/*Do not allow removal from an empty list.*/
if (list_size(list) == 0)
   return -1;

 // Remove the element from the list.
if (element == NULL) {
   *data = list->head->data;
   old_element = list->head;
   list->head = list->head->next;

   if (list_size(list) == 1)
      list->tail = NULL;
   }
else {
   if (element->next == NULL)
      return -1;

   *data = element->next->data;
   old_element = element->next;
   element->next = element->next->next;

   if (element->next == NULL)
      list->tail = element;
}

free(old_element);
list->size--;
return 0;
}

#endif

