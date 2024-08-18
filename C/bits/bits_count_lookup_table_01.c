/**
 * This program demos the bits set in a variable using a look up table.
 */
#include <stdio.h>

/* printf format
%[flags][width][.precision][length]specifier */
/**
 * we prepare a table of bits set in 8 bit value starting from 0-255
 * Then based on the input value, we find the index value in the array.
 */

#define ARR_SIZE 256

int main(void){
    unsigned char v=0xc0;
    unsigned char c;
    int i;
    unsigned char bits_set_table[ARR_SIZE];

    bits_set_table[0]=0;

    for(i=0; i<ARR_SIZE; i++){
      bits_set_table[i] = (i & 1) + bits_set_table[i / 2]; //?
      printf("[%d]=%hhx\n",i,bits_set_table[i]);
    }

    c = bits_set_table[ v & 0xff];

    printf("bits set in v=%hhx: %d\n",v,c);
    return 0;
}
