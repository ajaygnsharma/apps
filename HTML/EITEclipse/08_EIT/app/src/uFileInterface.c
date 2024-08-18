 /************************************************
   Mark Butcher              Bsc (Hons) MPhil MIET
   Mark@uTasker.com                www.uTasker.com

   Copyright (C) M.J.Butcher Consulting 2004..2008
   Licensed to Newport Electronics, INC
   ***********************************************
   uFileInterface.c
   4.9.2008

   This file contains simple file system support
   taken from the uTasker project and used for 
   saving SW upload data in a form compatible with
   the boot loader.

   */

/* =================================================================== */
/*                           include files                             */
/* =================================================================== */
#include "uTasker.h"                                                     // include certain uTasker support

/* =================================================================== */
/*                          local definitions                          */
/* =================================================================== */
#define LAST_FULL_SECTOR   ((FILE_SYSTEM_SIZE + FILE_GRANULARITY - SUB_FILE_SIZE)/SINGLE_FILE_SIZE)
#ifndef CAST_EXTENDED_POINTER_ARITHMETIC
    #define CAST_EXTENDED_POINTER_ARITHMETIC CAST_POINTER_ARITHMETIC
#endif

#define _FILE_GRANULARITY    SINGLE_FILE_SIZE

#if (SPI_FLASH_PAGE_LENGTH != 256 && SPI_FLASH_PAGE_LENGTH != 512) && (defined SPI_FILE_SYSTEM && defined FLASH_FILE_SYSTEM)
    static void fnGetLowerBoundary(MAX_FILE_LENGTH *length, MAX_FILE_LENGTH granularity)
    {
        MAX_FILE_LENGTH lower_boundary = granularity;
        while (lower_boundary <= *length) {
            lower_boundary += granularity;
        }
        *length = lower_boundary - granularity;
    }

    #define ROUNDDOWN_BOUNDARY(length, GRANULARITY) fnGetLowerBoundary(&length, GRANULARITY);
    #define ROUNDUP_BOUNDARY(length, GRANULARITY)   fnGetLowerBoundary(&length, GRANULARITY); length += GRANULARITY
#else
    #define ROUNDUP_BOUNDARY(length, GRANULARITY)  length &= ~(GRANULARITY-1); length += GRANULARITY 
    #define ROUNDDOWN_BOUNDARY(length, GRANULARITY) length &= ~(GRANULARITY-1);
#endif

#ifdef SUB_FILE_SIZE   
    #define SUB_FILE  , unsigned char ucSubfile
    #define SUB_FILE_OFF  ,0
#else
    #define SUB_FILE
    #define SUB_FILE_OFF
#endif


/* =================================================================== */
/*                      local variable definitions                     */
/* =================================================================== */
static MAX_FILE_LENGTH LengthInProgress = 0;
static MEMORY_RANGE_POINTER FileInProgress;
#if defined NO_ACCUMULATIVE_WORDS
    static unsigned char ucStartByte;
    static unsigned char ucEndByte;
    static unsigned char ucEndByteValid;
#elif defined FLASH_LINE_SIZE                                            // {6}
    static unsigned char ucFirstLine[FLASH_LINE_SIZE];                   // space for collecting first FLASH line of a file
#endif



#ifdef SUB_FILE_SIZE
// Return file type - whether it is a sub-file or not 
//
extern unsigned char fnGetFileType(CHAR *ptrFileName)
{
    if (*ptrFileName >= ('0' + LAST_FULL_SECTOR)) {
        return SUB_FILE_TYPE;
    }
    return 0;
}
#endif

