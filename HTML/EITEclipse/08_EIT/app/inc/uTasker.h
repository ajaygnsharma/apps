 /************************************************
   Mark Butcher              Bsc (Hons) MPhil MIET
   Mark@uTasker.com                www.uTasker.com

   Copyright (C) M.J.Butcher Consulting 2004..2008
   Licensed to Newport Electronics, INC
   ***********************************************
   uTasker.h
   4.9.2008

   This file contains configuration and prototypes
   used by the uTasker code in the project

   */

#ifndef __CONFIG__
#define __CONFIG__


#define _LPC23XX

#ifdef _WINDOWS
  #define MEM_FACTOR 1.2                                                 // Windows tends to use more memory so expand heap slightly
#else
  #define MEM_FACTOR 1.0
#endif


// Major hardware dependent settings for this project
//
#ifdef _LPC23XX
    // define the chip type used in the project
    //
    //#define LPC2101                                                    // 8k   FLASH,  2K RAM, LQFP48 / PLCC44
    //#define LPC2102                                                    // 16k  FLASH,  4K RAM, LQFP48 / PLCC44
    //#define LPC2103                                                    // 32k  FLASH,  8K RAM, LQFP48 / PLCC44
    //#define LPC2104                                                    // 128k FLASH, 16K RAM, LQFP48
    //#define LPC2105                                                    // 128k FLASH, 32K RAM, LQFP48
    //#define LPC2106                                                    // 128k FLASH, 64K RAM, LQFP48
    //#define LPC2364FBD100                                              // 128k FLASH, 34K RAM total, LQFP100
    //#define LPC2366FBD100                                              // 256k FLASH, 58K RAM total, LQFP100
      #define LPC2368FBD100                                              // 512k FLASH, 58K RAM total, LQFP100
    //#define LPC2378FBD144                                              // 512k FLASH, 58K RAM total, LQFP144
    //#define LPC2387FBD100                                              // 512k FLASH, 98K RAM total, LQFP100

    #if defined LPC2364FBD100 || defined LPC2366FBD100 || defined LPC2368FBD100 || defined LPC2378FBD144 || defined LPC2387FBD100
      //#define OLIMEX_LPC2378_STK                                       // low cost evaluation board from Olimex
        #define KEIL_MCB2300                                             // Keil evaluation board
        #if defined KEIL_MCB2300
            #define TARGET_HW       "KEIL MCB2300"
        #elif defined OLIMEX_LPC2378_STK
            #define TARGET_HW       "OLIMEX LPC2378-STK"
        #endif

        #define OUR_HEAP_SIZE (HEAP_REQUIREMENTS)((14*1024)*MEM_FACTOR)  // the LPC23xx uses dedicated Ethernet buffers so doesn't need heap for these
    #else
        #if defined LPC2101 || defined LPC2102 || defined LPC2103
            #define OLIMEX_LPC_P2103                                     // OLIMEX prototyping board (LPC2303)
        #else
            #define IAR_LPC210X                                          // IAR evaluation board (LPC2106)
        #endif
        #if defined IAR_LPC210X
            #define TARGET_HW           "IAR LPC210X Card"
        #elif defined OLIMEX_LPC_P2103
            #define TARGET_HW           "OLIMEX LPC-P2103"
        #endif
        #if defined LPC2101 || defined LPC2102 || defined LPC2103
            #define OUR_HEAP_SIZE (HEAP_REQUIREMENTS)((1.4*1024)*MEM_FACTOR)
            #define NO_FLASH_SUPPORT                                     // no parameter or file system
        #else
            #define OUR_HEAP_SIZE (HEAP_REQUIREMENTS)((6*1024)*MEM_FACTOR)
        #endif

      //#define PLCC44                                                   // PLCC44 rather than LQFP48

        #define DEVICE_WITHOUT_ETHERNET                                  // LPC21XX has no Ethernet interface
        #define _LPC21XX                                                 // LPC21XX subset
    #endif

    #define TICK_RESOLUTION      50                                      // 50 ms system time period - max possible with LPC23XX at 70MHz PIT would be about 582s !

    #define DEVICE_WITHOUT_USB                                           // LPC23XX doesn't support USB yet
