/* interrupt handlers called from assembler stubs */
#include "wdt.h"

void cmx_undef(void);
void cmx_swi(int);
void cmx_prefetch(void);
void cmx_abort(void);
void cmx_fiq(void);

void cmx_undef(void)
{
	/* put code here */
}

/* If using Thumb mode you may want to declare the following function
   as __arm. e.g. __arm void cmx_swi(int SWI_num)
*/
void cmx_swi(int SWI_num)
{
	switch (SWI_num)
		{
		/* put code here */
		default:
			break;
		}
}

void cmx_prefetch(void)
{
	/* put code here */
}

void cmx_abort(void)
{
	WDTInit();
    /* put code here */
}

void cmx_fiq(void)
{
	/* put code here */
}
