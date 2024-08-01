/*
 * main.c
 *
 *  Created on: Aug 10, 2017
 *      Author: ajay.sharma
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum flash_type_e{
    MACRONIX_8MB,
    MACRONIX_16MB,
    WINBOND_8MB,
    WINBOND_16MB
};

struct snf_s{
    uint32_t snf_firmware_a_addr;
    uint32_t snf_firmware_a_size;
};

static struct snf_s macronix_8MB =
{
    .snf_firmware_a_addr = 0x010000,
    .snf_firmware_a_size = (1024 * 1024),
};

static struct snf_s macronix_16MB =
{
    .snf_firmware_a_addr = 0x010000,
    .snf_firmware_a_size = (2048 * 1024),
};

static struct snf_s *snf;

void snf_map_load(uint8_t spi_type){
    if(spi_type == MACRONIX_8MB){
        snf = &macronix_8MB;
    }else{
        snf = &macronix_16MB;
    }
}

uint32_t snf_map_get_firmware_addr(void){
    return(snf->snf_firmware_a_addr);
}

uint32_t snf_map_get_firmware_size(void){
    return(snf->snf_firmware_a_size);
}

int main(void){
    uint8_t spi_type = MACRONIX_8MB;

    snf_map_load(spi_type);
    /*printf("addr=%x, size=%d\n", \
            snf_map_get_firmware_addr(),\
            snf_map_get_firmware_size());*/

    spi_type = MACRONIX_16MB;
    snf_map_load(spi_type);
    /*printf("addr=%x, size=%d\n", \
            snf_map_get_firmware_addr(),\
            snf_map_get_firmware_size());*/

    return 0;
}
