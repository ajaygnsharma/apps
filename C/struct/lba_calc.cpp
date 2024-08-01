/*
 * lba_calc.c
 *
 *  Created on: Feb 22, 2013
 *      Author: sharmaj
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

typedef unsigned int UINT32;
typedef unsigned char UINT8;
typedef unsigned short UINT16;

struct sLpg{
			UINT32 curLPN;
			UINT32 bndrySttLBA;
			UINT32 bndryEndLBA;
			UINT32 reqLBA;
			bool   aligned;
			//UINT32 deltaLBA;
			bool   rmwNdd;
			bool   sttLpg;
			bool   endLpg;
			UINT8  mrgMask;
};

struct sCmdTbl{
			UINT32 sttLba;
			UINT16 lbaCnt:16;
			UINT16 lbaSz:11;
			UINT32 ccohHdl;
			UINT16 sectorPerLpage:1;
			UINT8  cmdType:2;
			bool   valid:1;
			bool   error:1;
			UINT8  ccohStat;
			UINT8  cmdStat:2;
			UINT16 rdLpageIss:16;
			UINT16 cmpl:16;
			UINT8 *addr;
			UINT32 htag;
			UINT32 lpCnt;
			UINT32 reqLPCnt;
			UINT32 xferLPCnt;
		};

UINT32 calcLPN(UINT32 lba){
  UINT32 lpn=0;
  if(lpn>=0){
	  lpn=lba/8;
  }
  return lpn;
}

UINT32 fillCurLpg(struct sLpg *lpg, UINT32 reqLBA, struct sCmdTbl *tc){
	UINT32 endLBA;
	if( (reqLBA>=0) && (tc->lbaCnt>0) ){
		endLBA = tc->sttLba + (tc->lbaCnt-1);
	}else{
		cerr<<"Incorrect requested LBA"<<endl;
		return(-1);
	}

	lpg->curLPN      = calcLPN(reqLBA);
	lpg->bndrySttLBA = (reqLBA / 8) * 8;

	if( ((lpg->bndrySttLBA + 8)-1) <= endLBA ){
		lpg->bndryEndLBA = (lpg->bndrySttLBA + 8)-1;
	}else{
		lpg->bndryEndLBA = endLBA;
	}

	if(( tc->sttLba / 8 ) == lpg->curLPN )
		lpg->sttLpg = true;
	else
		lpg->sttLpg = false;

	if( ( endLBA / 8) == lpg->curLPN )
		lpg->endLpg = true;
	else
		lpg->endLpg = false;

	lpg->reqLBA      = reqLBA;

	if(lpg->endLpg){
		if( (((endLBA - lpg->reqLBA)+1) % 8) ==0 ){
			lpg->aligned = true;
			lpg->rmwNdd  = false;
		}else{
			lpg->rmwNdd  = true;
			lpg->aligned = false;
		}
	}else{
		if((lpg->reqLBA % 8)==0){
			lpg->aligned = true;
			lpg->rmwNdd  = false;
		}else{
			lpg->rmwNdd  = true;
			lpg->aligned = false;
		}
	}


	if(lpg->sttLpg && lpg->rmwNdd){
		lpg->mrgMask = 0;
		for(UINT32 i = reqLBA; i <= lpg->bndryEndLBA; i++)
			lpg->mrgMask |= ( 1 << (i%8) );
	}else if(lpg->endLpg && lpg->rmwNdd){
		lpg->mrgMask = 0;
		for(UINT32 i = reqLBA; i <= endLBA; i++){
			lpg->mrgMask |= ( 1 << (i%8) );
		}
	}else{
		lpg->mrgMask = 0;
	}

	return (lpg->bndryEndLBA+1);
}



void printLpg(struct sLpg *lpg){
	printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%.2x\n",\
		lpg->reqLBA,\
		lpg->curLPN,\
		lpg->bndrySttLBA,\
		lpg->bndryEndLBA,\
		lpg->aligned,\
		lpg->rmwNdd,\
		lpg->sttLpg,\
		lpg->endLpg,\
		lpg->mrgMask);
}

void test_case(UINT32 lba, UINT32 lbaCnt){
	struct sLpg *curLpg;
	struct sCmdTbl *tc;
	UINT32 nxtLBA;

	tc         = new sCmdTbl;
	tc->sttLba = lba;
	tc->lbaCnt = lbaCnt;

	UINT32 reqLBA = tc->sttLba;
	printf("\t\t\t\t\tREQ: sttLBA=%d, lbaCnt=%d\n",lba,lbaCnt);
	printf("reqLBA|curLPN|BSttLBA|BEndLBA|Algnd|rmwNdd|sttLpg|endLpg|mrgMsk\n");
	do{
		curLpg = new sLpg;
		nxtLBA = fillCurLpg(curLpg, reqLBA, tc);

		if(nxtLBA != -1){
		  printLpg(curLpg);
		  reqLBA = nxtLBA;
		}else
			return;

		free(curLpg);
	}while(!curLpg->endLpg);

	free(tc);
}


int main(void){
	test_case(0, 0);
	test_case(0, 1);
	test_case(0, 2);
	test_case(0, 3);
	test_case(0, 4);
	test_case(0, 5);
	test_case(0, 6);
	test_case(0, 7);
	test_case(0, 8);
	test_case(0, 9);
	test_case(0, 10);
	test_case(0, 20);
	test_case(0, 31);

	cerr << "---------------------------------------------------" <<endl;
	test_case(1, 0);
	test_case(1, 1);
	test_case(1, 2);
	test_case(1, 3);
	test_case(1, 4);
	test_case(1, 5);
	test_case(1, 6);
	test_case(1, 7);
	test_case(1, 8);
	test_case(1, 9);
	test_case(1, 10);
	test_case(1, 20);
	test_case(1, 31);
	test_case(1, 32);

	cerr << "---------------------------------------------------" <<endl;
	test_case(8, 0);
	test_case(8, 1);
	test_case(8, 2);
	test_case(8, 3);
	test_case(8, 4);
	test_case(8, 5);
	test_case(8, 6);
	test_case(8, 7);
	test_case(8, 8);
	test_case(8, 9);
	test_case(8, 10);
	test_case(8, 20);
	test_case(8, 31);
	test_case(8, 32);


	//test_case(0, -1);

	//test_case(6, 10);
	//test_case(6, 11);
}