#ifdef SUPPORT_MIME_IDENTIFIER
// Interpret a file extension and determine a MIME type
//
extern unsigned char fnGetMimeType(CHAR *ptrFileName) 
{
    unsigned char ucMimeType = 0;
    int iExtensionFound = 0;
    while ((*ptrFileName >= ' ') && (*ptrFileName != '?')) {        
        if ((*ptrFileName == ' ') && (iExtensionFound != 0)) {    
            break;
        }
        if (*ptrFileName == '.') {
            iExtensionFound = 1;                                         // mark that we have found an extension seperator
        }
        ptrFileName++;
    }                                                                    // search to the end of the file name

    if (iExtensionFound) {
        while (*ptrFileName != '.') {
            ptrFileName--;
        }

        while (ucMimeType < UNKNOWN_MIME) {
            if (uStrEquiv((ptrFileName+1), cMimeTable[ucMimeType])) {
                return ucMimeType;
            }
            ucMimeType++;
        }
    }

    return UNKNOWN_MIME;
}
#endif

// Converts a file name to its file offset in the uFileSystem
//
extern unsigned char fnConvertName(CHAR cName)
{
    if (cName < 'A') {                                                   // 0...9
       cName -= '0';
    }
    else if (cName < 'a') {                                              // A..Z
        cName -= ('A' - 10);
    }
    else {
       cName -= ('a' - 36);                                              // a..z
    }
    return (unsigned char)cName;
}

// Simple one number / letter file names
//
static MAX_FILE_SYSTEM_OFFSET fnGetFilePointer(CHAR *ucIp_Data)
{
    MAX_FILE_SYSTEM_OFFSET FileLocation;

#if defined SPI_SW_UPLOAD  
    if (SW_UPLOAD_FILE()) {
        return (MAX_FILE_SYSTEM_OFFSET)(FILE_SYSTEM_SIZE/* - PAR_BLOCK_SIZE*/); // special location for external SPI FLASH {10} [remove PAR_BLOCK_SIZE]
    }
#endif
    FileLocation = fnConvertName(*ucIp_Data);                            // first letter/number signals which block it should be put in
#ifdef SUB_FILE_SIZE   
    #define LAST_FULL_SECTOR   ((FILE_SYSTEM_SIZE + FILE_GRANULARITY - SUB_FILE_SIZE)/SINGLE_FILE_SIZE)
    if (FileLocation >= LAST_FULL_SECTOR) {
        FileLocation -= LAST_FULL_SECTOR;
        if (FileLocation >= LAST_SUB_FILE_BLOCK) {                       // limit to physical range - if outside we set to last...
            FileLocation = (LAST_SUB_FILE_BLOCK-1);
        }
        return (FileLocation * SUB_FILE_SIZE);                           // return an offset from start of file
    }
#endif

    if (FileLocation >= LAST_FILE_BLOCK) {                               // limit to physical range - if outside we set to last...
        FileLocation = (LAST_FILE_BLOCK-1);
    }
    return (FileLocation * SINGLE_FILE_SIZE);                            // return an offset from start of file
}

// This routine returns a pointer to a file (which can either be empty, exits or overlap - this is not checked here)
//
extern MEMORY_RANGE_POINTER uOpenFile(CHAR *ptrfileName)
{
    MEMORY_RANGE_POINTER ptrFlash = uFILE_SYSTEM_START + fnGetFilePointer(ptrfileName);

    return (ptrFlash);                                                   // return file pointer
}

// Write the file length and close write process
//
#ifdef SUPPORT_MIME_IDENTIFIER
    extern MAX_FILE_LENGTH uFileCloseMime(MEMORY_RANGE_POINTER ptrFile, unsigned char *ucMimeType)
#else
    extern MAX_FILE_LENGTH uFileClose(MEMORY_RANGE_POINTER ptrFile)
