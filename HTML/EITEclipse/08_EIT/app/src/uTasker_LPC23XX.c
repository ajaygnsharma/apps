 /************************************************
   Mark Butcher              Bsc (Hons) MPhil MIET
   Mark@uTasker.com                www.uTasker.com

   Copyright (C) M.J.Butcher Consulting 2004..2008
   Licensed to Newport Electronics, INC
   ***********************************************
   uTasker_LPC23XX.c
   4.9.2008

   This file contains extracts from the uTasker
   LPC23XX HW interface, as required to support
   the uFileSystem used to enable software uploads

   */

#define _LPC23XX
#ifdef _LPC23XX

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */

#define OPSYS_CONFIG                                                     // this module owns the operating system configuration
#define INITHW  static
#define _SIM_PORT_CHANGE
#include "uTasker.h"
#include <string.h>


/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */

#ifdef COMPILE_IAR
    #define __interrupt   __irq __arm
    #ifdef _LPC21XX
        #define acknowledge_irq()   VICVectAddr = 0;
    #else
        #define acknowledge_irq()   VICAddress = 0;                      // IAR enables direct calls to interrupt routine
    #endif
#else
    #define __interrupt__
    #define __interrupt
    #define __root
    #define acknowledge_irq()                                            // GNU has a problem doing this so uses a dispatcher in assember which handles the acknowledge
#endif


/* =================================================================== */
/*                       local structure definitions                   */
/* =================================================================== */


/* =================================================================== */
/*                global function prototype declarations               */
/* =================================================================== */


/* =================================================================== */
/*                 local function prototype declarations               */
/* =================================================================== */

#if !defined SPI_INTERFACE && (defined SPI_FILE_SYSTEM || defined SPI_SW_UPLOAD)
    static void fnConfigSPIFileSystem(unsigned char ucSpeed); 
    #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        #if !defined SPI_FLASH_ST
            #define SPI_FLASH_ATMEL                                      // default if not otherwise defined
        #endif
        #define _SPI_DEFINES
            #include "spi_flash_lpc_atmel.h"
            #include "spi_flash_lpc_stmicro.h"
        #undef _SPI_DEFINES
    #endif
#endif

/* =================================================================== */
/*                             constants                               */
/* =================================================================== */


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */

#ifdef FLASH_FILE_SYSTEM
    static unsigned long ulFlashRow[FLASH_ROW_SIZE/sizeof(unsigned long)]; // FLASH row backup buffer (on word boundary)
    static unsigned char *ucFlashRow = (unsigned char *)ulFlashRow;      // pointer to array of bytes
#endif

#if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    #if !defined SPI_FLASH_DEVICE_COUNT
        #define SPI_FLASH_DEVICE_COUNT 1
    #endif
    static int SPI_FLASH_Danger[SPI_FLASH_DEVICE_COUNT] = {0};           // signal that the FLASH status should be checked before using since there is a danger that it is still busy
    static unsigned char ucSPI_FLASH_Type[SPI_FLASH_DEVICE_COUNT];       // list of attached FLASH devices

    #ifdef SPI_FLASH_MULTIPLE_CHIPS
        unsigned long ulChipSelect[SPI_FLASH_DEVICE_COUNT] = {
            CS0_LINE,
            CS1_LINE                                                     // at least 2 expected when multiple devices are defined
        #ifdef CS2_LINE
            ,CS2_LINE
            #ifdef CS3_LINE
            ,CS3_LINE
            #endif
        #endif
        };
        #define EXTENDED_CS , &iChipSelect
        #define _EXTENDED_CS  iChipSelect,
    #else
        #define EXTENDED_CS
        #define _EXTENDED_CS
    #endif
#endif


/* =================================================================== */
/*                     global variable definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                      local function definitions                     */
/* =================================================================== */

/* =================================================================== */
/*                      global function definitions                    */
/* =================================================================== */



extern void fnInitFlashDriver(void)
{
    uMemset(ucFlashRow, 0xff, FLASH_ROW_SIZE);                           // flush the FLASH ROW backup buffer when starting
}


