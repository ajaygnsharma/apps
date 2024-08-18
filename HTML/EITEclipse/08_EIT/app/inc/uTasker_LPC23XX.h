 /************************************************
   Mark Butcher              Bsc (Hons) MPhil MIET
   Mark@uTasker.com                www.uTasker.com

   Copyright (C) M.J.Butcher Consulting 2004..2008
   Licensed to Newport Electronics, INC
   ***********************************************
   uTasker_LPC23XX.h
   4.9.2008

   This file contains specific LPC23XX defines
   used by the uTasker code in the project

   */

#ifdef _WINDOWS
    #include "simLPC23XX.h"
#else
  #ifdef COMPILE_IAR
      #include <intrinsic.h>
  #endif
#endif

#ifdef _GNU
    #define _VOLATILE volatile
#else
    #define _VOLATILE
#endif

#define _MALLOC_ALIGN                                                    // support malloc with align option since LAN memory should be on specific boundary
#define _ALIGN_HEAP_4                                                    // ensure long word alignment
#define _LITTLE_ENDIAN                                                   // compile project in LITTLE ENDIAN mode since the LPC23xx is fixed in this mode


#define IRC_FREQUENCY    4000000                                         // internal RC nominal frequncy

#ifdef DISABLE_PLL
    #define MASTER_CLOCK CRYSTAL_FREQ
#else
    #define MASTER_CLOCK (PLL_OUTPUT_FREQ_INT/POST_DIVIDE)
#endif
#define CCLK             MASTER_CLOCK
#define PLL_OUTPUT_FREQ  MASTER_CLOCK
#define USB_CLOCK        48000000                                        // for USB to work this must be the case. It is assumed that it is correct if used.

#define PCLK             MASTER_CLOCK
#define UART_CLOCK       (PCLK/4)
#define SSP_CLOCK        (PCLK/4)
#define IIC_CLOCK        (PCLK/8)


#define CAST_POINTER_ARITHMETIC unsigned long                            // LPC23XX uses 32 bit pointers
#ifdef _LPC21XX
    #define PLL_FREQUENCY_CONFIGURATION      (((PLL_MUL-1)&0x1f) | (POST_DIVIDE<<5))
#else
    #define PLL_FREQUENCY_CONFIGURATION      (((PLL_MUL-1) & 0x7fff) | (((OSC_DIVIDE-1) & 0xff) << 16))
#endif
#define CPU_CLOCK_CONFIGURATION              ((POST_DIVIDE-1)&0x0ff)
#define USB_FREQUENCY_CONFIGURATION          (((PLL_OUTPUT_FREQ_INT/USB_CLOCK)-1)&0x0f)

#define PART_ID_LPC2364                      0x1600f902
#define PART_ID_LPC2366                      0x1600f923
#define PART_ID_LPC2368                      0x1600f925
#define PART_ID_LPC2378                      0x1600fd25
#define PART_ID_LPC2387                      not yet known

#ifdef _LPC21XX
    #define INTERRUPT_COUNT  15
    #if defined LPC2101 || defined LPC2102 || defined LPC2103
        #define FLASH_START_ADDRESS          0                           // up to 32k
        #define START_OF_FLASH               FLASH_START_ADDRESS
        #define RAM_START_ADDRESS            0x40000000                  // up to 8k

        #define FLASH_GRANULARITY_SMALL      (4*1024)
        #define FLASH_GRANULARITY_LARGE      (4*1024)

        #define FLASH_SECTOR_0_OFFSET        0
        #define FLASH_SECTOR_SIZE_0          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_1_OFFSET        0x1000
        #define FLASH_SECTOR_SIZE_1          FLASH_GRANULARITY_LARGE     // last sector in LPC2101
        #define FLASH_SECTOR_2_OFFSET        0x2000
        #define FLASH_SECTOR_SIZE_2          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_3_OFFSET        0x3000
        #define FLASH_SECTOR_SIZE_3          FLASH_GRANULARITY_LARGE     // last sector in LPC2102
        #define FLASH_SECTOR_4_OFFSET        0x4000
        #define FLASH_SECTOR_SIZE_4          FLASH_GRANULARITY_LARGE     
        #define FLASH_SECTOR_5_OFFSET        0x5000
        #define FLASH_SECTOR_SIZE_5          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_6_OFFSET        0x6000
        #define FLASH_SECTOR_SIZE_6          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_7_OFFSET        0x7000
        #define FLASH_SECTOR_SIZE_7          FLASH_GRANULARITY_LARGE     // last sector in LPC2103
    #else
        #define FLASH_START_ADDRESS          0                           // up to 128k
        #define START_OF_FLASH               FLASH_START_ADDRESS
        #define RAM_START_ADDRESS            0x40000000                  // up to 64k

        #define FLASH_GRANULARITY_SMALL      (8*1024)
        #define FLASH_GRANULARITY_LARGE      (8*1024)

        #define FLASH_SECTOR_0_OFFSET        0
        #define FLASH_SECTOR_SIZE_0          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_1_OFFSET        0x2000
        #define FLASH_SECTOR_SIZE_1          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_2_OFFSET        0x4000
        #define FLASH_SECTOR_SIZE_2          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_3_OFFSET        0x6000
        #define FLASH_SECTOR_SIZE_3          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_4_OFFSET        0x8000
        #define FLASH_SECTOR_SIZE_4          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_5_OFFSET        0xA000
        #define FLASH_SECTOR_SIZE_5          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_6_OFFSET        0xC000
        #define FLASH_SECTOR_SIZE_6          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_7_OFFSET        0xE000
        #define FLASH_SECTOR_SIZE_7          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_8_OFFSET        0x10000
        #define FLASH_SECTOR_SIZE_8          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_9_OFFSET        0x12000
        #define FLASH_SECTOR_SIZE_9          FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_10_OFFSET       0x14000
        #define FLASH_SECTOR_SIZE_10         FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_11_OFFSET       0x16000
        #define FLASH_SECTOR_SIZE_11         FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_12_OFFSET       0x18000
        #define FLASH_SECTOR_SIZE_12         FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_13_OFFSET       0x1A000
        #define FLASH_SECTOR_SIZE_13         FLASH_GRANULARITY_LARGE
        #define FLASH_SECTOR_14_OFFSET       0x1C000
        #define FLASH_SECTOR_SIZE_14         FLASH_GRANULARITY_LARGE     // last usable sector in 128k part
    #endif
