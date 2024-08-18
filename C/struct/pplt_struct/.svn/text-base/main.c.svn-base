/*
 * main.c
 *
 *  Created on: Aug 30, 2012
 *      Author: ajsharm2
 */
#include "ManufactureInfo.h"


/* I was testing to see if I can populate the structure with the
 * correct value of hwmac, to understand structures better.
 */
int main(){
  int ret=0;
  manufacture_info *mninfoptr;

  mninfoptr = malloc(sizeof(manufacture_info));

  ret = getHwMacAddr(&mninfoptr->hwmac);
	if (ret != 0) {
		DEBUG("MAC FAIL=[%s]\n", mninfoptr->hwmac);
		return -1;
	}
	DEBUG("MAC=[%s]\n", mninfoptr->hwmac);
	return 0;
}
