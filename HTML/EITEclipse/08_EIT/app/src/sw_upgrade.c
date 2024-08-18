 /************************************************
   Mark Butcher              Bsc (Hons) MPhil MIET
   Mark@uTasker.com                www.uTasker.com

   Copyright (C) M.J.Butcher Consulting 2004..2008
   Licensed to Newport Electronics, INC
   ***********************************************
   sw_upgrade.c
   4.9.2008

   This file contains the functions to start a post
   and save each subsequent data frame received.
   */


#include "micronet.h"

#include "uTasker.h"                                                     // include uTasker support

#ifdef SUB_FILE_SIZE
    #define SUBFILE_WRITE  , ucSubFileInProgress
    #define SUB_FILE_ON    ,SUB_FILE_TYPE
#else
    #define SUBFILE_WRITE
    #define SUB_FILE_ON
#endif
static unsigned char *ptrFile = 0;
static unsigned char ucMimeType;
static unsigned char ucSubFileInProgress = 0;

// This is called on each subsequent block of data to be saved during a software upload
//
extern int fnSaveUpload(void)
{
    if (PostBODYlength >= PostFileLen) {                                 // last frame in upload
       uFileWrite(ptrFile, BODYptr, PostFileLen SUBFILE_WRITE);          // save last frame data content
#ifdef SUPPORT_MIME_IDENTIFIER
        uFileCloseMime(ptrFile, &ucMimeType);                            // this will cause the file length and type to be written in the file
#else
        uFileClose(ptrFile);                                             // this will cause the file length to be written in the file
#endif
        PostFileLen = 0;
        posting_data = 0;                                                // finished
        return 1;
    }
    else {
        uFileWrite(ptrFile, BODYptr, PostBODYlength SUBFILE_WRITE);      // save complete frame data content
        PostFileLen -= PostBODYlength;                                   // remaining length
    }
    return 0;
}

// This function is called when the specified post function type (upgrade - as specified in webpages.c) is called
//
void post_software(PSOCKET_INFO socket_ptr)
cmx_reentrant 
{
    const CHAR upload_file[] = "0.bin";									 // open the file for SW upload - this is the file name
    ptrFile = uOpenFile((CHAR *)upload_file);                            // get file pointer (to new file or file to overwrite)
#ifdef SUPPORT_MIME_IDENTIFIER
    ucMimeType = fnGetMimeType((CHAR *)upload_file);                     // get the type of file being saved
#endif
#ifdef SUB_FILE_SIZE
    ucSubFileInProgress = fnGetFileType((CHAR *)upload_file);            // get file characteristics so that it is later handled correctly
#endif
    posting_data = 1;                                                    // ensure that following data is not parsed since it is binary
    fnSaveUpload();    													 // it is assumed that the file is larger than one TCP frame, so will not be terminated on first write
}
