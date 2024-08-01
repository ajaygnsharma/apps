/*
 * tmpl_struct.c
 *
 *  Created on: Oct 25, 2012
 *      Author: ajsharm2
 */
#include "tmp_struct.h"

/* this is typical and most commonly used way.
 * structure pointers are passed and are memory is allocated in caller.
 * The overhead is less since we are passing pointers to structures.
 */
void init1(foo_t *foo){
	//foo=malloc(sizeof(foo_t));
	/*Donot malloc here. We are assuming caller allocates the space for foo */

	foo->a=1;
	foo->b=1;
	foo->c=1;
	foo->d=1;
}

foo_t * new_foo(int a, int b, int c, int d){
	foo_t *foo;

	foo=malloc(sizeof(foo_t));

	foo->a=a;
	foo->b=b;
	foo->c=c;
	foo->d=d;

	return foo;
}


int init3(foo_t **foo){
	foo_t *tmp;
	tmp= malloc(sizeof(foo_t));

	tmp->a=1;
	tmp->b=2;
	tmp->c=3;
	tmp->d=4;

	*foo = tmp;
	return 1;
}

void init4(p_bar_t p_bar){
	p_bar->a=1;
	p_bar->b=2;
}
