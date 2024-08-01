/*
 * incomplete_type.c
 *
 *  Created on: Feb 6, 2013
 *      Author: sharmaj
 */
#include <stdio.h>

struct sCmdTbl{
	int sttLBA;
	char *err;
};

int main(void){
	struct sCmdTbl cmdTblEntry;

	cmdTblEntry.sttLBA=2;
	cmdTblEntry.err='\0';

	printf("cmdTblEntry.sttLBA:%d\n",cmdTblEntry.sttLBA);
	return 0;
}
