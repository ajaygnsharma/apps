#ifndef flash
#define flash	1

#define IAP_LOCATION 0x7ffffff1
#define CCLK         72000000            /* System Clock CCLK */
#define M32(adr)     (*((volatile unsigned long  *) (adr)))

typedef void (*IAP)(unsigned long [], unsigned long[]);

/* Write a buffer to the flash */
void          _fl_write(unsigned char *buf);
/* Read a buffer from the flash */
unsigned char _fl_read(unsigned char *str,unsigned long location,unsigned long str_size);


/* Lower level functions: Read and write to the last sector of the flash */
extern int    __fl_ProgramLastSector(unsigned char *buf);
extern int    __fl_EraseLastSector(void);

#endif