extern unsigned char *fnClearUploadSpace(void)
{
  unsigned long *ptrFlash    = (unsigned long *)fnGetFlashAdd(uFILE_SYSTEM_START); // start of file system in FLASH
  unsigned long *ptrFlashEnd = (unsigned long *)fnGetFlashAdd(uFILE_SYSTEM_END); // end of file system
  unsigned char *ptrNonBlankStart, *ptrNonBlankStop = 0;

  while (ptrFlash < ptrFlashEnd) 
  {
    if (*ptrFlash != 0xffffffff) 
    {
      // check that all locations are deleted
      ptrNonBlankStart = (unsigned char *)ptrFlash++;
      // the first non-blank location found
      while (ptrFlash < ptrFlashEnd) 
      {
        if (*ptrFlash != 0xffffffff) 
        {
          // check that all locations are deleted
          ptrNonBlankStop = (unsigned char *)ptrFlash;
          // the last non-blank location found
        }
        ptrFlash++;
      }
      fnEraseFlashSector(fnPutFlashAdd(ptrNonBlankStart), (MAX_FILE_LENGTH)(ptrNonBlankStop - ptrNonBlankStart)); 
      // delete thememory containing non-deleted FLASH
      
      return fnPutFlashAdd(ptrNonBlankStart);
      // return the first non-blank address found
    }
    ptrFlash++;
  }
  return 0;
  // the file system content is empty
}


#ifdef FLASH_FILE_SYSTEM
static unsigned long fnGetFlashSector(unsigned long FlashAddress)
{	
    #ifdef _LPC21XX
    return (FlashAddress/FLASH_GRANULARITY_LARGE);
    #else
    FlashAddress -= FLASH_START_ADDRESS;
    if (FlashAddress < FLASH_SECTOR_8_OFFSET) {                          // one of the first small sectors
        return (FlashAddress/FLASH_GRANULARITY_SMALL);
    }
    else if (FlashAddress < FLASH_SECTOR_22_OFFSET) {                    // one of the middle large sectors
        FlashAddress -= FLASH_SECTOR_8_OFFSET;
        return ((FlashAddress/FLASH_GRANULARITY_LARGE) + 8);
    }
    else {                                                               // one of the last small sectors
        FlashAddress -= FLASH_SECTOR_22_OFFSET;
        return ((FlashAddress/FLASH_GRANULARITY_SMALL) + 22);
    }
    #endif
}