#endif
{
    MAX_FILE_LENGTH Len;

#ifdef NO_ACCUMULATIVE_WORDS
    #ifdef SUPPORT_MIME_IDENTIFIER
    unsigned char ucMimeTypeWord[2];
    #endif

    if (ucEndByteValid) {
        fnWriteBytesFlash((FileInProgress + LengthInProgress++), &ucEndByte, 1); // add last byte to end of file
        ucEndByteValid = 0;
    }
#endif
    Len = LengthInProgress;
    if (!Len) {
#ifdef PROTECTED_FILE
        fnProtectFile(); 
#endif
        return 0;
    }

#ifdef SUPPORT_MIME_IDENTIFIER
    #if defined NO_ACCUMULATIVE_WORDS
    ucMimeTypeWord[0] = *ucMimeType;
    ucMimeTypeWord[1] = ucStartByte;
    fnWriteBytesFlash((ptrFile + sizeof(MAX_FILE_LENGTH)), ucMimeTypeWord, 2);
    #elif defined FLASH_LINE_SIZE                                       
    ucFirstLine[sizeof(MAX_FILE_LENGTH)] = *ucMimeType;                  // add mime type to the FLASH line buffer
    #else
    fnWriteBytesFlash((ptrFile + sizeof(MAX_FILE_LENGTH)), ucMimeType, 1);
    #endif
#endif

    LengthInProgress = 0;
#ifdef FLASH_LINE_SIZE                                           
    if (Len >= (FLASH_ROW_SIZE - FILE_HEADER)) {
        fnWriteBytesFlash(ptrFile,0,0);                                  // close any outstanding FLASH buffer 
    }
    uMemcpy(ucFirstLine, &Len, sizeof(MAX_FILE_LENGTH));                 // add length to first line buffer
    fnWriteBytesFlash(ptrFile, ucFirstLine, sizeof(ucFirstLine));        // add the first line
    if ((fnWriteBytesFlash(ptrFile,0,0)) != 0) {                         // and then write it to complete the work 
        Len = 0;
    }
#else
    if ((fnWriteBytesFlash(ptrFile, (unsigned char *)&Len, sizeof(MAX_FILE_LENGTH))) != 0) { // write the length to the file
        Len = 0;
    }
#endif
#ifdef PROTECTED_FILE
    fnProtectFile(); 
#endif
    return (Len);
}


// This routine deletes present files which reside at the position of the new file and then writes the new data
//
#ifdef SUB_FILE_SIZE
    MAX_FILE_LENGTH uFileWrite(MEMORY_RANGE_POINTER ptrFile, unsigned char *ptrData, MAX_FILE_LENGTH DataLength, unsigned char ucSubFile)
#else
    MAX_FILE_LENGTH uFileWrite(MEMORY_RANGE_POINTER ptrFile, unsigned char *ptrData, MAX_FILE_LENGTH DataLength)
