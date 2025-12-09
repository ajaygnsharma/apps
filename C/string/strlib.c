/*
 * strlib.c
 *
 *  Created on: Nov 5, 2012
 *      Author: ajsharm2
 */
#include <stdio.h>

#define EX_5 1

#if EX_1
/* chng_name
 * This is the correct way of changing the name since we are passing a
 * reference pointer to the string. Remember that always:
 * a "copy" of the string is passed. You need double ptr(ref ptr) to dereference
 * the caller's(original string).
 * Here *name is a ref ptr and pts to caller's string. */
void chng_name_01(char **str){
  *str="bar";
}

void tst_chng_name_01(void){
  char *name="foo";

  printf("before change name =%s\n",name);
  chng_name_01(&name);
  printf("after change name =%s\n",name);
}
#endif

#if EX_2
void chng_name_02(char *str){
  /* strlib.c:27: warning: assignment makes integer from pointer without a cast*/
  /* but it does manage to change the caller's str. for some reason here it changes
   * the caller's string. It seems it upgraded to a double ptr(ref ptr).
   * but we should not be doing like this. use above case */
  *str="bar";
}

void tst_chng_name_02(void){
  char *name="foo";

  printf("before change name =%s\n",name);
  /* strlib.c:36: warning: passing argument 1 of 'chng_name_02' from incompatible pointer type */
  chng_name_02(&name);

  printf("after change name =%s\n",name); /* prints bar */
}
#endif

#if EX_3
void chng_name_03(char *str){
  /* this does not change the caller's str, since the str is a stack variable
   * and points to a local string 'bar'. str is initialized as "foo" but is a
   * copy of "name" */
  str="bar";
}

void tst_chng_name_03(void){
  char *name="foo";

  printf("before change name =%s\n",name);
  /* strlib.c:59: warning: passing argument 1 of 'chng_name_03' from incompatible pointer type */
  chng_name_03(&name);

  printf("after change name =%s\n",name); /* prints bar */
}
#endif

#if EX_4
/* this does not give any warning or error. but the caller's string is passed
 * as a copy and changing it in the callee does not alter it at all. */
void chng_name_04(char *str){
  /* this does not change the caller's str, since the str is a stack variable
   * and points to a local string 'bar'. Here too str is a copy of "name" */
  str="bar";
}

void tst_chng_name_04(void){
  char *name="foo";

  printf("before change name =%s\n",name);
  chng_name_04(name);

  printf("after change name =%s\n",name); /* prints bar */
}
#endif

#if EX_5
/* Segmentation fault (core dumped ) */
void chng_name_05(char *str){
  /* str is trying to change memory that the program did not assign itself.
   * *str = "bar" and not str = "bar".
   * strlib.c:96: warning: assignment makes integer from pointer without a cast*/
  *str="bar";
}

void tst_chng_name_05(void){
  char *name="foo";

  printf("before change name =%s\n",name);
  chng_name_05(name);

  printf("after change name =%s\n",name); /* prints bar */
}
#endif




int main(void){
  //tst_chng_name_01();
  //tst_chng_name_02();
  //tst_chng_name_03();
  //tst_chng_name_04();
	tst_chng_name_05();

  return 0;
}



#if 0
void chomp(char **s) {
  int end = strlen(*s) - 1;
  if(end >= 0 && *(*s+end) == '\n'){
    *(*s+end) = '\0';
  }
}
#endif

void chomp(char **s) {
  int end = strlen(*s) - 1;
  if(end >= 0 && (*s)[end] == '\n'){
    (*s)[end] = '\0';
  }
}

