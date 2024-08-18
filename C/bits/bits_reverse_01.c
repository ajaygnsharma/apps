#include <stdio.h>
#include <limits.h>

/**
 * obvious way of swapping bits of char
 * its sort of like ROTATE instruction
 * 1100_0000 -> 0110_0000 shift right , 0 is set in rev bit and shift left
 * 0110_0000 -> 0011_0000 shift right "
 * 0011_0000 -> 0001_1000 shift right "
 * 0001_1000 -> 0000_1100 shift right  "
 * 0000_1100 -> 0000_0110 shift right , "
 * 0000_0110 -> 0000_0011 shift right , 1 is set in rev bit
 * 0000_0001 -> 0000_0000 shift right , 1 is set in rev bit
 * orig value is shifted right, rev value if shifted left but set with
 * orig's LSB.
 */
int main(void){
    unsigned char v = 0xC0;   //value
    unsigned char r = v;      //rev value
    unsigned char s = CHAR_BIT - 1; //size of char type

    printf("v=%#hhx, r=%#hhx\n",v,r);

    // NOTE: first bit does not require masking. So shifting r left by 1
    // does not loose the value since its already holding the value of v.
    // So: First bit is OK to shift imagine a value that is only 2 bits. It
    // may seem obvious.  And shifting v by 1 to right is correct since we
    // need to start with something.
    for(v >>= 1; v; v >>= 1){ // Right shift v
        r <<= 1;              // left shift r. make room for a LSB
        r |= (v & 1);         // SET 'r' by geting LSB of v
        s--;                  // decrement count
    }
    r <<= s;                  //if 'v' zeroed, right shift the remaining bits
                              //since they were zero

    printf("v=%#hhx, r=%#hhx\n",v,r);
    return 0;
}