#endif
{
#ifdef SUPPORT_MIME_IDENTIFIER
    #define MIME_TYPE , &ucMimeType
    #define UOPENNEXTFILE uOpenNextMimeFile
    unsigned char ucMimeType;
#else
    #define UOPENNEXTFILE uOpenNextFile
    #define MIME_TYPE
#endif

    if (!DataLength) {
        return LengthInProgress;
    }

    if (!LengthInProgress) {                                             // first write
        MAX_FILE_LENGTH FileLength;
        MEMORY_RANGE_POINTER prtPresentFile;
        MEMORY_RANGE_POINTER ptrNewFile = ptrFile + FILE_HEADER;         // leave space for file header
#ifdef SPI_SW_UPLOAD                                              
        MEMORY_RANGE_POINTER ptrFileSysEnd = (MEMORY_RANGE_POINTER)(uFILE_START + FILE_SYSTEM_SIZE);
        MEMORY_RANGE_POINTER ptrSPI_FlashEnd = (ptrFileSysEnd + SPI_DATA_FLASH_SIZE);
#endif
        FileInProgress = ptrNewFile;
#ifdef SPI_SW_UPLOAD                                           
        if ((ptrFile >= ptrFileSysEnd) && (ptrFile < ptrSPI_FlashEnd)) { // we are working from external SPI FLASH memory
            if ((FileLength = uGetFileLength(ptrFile)) != 0) {           // if there is already data in the SPI FLASH
                uFileErase(ptrFile, FileLength);                         // erase the file to make space for new upload
            }
        }
#endif
#if defined NO_ACCUMULATIVE_WORDS
        ucEndByteValid = 0;
        ucEndByte = 0;
#elif defined FLASH_LINE_SIZE
        uMemset(ucFirstLine, 0xff, sizeof(ucFirstLine));                 // flush FLASH line backup buffer
#endif
#ifdef PROTECTED_FILE
        fnUnprotectFile(); 
#endif

#ifdef SUB_FILE_SIZE
        if (ucSubFile == 0) {
#endif
        if ((prtPresentFile = UOPENNEXTFILE(0, &FileLength  MIME_TYPE  SUB_FILE_OFF)) != 0) { // get details of first file in the system
            do {
                FileLength += (_FILE_GRANULARITY + (FILE_HEADER-1));     // round the length up to next file block size
                ROUNDDOWN_BOUNDARY(FileLength, _FILE_GRANULARITY); 
#if FILE_GRANULARITY != SINGLE_FILE_SIZE                          
                if ((((ptrNewFile + DataLength) >= prtPresentFile) && (ptrFile <= prtPresentFile)) || ((ptrNewFile >= prtPresentFile) && (ptrNewFile < (prtPresentFile + FileLength))))
#else
                if ((ptrNewFile >= prtPresentFile) && (ptrNewFile < (prtPresentFile + FileLength))) 
#endif
                {
                    uFileErase(prtPresentFile, FileLength);              // erase present file to make space for new one
#if FILE_GRANULARITY != SINGLE_FILE_SIZE                      
                    if ((prtPresentFile + FileLength) >= (ptrNewFile + DataLength)) {
                        break;                                           // we now have enough space to save the new data
                    }
#else
                    if (FileLength >= DataLength) {
                        break;                                           // we now have enough space to save the new data
                    }
#endif
#if FILE_GRANULARITY == SINGLE_FILE_SIZE    
                    ptrNewFile += FileLength;                            // we may still have to make more space
#endif
                }
                prtPresentFile = UOPENNEXTFILE(prtPresentFile, &FileLength  MIME_TYPE  SUB_FILE_OFF); // try next present file
            }
#if FILE_GRANULARITY == SINGLE_FILE_SIZE  
            while (prtPresentFile && (prtPresentFile <= ptrNewFile));
#else
            while (prtPresentFile != 0);
#endif
        }
#ifdef SUB_FILE_SIZE
        }
#endif

#ifdef NO_ACCUMULATIVE_WORDS
        if ((unsigned char)((CAST_EXTENDED_POINTER_ARITHMETIC)FileInProgress + LengthInProgress) & 0x1) {  // we have to handle odd addresses carefully
            ucStartByte = *ptrData++;                                    // save the first byte and move to even boundary
            LengthInProgress++;
            DataLength--;
        }
#endif
    }
    else {                                                               // this is a follow on write so check that we have enough space to write - or else make space by deleting next file
        unsigned short usPresentBlock = (unsigned short)(((FileInProgress - uFILE_SYSTEM_START) + LengthInProgress)/FILE_GRANULARITY);
        unsigned short usNextBlock    = (unsigned short)((((FileInProgress - uFILE_SYSTEM_START) + LengthInProgress + DataLength) - 1)/FILE_GRANULARITY);

#ifdef SPI_SW_UPLOAD         
        MEMORY_RANGE_POINTER ptrFileSysEnd = (MEMORY_RANGE_POINTER)(uFILE_START + FILE_SYSTEM_SIZE);
        MEMORY_RANGE_POINTER ptrSPI_FlashEnd = ptrFileSysEnd + SPI_DATA_FLASH_SIZE;

        if (((FileInProgress + LengthInProgress) >= ptrFileSysEnd) && ((FileInProgress + LengthInProgress) < ptrSPI_FlashEnd)) { // we are working from external SPI FLASH memeory
            if ((LengthInProgress + DataLength) < (SPI_DATA_FLASH_SIZE - FILE_HEADER)) {  // Destination is SPI data flash
                fnWriteBytesFlash((FileInProgress + LengthInProgress), ptrData, DataLength);
                LengthInProgress += DataLength;
            }

            return LengthInProgress;
        }
#endif
        if (usNextBlock > (LAST_FILE_BLOCK + 1)) {                       // limit writes to within valid FLASH range (anything more is cut)
            return LengthInProgress;                                     // don't allow any (more) writes to take place
        }

#ifdef SUB_FILE_SIZE
        if (ucSubFile == 0) {
#endif
#if FILE_GRANULARITY != SINGLE_FILE_SIZE        
        usPresentBlock /= (SINGLE_FILE_SIZE/FILE_GRANULARITY);
        usNextBlock /= (SINGLE_FILE_SIZE/FILE_GRANULARITY);
#endif
        while (usPresentBlock != usNextBlock) {
            MEMORY_RANGE_POINTER ptrNextBlock = uFILE_SYSTEM_START + (usPresentBlock+1)*_FILE_GRANULARITY;
            MAX_FILE_LENGTH NextLength = uGetFileLength(ptrNextBlock);
            if (NextLength) {
                uFileErase(ptrNextBlock, (MAX_FILE_LENGTH)(NextLength + (FILE_HEADER-1)));// since we want to write to location occupied already, we delete the existing file
            }
            usPresentBlock++;
        }
#ifdef SUB_FILE_SIZE
        }
#endif
    }

#if defined NO_ACCUMULATIVE_WORDS
    if (ucEndByteValid) {
        *(--ptrData) = ucEndByte;                                        // we assume the input buffer can accept this- FTP is OK
        DataLength++;
        ucEndByteValid = 0;
    }
    if (DataLength & 0x01) {                                             // we can't write odd length so save end byte
        DataLength--;
        ucEndByte = *(ptrData + DataLength);
        ucEndByteValid = 1;
    }
#endif
#ifdef FLASH_LINE_SIZE           
    if (LengthInProgress < (FLASH_LINE_SIZE - FILE_HEADER)) {            // save first FLASH line of file in backup buffer
        size_t CopyLength = (FLASH_LINE_SIZE - FILE_HEADER);
        if (DataLength < CopyLength) {
            CopyLength = DataLength;
        }
        uMemcpy(&ucFirstLine[FILE_HEADER + LengthInProgress], ptrData, CopyLength);
        LengthInProgress += CopyLength;
        ptrData += CopyLength;
        DataLength -= CopyLength;
    }
#endif
    fnWriteBytesFlash((FileInProgress + LengthInProgress), ptrData, DataLength); // we are sure that we have enough space to save the present data so save it
    LengthInProgress += DataLength;
    return LengthInProgress;
}


