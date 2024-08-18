#ifndef LIST_H_
#define LIST_H_

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

struct node_s{
  int data;
  struct node_s *link;
};

typedef struct node_s node_t;

struct list_init_s{
  node_t *(*build)();
  void (*push)(node_t **head_ref, void *data);
  void (*null)(node_t **head_ref);
  node_t *(*append)(node_t *,int);
  node_t *(*append_after)(node_t *,int,int);
  node_t *(*del)(node_t *, int);
  void (*delete_list)(node_t *);
};

struct list_operation_s{
  void (*print)(node_t *);
  int  (*length)(node_t *);
  node_t *(*swap)(node_t *, int);
  node_t *(*sum_nodes)(node_t *,node_t *);
  node_t *(*odd_list)(node_t *);
  node_t *(*r_sort)(node_t *);
  node_t *(*reverse)(node_t *);
};

#endif
