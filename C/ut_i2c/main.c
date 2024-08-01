/*
 * main.c
 *
 *  Created on: Jun 30, 2016
 *      Author: ajay.sharma
 */
#include <stdio.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned int uint32_t;

int                             resolution = 125;


#define I2C_DATA_LEN 8

typedef struct {
    uint8_t                     addr;
    uint8_t                     reg;
    uint8_t                     len;
    uint8_t                     data[I2C_DATA_LEN];
    volatile uint8_t            valid;
} i2cr_rsp_data_t;

i2cr_rsp_data_t             rsp;

int main(void){
    uint16_t                    raw_data;
    uint32_t t = 0;
    int16_t                     temperature = 0;

    rsp.data[0] = 0x19;
    rsp.data[1] = 0x60;

    raw_data = (rsp.data[0] << 8) | rsp.data[1];
    t = resolution * (raw_data >> 4);
    t /= 1000;
    temperature = (int16_t)t;

    printf("t=%d\n",temperature);

    rsp.data[0] = 0x1e;
    rsp.data[1] = 0x20;

    raw_data = (rsp.data[0] << 8) | rsp.data[1];
    t = resolution * (raw_data >> 4);
    t /= 1000;
    temperature = (int16_t)t;

    printf("t=%d\n",temperature);

    return 0;
}

