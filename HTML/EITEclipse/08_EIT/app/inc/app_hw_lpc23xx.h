 /************************************************
   Mark Butcher              Bsc (Hons) MPhil MIET
   Mark@uTasker.com                www.uTasker.com

   Copyright (C) M.J.Butcher Consulting 2004..2008
   Licensed to Newport Electronics, INC
   ***********************************************
   app_hw_lpc23xx.h
   4.9.2008

   This file contains specific hardware configurations
   used by the uTasker code in the project

   */

#if !defined __APP_HW_LPC23XX__
#define __APP_HW_LPC23XX__

#ifdef _WINDOWS
    #define _SIM_PORTS fnSimPorts()
#else
    #define _SIM_PORTS
#endif


#define LPC23XX_WORKAROUND_MAM_ON                                        // utilise workaround due to memory accelerator errata

//#define DISABLE_PLL                                                    // use this to define operation at oscillator/RC frequency
//#define USE_RC_OSCILLATOR                                              // run from internal RC rather than crystal source
//#define USE_32K_OSCILLATOR                                             // run from 32kHz crystal
#ifdef _LPC21XX
    #define CRYSTAL_FREQ    14745600                                     // crystal or clock input frequency (10 to 25MHz)
    #define PLL_MUL         4                                            // 1..32
    #define POST_DIVIDE     1                                            // 0,1,2 or 3 (divide1, 2, 4 or 8)

    #define PLL_OUTPUT_FREQ_INT (CRYSTAL_FREQ*PLL_MUL)
                                                                         // rules for setting the frequency
    #if CRYSTAL_FREQ < 10000000 || CRYSTAL_FREQ > 25000000               // note that the LPC2101/2/3 can use a crystal in the range 1MHz..25MHz,, but only when neither the PLL nor the Boot loader is used.
        #error Clock frequency out of range: must be between 10 and 25MHz
    #endif
    #if POST_DIVIDE > 3
        #error Divider value must be 0,1,2 or 3 (0 = 1, 1 = 2, 2 = 4, 3 = 8)
    #endif
    #if PLL_MUL < 1 || PLL_MUL > 32
        #error Multiplier value must be between 1 and 32
    #endif
    #if defined LPC2104 || defined LPC2105 || defined LPC2106
        #if ((PLL_OUTPUT_FREQ_INT*2*(1<<POST_DIVIDE)) < 156000000) || ((PLL_OUTPUT_FREQ_INT*2*(1<<POST_DIVIDE)) > 320000000)
            #error PLL range invalid: intermediate frequency not within 126 and 320MHz range
        #endif
    #else
        #if ((PLL_OUTPUT_FREQ_INT*2*(1<<POST_DIVIDE)) < 126000000) || ((PLL_OUTPUT_FREQ_INT*2*(1<<POST_DIVIDE)) > 320000000)
            #error PLL range invalid: intermediate frequency not within 126 and 320MHz range
        #endif
    #endif
    #if (PLL_OUTPUT_FREQ_INT < 10000000) || (PLL_OUTPUT_FREQ_INT > 60000000)
        #error CPU speed invalid: not within 10 and 60MHz range
    #endif
