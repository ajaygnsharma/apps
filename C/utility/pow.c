/*
 * pow.c
 *
 *  Created on: Feb 25, 2013
 *      Author: sharmaj
 */
#include "stdio.h"

long raise_pow(int num, int power){
	long pow = num ^ power;
	return pow;
}



int main(){
	const int POW = 12;
	printf("raising power 2^%d=%ld\n", POW, raise_pow(2, POW));
	return 0 ;
}
