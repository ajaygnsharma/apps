/*
 * main.c
 *
 *  Created on: Feb 1, 2017
 *      Author: ajay.sharma
 */
#include <stdio.h>

struct list{
	int data;
	struct l *next;
};

struct list build_list(){
	struct list *l;
	int i = 0;
	for(i = 0; i < 4; i++){
		l = malloc(sizeof(struct list));
		l->data = i+1;
		l->next =
	}
}

int main(){
  struct list l;


}

