/*
 * main.c
 *
 *  Created on: May 9, 2012
 *      Author: user1
 *  insertion sort using pointers.
 */
#include <stdio.h>

#define arr_length(arr) \
	(sizeof(arr)==0 ? 0:sizeof(arr)/sizeof((arr)[0]))

int arr[100]={1,4,5,2,69,20,22,10};

void shift_el(unsigned int i){
  int ivalue;
  for(ivalue = *(arr+i); i && *(arr+(i-1)) > ivalue; i--)
	  *(arr+i) = *(arr+(i-1));
  *(arr+i) = ivalue;
}

void insertion_sort(){
  unsigned int i, len=arr_length(arr);
  for(i=1;i<len;i++)
	  if(*(arr+i) < *(arr+(i-1)))
		  shift_el(i);
}

void print_arr(){
  unsigned int i, len=arr_length(arr);
  for(i=1;i<len;i++)
	  if(*(arr+i))
		  printf("%d\n",*(arr+i));

}

int main(void){
  insertion_sort();
  print_arr();
  return 0;
}