#else
    #define INTERRUPT_COUNT  31
    #define FLASH_START_ADDRESS              0                           // up to 512k
    #define START_OF_FLASH                   FLASH_START_ADDRESS
    #define RAM_START_ADDRESS                0x40000000                  // up to 32k
    #define ETHERNET_RAM_START_ADDRESS       0x7fe00000                  // 16k
    #define ETHERNET_RAM_SIZE                (16 * 1024)
    #define USB_RAM_START                    0x7fd00000                  // 8k
    #define USB_RAM_SIZE                     (8 * 1024)

    #define FLASH_GRANULARITY_SMALL          (4*1024)                    // small blocks
    #define FLASH_GRANULARITY_LARGE          (32*1024)                   // large blocks

    #define FLASH_SECTOR_0_OFFSET            0
    #define FLASH_SECTOR_SIZE_0              FLASH_GRANULARITY_SMALL     // initially small blocks
    #define FLASH_SECTOR_1_OFFSET            0x1000
    #define FLASH_SECTOR_SIZE_1              FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_2_OFFSET            0x2000
    #define FLASH_SECTOR_SIZE_2              FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_3_OFFSET            0x3000
    #define FLASH_SECTOR_SIZE_3              FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_4_OFFSET            0x4000
    #define FLASH_SECTOR_SIZE_4              FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_5_OFFSET            0x5000
    #define FLASH_SECTOR_SIZE_5              FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_6_OFFSET            0x6000
    #define FLASH_SECTOR_SIZE_6              FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_7_OFFSET            0x7000
    #define FLASH_SECTOR_SIZE_7              FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_8_OFFSET            0x8000                      // end large blocks

    #define FLASH_SECTOR_SIZE_8              FLASH_GRANULARITY_LARGE     // last sector in 64k part
    #define FLASH_SECTOR_9_OFFSET            0x10000
    #define FLASH_SECTOR_SIZE_9              FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_10_OFFSET           0x18000
    #define FLASH_SECTOR_SIZE_10             FLASH_GRANULARITY_LARGE     // last sector in 128k part
    #define FLASH_SECTOR_11_OFFSET           0x20000
    #define FLASH_SECTOR_SIZE_11             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_12_OFFSET           0x28000
    #define FLASH_SECTOR_SIZE_12             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_13_OFFSET           0x30000
    #define FLASH_SECTOR_SIZE_13             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_14_OFFSET           0x38000
    #define FLASH_SECTOR_SIZE_14             FLASH_GRANULARITY_LARGE     // last sector in 256k part
    #define FLASH_SECTOR_15_OFFSET           0x40000
    #define FLASH_SECTOR_SIZE_15             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_16_OFFSET           0x48000
    #define FLASH_SECTOR_SIZE_16             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_17_OFFSET           0x50000
    #define FLASH_SECTOR_SIZE_17             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_18_OFFSET           0x58000
    #define FLASH_SECTOR_SIZE_18             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_19_OFFSET           0x60000
    #define FLASH_SECTOR_SIZE_19             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_20_OFFSET           0x68000
    #define FLASH_SECTOR_SIZE_20             FLASH_GRANULARITY_LARGE
    #define FLASH_SECTOR_21_OFFSET           0x70000
    #define FLASH_SECTOR_SIZE_21             FLASH_GRANULARITY_LARGE

    #define FLASH_SECTOR_22_OFFSET           0x78000                     // further small blocks
    #define FLASH_SECTOR_SIZE_22             FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_23_OFFSET           0x79000
    #define FLASH_SECTOR_SIZE_23             FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_24_OFFSET           0x7a000
    #define FLASH_SECTOR_SIZE_24             FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_25_OFFSET           0x7b000
    #define FLASH_SECTOR_SIZE_25             FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_26_OFFSET           0x7c000
    #define FLASH_SECTOR_SIZE_26             FLASH_GRANULARITY_SMALL
    #define FLASH_SECTOR_27_OFFSET           0x7d000
    #define FLASH_SECTOR_SIZE_27             FLASH_GRANULARITY_SMALL     // last sector in 512k part - last 8k are occupied by ISP routines
