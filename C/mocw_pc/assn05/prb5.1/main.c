/*
 * main.c
 *
 *  Created on: Jul 31, 2012
 *      Author: user1
 */
#include "llist.h"

/* test the llist */
int main(void){
  struct node *head='\0';
  head=addback(head, 2);
  head=addback(head, 4);
  head=addback(head, 5);
  display(head);
  struct node *tmp='\0';
  tmp=find(head,3);
  if(tmp!='\0')
    printf("found:%d\n",tmp->data);
  else
	puts("cannot find 3");


#if 0
  tmp=malloc(sizeof(struct node));
  struct node *new_headnode='\0';

  tmp->data=2;
  new_headnode=delnode(head,tmp);

  tmp->data=4;
  new_headnode=delnode(new_headnode,tmp);

  tmp->data=5;
  new_headnode=delnode(new_headnode,tmp);
  display(new_headnode);
#endif
  struct node *new_head;
  new_head=freelist(head);
  display(new_head);
  return 0;
}