static int fnFlashCommand(int iCommand, unsigned char *ptrFlashAddress, MAX_FILE_LENGTH Length)
{
#define WRITE_FLASH_BUFFER 0
#define DELETE_FLASH       1
    IAP iap_entry = IAP_LOCATION;
    unsigned long command[5];                                            // command buffer to be passed to IAP routine
    unsigned long result[3];                                             // result buffer for use by the IAP routine
    switch (iCommand) {
    case WRITE_FLASH_BUFFER:                                             // write a present FLASH row backup buffer to the FLASH memory
        {
            unsigned long ulSector = fnGetFlashSector((unsigned long)ptrFlashAddress);
            command[0] = FLASH_PREPARE_SECTOR_TO_WRITE;                  // prepare the sector for the command
            command[1] = ulSector;
            command[2] = ulSector;
            uDisable_Interrupt();                                        // protect FLASH routine from interrupts
            iap_entry(command, result);                                  // call the IAP routine
            uEnable_Interrupt();
            if (result[0] != CMD_SUCCESS) {
                return 1;                                                // sector invalid or busy
            }
            command[0] = FLASH_COPY_RAM_TO_FLASH;
            command[1] = (unsigned long)ptrFlashAddress;                 // flash destination start address (256 byte boundard)
            command[2] = (unsigned long)ucFlashRow;                      // source ram address is always the ROW backup buffer (word aligned)
            command[3] = FLASH_ROW_SIZE;                                 // allowed: 256 | 512 | 1024 | 4096 | 8192 - we use always the row size
            command[4] = (MASTER_CLOCK/1000);                            // system clock frequency (CCLK) in kHz
            uDisable_Interrupt();                                        // protect FLASH routine from interrupts
            iap_entry(command, result);                                  // call the IAP routine
            uEnable_Interrupt();
            if (result[0] != CMD_SUCCESS) {
                return 1;                                                // write error
            }
        }
        break;

    case DELETE_FLASH:                                                   // delete a sector or several sectors in FLASH
        {
            unsigned long ulSectorStart = fnGetFlashSector((unsigned long)ptrFlashAddress);           // get the first sector to be deleted
            unsigned long ulSectorEnd   = fnGetFlashSector((unsigned long)(ptrFlashAddress + (Length - 1)));// get the last sector to be deleted
            if (Length == 0) {                                           // don't delete if length 0 is given
                break;
            }
            command[0] = FLASH_PREPARE_SECTOR_TO_WRITE;                  // prepare the sector for the command
            command[1] = ulSectorStart;
            command[2] = ulSectorEnd;
            uDisable_Interrupt();                                        // protect FLASH routine from interrupts
            iap_entry(command, result);                                  // call the IAP routine
            uEnable_Interrupt();
            if (result[0] != CMD_SUCCESS) {
                return 1;                                                // sector invalid or busy
            }
            command[0] = FLASH_ERASE_SECTORS;
            command[3] = (MASTER_CLOCK/1000);                            // system clock frequency (CCLK) in kHz
            uDisable_Interrupt();                                        // protect FLASH routine from interrupts
            iap_entry(command, result);                                  // call the IAP routine
            uEnable_Interrupt();
            if (result[0] != CMD_SUCCESS) {
                return 1;                                                // erase error
            }
        }
        break;
    }
    return 0;
}
#endif


static void fnGetPars(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size);

extern void fnGetParsFile(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
    fnGetPars(ParLocation, ptrValue, Size);
}



#if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM) 
    #define _SPI_FLASH_INTERFACE                                         // insert manufacturer dependent code
        #include "spi_flash_lpc_atmel.h"
        #include "spi_flash_lpc_stmicro.h"
    #undef _SPI_FLASH_INTERFACE

// Routine to request local SPI FLASH type
//
extern unsigned char fnSPI_Flash_available(void)
{
    return ucSPI_FLASH_Type[0];
}


#ifdef SPI_FLASH_MULTIPLE_CHIPS 

extern unsigned char fnSPI_FlashExt_available(int iExtension)
{
    if (iExtension > SPI_FLASH_DEVICE_COUNT) {
        return 0;
    }
    return ucSPI_FLASH_Type[iExtension];
}

// Calculate the device to access from the absolute address. It is assumed that all devices are operational.
//
static int fnGetSPI_Device(unsigned char **ptrFlash)                     // define the device to be accessed
{
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE)) {
        return 0;                                                        // address is in the first SPI device
    }
    #if SPI_FLASH_DEVICE_COUNT >= 2
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)) {
        *ptrFlash -= SPI_DATA_FLASH_0_SIZE;
        return 1;                                                        // address is in the second SPI device
    }
        #if SPI_FLASH_DEVICE_COUNT >= 3
    if (*ptrFlash < (unsigned char *)(SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE)) {
        *ptrFlash -= (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE);
        return 2;                                                        // address is in the third SPI device
    }
    *ptrFlash -= (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE + SPI_DATA_FLASH_2_SIZE);
    return 3;                                                            // address is in the fourth and last SPI device

        #else
    *ptrFlash -= (SPI_DATA_FLASH_0_SIZE + SPI_DATA_FLASH_1_SIZE);
    return 2;                                                            // address is in the third and last SPI device
        #endif
    #else
    *ptrFlash -= SPI_DATA_FLASH_0_SIZE;
    return 1;                                                            // address is in the second and last SPI device
    #endif
}
#endif

// Return the page number and optionally the address offset in the page
//
#ifdef SPI_FLASH_MULTIPLE_CHIPS
    static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset, int *iChipSelect)
