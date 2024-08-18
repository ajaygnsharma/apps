#ifndef LIST_OPERATION_INIT_H_
#define LIST_OPERATION_INIT_H_

#include "list.h"

void print_list(node_t *head);
int  length_list(node_t *head);
int  length_list_using_for_loop(node_t *head);
node_t *swap_2(node_t *h, int);
node_t *sum_nodes(node_t *h1, node_t *h2);
node_t *odd_list(node_t *h);
node_t *reverse_using_link_adjust(node_t *h);

#endif
