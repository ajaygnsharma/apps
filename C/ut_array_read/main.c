/*
 * main.c
 *
 *  Created on: Sep 19, 2017
 *      Author: ajay.sharma
 */

#include <string.h>
#include <stdio.h>

typedef unsigned char uint8_t;

void read_reg(uint8_t *rsp){
    uint8_t data[2] = { 23, 45 };
    //rsp[0] = data[0];
    //rsp[1] = data[1];
    memcpy((void *)rsp, (const void *)data, sizeof(data));
}

int main(void){
    uint8_t rsp[2];
    read_reg(rsp);
    printf("rsp[0]=%d,rsp[1]=%d",rsp[0], rsp[1]);
    return 0;
}
