/*
 * main.c
 *
 *  Created on: May 22, 2012
 *      Author: user1
 */
#include <stdio.h>
#include "mylib.h"

int main(void){
  char *str=",ajay,smita,babbu";
  char *token=NULL;

  printf("tokens:\n");

  token=strtok_a(str,",");
  printf("%s\n",token);

  /*token=strtok_a(NULL,",");
  printf("%s\n",token);

  token=strtok_a(NULL,",");
  printf("%s\n",token);*/

  return 0;
}