#else
    #if defined USE_RC_OSCILLATOR
        #define CRYSTAL_FREQ           IRC_FREQUENCY
    #elif defined USE_32K_OSCILLATOR
        #define CRYSTAL_FREQ           32768
    #else
        #define CRYSTAL_FREQ           12000000                          // crystal or clock input frequency (1 to 24MHz)
    #endif
    #define OSC_DIVIDE      1                                            // 1..255
    #define PLL_MUL         12                                           // 1..32768
    #define POST_DIVIDE     4                                            // 1..256

    // Rules for setting the frequency
    // The input can be divided by between 1 and 256
    // The PLL input must be in the range 32kHz to 50MHz
    // The PLL can multiply by between 1 and 32768
    // The PLL frequency must be in the range 275MHz and 550MHz
    // The output frequence is equal to (2 * PLL_MUL * CRYSTAL_FREQ) / (OSC_DIVIDE * POST_DIVIDE)
    // The compiler throws an error if any setting is out of range
    // WARNING. For low clock inputs (like 32k) there are some other restrictions and so the data sheet must be carefully studied!

    // ERRATA - PLL. Devices '-' and 'A' may not operate at above 60MHz from FLASH!! Also the PLL output is restricted to 288MHz!!

    #define PLL_OUTPUT_FREQ_INT ((2*CRYSTAL_FREQ*PLL_MUL)/OSC_DIVIDE)

    #if CRYSTAL_FREQ < 1000000 || CRYSTAL_FREQ > 24000000
        #error Clock frequency out of range: must be between 1 and 24MHz
    #endif
    #if CRYSTAL_FREQ/OSC_DIVIDE < 32000 || CRYSTAL_FREQ/OSC_DIVIDE > 50000000
        #error Clock frequency out of range: PLL input frequency must be within 32kHz and 50MHz
    #endif
    #if ((((CRYSTAL_FREQ)/OSC_DIVIDE) * PLL_MUL * 2) < 275000000) || ((((CRYSTAL_FREQ)/OSC_DIVIDE) * PLL_MUL * 2) > 550000000)
        #error PLL range invalid: intermediate frequency not within 275 and 550MHz range
    #endif

    // Check that the USB frequency can be generated from PLL output
    #if PLL_OUTPUT_FREQ_INT != 288000000 && PLL_OUTPUT_FREQ_INT != 384000000 && PLL_OUTPUT_FREQ_INT != 480000000
        #error USB Clock frequency out of range: must be exactly 48MHz!!
    #endif
#endif



#ifdef _LPC23XX
    #if defined LPC2378FBD144
        #define DEVICE_144_PIN                                           // LPC2378 as opposed to LPC2364, LPC2366, LPC2368
        #define SIZE_OF_FLASH (512*1024)                                 // 512k FLASH
        #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2378        // Part id which can be read via ISP/IAP
    #elif defined (LPC2387FBD100)
        #define SIZE_OF_FLASH (512*1024)                                 // 512k FLASH
        #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2378        // Part id which can be read via ISP/IAP
    #elif defined (LPC2368FBD100)
        #define SIZE_OF_FLASH (512*1024)                                 // 512k FLASH
        #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2368        // Part id which can be read via ISP/IAP
    #elif defined (LPC2366FBD100)
        #define SIZE_OF_FLASH (256*1024)                                 // 256k FLASH
        #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2366        // Part id which can be read via ISP/IAP
    #else
        #define SIZE_OF_FLASH (128*1024)                                 // 128k FLASH
        #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2364        // Part id which can be read via ISP/IAP
    #endif
#else
    #if defined LPC2101
        #define SIZE_OF_FLASH (8*1024)                                   // 8k FLASH
        #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2101        // Part id which can be read via ISP/IAP
    #elif defined LPC2102
        #define SIZE_OF_FLASH (16*1024)                                  // 16k FLASH
        #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2102        // Part id which can be read via ISP/IAP
    #elif defined LPC2103
        #define SIZE_OF_FLASH (32*1024)                                  // 32k FLASH
        #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2101        // Part id which can be read via ISP/IAP
    #elif defined LPC2104
        #define SIZE_OF_FLASH (128*1024)                                 // 128k FLASH
        #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2104        // Part id which can be read via ISP/IAP
    #elif defined (LPC2105)
        #define SIZE_OF_FLASH (128*1024)                                 // 128k FLASH
        #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2105        // Part id which can be read via ISP/IAP
    #else                                                                // (LPC2106)
        #define SIZE_OF_FLASH (128*1024)                                 // 128k FLASH
        #define PART_IDENTIFICATION_NUMBER        PART_ID_LPC2106        // Part id which can be read via ISP/IAP
    #endif
#endif

// Memory set up for this target
//


