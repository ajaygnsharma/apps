/*
 ============================================================================
 Name        : admin_password.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t admin_password;

void administrator_password(void)
{
  char *p = "TE8117690";
  uint32_t bp ;
  uint32_t bpul ;

  bpul = 1423 ; //  set initial value

  if (*p == 0) {
    bpul += 0xA5C3 ;  //  43858
  } else {
    while (*p != 0) {
      bp = (unsigned long) *p++ ;
      bpul += bp ;
    }
  }
  bpul &= 0x0000FFFF;
  admin_password = bpul;
#ifndef NDEBUG
  printf("admin=%u\n", admin_password);
#endif
}

int main(void) {
	administrator_password();
	return EXIT_SUCCESS;
}
