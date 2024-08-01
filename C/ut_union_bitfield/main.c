/*
 * main.c
 *
 *  Created on: Jul 13, 2017
 *      Author: ajay.sharma
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    uint32_t loops:8;
    uint32_t period:16;
    uint32_t rsvd:8;
}time_bits_t;

typedef struct{
    uint32_t current:16;
    uint32_t resistance:16;
}threshold_bits_t;

typedef struct{
    union{
        uint32_t cast_val;
        time_bits_t   bits;
    }part_1;
    union{
        uint32_t cast_val;
        threshold_bits_t bits;
    }part_2;
}cap_leak_test_bist_req_data_t;


int main(void){
    cap_leak_test_bist_req_data_t r;

    r.part_1.bits.rsvd = 0;
    r.part_1.bits.loops  = 5;
    r.part_1.bits.period = 10;
    r.part_2.bits.current = 4158;
    r.part_2.bits.resistance = 8300;

    printf("r.part_1.cast_val=%x, r.part_2.cast_val=%x",
            r.part_1.cast_val, r.part_2.cast_val);
    return 0;
}
