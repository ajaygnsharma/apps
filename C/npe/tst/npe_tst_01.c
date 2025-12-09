/*
 * ne_tst_01.c
 *
 *  Created on: Feb 28, 2012
 *      Author: user1
 *      C programming test: #15.
 */
#include <stdio.h>

int main(void){
	int a=5;
	int b=7;
	int *p=&a;
	int *q=&b;

	printf("%d\n",++a);
	printf("%d\n",++(*p));
	printf("%d\n",--(*q));
	printf("%d\n",--(b));

	return 0;
}

