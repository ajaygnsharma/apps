/*****************************************************************************
*                                                                            *
*  -------------------------------- list.h --------------------------------  *
*                                                                            *
*****************************************************************************/

#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdio.h>

/*****************************************************************************
*                                                                            *
*  Define a structure for linked list elements.                              *
*                                                                            *
*****************************************************************************/

typedef struct ListElmt_{
  void               *data;
  struct ListElmt_   *next;
}ListElmt;

struct node{
	void *data;
	struct node *nxt;
};


/*****************************************************************************
*                                                                            *
*  Define a structure for linked lists.                                      *
*                                                                            *
*****************************************************************************/

typedef struct List_ {

int                size;

int                (*match)(const void *key1, const void *key2);
void               (*destroy)(void *data);

ListElmt           *head;
ListElmt           *tail;

} List;

/*****************************************************************************
*                                                                            *
*  --------------------------- Public Interface ---------------------------  *
*                                                                            *
*****************************************************************************/

void list_init(List *list, void (*destroy)(void *data));

void list_destroy(List *list);

int list_ins_next(List *list, ListElmt *element, const void *data);

int list_rem_next(List *list, ListElmt *element, void **data);


#define list_size(list) ((list)->size)

#define list_head(list) ((list)->head)

#define list_tail(list) ((list)->tail)

#define list_is_head(list, element) ((element) == (list)->head ? 1 : 0)

#define list_is_tail(element) ((element)->next == NULL ? 1 : 0)

#define list_data(element) ((element)->data)

#define list_next(element) ((element)->next)


struct node *build_one_two_three();
void print_list(struct node *head);
int length(struct node *head);
void push(struct node **head_ref, void *data);
void change_to_null(struct node **head_ref);
struct node* AppendNode(struct node** headRef, int num);
struct node *append_node_w_push(struct node **headRef, int num);
struct node *copy_list(struct node* head);
void test_ring_buf_one_two_three();
void test_ring_buf_append();

#endif
