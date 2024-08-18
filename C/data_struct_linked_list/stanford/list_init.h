#ifndef LIST_INIT_H_
#define LIST_INIT_H_

#include "list.h"

node_t *build_one_two_three();
void    push_list(node_t **head_ref, void *data);
void    change_to_null(node_t **head_ref);
node_t *unit_test_add_at_head();
node_t *unit_test_build_with_special_case();
node_t *unit_test_build_with_dummy_node(void);
node_t *unit_test_build_with_local_ref(void);
node_t *add_at_tail(node_t *, int);
node_t *add_after(node_t *, int, int);
node_t *del_node(node_t *, int);
void    del_list_1(node_t *h);
void    del_list_2(node_t *h);
int     recursive_length_1(node_t *h);
int     recursive_length_2(node_t *h);
node_t *recursive_add_at_tail(node_t *, int);

#endif
