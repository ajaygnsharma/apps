/*
 * main.c
 *
 *  Created on: Apr 20, 2012
 *      Author: leonyuwin7
 */
#include <stdio.h>

struct s{
  char a;
  char b;
  char c;
};

int main(int argc, char *argv[]){
  struct s s1;

  scanf("%c%c%c",&s1.a,&s1.b,&s1.c);
  printf("%c,%c,%c\n",s1.a,s1.b,s1.c);

  return 0;
}

