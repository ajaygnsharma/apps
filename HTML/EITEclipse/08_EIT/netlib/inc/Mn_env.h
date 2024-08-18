/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef MN_ENV_H_INC
#define MN_ENV_H_INC 1

/* processor specific includes */

#if (defined(POLARM2368) || defined(CMXARM2368) || \
   defined(POLARM2468) || defined(CMXARM2468))
#if (defined(__CC_ARM))        /* Keil Realview tools */
#include <lpc23xx.h>
#elif (defined(__GNUC__))
#include "lpc23xx.h"
#elif (defined(__IAR_SYSTEMS_ICC__))
#include "intrinsics.h"
#if (defined(POLARM2368) || defined(CMXARM2368))
#include "iolpc2368.h"
#elif (defined(POLARM2468) || defined(CMXARM2468))
#include "iolpc2468.h"
#endif
#if (__VER__ < 441)
#define VICIntEnClr     VICIntEnClear
#define VICVectCntl4    VICVectPriority4
#define VICVectCntl7    VICVectPriority7
#define VICVectCntl21   VICVectPriority21
#else
/* IAR changed the definitions after v4.41a */
#define VICIntEnable    VICINTENABLE
#define VICIntEnClr     VICINTENCLEAR
#define VICVectCntl4    VICVECTPRIORITY4
#define VICVectCntl7    VICVECTPRIORITY7
#define VICVectCntl21   VICVECTPRIORITY21
#define VICVectAddr4    VICVECTADDR4
#define VICVectAddr7    VICVECTADDR7
#define VICVectAddr21   VICVECTADDR21
#define VICAddress      VICADDRESS
#define VICIntSelect    VICINTSELECT
#endif
#endif
#endif

/* end processor specific includes */

#if (!(defined(MCHP_C18)))
#include <limits.h>     /* for CHAR_BIT, used by tcp.c */
#endif

/* MPLAB Pic18 compiler doesn't define CHAR_BIT */
#if (!defined(CHAR_BIT))
#define CHAR_BIT     8
#endif

/* processor specific defines */
#define DYNAMIC_MEM_AVAILABLE 1     /* Set to 1 if malloc and free available */
                                    /* Currently only used by FTP */
#define GET_TICK_ATOMIC       0     /* Set to 1 if instruction to get timer
                                       tick in mn_timer.c is atomic. If not
                                       sure set to 0. */
#define USE_SEND_BUFF         1     /* Set to 1 to use the standard send_buff,
                                       set to 0 if supplying your own transmit
                                       buffer for ethernet.
                                    */
#define USE_RECV_BUFF         1     /* Set to 1 to use the standard recv_buff,
                                       set to 0 if supplying your own receive
                                       buffer for ethernet.
                                    */

#if (defined(POLARM2368) || defined(CMXARM2368) || \
   defined(POLARM2378) || defined(CMXARM2378) || \
   defined(POLARM2468) || defined(CMXARM2468))

void start_xmit(void);
#if (defined(__CA__) || defined(__CC_ARM) || defined(__GNUC__))
#if (RTOS_USED == RTOS_NONE)
extern void K_OS_Disable_Interrupts (void);
extern void K_OS_Enable_Interrupts (void);
#endif      /* (RTOS_USED = RTOS_NONE) */
#endif

#if (defined(__CC_ARM))        /* Keil Realview tools */
#define MN_ONE_SECOND   100            /* number of timer ticks in one second */
#define DISABLE_INTERRUPTS    K_OS_Disable_Interrupts()
#define ENABLE_INTERRUPTS     K_OS_Enable_Interrupts()
#define XMIT_INT_ON           { MN_XMIT_BUSY_SET;  start_xmit(); }
#define MN_LITTLE_ENDIAN      1  /* set to same endianness as project files */
#elif (defined(__GNUC__))
#define MN_ONE_SECOND   100            /* number of timer ticks in one second */
#define DISABLE_INTERRUPTS    K_OS_Disable_Interrupts()
#define ENABLE_INTERRUPTS     K_OS_Enable_Interrupts()
#define XMIT_INT_ON           { MN_XMIT_BUSY_SET;  start_xmit(); }
#define MN_LITTLE_ENDIAN      1  /* set to same endianness as project files */
#elif (defined(__IAR_SYSTEMS_ICC__))
#define MN_ONE_SECOND   100            /* number of timer ticks in one second */
#define DISABLE_INTERRUPTS    __disable_interrupt()
#define ENABLE_INTERRUPTS     __enable_interrupt()
#define XMIT_INT_ON           { MN_XMIT_BUSY_SET;  start_xmit(); }
#define MN_LITTLE_ENDIAN      1  /* set to same endianness as project files */
#endif
#endif