#else
    static unsigned short fnGetSPI_FLASH_location(unsigned char *ptrSector, unsigned short *usPageOffset)
#endif
{
    unsigned short usPageNumber;
    #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    ptrSector -= (SPI_FLASH_START);                                      // location relative to the start of the SPI FLASH chip address
    #else
    ptrSector -= (uFILE_START + FILE_SYSTEM_SIZE);                       // location relative to the start of the SPI FLASH chip address
    #endif

    #ifdef SPI_FLASH_MULTIPLE_CHIPS                                      // determine the device to address
    *iChipSelect = fnGetSPI_Device(&ptrSector);                          // define the device to be accessed
    #endif

    usPageNumber = (unsigned short)(((CAST_POINTER_ARITHMETIC)ptrSector)/SPI_FLASH_PAGE_LENGTH); // the page the address is in
    if (usPageOffset != 0) {
        *usPageOffset = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector - (usPageNumber * SPI_FLASH_PAGE_LENGTH)); // offset in the page
    }
    return usPageNumber;
}
#endif


// The LPC23xx writes blocks of 256 (FLASH_ROW_SIZE) bytes therefore these are collected here until a block boundary is crossed or else a closing command is received
//
extern int fnWriteBytesFlash(unsigned char *ucDestination, unsigned char *ucData, MAX_FILE_LENGTH Length)
{
#ifdef FLASH_FILE_SYSTEM
    static unsigned char *ptrOpenBuffer = 0;
    unsigned long ulBufferOffset = ((CAST_POINTER_ARITHMETIC)ucDestination & (FLASH_ROW_SIZE-1));
    unsigned char *ptrFlashBuffer;
    MAX_FILE_LENGTH BufferCopyLength;
#endif

    #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM) // {9}{29}
        #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)   // {29}
    if (ucDestination >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH))) // we are working from external SPI FLASH memory 
        #else
    if ((ucDestination >= (uFILE_SYSTEM_END)) && (ucDestination < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE))))  // we are working from external SPI FLASH memory
        #endif
    {
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
        #endif
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        unsigned short usDataLength;
        #ifdef FLASH_LINE_SIZE
        if (ucData == 0) {                                               // ignore command to close an open line buffer (compatibility with internal FLASH)
            return 0;
        }
        #endif
        usPageNumber = fnGetSPI_FLASH_location(ucDestination, &usPageOffset EXTENDED_CS);
        while (Length != 0) {
            usDataLength = (unsigned short)Length;
        #ifdef SPI_FLASH_ST
            if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
                usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
            }
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);        // write enable
            fnSPI_command(PAGE_PROG, (usPageNumber<<8) | usPageOffset, _EXTENDED_CS ucData, usDataLength);// copy new content
            Length -= usDataLength;
            ucData += usDataLength;
            usPageNumber++;
            usPageOffset = 0;
        #else
            if ((usPageOffset != 0) || (Length < SPI_FLASH_PAGE_LENGTH)) { // are we writing a partial page?
                fnSPI_command(MAIN_TO_BUFFER_1, usPageNumber, _EXTENDED_CS 0, 0); // copy main memory to buffer
            }
            if (usDataLength > (SPI_FLASH_PAGE_LENGTH - usPageOffset)) {
                usDataLength = (SPI_FLASH_PAGE_LENGTH - usPageOffset);
            }
            fnSPI_command(WRITE_BUFFER_1, usPageOffset, _EXTENDED_CS ucData, usDataLength);// copy new content
            fnSPI_command(PROG_FROM_BUFFER_1, usPageNumber, _EXTENDED_CS 0, 0); // program to main memory
            Length -= usDataLength;
            ucData += usDataLength;
            usPageNumber++;
            usPageOffset = 0;
        #endif
        }
        return 0;
    }
    #endif
    #ifdef FLASH_FILE_SYSTEM
    if (ucData == 0) {                                                   // close an open buffer
        ucDestination = ptrOpenBuffer;
        ulBufferOffset = FLASH_ROW_SIZE;
    }
    else {
        ptrOpenBuffer = (unsigned char *)((CAST_POINTER_ARITHMETIC)ucDestination & ~(FLASH_ROW_SIZE-1));
    }
    do {
        BufferCopyLength = (FLASH_ROW_SIZE - ulBufferOffset);            // remaining buffer space to end of present backup buffer
        if (BufferCopyLength > Length) {
            BufferCopyLength = Length;
        }
        ptrFlashBuffer = ucFlashRow + ulBufferOffset;                    // pointer set in FLASH row backup buffer
        uMemcpy(ptrFlashBuffer, ucData, BufferCopyLength);               // copy the input data to the FLASH row backup buffer
        ucData += BufferCopyLength;
        Length -= BufferCopyLength;                                      // remaining data length
        ptrFlashBuffer += BufferCopyLength;                              // next copy location
        if (ptrFlashBuffer >= (ucFlashRow + FLASH_ROW_SIZE)) {           // a complete backup buffer is ready to be copied to FLASH
            fnFlashCommand(WRITE_FLASH_BUFFER, ptrOpenBuffer, 0);        // perform the write command
            uMemset(ucFlashRow, 0xff, FLASH_ROW_SIZE);                   // flush ready for next buffer use
            ptrFlashBuffer = ucFlashRow;                                 // set pointer to start of FLASH row backup buffer
            ulBufferOffset = 0;
            ptrOpenBuffer += FLASH_ROW_SIZE;
        }
    } while (Length != 0);
    #endif
    return 0;
}

