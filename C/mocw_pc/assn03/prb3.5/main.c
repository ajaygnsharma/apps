/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) user1 2012 <user1@>
 * 
 * prb3.5 is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * prb3.5 is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * main.c
 *
 *  Created on: Apr 18, 2012
 *      Author: user1
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/time.h>

/*****************************************************************************/

char *version = "1.0.0";

/*****************************************************************************/

struct stats {
  unsigned int org_st_code;
  int org_cntry_code;
  int dst_st_code;
  int dst_cntry_code;
  char st_abbr[2+1];
  char st_name[20+1];
  int ret_num;
  int exmpt_num;
  int aggr_agi;
};

/*****************************************************************************/
int getdata(FILE *fp, struct stats *st){
  size_t len;
  char *line=NULL;
  long aggr_sum=0;
	
  if (fseek(fp, 0, SEEK_SET) < 0)
		return(-1);
	
  if(getline(&line,&len,fp)==-1)
    return(-2);	  
  
  while(!feof(fp)){
	getline(&line,&len,fp);
    sscanf(line, "\"%d\"  \"%d\"  \"%d\" \"%d\" \"%2s\" \"%[_A-Z]\"  %d  %d  %d",
    		&st->org_st_code, &st->org_cntry_code,\
			&st->dst_st_code, &st->dst_cntry_code,\
            st->st_abbr,st->st_name,&st->ret_num,\
            &st->exmpt_num,&st->aggr_agi);
    if(st->org_st_code==25){
       printf("%d, %d\n",st->org_st_code,st->aggr_agi);
       aggr_sum+=st->aggr_agi;
	}
  }
  printf("Total:%ld\n",aggr_sum);
  return(0);
}

/*****************************************************************************/
void usage(FILE *fp, int rc){
	fprintf(fp, "Usage: cpu [-h?rai] [-s seconds] [-c count] "
		"[-d <device>]\n\n");
	fprintf(fp, "        -h?            this help\n");
	fprintf(fp, "        -v             print version info\n");
	fprintf(fp, "        -r             repeated output\n");
	fprintf(fp, "        -a             print system average\n");
	fprintf(fp, "        -i             idle measurement via busy loop\n");
	fprintf(fp, "        -c count       repeat count times\n");
	fprintf(fp, "        -s seconds     seconds between output\n");
	fprintf(fp, "        -d <device>    proc device to use (default /proc/stat)\n");
	exit(rc);
}

/*****************************************************************************/

int main(int argc, char *argv[])
{
	FILE		*fp;
	struct stats st;
	char		*procdevice;

	//procdevice = "/proc/stat";
	procdevice = "stateoutflow0708.txt";
	
	/*
	 *	Check device is real, and open it.
	 */
	if ((fp = fopen(procdevice, "r")) == NULL) {
		fprintf(stderr, "ERROR: failed to open %s, errno=%d\n",
			procdevice, errno);
		exit(0);
	}

	if (setvbuf(fp, NULL, _IONBF, 0) != 0) {
		fprintf(stderr, "ERROR: failed to setvbuf(%s), errno=%d\n",
			procdevice, errno);
		exit(0);
	}

	getdata(fp, &st);

	fclose(fp);
	exit(0);
}
