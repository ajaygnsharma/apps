#include <stdio.h>

unsigned int reverse_bits_xor();
unsigned int reverse_bits_mask_shift(register unsigned int x);

void main(){
	const char SWAP_BITS_XOR = 1;
	const char SWAP_BITS_MASK_SHIFT = 0;
	unsigned int r = 0;

	if(SWAP_BITS_XOR){
		r = reverse_bits_xor(0x2f);
	}
	if(SWAP_BITS_MASK_SHIFT){
		r = reverse_bits_mask_shift(0xaa);
	}
	printf("%x\n",r);
}

/**
 * Does a shift using masks
 */
unsigned int reverse_bits_mask_shift(register unsigned int x){
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
    return((x >> 16) | (x << 16));
}

// bits to swap reside in b
unsigned int reverse_bits_xor(unsigned int b){
	unsigned int i, j; // positions of bit sequences to swap
	unsigned int n;    // number of consecutive bits in each sequence
	unsigned int r;    // bit-swapped result goes here

	i = 1;
	j = 5;
	n = 3;

	//b = 00101111;
	unsigned int x = ((b >> i) ^ (b >> j)) & ((1U << n) - 1); // XOR temporary
	r = b ^ ((x << i) | (x << j));
	return r;
}
