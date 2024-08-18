/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef MN_DEFS_H_INC
#define MN_DEFS_H_INC   1

#if (!defined(MNCONFIG_H_INC))
#include "mnconfig.h"
#endif      /* (!defined(MNCONFIG_H_INC)) */

#if (!defined(MN_OS_H_INC))
#include "mn_os.h"
#endif      /* (!defined(MN_OS_H_INC)) */

#if (RTOS_USED == RTOS_NONE)
typedef unsigned char byte;
typedef unsigned short word16;
typedef unsigned int bit_word16;
typedef signed short sign_word16;
typedef unsigned long word32;
typedef void (*CMX_FP)(void); 

typedef word16 TIMER_TICK_T;

#elif (RTOS_USED == RTOS_CMX_TINYP)
#if (!defined(bit_word16))
typedef unsigned int bit_word16;
#endif
typedef void (*CMX_FP)(void); 

#endif      /* (RTOS_USED == RTOS_NONE) */

typedef signed char SCHAR;

#ifndef PTR_NULL
#define PTR_NULL     (0x00)
#endif

#if (defined(POLEZ8) || defined(CMXEZ8) || defined(POLEZ8E) || defined(CMXEZ8E))
#ifndef BYTE_PTR_NULL
#define BYTE_PTR_NULL ((byte *)PTR_NULL)
#endif
#else
#ifndef BYTE_PTR_NULL
#define BYTE_PTR_NULL (PTR_NULL)
#endif
#endif

#ifndef TRUE
#define TRUE   1
#endif
#ifndef FALSE
#define FALSE  0
#endif

/*#if ((defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || \
   defined(CMXPIC18E) || defined(POLP18F97J60) || defined(CMXP18F97J60)) && \
   !(defined(HI_TECH_C) || defined(__IAR_SYSTEMS_ICC__)))
*/
#if 0
#define RECV_COUNT_T byte
#define SEND_COUNT_T byte
#else
#define RECV_COUNT_T word16
#define SEND_COUNT_T word16
#endif

#if defined(__C51__)    /* Keil 8051 */
#if (RTOS_USED == RTOS_NONE)
#define cmx_reentrant
#define cmx_const       code
#endif      /* (RTOS_USED == RTOS_NONE) */
#endif

#if (defined(_CC51))     /* Tasking 8051 */
#if (RTOS_USED == RTOS_NONE)
#define cmx_const       _rom
#endif      /* (RTOS_USED == RTOS_NONE) */
#endif      /* (defined(_CC51)) */

#if (defined(POLAVR) || defined(CMXAVR) || defined(POLAVRE) || \
   defined(CMXAVRE) || defined(POLAVRR) || defined(CMXAVRR))
#if (defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__))
#include <ina90.h>
/*********************************************************
   To use FLASH change the define below to:
   #define cmx_const flash
   otherwise change the define below to:
   #define cmx_const
*********************************************************/
#define cmx_const flash
#endif
#endif

#if ((defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || \
   defined(CMXPIC18E) || defined(POLP18F97J60) || defined(CMXP18F97J60)) && \
   !(defined(HI_TECH_C) || defined(__IAR_SYSTEMS_ICC__)))
#define cmx_const       rom
/* #define cmx_const */
#endif

#if (defined(POLX86) || defined(CMXX86))
#define cmx_const
#endif

#if (defined(__ICC8051__))           /* IAR 8051 */
#define cmx_const       const __code
#endif

#ifndef cmx_reentrant
#define cmx_reentrant
#endif
#ifndef cmx_const
#define cmx_const const
#endif

#if (defined(POLAVR) || defined(CMXAVR) || defined(POLAVRE) || defined(CMXAVRE) || \
   defined(POLAVRR) || defined(CMXAVRR))
#if (defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__))
#define PCONST_BYTE  byte cmx_const *
#endif      /* #if (defined(__IAR_SYSTEMS_ICC)) */
#elif defined(__C51__)        /* Keil 8051 */
#define PCONST_BYTE  byte cmx_const *
#elif (defined(__ICC8051__))           /* IAR 8051 */
#define PCONST_BYTE  byte cmx_const *
#else
#define PCONST_BYTE  cmx_const byte *
#endif   /* #if (defined(POLAVR) || defined(CMXAVR)) */

#if (MN_USE_LONG_FSIZE)
typedef word32 SEND_LEN_T;
#else
typedef word16 SEND_LEN_T;
#endif      /* (MN_USE_LONG_FSIZE) */

#define IP_ADDR_LEN  4     /* number of bytes in IP address */
#define ETH_ADDR_LEN 6     /* number of bytes in ethernet hardware address */

#endif   /* #ifndef MN_DEFS_H_INC */
