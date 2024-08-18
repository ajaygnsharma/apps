/*
 * struct_sz.c
 *
 *  Created on: Mar 4, 2013
 *      Author: sharmaj
 */

#include <inttypes.h>
#include <errno.h>
#include <iostream>

using namespace std;

/*typedef   unsigned long long  UINT64;     */
typedef   uint64_t            UINT64;
typedef     uint32_t            UINT32;
typedef     uint16_t            UINT16;
typedef     unsigned char       UINT8;
typedef     unsigned char       UCHAR;

struct sCmdTbl{
    UINT64 sttLba:33;
        unsigned lbaCnt:16;
        unsigned lbaSz:11;
        unsigned ccohHdl:11;
        unsigned hw_iid:4;
        unsigned sectorPerLpage:1;
        unsigned cmdType:2;
        unsigned   valid:1;
        unsigned   error:1;
        unsigned cnnct_stat:1;
        unsigned  ccohStat:1;
        unsigned  slotStat:1;
        unsigned  atmcStot:8;
        unsigned  cmdStat:2;
        unsigned issd:16;
        unsigned cmpltd:16;
        unsigned ltag:11;
        unsigned ptyp:2;
        unsigned ppos:1;
        unsigned rc:1;
        unsigned ac:1;
        unsigned gc:1;
        unsigned pact:2;
        unsigned hw_ms:3;
        unsigned ref_tag:32;
        unsigned app_tag:16;
        unsigned app_tag_mask:16;
        unsigned hw_pi:2;
        unsigned prev_ptr:10;
        unsigned nxt_ptr:10;
        unsigned ctag:10;
};


int main(){
  struct sCmdTbl cmdTblEntry;

  cout<<"size of sCmdTbl="<<sizeof(cmdTblEntry)<<endl;

  return 0;
}

