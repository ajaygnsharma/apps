/*
 * mit_c_quiz_01.c
 *
 *  Created on: Feb 29, 2012
 *      Author: user1
 */
#include <stdio.h>

int main(void){
	int x=017;
	int y=12;

	if(x>y){
		printf("x>:%d\n",x); //x > as it is 15 in decimal
	}else{
		printf("y>:%d\n",y);
	}

	//short int s=0xFFFF12; error: warning: overflow in implicit constant conversion
	short int s=0xFF12; //correct
	printf("s:%d\n",s); //s = -238

	char c=-1;
	unsigned char uc=-1;
	printf("c:%d,uc:%d\n",c,uc); //c=-1, uc=255

	//puts("hel"+"lo"); error: invalid operands to binary + (have ‘char *’ and ‘char *’)
	puts("hel""lo"); //hello

	enum sz{S=0,L=3,XL};
	printf("XL:%d\n",XL); //XL=4 as XL is incremented after L to be L+1: 3+1=4

	enum sz1{S1=0,L1=-3,XL1};
	printf("XL1:%d\n",XL1); //XL1=-2 as XL1 is incremented after L1 to be L1+1 = -3+1 = -2

	return 0;
}