#define FLASH_GRANULARITY          FLASH_GRANULARITY_LARGE               // smallest sector which can be erased independently
#define MAX_SECTOR_PARS            ((FLASH_GRANULARITY - (2*FLASH_LINE_SIZE))/FLASH_LINE_SIZE) // The number of user bytes fitting into first parameter block


// SPI FLASH system setup
//
#if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM 
    #define EEPROM_32K                                                   // Device size used
#else
    #define FILES_ADDRESSABLE                                            // file system is addressable (doesn't have to be retrieved)
#endif

#ifdef EEPROM_16K
    #define EEPROM_PAGE_SIZE 64                                          // defined by device

    #define FILE_GRANULARITY (1024)                                      // File made up of 1k byte FLASH blocks
    #define SINGLE_FILE_SIZE (1*FILE_GRANULARITY)                        // each file a multiple of 1k
    #define FILE_SYSTEM_SIZE (16*SINGLE_FILE_SIZE)                       // 16k reserved for file system (including parameter blocks)
#endif
    #ifdef EEPROM_32K
    #define EEPROM_PAGE_SIZE 64                                          // defined by device

    #define FILE_GRANULARITY (1024)                                      // File made up of 1k byte FLASH blocks
    #define SINGLE_FILE_SIZE (1*FILE_GRANULARITY)                        // each file a multiple of 1k
    #define FILE_SYSTEM_SIZE (32*SINGLE_FILE_SIZE)                       // 32k reserved for file system (including parameter blocks)
#endif
#ifdef EEPROM_64K
    #define EEPROM_PAGE_SIZE 128                                         // respect larger page size in SPI EEPROM

    #define FILE_GRANULARITY (2*1024)                                    // File made up of 2k byte FLASH blocks
    #define SINGLE_FILE_SIZE (1*FILE_GRANULARITY)                        // each file a multiple of 2k
    #define FILE_SYSTEM_SIZE (32*SINGLE_FILE_SIZE)                       // 64k reserved for file system (including parameter blocks)
#endif

//#define SPI_FLASH_MULTIPLE_CHIPS                                       // activate when multiple physical chips are used
#define CS0_LINE            PORT0_BIT14                                  // CS0 line used when SPI FLASH is enabled
#define CS1_LINE            PORT0_BIT17                                  // CS1 line used when extended SPI FLASH is enabled
#define CS2_LINE            PORT0_BIT18                                  // CS2 line used when extended SPI FLASH is enabled
#define CS3_LINE            PORT0_BIT19                                  // CS3 line used when extended SPI FLASH is enabled
#define CS_SET_PORT         FIO0SET
#define CS_CLR_PORT         FIO0CLR
#define SPI_CS0_PORT        FIO0PIN
#define SPI_CS1_PORT        FIO0PIN
#define SPI_CS2_PORT        FIO0PIN
#define SPI_CS3_PORT        FIO0PIN

#define PCSSP_X             PCSSP1                                       // use SSP1
#define SSPCR0_X            SSP1CR0
#define SSPCR1_X            SSP1CR1
#define SSPCPSR_X           SSP1CPSR
#define SSPDR_X             SSP1DR
#define SSPSR_X             SSP1SR

#define CONFIGURE_SPI_PINS()        PINSEL0 |= (PINSEL0_P0_8_MISO1 | PINSEL0_P0_9_MOSI1 | PINSEL0_P0_7_SCK1); // MOSI, MISO and SCK pins enabled - on SSP1
// Alternative possibilities for SSP1 on LPC2378 only
//#define CONFIGURE_SPI_PINS()        PINSEL0 |=(PINSEL0_P0_12_MISO1 | PINSEL0_P0_13_MOSI1 | PINSEL0_P0_7_SCK1); PINSEL3 |= PINSEL3_P1_31_SCK1;
                                                                         
