/**
 * This is used to swap the value of a and b
 */
#include <stdio.h>

/* printf format
%[flags][width][.precision][length]specifier */

int main(void){
    char a = 0xC0, b = 0x81;
    printf("a=%#hhx, b=%#hhx\n",a,b);

    a^=b;
    b^=a;
    a^=b;

    printf("a=%#hhx, b=%#hhx\n",a,b);
    return 0;
}
