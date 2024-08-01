/*
 * main_03.c
 *
 *  Created on: Aug 10, 2017
 *      Author: ajay.sharma
 */


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

#define SNF_8MB_FIRMWARE_A_ADDR         (0x010000)
#define SNF_8MB_FIRMWARE_A_SIZE         (1024 * 1024)

#define SNF_16MB_FIRMWARE_A_ADDR        (0x020000)
#define SNF_16MB_FIRMWARE_A_SIZE        (2048 * 1024)

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

static struct snf_s snf;

void snf_map_load(uint8_t spi_type){
    if(spi_type == MACRONIX_8MB){
        snf.snf_firmware_a_addr = SNF_8MB_FIRMWARE_A_ADDR;
        snf.snf_firmware_a_size = SNF_8MB_FIRMWARE_A_SIZE;
    }else{
        snf.snf_firmware_a_addr = SNF_16MB_FIRMWARE_A_ADDR;
        snf.snf_firmware_a_size = SNF_16MB_FIRMWARE_A_SIZE;
    }
}

uint32_t snf_map_get_firmware_addr(void){
    return(snf.snf_firmware_a_addr);
}

uint32_t snf_map_get_firmware_size(void){
    return(snf.snf_firmware_a_size);
}

int main(void){
    uint8_t spi_type = MACRONIX_8MB;

    snf_map_load(spi_type);
#if(0)
    printf("addr=%x, size=%d\n", \
            snf_map_get_firmware_addr(),\
            snf_map_get_firmware_size());
#endif
    spi_type = MACRONIX_16MB;
    snf_map_load(spi_type);
#if(0)
    printf("addr=%x, size=%d\n", \
            snf_map_get_firmware_addr(),\
            snf_map_get_firmware_size());
#endif
    return 0;
}
