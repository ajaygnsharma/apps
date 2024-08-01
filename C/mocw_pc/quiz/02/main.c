/*
 * main.c
 *
 *  Created on: May 9, 2012
 *      Author: user1
 */
#include <stdio.h>

/* Test if the msg[] gets the main call.
 * It wont since the local variable is
 * no more in scope. This pgm prints junk.
 */
char *get_msg(){
	char msg[]="Aren't pointers fun?";
	return msg;
}

int main(void){
	char *str=get_msg();
	puts(str);
	return 0;
}