#endif


/**************** Specify a file system for use by FTP, HTML and such functions *******************************/
#ifndef NO_FLASH_SUPPORT
  //#define USE_PARAMETER_BLOCK                                          // enable a parameter block for storing and retrieving non-volatile information
  //#define USE_PAR_SWAP_BLOCK                                           // we support a backup block which can be restored if desired (it is recommended to use this together with USE_PARAMETER_BLOCK

  //#define SPI_FILE_SYSTEM                                              // we have an external file system via SPI interface
    #define FLASH_FILE_SYSTEM                                            // we have an internal file system in FLASH
  //#define NVRAM                                                        // we have an external file system in non-volatile RAM
#endif

#if defined (SPI_FILE_SYSTEM) || defined (FLASH_FILE_SYSTEM)
    #define ACTIVE_FILE_SYSTEM
#endif

#define SUPPORT_MIME_IDENTIFIER                                          // if the file type is to be handled (eg. when mixing HTML with JPGs etc.) this should be set - note that the file system header will be adjusted

#ifdef FLASH_FILE_SYSTEM
    #ifdef SPI_FILE_SYSTEM
      //#define SPI_FLASH_SST25                                          // use SST SPI FLASH
        #define SPI_FLASH_ST                                             // use ST FLASH rather than ATMEL
      //#define SPI_DATA_FLASH                                           // FLASH type is data FLASH supporting sub-sectors (relevant for ST types)
        #if defined SPI_FLASH_ST
            #define FILE_GRANULARITY (SPI_FLASH_BLOCK_LENGTH)            // (65535/4096 byte blocks) file granularity is equal to a multiple of the FLASH granularity (as defined by the device)
        #elif defined SPI_FLASH_SST25
            #define FILE_GRANULARITY (SPI_FLASH_BLOCK_LENGTH)            // (4096 byte blocks) file granularity is equal to sub-sector FLASH granularity (as defined by the device)
        #else
            #define FILE_GRANULARITY (8*SPI_FLASH_BLOCK_LENGTH)          // (4224/2112 byte blocks) file granularity is equal to a multiple of the FLASH granularity (as defined by the device)
        #endif
    #else
        #define FILE_GRANULARITY (FLASH_GRANULARITY)                     // file granularity is equal to the FLASH granularity (as defined by the device)
    #endif
#endif

#define uFILE_SYSTEM_START (MEMORY_RANGE_POINTER)(uFILE_START)
#define uFILE_SYSTEM_END   (MEMORY_RANGE_POINTER)(uFILE_START + FILE_SYSTEM_SIZE)
#if defined _STR91XF || defined _LPC23XX && (defined FLASH_FILE_SYSTEM && !defined SPI_FILE_SYSTEM) // positions parameter block in different FLASH bank and uses sub-files
    #define LAST_FILE_BLOCK    (unsigned short)((FILE_SYSTEM_SIZE)/FILE_GRANULARITY) // last block in our file system
    #define LAST_SUB_FILE_BLOCK  (unsigned short)((FILE_SYSTEM_SIZE)/SUB_FILE_SIZE)  // last sub-block in our file system
#else
    #define LAST_FILE_BLOCK    (unsigned short)((FILE_SYSTEM_SIZE)/FILE_GRANULARITY) // last block in our file system
#endif


