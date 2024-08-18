/*
 * alloc.c
 *
 *  Created on: Feb 21, 2013
 *      Author: sharmaj
 */
#include <setjmp.h>

struct soo{
  int data;
};

int main(void){
	struct soo *ptr;
	jmp_buf jmp_buf1;

	if( (ptr==setjmp(jmp_buf1)) == 0 ){
		get_as_s_struct();
	}else{
		ptr->data=10;
		ptr->data++;
		printf("struct soo data=%d\n", ptr->data);
	}
}

void get_as_s_struct(){
	struct soo v;
	longjmp(v);
}

