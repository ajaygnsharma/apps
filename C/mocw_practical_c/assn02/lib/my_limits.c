/*
 * limits.c
 *
 *  Created on: Mar 25, 2012
 *      Author: user1
 */
#include <limits.h> /* limits.h is in /usr/bin/include */
#include <float.h> /* float.h is in: /usr/lib/gcc/i486-linux-gnu/4.4/include */

char char_size(void){
	return sizeof(char);
}

char char_min(void){
	return (CHAR_MIN);
}

char char_max(void){
	return (CHAR_MAX);
}

unsigned char uchar_size(void){
	return sizeof(unsigned char);
}

unsigned char uchar_min(void){
	return (0);
}

unsigned char uchar_max(void){
	return (UCHAR_MAX);
}

short short_size(void){
	return sizeof(short);
}

short short_min(void){
	return (SHRT_MIN);
}

short short_max(void){
	return (SHRT_MAX);
}

int int_size(void){
	return (sizeof(int));
}

int int_min(void){
	return (INT_MIN);
}

int int_max(void){
	return(INT_MAX);
}

unsigned int uint_size(void){
	return(sizeof(unsigned int));
}

unsigned int uint_min(void){
	return 0;
}

unsigned int uint_max(void){
	return(UINT_MAX);
}

unsigned long ulong_size(void){
	return(sizeof(unsigned long));
}

unsigned long ulong_min(void){
	return 0;
}

unsigned long ulong_max(void){
	return(ULONG_MAX);
}

float float_size(void){
	return(sizeof(float));
}

float float_min(void){
	return(FLT_MIN);
}

float float_max(void){
	return(FLT_MAX);
}
