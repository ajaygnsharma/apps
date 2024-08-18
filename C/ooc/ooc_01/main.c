/*
 * main.c
 *
 *  Created on: Feb 3, 2011
 *      Author: ajay
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//class/structure
typedef struct pstring_t {
        char * chars;	//data
        int (* length)();//method
}PString;

//class method
int length(PString * self) {
    return strlen(self->chars);
}

//constructor for the string
PString * initializeString(int n)
{
    PString *str=(PString*)malloc(sizeof(PString));
    str->chars=(char *)malloc(sizeof(char)*n);
    str->length = length;
    return str;
}

int main(){
  PString *p=initializeString(30); //create an object
  strcpy(p->chars,"Hello");
  printf("\n%d",p->length(p));     //Call object's method
 return 0;
}