// Erase FLASH sectors. The pointer can be anywhere in the first sector to be erased
//
extern int fnEraseFlashSector(unsigned char *ptrSector, MAX_FILE_LENGTH Length)
{
#if defined FLASH_FILE_SYSTEM
    unsigned long ulPage = ((CAST_POINTER_ARITHMETIC)ptrSector & ~(FLASH_GRANULARITY-1)); // ensure page boundary
    ulPage -= FLASH_START_ADDRESS;
#endif
#ifdef PROTECTED_FILE
    int iUnprotected = 0;                                                // device presently protected
#endif
    do {
    #if defined SPI_FILE_SYSTEM && !defined FLASH_FILE_SYSTEM
        static const unsigned char ucWriteEnable[] = {M95XXX_WRITE_ENABLE, TERMINATE_WRITE}; // enable write
        unsigned short ucDeleted = 0;
        unsigned short usTemp = (unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector);

        usTemp &= ~(EEPROM_PAGE_SIZE-1);                                 // we want to erase a block (program them to 0xff)
        Length += ((unsigned short)((CAST_POINTER_ARITHMETIC)ptrSector) - usTemp);
        ptrSector = (unsigned char *)usTemp;
        while (ucDeleted < FLASH_GRANULARITY) {                          
            fnWaitWriteComplete();                                       // wait until free to write
        #ifdef SPI_INTERFACE
            fnWrite(SPI_handle, (unsigned char *)ucWriteEnable, sizeof(ucWriteEnable)); // prepare write
            fnWriteBytesEEPROM(ptrSector, ucDel, EEPROM_PAGE_SIZE);      // delete a page
        #else
            fnSendSPIMessage((unsigned char *)ucWriteEnable, sizeof(ucWriteEnable)); // prepare write
            fnWriteBytesEEPROM(ptrSector, 0, EEPROM_PAGE_SIZE);          // delete a page
        #endif
            ucDeleted += EEPROM_PAGE_SIZE;
            ptrSector += EEPROM_PAGE_SIZE;
        }
        if (Length <= FLASH_GRANULARITY) {
            break;
        }
        Length -= FLASH_GRANULARITY;
    #else                                                                // end SPI_FILE_SYSTEM
        #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
            #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        if (ptrSector >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH))) // we are working from external SPI FLASH memory
            #else
        if ((ptrSector >= (uFILE_SYSTEM_END)) && (ptrSector < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE)))) // we are working from external SPI FLASH memory 
            #endif
        {            
            #ifdef SPI_FLASH_MULTIPLE_CHIPS                              // delete in SPI FLASH
            int iChipSelect;
            #endif
            unsigned short usPageNumber, usPageOffset;
            #ifndef SPI_FLASH_ST
            unsigned short usLength;
            unsigned char  ucCommand;
            #endif
            usPageNumber = fnGetSPI_FLASH_location(ptrSector, &usPageOffset EXTENDED_CS);
            Length += usPageOffset;                                      // length with respect to start of present page
            ptrSector -= usPageOffset;                                   // set pointer to start of present page

            #ifdef SPI_FLASH_ST
            fnSPI_command(WRITE_ENABLE, 0, _EXTENDED_CS 0, 0);           // enable the write
                #ifdef SPI_DATA_FLASH
            fnSPI_command(SUB_SECTOR_ERASE, ((unsigned long)usPageNumber<<8), _EXTENDED_CS 0, 0); // delete appropriate sub-sector
                #else
            fnSPI_command(SECTOR_ERASE, ((unsigned long)usPageNumber<<8), _EXTENDED_CS 0, 0); // delete appropriate sector
                #endif
            if (Length <= SPI_FLASH_SECTOR_LENGTH) {
                break;                                                   // delete complete
            }
            ptrSector += SPI_FLASH_SECTOR_LENGTH;
            Length -= SPI_FLASH_SECTOR_LENGTH;
            #else
            if ((Length >= SPI_FLASH_BLOCK_LENGTH) && (usPageNumber%8 == 0)) { // if delete range corresponds to a block, use faster block delete
                usLength = SPI_FLASH_BLOCK_LENGTH;
                ucCommand = BLOCK_ERASE;
            }
            else {
                usLength = SPI_FLASH_PAGE_LENGTH;
                ucCommand = PAGE_ERASE;
            }
            fnSPI_command(ucCommand, usPageNumber, _EXTENDED_CS 0, 0);   // delete appropriate page/block
            if (Length <= usLength) {
                break;                                                   // delete complete
            }
            ptrSector += usLength;
            Length -= usLength;
            #endif
            continue;
        }
        #endif
        #ifdef FLASH_FILE_SYSTEM
        fnFlashCommand(DELETE_FLASH, ptrSector, Length);
        break;
        #endif

    #endif                                                               // #endif not SPI_FILE_SYSTEM
    } while (Length);

