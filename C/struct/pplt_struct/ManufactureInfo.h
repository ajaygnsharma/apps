/*
 * ManufactureInfo.h
 *
 *  Created on: Aug 30, 2012
 *      Author: ajsharm2
 */

#ifndef MANUFACTUREINFO_H_
#define MANUFACTUREINFO_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG_ON 1

#ifdef DEBUG_ON
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif


#define INFO_LEN             50


typedef struct {
	unsigned char magic[INFO_LEN];
	char hwmac[INFO_LEN];
	unsigned char sn[INFO_LEN];
	unsigned char udi[INFO_LEN];
	unsigned char vid[INFO_LEN];
	unsigned char	pid[INFO_LEN];
	unsigned char ciscosn[INFO_LEN];
	unsigned char proid[INFO_LEN];
	unsigned char verid[INFO_LEN];
	unsigned char partnum[INFO_LEN];
	unsigned char topasmrevision[INFO_LEN];
}manufacture_info;

int getHwMacAddr(char *tmp);
int get_CISCOSN(char **tmp);

#endif /* MANUFACTUREINFO_H_ */