/* end processor specific defines */

#ifndef STRCPY_DEFINED
/* This macro used by vfile.c only. */
#define STRCPY_DEFINED        1
#define STRCPY(A,B)           strcpy((char *)(A),(char *)(B))
#endif

#ifndef MEMSET_DEFINED
#define MEMSET_DEFINED        1
#define MEMSET(A,B,C)         memset((A),(B),(C))
#endif

#ifndef MN_DSP
#define MN_DSP 0
#endif

#ifndef MN_SEND_ROM_PKT
#define MN_SEND_ROM_PKT 0
#endif

/* ------------------------------------------- */
/* You shouldn't need to change anything below */
/* ------------------------------------------- */

/* Convert word16 from host to network order */
#if MN_LITTLE_ENDIAN
#define hs2net(w)    ((word16)(((w)>>8) | ((w)<<8)))
#else
#define hs2net(w)    ((word16)(w))
#endif
#define net2hs(w)    hs2net(w)

/* Convert two bytes into a word16 */
#if (defined(MCHP_C18) || defined(HI_TECH_C))
#define MK_WORD16(H,L)  ( (word16)( ((word16)(H)<<8) | (L) ) )
#elif (defined(NC30) || defined(NC308) || defined(ZILOG_Z8) || defined(ZILOG))
#define MK_WORD16(H,L)  ( (word16)( ((word16)(H)<<8) | (L) ) )
#else
#define MK_WORD16(H,L)  ( (word16)( ((H)<<8) | (L) ) )
#endif

/* Convert two bytes into a word16 for checksum routines */
#if (defined(MCHP_C18) || defined(HI_TECH_C))
#define CSUM_WORD16(H,L)   ( ( ((word16)(H)<<8) & 0x0000FFFF) | (L) )
#elif (defined(NC30) || defined(NC308) || defined(ZILOG_Z8) || defined(ZILOG))
#define CSUM_WORD16(H,L)   ( ((word16)(H)<<8) | (L) )
#else
#define CSUM_WORD16(H,L)   ( ( ((H)<<8) & 0x0000FFFF) | (L) )
#endif

/* convert a byte to a word16 and shift it into the upper byte */
#if (defined(MCHP_C18) || defined(HI_TECH_C))
#define LSHIFT8(B)   ( (word16)((word16)(B)<<8) )
#elif (defined(NC30) || defined(NC308) || defined(ZILOG_Z8) || defined(ZILOG))
#define LSHIFT8(B)   ( (word16)((word16)(B)<<8) )
#else
#define LSHIFT8(B)   ( (word16)((B)<<8) )
#endif

/* extract the low or high byte out of a word16 */
#define HIGHBYTE(w)     ((byte)((w)>>8))
#define LOWBYTE(w)      ((byte)((w)&0x00ff))

/* extract the low or high word16 out of a word32 */
#define HIGHWORD(l)     ((word16)(((word32)(l) >> 16) & 0x0000FFFF))
#define LOWWORD(l)      ((word16)((l) & 0x0000FFFF))

/* extract bytes out of a word32 */
#define WORD32_BYTE0(l) ((byte)(((word32)(l)) & 0x000000FF))
#define WORD32_BYTE1(l) ((byte)(((word32)(l) >>  8) & 0x000000FF))
#define WORD32_BYTE2(l) ((byte)(((word32)(l) >> 16) & 0x000000FF))
#define WORD32_BYTE3(l) ((byte)(((word32)(l) >> 24) & 0x000000FF))

/* convert a byte to a word32 and shift it left one byte */
#define WORD32_LSHIFT8(B)  ( (word32)((word32)(B)<<8) )

/* convert a byte to a word32 and shift it left two bytes */
#define WORD32_LSHIFT16(B)  ( (word32)((word32)(B)<<16) )

/* convert a byte to a word32 and shift it left three bytes */
#define WORD32_LSHIFT24(B)  ( (word32)((word32)(B)<<24) )

/* convert 4 bytes to a word32 */
#define MK_WORD32(A,B,C,D)  \
((word32)( ((word32)(A)<<24) | ((word32)(B)<<16) | ((word32)(C)<<8) | (D) ))

#endif      /* #ifndef MN_ENV_H_INC */