// tries to match a string, where lower and upper case are treated as equal
//
extern unsigned short uStrEquiv(const CHAR *cInput, const CHAR *cMatch)
{
    unsigned short usMatch = 0;
    CHAR cReference;

    while ((cReference = *cMatch) != 0) {
        if (*cInput != cReference) {
            if (cReference >= 'a') {                                     // verify that it is not the case which doesn't match
                cReference -= ('a' - 'A');                               // try capital match
            }
            else if (cReference >= 'A') {
                cReference += ('a' - 'A');                               // try small match
            }
            if (*cInput != cReference) {                                 // last chance
                return 0;
            }
        }
        cMatch++;
        cInput++;
        usMatch++;
    }
    return usMatch;                                                      // return the length of match
}


// This routine returns a pointer to the next file in the system, or zero when no more
//
#ifdef SUPPORT_MIME_IDENTIFIER
  extern MEMORY_RANGE_POINTER uOpenNextMimeFile(MEMORY_RANGE_POINTER ptrfileLocation, MAX_FILE_LENGTH *FileLength, unsigned char *ucMimeType  SUB_FILE)
#else
  extern MEMORY_RANGE_POINTER uOpenNextFile(MEMORY_RANGE_POINTER ptrfileLocation, MAX_FILE_LENGTH *FileLength  SUB_FILE)
