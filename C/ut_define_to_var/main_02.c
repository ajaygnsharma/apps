/*
 * main_02.c
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

#define SNF_FIRMWARE_A_ADDR SNF_8MB_FIRMWARE_A_ADDR
#define SNF_FIRMWARE_A_SIZE SNF_8MB_FIRMWARE_A_SIZE

enum flash_type_e{
    MACRONIX_8MB,
    MACRONIX_16MB,
    WINBOND_8MB,
    WINBOND_16MB
};

uint8_t spi_type = MACRONIX_8MB;

void snf_map_load(void){
    if(spi_type == MACRONIX_8MB){
    }else{
#undef  SNF_FIRMWARE_A_ADDR
#define SNF_FIRMWARE_A_ADDR SNF_16MB_FIRMWARE_A_ADDR
    }
}

/**
 * This is a unit test. The value of the address will always be
 * 0x20000. Since the cpp macro preprocessor will run once and see
 * that the spi_type = MACRONIX_16MB and change the
 * SNF_FIRMWARE_A_ADDR to 0x020000.
 */
int main(void){
    snf_map_load();
    printf("addr=%x, size=%d\n", SNF_FIRMWARE_A_ADDR,\
            SNF_FIRMWARE_A_SIZE);
    spi_type = MACRONIX_16MB;
    snf_map_load();
    printf("addr=%x, size=%d\n", SNF_FIRMWARE_A_ADDR,\
                SNF_FIRMWARE_A_SIZE);

    return 0;
}