#ifdef SPI_FLASH_ST                                                      // ST SPI FLASH used
    #define SPI_FLASH_STM25P40                                           // the available ST chips
  //#define SPI_FLASH_STM25P80                                               
  //#define SPI_FLASH_STM25P16                                               
  //#define SPI_FLASH_STM25P32                                               
  //#define SPI_FLASH_STM25P64                                               

    #if defined SPI_FLASH_STM25P40
        #define SPI_FLASH_PAGES             (8*256)
    #elif defined SPI_FLASH_STM25P80
        #define SPI_FLASH_PAGES             (16*256)
    #elif defined SPI_FLASH_STM25P16
        #define SPI_FLASH_PAGES             (32*256)
    #elif defined SPI_FLASH_STM25P32
        #define SPI_FLASH_PAGES             (64*256)
    #elif defined SPI_FLASH_STM25P64
        #define SPI_FLASH_PAGES             (128*256)
    #endif

    #define SPI_FLASH_PAGE_LENGTH 256
    #ifdef SPI_DATA_FLASH
        #define SPI_FLASH_SECTOR_LENGTH (16*SPI_FLASH_PAGE_LENGTH)       // sub-sector size of data FLASH
    #else
        #define SPI_FLASH_SECTOR_LENGTH (256*SPI_FLASH_PAGE_LENGTH)      // sector size of code FLASH
    #endif
    #define SPI_FLASH_BLOCK_LENGTH  SPI_FLASH_SECTOR_LENGTH
#else
    #define SPI_FLASH_PAGE_LENGTH 264                                    // standard page size (B-device only allows 264)
    //#define SPI_FLASH_PAGE_LENGTH 256                                  // size when power of 2 mode selected (only possible on D-device)
    #define SPI_FLASH_BLOCK_LENGTH (8*SPI_FLASH_PAGE_LENGTH)             // block size - a block can be deleted
    #define SPI_FLASH_SECTOR_LENGTH (64*4*SPI_FLASH_PAGE_LENGTH)         // exception sector 0a is 2k and sector 0b is 62k
    #define SPI_FLASH_PAGES         2*1024                               // 512k part expected
#endif
#define SPI_DATA_FLASH_0_SIZE   (SPI_FLASH_PAGES*SPI_FLASH_PAGE_LENGTH) 
#define SPI_DATA_FLASH_1_SIZE   SPI_DATA_FLASH_0_SIZE 
#define SPI_DATA_FLASH_2_SIZE   SPI_DATA_FLASH_0_SIZE
#define SPI_DATA_FLASH_3_SIZE   SPI_DATA_FLASH_0_SIZE
#ifdef SPI_FLASH_MULTIPLE_CHIPS
    #define SPI_FLASH_DEVICE_COUNT  4
    #if SPI_FLASH_DEVICE_COUNT >= 4
        #define SPI_DATA_FLASH_SIZE     (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE + SPI_DATA_FLASH_3_SIZE)
        #define CONFIGURE_CS_LINES()    FIO0SET = (CS0_LINE | CS1_LINE | CS2_LINE | CS3_LINE); FIO0DIR |= (CS0_LINE | CS1_LINE | CS2_LINE | CS3_LINE); _SIM_PORTS
    #elif SPI_FLASH_DEVICE_COUNT >= 3
        #define SPI_DATA_FLASH_SIZE     (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE)
        #define CONFIGURE_CS_LINES()    FIO0SET = (CS0_LINE | CS1_LINE | CS2_LINE); FIO0DIR |= (CS0_LINE | CS1_LINE | CS2_LINE); _SIM_PORTS
    #else
        #define SPI_DATA_FLASH_SIZE     (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)
        #define CONFIGURE_CS_LINES()    FIO0SET = (CS0_LINE | CS1_LINE); FIO0DIR |= (CS0_LINE | CS1_LINE); _SIM_PORTS
    #endif
#else
    #define SPI_DATA_FLASH_SIZE         SPI_DATA_FLASH_0_SIZE
    #define CONFIGURE_CS_LINES()        FIO0SET = CS0_LINE; FIO0DIR |= CS0_LINE; _SIM_PORTS
