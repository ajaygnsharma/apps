/*
 * main.c
 *
 *  Created on: Oct 25, 2012
 *      Author: ajsharm2
 */
#include "tmp_struct.h"

//#define EX_6 1

int main(void){

#if EX_1
  foo_t *foo;
  init1(foo);

  printf("foo->a=%d\n",foo->a);
  printf("foo->b=%d\n",foo->b);
  printf("foo->c=%d\n",foo->c);
  printf("foo->d=%d\n",foo->d);

  /* wrong: will give segmentation fault,
  since foo is pointing to unitialized mem */
#endif

#if EX_2
  /* this works since foo is a valid variable and contains memory */
  foo_t foo;
  init1(&foo); // pass address of structure so that

  printf("foo.a=%d\n",foo.a);
  printf("foo.b=%d\n",foo.b);
  printf("foo.c=%d\n",foo.c);
  printf("foo.d=%d\n",foo.d);
#endif

#if EX_3
  /* Will give segmentation fault, because we are passing address to
   * pointer rather than the actual structure itself.
   * use init1(foo) instead. See below
   */
  foo_t *foo;
  foo=malloc(sizeof(foo_t));

  init1(&foo);
  printf("foo->a=%d\n",foo->a);
  printf("foo->b=%d\n",foo->b);
  printf("foo->c=%d\n",foo->c);
  printf("foo->d=%d\n",foo->d);
#endif

#if EX_4
  foo_t *foo;
  foo=malloc(sizeof(foo_t));
  init1(foo);

  printf("foo->a=%d\n",foo->a);
  printf("foo->b=%d\n",foo->b);
  printf("foo->c=%d\n",foo->c);
  printf("foo->d=%d\n",foo->d);
#endif

#if EX_5
  /* Used for like a substitute of new keyword in classes.
   * where new_foo() will create a new instance of foo_t in heap.
   * Needs to be freed later.
   */
  foo_t *foo;

  foo=new_foo(1,2,3,4);
  printf("foo->a=%d\n",foo->a);
  printf("foo->b=%d\n",foo->b);
  printf("foo->c=%d\n",foo->c);
  printf("foo->d=%d\n",foo->d);
  free(foo);
#endif

#if EX_6
  foo_t *foo;
  int ret=-1;
  ret=init3(&foo);
  printf("foo->a=%d\n",foo->a);
  printf("foo->b=%d\n",foo->b);
  printf("foo->c=%d\n",foo->c);
  printf("foo->d=%d\n",foo->d);
#endif

#if EX_7
  foo_t *foo;
  int ret=-1;
  ret=init3(foo); /* warning:  passing argument 1 of 'init3' from
  incompatible pointer type, RUNS: segmentation fault */
  printf("foo->a=%d\n",foo->a);
  printf("foo->b=%d\n",foo->b);
  printf("foo->c=%d\n",foo->c);
  printf("foo->d=%d\n",foo->d);
#endif

#if EX_8
  foo_t **foo;
  int ret=-1;
  ret=init3(&foo); /* Warning passing arg of init3 from incompatible
  poiner type. error : as well as request for member 'a' in something not a
  structure or union */
  printf("foo->a=%d\n",foo->a);
  printf("foo->b=%d\n",foo->b);
  printf("foo->c=%d\n",foo->c);
  printf("foo->d=%d\n",foo->d);
#endif

#if EX_9
  /* p_bar_t is a pointer type to a struct of type bar_s.
   * Every variable created using this type is a pointer type to a struct bar_s.
   * so memory needs to be allocated first and this pointer should point to that
   * or should point to another initialized variable of type struct_s.
   * Passing it as an argument to a function does not require address since it
   * is a pointer. In the function definition, the * symbol is not required since
   * p_bar_t is a pointer already. bar variable is pointer so referring to members
   * of struct is done using '->' symbol and not '.'.
   */
  p_bar_t p_bar;
  p_bar = malloc(sizeof(struct bar_s));
  init4(p_bar);
  printf("bar->a=%d\n",p_bar->a);
  printf("bar->b=%d\n",p_bar->b);
#endif

#if EX_10
  /* Here: 'bar' is 'struct bar_s' variable and is a valid memory. But 'p_bar'
   * is not when its declared the first time. When 'p_bar = &bar;' is done.
   * then 'p_bar' points to a valid memory block. Operations on 'p_bar'
   * modify 'bar' member values.
   */
  struct bar_s bar;
  p_bar_t p_bar;

  p_bar = &bar;
  init4(p_bar);
  printf("bar->a=%d\n",p_bar->a);
  printf("bar->b=%d\n",p_bar->b);
#endif

  return 0;
}