#endif
{
    if (!ptrfileLocation) {
        ptrfileLocation = uFILE_SYSTEM_START;                            // get first file found when zero passed
    }
    else {
        MAX_FILE_LENGTH ThisLength = uGetFileLength(ptrfileLocation);    // set the pointer to next possible block
        ThisLength += (FILE_HEADER-1);
#ifdef SUB_FILE_SIZE
        if (ucSubfile != 0) {
            ThisLength &= ~(SUB_FILE_SIZE-1);                            // round the length up to next file sub-block size
            ThisLength += SUB_FILE_SIZE;
        }
        else {
            ROUNDUP_BOUNDARY(ThisLength, _FILE_GRANULARITY);             // round the length up to next file block size {5}
        }
#else
        ROUNDUP_BOUNDARY(ThisLength, _FILE_GRANULARITY);                 // round the length up to next file block size {5}
#endif
        ptrfileLocation += ThisLength;                                   // address of next possible block
    }

    while (1) {
        if (ptrfileLocation >= uFILE_SYSTEM_END-1) {                     // end of file system reached
            *FileLength = 0;                                             // file length is zero
            return 0;                                                    // nothing found
        }
        if ((*FileLength = uGetFileLength(ptrfileLocation)) != 0) {
#ifdef SUPPORT_MIME_IDENTIFIER
            fnGetParsFile((ptrfileLocation + sizeof(MAX_FILE_LENGTH)), ucMimeType, 1);
#endif
            return ptrfileLocation;                                      // next file found here
        }
#ifdef SUB_FILE_SIZE
        if (ucSubfile != 0) {
            ptrfileLocation += SUB_FILE_SIZE;                            // check next sub-block
        }
        else {
            ptrfileLocation += _FILE_GRANULARITY;                        // check next block
        }
#else
        ptrfileLocation += _FILE_GRANULARITY;                            // check next block
#endif
    }
}

// Erase all sectors occupied by file
//
extern int uFileErase(MEMORY_RANGE_POINTER ptrFile, MAX_FILE_LENGTH FileLength) 
{
    if (fnEraseFlashSector(ptrFile, FileLength)) {                       // erase a file block
        return  (-1);                                                    // return error if one found
    }
    return 0;
}

// This routine returns the length of the file, a zero if empty or invalid
//
extern MAX_FILE_LENGTH uGetFileLength(MEMORY_RANGE_POINTER ptrfile)
{
    MAX_FILE_LENGTH FileLength;

    fnGetParsFile(ptrfile, (unsigned char *)&FileLength, sizeof(MAX_FILE_LENGTH));

    if (FileLength == 0) {                                               // protect against corrupted file so that it can be deleted
        return _FILE_GRANULARITY;                                        // the size of a file block
    }

    if (FileLength == (MAX_FILE_LENGTH)(-1)) {                           // check for blank FLASH memory
        return 0;                                                        // no length so empty
    }

    // if a (corrupted) length should be too large, we limit it to end of the file system
    if (((CAST_EXTENDED_POINTER_ARITHMETIC)((MEMORY_RANGE_POINTER)(uFILE_START + FILE_SYSTEM_SIZE - FILE_HEADER)) - (CAST_EXTENDED_POINTER_ARITHMETIC)ptrfile) < FileLength) {
        FileLength = (MAX_FILE_LENGTH)((CAST_EXTENDED_POINTER_ARITHMETIC)(MEMORY_RANGE_POINTER)(uFILE_START + FILE_SYSTEM_SIZE - FILE_HEADER) - (CAST_EXTENDED_POINTER_ARITHMETIC)ptrfile); // the space remaining
    }

    return FileLength;
}