#endif


#define SPI_FLASH_START        (FLASH_START_ADDRESS + SIZE_OF_FLASH)     // SPI FLASH starts immediately after FLASH

#define SW_UPLOAD_FILE()       (*ucIp_Data == 'H') && (*(ucIp_Data+1) == 'S') && (*(ucIp_Data+2) == '.')  && (fnSPI_Flash_available() != 0)


// FLASH based File System setup
//
#ifdef FLASH_FILE_SYSTEM
    #if defined SPI_FILE_SYSTEM                                          // this is a test setup for external SPI FLASH, with the parameters at the end of internal FLASH
        #define uFILE_START (SPI_FLASH_START - PAR_BLOCK_SIZE)           // FLASH location at 2 parameter blocks short of end of internal FLASH
        #define SINGLE_FILE_SIZE (FILE_GRANULARITY)                      // each file a multiple of 16k
        #define FILE_SYSTEM_SIZE (32*SINGLE_FILE_SIZE + PAR_BLOCK_SIZE)  // 512k reserved for file system (plus parameter blocks)
    #else
        #ifdef _LPC21XX
            #define uFILE_START (FLASH_START_ADDRESS + FLASH_SECTOR_11_OFFSET)// FLASH location at 88k start
            #define SUB_FILE_SIZE    (FILE_GRANULARITY / 4)              // 2k sub file sizes
            #define SINGLE_FILE_SIZE (1*FLASH_GRANULARITY_LARGE)         // each file a multiple of 8k
            #define FILE_SYSTEM_SIZE (4*FLASH_GRANULARITY_LARGE)         // 32k reserved for file system (excluding parameter blocks)
        #else
            #define uFILE_START (FLASH_START_ADDRESS + FLASH_SECTOR_15_OFFSET)// FLASH location at 256k start
            #define SUB_FILE_SIZE    (FILE_GRANULARITY / 8)              // 4k sub file sizes
            #define SINGLE_FILE_SIZE (1*FLASH_GRANULARITY_LARGE)         // each file a multiple of 32k
            #define FILE_SYSTEM_SIZE ((7*FLASH_GRANULARITY_LARGE) + (4*FLASH_GRANULARITY_SMALL)) // 240k reserved for file system (plus 8k parameter blocks)
        #endif
    #endif
#endif


#define SAVE_COMPLETE_FLASH                                              // when simulating, save complete flash contents and not just the file system contents


#ifdef USE_PARAMETER_BLOCK
    #ifdef _LPC21XX
            #define PARAMETER_BLOCK_GRANULARITY     FLASH_GRANULARITY_SMALL
            #define PARAMETER_BLOCK_SIZE    PARAMETER_BLOCK_GRANULARITY  // use the smallest size for the device (the parameter block must be positioned in appropriate sectors)
            #define PARAMETER_BLOCK_START   FLASH_SECTOR_9_OFFSET        // 72k
            #ifdef USE_PAR_SWAP_BLOCK
                #define PAR_BLOCK_SIZE  (2*PARAMETER_BLOCK_SIZE)
            #else
                #define PAR_BLOCK_SIZE  (1*PARAMETER_BLOCK_SIZE)
            #endif
    #else
        #define PARAMETER_BLOCK_GRANULARITY     FLASH_GRANULARITY_SMALL
        #define PARAMETER_BLOCK_SIZE    PARAMETER_BLOCK_GRANULARITY      // use the smallest size for the device (the parameter block must be positioned in appropriate sectors)
        #define PARAMETER_BLOCK_START   FLASH_SECTOR_26_OFFSET           // 496k
        #ifdef USE_PAR_SWAP_BLOCK
            #define PAR_BLOCK_SIZE  (2*PARAMETER_BLOCK_SIZE)
        #else
            #define PAR_BLOCK_SIZE  (1*PARAMETER_BLOCK_SIZE)
        #endif
    #endif
#else
    #define PAR_BLOCK_SIZE  (0)
#endif



#endif