#ifdef PROTECTED_FILE
    if (iUnprotected != 0) {                                             // protect device again on exit
    }
#endif
    return 0;
}
#if defined ACTIVE_FILE_SYSTEM && defined _WINDOWS
    extern int iFetchingInternalMemory = 0;
#endif

static void fnGetPars(unsigned char *ParLocation, unsigned char *ptrValue, MAX_FILE_LENGTH Size)
{
#ifdef ACTIVE_FILE_SYSTEM
    #ifdef _WINDOWS
    if (iFetchingInternalMemory != 0) {
        uMemcpy(ptrValue, ParLocation, Size);
        return;
    }
    #endif
    #if defined SPI_SW_UPLOAD || (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
        #if (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)       
    if (ParLocation >= ((unsigned char *)(FLASH_START_ADDRESS + SIZE_OF_FLASH))) // we are working from external SPI FLASH memory 
        #else
    if ((ParLocation >= (uFILE_SYSTEM_END)) && (ParLocation < ((unsigned char *)(uFILE_START + FILE_SYSTEM_SIZE + SPI_DATA_FLASH_SIZE)))) // we are working from external SPI FLASH memory 
        #endif
    {
        #ifdef SPI_FLASH_MULTIPLE_CHIPS
        int iChipSelect;
        #endif
        unsigned short usPageNumber;
        unsigned short usPageOffset;
        usPageNumber = fnGetSPI_FLASH_location(ParLocation, &usPageOffset EXTENDED_CS);

        #ifdef SPI_FLASH_ST
        fnSPI_command(READ_DATA_BYTES, (unsigned long)((unsigned long)(usPageNumber << 8) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
        #else
            #if SPI_FLASH_PAGE_LENGTH >= 1024                                
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 11) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #elif SPI_FLASH_PAGE_LENGTH >= 512
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 10) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #else
        fnSPI_command(CONTINUOUS_ARRAY_READ, (unsigned long)((unsigned long)(usPageNumber << 9) | (usPageOffset)), _EXTENDED_CS ptrValue, Size);
            #endif
        #endif
        return;
    }
    #endif  
 
    uMemcpy(ptrValue, fnGetFlashAdd(ParLocation), Size);                 // the LPC23XX uses a file system in FLASH with no access restrictions so we can simply copy the data
#endif
}

