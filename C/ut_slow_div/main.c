/*
 * main.c
 *
 *  Created on: Oct 13, 2016
 *      Author: ajay.sharma
 */

#include <stdio.h>

typedef unsigned int uint32_t;
typedef signed int int32_t;

typedef unsigned short int uint16_t;
typedef signed short int int16_t;

typedef unsigned char uint8_t;
typedef signed char int8_t;

int main(void){
    uint8_t dv = 10 ;
    int16_t rm = 0;
    uint8_t dd = 0;

    int16_t qt = 374;
    rm = qt;

    while(rm > dv){
        rm = qt - dv;
        qt = rm;
        dd++;
    }

    printf("R=%d\n",dd);
    return 0;
}
