/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#ifndef MN_MEM_H_INC
#define MN_MEM_H_INC 1

#if ((MN_FTP_SERVER || MN_FTP_CLIENT) && MN_NEED_MEM_POOL)

#if (defined(POLNECV850) || defined(CMXNECV850) || defined(POLNECV850ES) || \
   defined(CMXNECV850ES))
extern int __sysheap[MN_MEM_POOL_SIZE>>2];
#define MEM_POOL_INIT         MEMSET((char *)__sysheap, 0, MN_MEM_POOL_SIZE)
#elif (defined(__C51__) || defined(__C166__) || defined(__CA__))  /* Keil */
#define MEM_POOL_INIT         init_mempool(&mem_pool[0],MN_MEM_POOL_SIZE)
#elif ((defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || \
    defined(CMXPIC18E) || defined(POLP18F97J60) || defined(CMXP18F97J60)) && \
   defined(HI_TECH_C))
extern char _Lheap[MN_MEM_POOL_SIZE];
#define MEM_POOL_INIT         MEMSET(_Lheap, 0, MN_MEM_POOL_SIZE)
#else
#define MEM_POOL_INIT
#endif

#else

#define MEM_POOL_INIT

#endif      /* ((MN_FTP_SERVER || MN_FTP_CLIENT) && MN_NEED_MEM_POOL) */

#endif      /* #ifndef MN_MEM_H_INC */
