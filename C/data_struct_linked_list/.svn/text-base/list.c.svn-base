/*
 * list.c
 *
 *  Created on: Nov 8, 2012
 *      Author: user1
 *      Covers section 1.1,1.2,1.3,1.4,1.5. 2.1, 2.3, 2.4
 *      Test
 */
/** list.c**/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "list.h"

/*list_init() - Initialize the list.  */
void list_init(List *list, void (*destroy)(void *data)) {
  list->size = 0;
  list->destroy = destroy;
  list->head = NULL;
  list->tail = NULL;

  return;
}

/*****************************************************************************
list_destroy: Remove each element.
*****************************************************************************/
void list_destroy(List *list) {
  void               *data;

  while (list_size(list) > 0) {
    if (list_rem_next(list, NULL, (void **)&data) == 0 && list->destroy !=
      NULL) {
      /*Call a user-defined function to free dynamically allocated data.*/
      list->destroy(data);
    }
  }

  /*No operations are allowed now, but clear the structure as a precaution.*/
  memset(list, 0, sizeof(List));

  return;
}

/*****************************************************************************
  list_ins_next: Allocate storage for the element.Insert the element into the
  list.
*****************************************************************************/
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

/*****************************************************************************
*                                                                            *
*  ----------------------------- list_rem_next ----------------------------  *
*                                                                            *
*****************************************************************************/

int list_rem_next(List *list, ListElmt *element, void **data) {
  ListElmt           *old_element;

/*Do not allow removal from an empty list.*/
if (list_size(list) == 0)
   return -1;

/*****************************************************************************
*                                                                            *
*  Remove the element from the list.                                         *
*                                                                            *
*****************************************************************************/

if (element == NULL) {

   /**************************************************************************
   *                                                                         *
   *  Handle removal from the head of the list.                              *
   *                                                                         *
   **************************************************************************/

   *data = list->head->data;
   old_element = list->head;
   list->head = list->head->next;

   if (list_size(list) == 1)
      list->tail = NULL;

   }

else {

   /**************************************************************************
   *                                                                         *
   *  Handle removal from somewhere other than the head.                     *
   *                                                                         *
   **************************************************************************/

   if (element->next == NULL)
      return -1;

   *data = element->next->data;
   old_element = element->next;
   element->next = element->next->next;

   if (element->next == NULL)
      list->tail = element;

}

/*****************************************************************************
*                                                                            *
*  Free the storage allocated by the abstract data type.                     *
*                                                                            *
*****************************************************************************/

free(old_element);

/*****************************************************************************
*                                                                            *
*  Adjust the size of the list to account for the removed element.           *
*                                                                            *
*****************************************************************************/

list->size--;

return 0;

}




/**
 * Section 1.1
 * build_one_two_three()
 * build a link list with 3 elements in the heap and return the head ptr */
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

// Section 1.2 TBD: in 7 steps build_one_two_three instead of 9.

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

/* section 1.3, 1.4, 1.5, 2.3 */
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

/* section 2.4 */
void change_to_null(struct node **head_ref){
	*head_ref=NULL;
}

/* section 2.1 */
void push(struct node **head_ref, void *data){
  struct node *new_node = malloc(sizeof(struct node));

  new_node->data = (void *)data;
  new_node->nxt = *head_ref;
  *head_ref = new_node;
}

/* section 4.1 ordinary push */
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

/* section 4.2 Append node with Push */
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