#ifdef USE_PARAMETER_BLOCK

#define PARAMETER_BLOCK_1 (unsigned char*)(PARAMETER_BLOCK_START)
#define PARAMETER_BLOCK_2 (PARAMETER_BLOCK_1 + PARAMETER_BLOCK_SIZE)
#define FLASH_SECTORS_IN_PAR_BLOCK (PARAMETER_BLOCK_SIZE/PARAMETER_BLOCK_GRANULARITY)

// The LPC23XX can not change lines of less that quadword length (16 bytes)
//
unsigned char fnGetValidPars(unsigned char ucValid)
{
    unsigned long ulValidUse[(2*FLASH_LINE_SIZE)/sizeof(unsigned long)];
    unsigned long ulCompare;

    // The first 2 long word locations of the first two FLASH lines defines the validity of the block. 0x55555555 0x55555555 means it is fully valid. 0xffffffff 0x55555555 means it contains temporary unvalidated data
    fnGetPars(PARAMETER_BLOCK_1, (unsigned char*)ulValidUse, sizeof(ulValidUse));

    if (PRESENT_TEMP_BLOCK != ucValid) {
        ulCompare = 0x55555555;                                          // we are looking for validated pattern
    }
    else {
        ulCompare = 0xffffffff;                                          // we are looking for temporary pattern
    }

    if ((ulValidUse[0] == 0x55555555) && (ulValidUse[FLASH_LINE_SIZE/sizeof(unsigned long)] == ulCompare)) {
        return 1;                                                        // first block matched request
    }
 #ifdef USE_PAR_SWAP_BLOCK
    else {
        fnGetPars(PARAMETER_BLOCK_2, (unsigned char*)ulValidUse, sizeof(ulValidUse));
        if ((ulValidUse[0] == 0x55555555) && (ulValidUse[FLASH_LINE_SIZE/sizeof(unsigned long)] == ulCompare)) {
            return 2;                                                    // second block matched request
        }
    }
 #endif
    return 0;                                                            // all other cases indicate the searched block doesn't exist
}


static int fnDeleteParBlock(unsigned char *ptrAddInPar)
{
    #ifdef FLASH_FILE_SYSTEM
    return (fnFlashCommand(DELETE_FLASH, ptrAddInPar, PARAMETER_BLOCK_SIZE));
    #else
    return 0;
    #endif
}

// Due to the fact that individual bytes in quadwords (16 bytes) can not be modified more that once, one quadword is used for each byte to be stored.
// This allows the parameter system to fulfil its function but is not very efficient due to the high overhead.
//
extern int fnGetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned char *ptrPar = PARAMETER_BLOCK_1;

#ifdef USE_PAR_SWAP_BLOCK
    if (ucValidBlock == 2) {
        ptrPar += (PARAMETER_BLOCK_2 - PARAMETER_BLOCK_1);
    }
#endif

    ptrPar += (usParameterReference * FLASH_LINE_SIZE);
    ptrPar += 2*FLASH_LINE_SIZE;                                         // first parameter starts after validation information

    while (usLength--) {
        fnGetPars(ptrPar, ucValue++, 1);
        ptrPar += FLASH_LINE_SIZE;
    }
    return 0;
}

