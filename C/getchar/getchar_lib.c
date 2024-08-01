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

  s1.a=getchar();
  s1.b=getchar();
  s1.c=getchar();

  putchar(s1.a);
  putchar(s1.b);
  putchar(s1.c);
  putchar('\n');

  return 0;
}

