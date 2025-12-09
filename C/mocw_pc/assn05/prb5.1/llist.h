/*
 * llist.h
 *
 *  Created on: Aug 9, 2012
 *      Author: user1
 */

#ifndef LLIST_H_
#define LLIST_H_

#include <stdio.h>
#include <stdlib.h>

struct node{
	int data;
	struct node *nxt;
};

void display(struct node *head);
struct node *addback(struct node *head, int data);
struct node *find(struct node *head, int data);
struct node *delnode(struct node *head, struct node *pelement);
//void freelist(struct node *head);
struct node *freelist(struct node *head);

#endif /* LLIST_H_ */