#endif

#define FLASH_LINE_SIZE                  16                              // lines made up of quad words (128 bits)
#define FLASH_ROW_SIZE                   256                             // FLASH writes are performed in rows of this size

typedef void (*IAP)(unsigned long [], unsigned long []);
#ifdef _WINDOWS
    extern void fnSimNXPFlash(unsigned long [], unsigned long []);
    #define IAP_LOCATION fnSimNXPFlash
#else
    #define IAP_LOCATION (IAP)0x7ffffff1                                 // Thumb mode execution address of Incircuit Application Programming routine
#endif

// 
//      Status Code                            Mnemonic                  Description
//
#define CMD_SUCCESS                                0                     // Command is executed successfully.
#define INVALID_COMMAND                            1                     // Invalid command.
#define SRC_ADDR_ERROR                             2                     // Source address is not on a word boundary.
#define DST_ADDR_ERROR                             3                     // Destination address is not on a correct boundary.
#define SRC_ADDR_NOT_MAPPED                        4                     // Source address is not mapped in the memory map. Count value is taken in to consideration where applicable.
#define DST_ADDR_NOT_MAPPED                        5                     // Destination address is not mapped in the memory map. Count value is taken in to consideration where applicable.
#define COUNT_ERROR                                6                     // Byte count is not multiple of 4 or is not a permitted value. 
#define INVALID_SECTOR                             7                     // Sector number is invalid.
#define SECTOR_NOT_BLANK                           8                     // Sector is not blank.
#define SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION    9                     // Command to prepare sector for write operation was not executed.
#define COMPARE_ERROR                              10                    // Source and destination data is not same.
#define BUSY                                       11                    // Flash programming hardware interface is busy.
#define PARAM_ERROR                                12                    // Insufficient number of parameters or invalid parameter.
#define ADDR_ERROR                                 13                    // Address is not on word boundary.
#define ADDR_NOT_MAPPED                            14                    // Address is not mapped in the memory map. Count value is taken into condiferation where applicable.
#define CMD_LOCKED                                 15                    // Command is locked.
#define INVALID_CODE                               16                    // Unlock code is invalid.
#define INVALID_BAUDRATE                           17                    // Invalid baud rate setting.
#define INVALID_STOP_BIT                           18                    // Invalid stop bit setting.
#define CODE_READ_PROTECTION_ENABLED               19                    // Code read protection enabled.
    

//      IAP Command                            Command Code              Status Code(s)
//
#define FLASH_PREPARE_SECTOR_TO_WRITE              50                    // CMD_SUCCESS | BUSY | INVALID_SECTOR
#define FLASH_COPY_RAM_TO_FLASH                    51                    // CMD_SUCCESS |SRC_ADDR_ERROR (Address not on word boundary) |
                                                                         // DST_ADDR_ERROR (Address not on correct boundary) | SRC_ADDR_NOT_MAPPED |
                                                                         // DST_ADDR_NOT_MAPPED | COUNT_ERROR (Byte count is not 512 | 1024 | 4096 | 8192) |
                                                                         // SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION | BUSY                                                          
#define FLASH_ERASE_SECTORS                        52                    // CMD_SUCCESS | BUSY | SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION | INVALID_SECTOR
#define FLASH_BLANK_CHECK_SECTORS                  53                    // CMD_SUCCESS |BUSY | SECTOR_NOT_BLANK | INVALID_SECTOR
#define FLASH_READ_PART_ID                         54                    // CMD_SUCCESS
#define FLASH_READ_BOOT_CODE_VERSION               55                    // CMD_SUCCESS
#define FLASH_COMPARE                              56                    // CMD_SUCCESS | COMPARE_ERROR | COUNT_ERROR (Byte count is not multiple of 4) |
                                                                         // ADDR_ERROR | ADDR_NOT_MAPPED  
#define FLASH_REVOKE_ISP                           57





