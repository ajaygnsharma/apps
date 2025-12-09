/*
 * strtok.c
 *
 *  Created on: May 22, 2012
 *      Author: user1
 */
#include <stdlib.h>
#include <string.h>

unsigned int strspn_a(const char *str, const char *delims);
unsigned int strcspn_a(const char *str, const char *delims);

static char *pnexttoken = NULL;

char *strtok_a(char *text, const char *delims){
	if(!text)
		text=pnexttoken;

	/* find next token in txt */
	text+=strspn_a((const char *)text,delims);
	if(*text=='\0')
		return NULL;

	/*find end of token in txt */
	pnexttoken=text+strcspn_a((const char *)text,delims);

	/* insert null-terminated at end */
	if(*pnexttoken!='\0')
		*pnexttoken++ = '\0';

	return text;
}

unsigned int strspn_a(const char *str, const char *delims){
	unsigned int i;
	//for(i=0;str[i]!='\0' && strpos(delims,str[i])>-1;i++);
	for(i=0;str[i]!='\0' && strstr(delims,str[i])>-1;i++);
	return i;
}


unsigned int strcspn_a(const char *str, const char *delims){
	unsigned int i;
	//for(i=0;str[i]!='\0' && strpos(delims,str[i])==-1;i++);
	for(i=0;str[i]!='\0' && strstr(delims,str[i])==-1;i++);
	return i;
}
