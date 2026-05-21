/*
This program tries to demonstrate the behavior of const variables in C.

In C, a const variable is a variable whose value cannot be changed after 
it has been initialized. However, the const qualifier only 
applies to the variable itself, not to the memory it points to. 
The variable 'a' is stored in a read-only section of memory, so trying to 
modify it through a pointer will result in a segmentation fault. 
However, the variable 'b' is stored on the stack, and while it 
is declared as const, the pointer 'p' can still point to it and modify its 
value, which is undefined behavior. This is why we see the new value of 'b'
printed as 500, even though it was declared as const.
*/

#include <stdio.h>


const int a = 100; // stored in read-only section
int *p;

int main(){
  const int b = 10; // stored on stack
  p = &a;
  //*p = 100; // ❌ Segmentation fault (read-only memory)

  p = &b;
  *p = 500; // ⚠️ const value changed!

  printf("new value = %d\n", b);
  return 0;
}