typedef unsigned long     UTASK_TICK;                                    // tick counts from 0 to 0xffffffff
typedef unsigned char     CONFIG_LIMIT;                                  // up to 255 configurations possible
typedef unsigned char     NETWORK_LIMIT;                                 // up to 255 nodes possible
typedef unsigned char     TASK_LIMIT;                                    // the system supports up to 255 tasks
typedef unsigned short    STACK_REQUIREMENTS;                            // the system supports up to 64k heap
typedef unsigned short    HEAP_REQUIREMENTS;                             // the system supports up to 64k heap
typedef unsigned char     TIMER_LIMIT;                                   // the system supports up to 255 timers
typedef   signed char     UTASK_TASK;                                    // task reference
typedef unsigned char     QUEUE_LIMIT;                                   // the system supports up to 255 queues
#define QUEUE_HANDLE      QUEUE_LIMIT                                    // as many queue handles as there are queues
typedef unsigned short    QUEUE_TRANSFER;                                // the system supports transfers to 64k bytes
typedef unsigned char     PHYSICAL_Q_LIMIT;                              // the system supports up to 255 physical queues
typedef unsigned short    DELAY_LIMIT;                                   // delays up to 64k TICKs
typedef unsigned short    MAX_MALLOC;                                    // upto 64k heap chunks
typedef unsigned short    LENGTH_CHUNK_COUNT;                            // http string insertion and chunk counter for dynamic generation {}
#if defined (_M5223X) || defined (_STR91XF) || defined (_HW_SAM7X) || defined (_LM3SXXXX) || defined (_LPC23XX) // {1}
    typedef unsigned long   MAX_FILE_LENGTH;                             // over 64k file lengths
    typedef unsigned long   MAX_FILE_SYSTEM_OFFSET;                      // offsets of over 64k in file system - use for file system sizes of larger than 64k
    #ifdef GLOBAL_HARDWARE_TIMER
        typedef unsigned long   CLOCK_LIMIT;                             // 32 bit hardware timer support
    #else
        typedef unsigned short  CLOCK_LIMIT;                             // 16 bit normal hardware timer support
    #endif
#else
    #if defined _HW_NE64 && (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        typedef unsigned long   MAX_FILE_LENGTH;                         // up to 64k file lengths
        typedef unsigned long   MAX_FILE_SYSTEM_OFFSET;                  // offsets of up to 64k in file system
    #else
        typedef unsigned short  MAX_FILE_LENGTH;                         // up to 64k file lengths
        typedef unsigned short  MAX_FILE_SYSTEM_OFFSET;                  // offsets of up to 64k in file system
    #endif
    typedef unsigned short  CLOCK_LIMIT;                                 // 16 bit normal and hardware timer support
#endif

// TCP/IP support
typedef signed char       USOCKET;                                       // socket support from 0..127 (negative values are errors)

// General variable types for portability
typedef char              CHAR;

#if defined _HW_NE64 && defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM // treat pointers as long so that extended virtual address range is possible
    #define MEMORY_RANGE_POINTER   unsigned long
#else
    #define MEMORY_RANGE_POINTER   unsigned char *
#endif

typedef unsigned int size_t;

#include "uTasker_LPC23XX.h"
#include "app_hw_lpc23xx.h"

// uFile system support
extern MEMORY_RANGE_POINTER uOpenFile(CHAR *ptrfileName);
extern MAX_FILE_LENGTH uGetFileLength(MEMORY_RANGE_POINTER ptrFile);
extern MAX_FILE_LENGTH uGetFileData(MEMORY_RANGE_POINTER ptrFile, MAX_FILE_SYSTEM_OFFSET FileOffset, unsigned char *ucData, MAX_FILE_LENGTH DataLength);
#ifdef SUB_FILE_SIZE
    extern MAX_FILE_LENGTH uFileWrite(MEMORY_RANGE_POINTER ptrFile, unsigned char *ptrData, MAX_FILE_LENGTH DataLength, unsigned char ucSubFile);
    extern unsigned char fnGetFileType(CHAR *ptrFileName);
        #define SUB_FILE_TYPE 0x01
    extern MEMORY_RANGE_POINTER uOpenNextFile(MEMORY_RANGE_POINTER ptrfileLocation, MAX_FILE_LENGTH *FileLength, unsigned char ucSubfile);
    extern MEMORY_RANGE_POINTER uOpenNextMimeFile(MEMORY_RANGE_POINTER ptrfileLocation, MAX_FILE_LENGTH *FileLength, unsigned char *ucMimeType, unsigned char ucSubfile);
    extern CHAR uGetSubFileName(MEMORY_RANGE_POINTER ptrFile);
#else
    extern MAX_FILE_LENGTH uFileWrite(MEMORY_RANGE_POINTER ptrFile, unsigned char *ptrData, MAX_FILE_LENGTH DataLength);
    extern MEMORY_RANGE_POINTER uOpenNextFile(MEMORY_RANGE_POINTER ptrfileLocation, MAX_FILE_LENGTH *FileLength);
    extern MEMORY_RANGE_POINTER uOpenNextMimeFile(MEMORY_RANGE_POINTER ptrfileLocation, MAX_FILE_LENGTH *FileLength, unsigned char *ucMimeType);
