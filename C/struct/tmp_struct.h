/*
 * tmp_struct.h
 *
 *  Created on: Oct 25, 2012
 *      Author: ajsharm2
 */

#ifndef TMP_STRUCT_H_
#define TMP_STRUCT_H_

#include <stdlib.h>
#include <stdio.h>

typedef struct{
	int a;
	int b;
	int c;
	int d;
}foo_t;

typedef struct bar_s{
	int a;
	int b;
} *p_bar_t;

void init1(foo_t *foo);
foo_t * new_foo(int a, int b, int c, int d);
int init3(foo_t **foo);
void init4(p_bar_t bar);

#endif /* TMP_STRUCT_H_ */
