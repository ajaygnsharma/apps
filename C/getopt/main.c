/*
 * main.c
 *
 *  Created on: Apr 18, 2012
 *      Author: Ajay Sharma
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

char *version = "1.0.0";

struct stats {
  unsigned char *ip;
  unsigned char *pswd;
};

/*****************************************************************************
 * getdata(): get the data from the file and print it on console */
int getdata(FILE *fp, struct stats *st){
  char ip[50+1];
  char pswd[50+1];

  if (fseek(fp, 0, SEEK_SET) < 0)
		return(-1);

  while(!feof(fp)){
	  fscanf(fp,"%50s %50s\n", ip, pswd);

	  st->ip=strdup(ip);
	  st->pswd=strdup(pswd);
	  printf("%s,%s\n",st->ip, st->pswd);
	  free(st->ip);
	  free(st->pswd);
  }

  return(0);
}

/*****************************************************************************
 * usage(): how to use the file */
void usage(FILE *fp, int rc){
	fprintf(fp, "Usage: ipp [-h] [-f filename]"
			"\n\n");
	fprintf(fp, "        -h            this help\n");
	fprintf(fp, "        -v             print version info\n");
	fprintf(fp, "        -f filename    file containing ip addr & password\n");
	exit(rc);
}

/*****************************************************************************
 * main() */
int main(int argc, char *argv[]){
	FILE		*fp;
	struct stats	st;
	int		c;
	char	*ipp_file;

	ipp_file = "./ipp.dat";

	while ((c = getopt(argc, argv, "vh?f:")) > 0) {
		switch (c) {
		case 'v':
			printf("%s: version %s\n", argv[0], version);
			exit(0);
		case 'f':
			ipp_file = optarg;
			break;
		case 'h':
		case '?':
			usage(stdout, 0);
			break;
		default:
			fprintf(stderr, "ERROR: unkown option '%c'\n", c);
			usage(stderr, 1);
			break;
		}
	}

	/* Check device is real, and open it. */
	if ((fp = fopen(ipp_file, "r")) == NULL) {
		fprintf(stderr, "ERROR: failed to open %s, errno=%d\n",
				ipp_file, errno);
		exit(0);
	}

	if (setvbuf(fp, NULL, _IONBF, 0) != 0) {
		fprintf(stderr, "ERROR: failed to setvbuf(%s), errno=%d\n",
				ipp_file, errno);
		exit(0);
	}

	getdata(fp, &st);

	fclose(fp);
	exit(0);
}

