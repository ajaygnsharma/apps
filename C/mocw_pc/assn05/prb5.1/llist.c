/*
 * llist.c
 *
 *  Created on: Jul 31, 2012
 *      Author: user1
 */
#include "llist.h"

/* display()
 * display all the elements of the list
 */
void display(struct node *head){
  struct node *tmp;
  tmp=head;
  if(head=='\0')
	  return;
  if(tmp->nxt=='\0'){
    printf("%d\n",tmp->data);
    return;
  }

  for(tmp=head; tmp!='\0';tmp=tmp->nxt){
    printf("%d\n",tmp->data);
  }
}

/*
 * addback()
 * add an element to the end of the list
 * return the new head node to the list
 */
struct node *addback(struct node *head, int data){
  struct node *tmp;
  if(head=='\0'){
    tmp = malloc(sizeof(struct node));
    if(tmp!='\0'){
      tmp->data=data;
      tmp->nxt='\0';
    }
    head=tmp;
    return head;
  }else{
    struct node *tail;
    for(tail=head; tail->nxt!='\0';tail=tail->nxt){
    }
    tmp = malloc(sizeof(struct node));
    if(tmp!='\0'){
      tmp->data=data;
      tmp->nxt='\0';
    }
    tail->nxt=tmp;
    return head;
  }
}

/**
 * find()
 * return pointer to the element in the list having the given data
 */
struct node *find(struct node *head, int data){
 struct node *tmp;
 for(tmp=head; tmp!='\0';tmp=tmp->nxt){
   if(tmp->data==data)
     return tmp;
 }
 return '\0';
}

/**
 * delnode()
 * deletes the element pointed to by pelement(obtained using find)
 * returns the updated head node.
 * NOTE: Make sure to conside the case when the pelement points to
 * head node
 */
struct node *delnode(struct node *head, struct node *pelement){
  struct node *delnode='\0';
  delnode=find(head, pelement->data);
  if(delnode!='\0'){
    struct node *pre_delnode;
    struct node *new_headnode;
    pre_delnode=head;
    new_headnode=pre_delnode;
    if(delnode!=head){
    	while(pre_delnode->nxt!=delnode){
    	    	pre_delnode=pre_delnode->nxt;
    	   }
    	pre_delnode->nxt=delnode->nxt;
    	free(delnode);
    }else{
      new_headnode=head->nxt;
      free(delnode);
    }

    head=new_headnode;
    return head;
  }else
    return '\0';
}

/**
 * freelist()
 * delete all the elements from the list.
 * NOTE: Make sure not to use any pointer after it is freed.
 */
struct node *freelist(struct node *head){
  struct node *tmp;
  struct node *new_head;
  new_head=head;
  for(tmp=head; tmp!='\0';tmp=tmp->nxt){
	  new_head=delnode(head,tmp);
	  head=new_head;
  }
  return new_head;
}