int fnSetParameters(unsigned char ucValidBlock, unsigned short usParameterReference, unsigned char *ucValue, unsigned short usLength)
{
    unsigned char *ptrPar = PARAMETER_BLOCK_1, *ptrStart;
    unsigned long ulValid = 0x55555555;
    unsigned long ulValidCheck;
    unsigned char ucCheck[FLASH_LINE_SIZE];
    int iBlockUse = 0;
    int iCommitNewStatus = 0;
#ifdef _WINDOWS                                                          // wehn simulating use a recognisable pattern
    uMemset(ucCheck, 0x33, FLASH_LINE_SIZE);
#else
    uMemset(ucCheck, 0xff, FLASH_LINE_SIZE);
#endif

    if (TEMP_PARS & ucValidBlock) {
        ucValidBlock &= ~TEMP_PARS;
        iBlockUse = 1;
    }

    if (ucValidBlock & BLOCK_INVALID) {                                  // no valid parameter blocks have been found so we can use the first for saving the data
        fnDeleteParBlock(PARAMETER_BLOCK_1);                             // we delete it to be absolutely sure it is fresh
#ifdef USE_PAR_SWAP_BLOCK
        fnDeleteParBlock(PARAMETER_BLOCK_2);
#endif
        ucValidBlock = 1;
    }

#ifdef USE_PAR_SWAP_BLOCK
    if (ucValidBlock == 2) {
        ptrPar += (PARAMETER_BLOCK_2 - PARAMETER_BLOCK_1);
    }
#endif
    ptrStart = ptrPar;
    ptrPar += (usParameterReference * FLASH_LINE_SIZE);
    ptrPar += (2*FLASH_LINE_SIZE);                                       // first parameter starts after validation information (each occupying one quadword line)

    if (!usLength) {
        if (iBlockUse) {                                                 // if we have a new set waiting, set it to use state
            iBlockUse = 0;
#ifdef USE_PAR_SWAP_BLOCK
            if (ptrStart == PARAMETER_BLOCK_1) {
                ptrPar = PARAMETER_BLOCK_2;
            }
            else {
                ptrPar = PARAMETER_BLOCK_1;
            }
            fnDeleteParBlock(ptrPar);
#endif
        }
        else {                                                           // we must delete the block
            fnDeleteParBlock(ptrStart);
            if (usParameterReference) {
                return 0;                                                // we don't validate it again
            }
        }
    }
    else {
        while (usLength--) {
            ucCheck[0] = *ucValue;
            fnWriteBytesFlash(ptrPar, ucCheck, FLASH_LINE_SIZE);         // write each parameter to FLASH
            ucValue++;
            ptrPar += FLASH_LINE_SIZE;
        }
        if (((CAST_POINTER_ARITHMETIC)ptrPar & (FLASH_ROW_SIZE-1)) != 0) {
            if (ptrPar > (ptrStart + FLASH_ROW_SIZE)) {
                fnWriteBytesFlash(0,0,0);                                // commit any open FLASH row buffer
            }
        }
    }
                                                                         // Now we must validate the parameter block which we have just written to
    fnGetPars(ptrStart, (unsigned char *)&ulValidCheck, sizeof(ulValidCheck));
    if (ulValidCheck != ulValid) {
        fnWriteBytesFlash(ptrStart, (unsigned char *)&ulValid, sizeof(ulValid)); // validate data
        iCommitNewStatus = 1;
    }
    if (!iBlockUse) {
        fnGetPars((ptrStart + FLASH_LINE_SIZE), (unsigned char *)&ulValidCheck, sizeof(ulValidCheck));
        if (ulValidCheck != ulValid) {
            fnWriteBytesFlash((ptrStart + FLASH_LINE_SIZE), (unsigned char *)&ulValid, sizeof(ulValid)); // use data
            iCommitNewStatus = 1;
        }
    }
    if (iCommitNewStatus != 0) {
        fnWriteBytesFlash(0,0,0);                                        // commit the parameter block status
    }
    return 0;
}
#endif
#endif
