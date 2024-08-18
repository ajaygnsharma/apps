/*
 * prob3_3.c
 *
 *  Created on: Apr 8, 2012
 *      Author: user1
 */
#include <stdlib.h>
#include <stdio.h>

int factorial_flp(int n){
  int i, ret=1;

  for(i=2; i<=n; i++)
    ret *=i;

  return ret;
}

int factorial_wh(int n){
  int i, ret=1;
  i=2;

  while(i<=n){
    ret *=i;
    i++;
  }

  return ret;
}


double rand_double(){
  /* generate random number in [0,1) */
  double ret=(double)rand();
  return ret/(RAND_MAX+1);
}

int sample_geometric_rv(double p){
  double q;
  int n=0;

  do{
    q=rand_double();
    n++;
  }while(q>=p);

  return n;
}

int sample_geometric_rv_wh(double p){
  double q=p;
  int n=0;

  while(q>=p){
    q=rand_double();
    n++;
  };

  return n;
}


int main(void){
  printf("factorial 4 for loop:%d\n",factorial_flp(4));
  printf("factorial 4 for loop:%d\n",factorial_wh(4));
  printf("sample geometric do while: %d\n",sample_geometric_rv(4));
  printf("sample geometric while: %d\n",sample_geometric_rv_wh(4));
  return 0;
}
