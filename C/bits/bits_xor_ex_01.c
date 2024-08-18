/**
 * This program shows the usage of XOR keyword in programming.
 * 1. toggles the value of a bit
 * 2. toggles the value again but this time makes it zero
 */
#include <stdio.h>


/* printf format
%[flags][width][.precision][length]specifier */

int main(void){
    char a = 0xC0, b = 0x81;
    printf("a=%#hhx, b=%#hhx\n",a,b);

    a^=(1<<2); // toggle bit 3 of a -> 0xc4
    b^=b;      // makes b zero

    printf("a=%#hhx, b=%#hhx\n",a,b);
    return 0;
}
