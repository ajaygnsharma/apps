/*********************************************************
Copyright (c) CMX Systems, Inc. 2007. All rights reserved
*********************************************************/

#include "micronet.h"

#if ((MN_FTP_SERVER || MN_FTP_CLIENT) && MN_NEED_MEM_POOL)
#if (defined(POLNECV850) || defined(CMXNECV850) || defined(POLNECV850ES) || \
   defined(CMXNECV850ES))
int __sysheap[MN_MEM_POOL_SIZE>>2];
word16 __sizeof_sysheap = MN_MEM_POOL_SIZE;
#elif ((defined(POLPIC18) || defined(CMXPIC18) || defined(POLPIC18E) || \
    defined(CMXPIC18E) || defined(POLP18F97J60) || defined(CMXP18F97J60)) && \
    defined(HI_TECH_C))
char _Lheap[MN_MEM_POOL_SIZE];
#else
/* used by FTP */
byte mem_pool[MN_MEM_POOL_SIZE];
#endif      /* (defined(POLNECV850)) */

#endif      /* ((MN_FTP_SERVER || MN_FTP_CLIENT) && MN_NEED_MEM_POOL) */