#endif

extern unsigned char *fnClearUploadSpace(void); //Ajay 

extern int             uCompareFile(MEMORY_RANGE_POINTER ptrFile, unsigned char *ptrData, MAX_FILE_LENGTH DataLength);
extern CHAR            uGetFileName(MEMORY_RANGE_POINTER ptrFile);
extern MAX_FILE_LENGTH uFileClose(MEMORY_RANGE_POINTER ptrFile);
extern MAX_FILE_LENGTH uFileCloseMime(MEMORY_RANGE_POINTER ptrFile, unsigned char *ucMimeType);
extern int             uFileErase(MEMORY_RANGE_POINTER ptrFile, MAX_FILE_LENGTH FileLength);
extern void            fnProtectFile(void);
extern void            fnUnprotectFile(void);
extern unsigned char   fnConvertName(CHAR cName);

extern unsigned char fnGetMimeType(CHAR *ptrFileName);
extern const CHAR *cMimeTable[];

extern unsigned short uStrEquiv(const CHAR *ptrTo, const CHAR *ptrFrom);


#ifdef SUPPORT_MIME_IDENTIFIER                                
    #define FILE_HEADER (sizeof(MAX_FILE_LENGTH) + 1)                     // file length followed by MIME identifier
#else
    #define FILE_HEADER (sizeof(MAX_FILE_LENGTH))
#endif

#define uMemcpy memcpy
#define uMemset memset

#define uDisable_Interrupt() __disable_irq()
#define uEnable_Interrupt()  __enable_irq()

// FLASH support
extern int  fnEraseFlashSector(MEMORY_RANGE_POINTER ptrSector, MAX_FILE_LENGTH Length);
extern int  fnWriteBytesFlash(MEMORY_RANGE_POINTER ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length);
extern void fnProtectFlash(unsigned char *ptrSector, unsigned char ucProtection);
  #define PROTECT_SECTOR   1
  #define UNPROTECT_SECTOR 0

#ifdef _WINDOWS
    extern unsigned char *fnGetFlashAdd(unsigned char *ucAdd);           // routine to map real hardware address to simulated FLASH
    extern unsigned char *fnPutFlashAdd(unsigned char *ucAdd);           // inverse of fnGetFlashAdd
    extern int  fnIsProtected(unsigned char *ptrSectorPointer);
    extern const unsigned long fnGetSectorSize(unsigned char *ptrSector);
#else
    #define fnGetFlashAdd(x) x                                           // macro to map to direct FLASH memory access
    #define fnPutFlashAdd(x) x                                           // macro to map to direct FLASH memory access
#endif
extern void fnUnprotectSector(volatile unsigned short *usSectorPointer);
extern void fnProtectSector(volatile unsigned short *usSectorPointer);

extern void fnGetParsFile(MEMORY_RANGE_POINTER ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size);

#ifdef OPSYS_CONFIG                                                      // this is only set in the hardware module
    #ifdef SUPPORT_MIME_IDENTIFIER
    const CHAR *cMimeTable[] = {                                         // keep the ordering in the table to match the MIME type defines below!!
        (const CHAR *)"HTM",                                             // HTML file - will be interpreted by web server
        (const CHAR *)"JPG",                                             // JPG image
        (const CHAR *)"GIF",                                             // GIF image
        (const CHAR *)"CSS",                                             // CSS Cascading Style Sheets
        (const CHAR *)"JS",                                              // Java script
        (const CHAR *)"BIN",                                             // binary data file
        (const CHAR *)"TXT",                                             // text data file
        (const CHAR *)"???",                                             // all other types will be displayed as unknown
    };
    #endif
#endif

// File type identifiers
#define MIME_HTML                  0
#define MIME_JPG                   1
#define MIME_GIF                   2
#define MIME_CSS                   3
#define MIME_JAVA_SCRIPT           4
#define MIME_BINARY                5
#define MIME_TXT                   6
#define UNKNOWN_MIME               7                                     // this entry is needed to terminate the list


#endif
