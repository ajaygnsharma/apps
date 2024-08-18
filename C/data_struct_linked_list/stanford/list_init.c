
#include "list_init.h"

/**
 * Page 6
 * Section 1.1:
 * I converted the names from XsomeThing to x_some_thing.
 * build_one_two_three()
 * build a link list with 3 elements in the heap and return the head ptr
 * */
node_t *build_one_two_three(){
  node_t *head = NULL;
  node_t *second = NULL;
  node_t *third = NULL;

  head = malloc(sizeof(node_t));
  second = malloc(sizeof(node_t));
  third = malloc(sizeof(node_t));

  head->data = 1;
  head->link = second;

  second->data = 2;
  second->link = third;

  third->data = 3;
  third->link = NULL;

  return head;
}

#if 0
node_t *build_one_two_three(){
	node_t *t = NULL;
	printf("hi\n");
	return t;
}
#endif

/**
 * Page 14
 * This function does the correct push and below tests it.
 * section 2.1
 * This can also be called: append_at_beginning a new node */
void push_list(node_t **head_ref, void *data){
  node_t *new_node = malloc(sizeof(node_t));

  new_node->data = (int)data;
  new_node->link = *head_ref;
  *head_ref = new_node;
}

/**
 * Page 17, 18
 * section 2.4
 * Using a reference pointer(pointer to a pointer) to change the caller's head
 * pointer. Here its initialized to NULL.
 * */
void change_to_null(node_t **head_ref){
	*head_ref=NULL;
}

/**
 * Simple push an element to the end or in other words
 * add it at the end of the list. if list NULL, return the new
 * node.
 * */
node_t *add_at_tail(node_t *h, int num) {
   node_t *c = h;
   node_t *n;

   n       = malloc(sizeof(node_t));
   n->data = num;
   n->link = NULL;

   // special case for length 0
   if (c == NULL){
      return n;
   }else{
      // Locate the last node
      while (c->link != NULL) {
          c = c->link;
      }
      c->link = n;
   }
   return h;
}

node_t *add_after(node_t *h, int after, int num){
  node_t *c = h;
  node_t *t;
  node_t *n;

  t = malloc(sizeof(node_t));
  t->data = num;
  t->link = NULL;

  if(c == NULL){
    return t; // List is null, then return this new node.
  }else{
    while(c->data != after){
      c = c->link;
    }
  }
  n = c->link; //next link after current in the list
  c->link = t; //link current element to new
  t->link = n; //link new element to next element of the list

  return h;
}

/**
 * Delete a individual node specified by the number.
 */
node_t *del_node(node_t *h, int num){
  node_t *c = h;
  node_t *p = c;
  node_t *n = NULL;

  while(c->data != num){
    if(c == NULL)
      return h;
    p = c;
    c = c->link;
  }

  n = c->link;
  if(c->data == h->data){ // Deleting the first element.
    h = n;
  }else{
    p->link = n;
  }
  free(c);
  return h;
}

#define OPTION_DEBUG 0

/**
 * Delete a full list using above individual element deletion idea
 */
void del_list_1(node_t *h){
  node_t *c = h;

  while(c != NULL){
    c = del_node(c, c->data);
  }

  return;
}

/**
 * Delete a full list by deleting one element and moving to another
 */
void del_list_2(node_t *h){
  node_t *c = h;
  node_t *n = NULL;

  if(c == NULL)
    return;

  while(c != NULL){
    n = c->link;
    free(c);
    c = n;
  }

  return;
}

/**
 * Calculate the length of list using recursive method with static variable
 * for length.
 */
int recursive_length_1(node_t *h){
  node_t *c = h;
  static int l=0; //Have to use static !

  if(c == NULL){
    return l;
  }else{
    l++;
    c = c->link;
    l = recursive_length_1(c);
  }
  return l;
}

/**
 * Calculate the length of list using recursive method. Simplest
 */
int recursive_length_2(node_t *h){
  node_t *c = h;

  if(c == NULL){
    return 0;
  }else{
    return 1 + recursive_length_2(c->link);
  }
}

/**
 * Simple push an element to the end or in other words
 * add it at the end of the list. if list NULL, return the new
 * node.
 * */
node_t *recursive_add_at_tail(node_t *h, int num) {
   node_t *c = h;
   node_t *t = NULL;

   // special case for length 0
   if (c->link == NULL){
       t       = malloc(sizeof(node_t));
       t->data = num;
       t->link = NULL;
       c->link = t;

       return c;
   }else{
      // Locate the last node
      if (c->link != NULL) {
	recursive_add_at_tail(c->link, num);
      }
   }
   return h;
}

/**
 * Simple push an element to the end or in other words
 * add it at the end of the list. if list NULL, return the new
 * node.
 * */
node_t *recursive_add_after(node_t *h, int num) {
   node_t *c = h;
   node_t *t = NULL;

   // special case for length 0
   if (c->link == NULL){
       t       = malloc(sizeof(node_t));
       t->data = num;
       t->link = NULL;
       c->link = t;

       return c;
   }else{
      // Locate the last node
      if (c->link != NULL) {
	recursive_add_at_tail(c->link, num);
      }
   }
   return h;
}
