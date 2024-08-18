#include "dict.h"
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <linux/version.h>
#include <utmp.h>
#include <errno.h>
#include <time.h>
#include <termios.h>
#ifdef SHADOW_PWD
#include <shadow.h>
#endif

#include "pathnames.h"

#define BUF_SIZ 100	/* max size of a line in inittab */
#define NUMCMD 30	/* step size when realloc more space for the commands */
#define NUMTOK 20	/* max number of tokens in inittab command */

//#define DEBUGGING

struct initline {
  char	**toks;
  char wrd[20];
  char mng[50];
  char	*line;
  char	*fullline;
  unsigned char xcnt;
};

struct initline *inittab;

/* How many struct initline's will fit in the memory pointed to by inittab */
int inittab_size = 0; 
int numcmd;

static int  read_initfile(const char *);

static void init_itab(struct initline *p) {
	bzero(p, sizeof(struct initline));
}

static void clear_itab(struct initline *p) {
	if (p->line)
		free(p->line);
	if (p->fullline)
		free(p->fullline);
	if (p->toks)
		free(p->toks);
	init_itab(p);
}

void read_inittab(void)
{
	int i;

	/*
	 * free any old data and start again
	 */
	for (i = 0; i < numcmd; i++)
		clear_itab(&inittab[i]);
	numcmd = 0;

	i = 0;
	if (read_initfile(_PATH_INITTAB) == 0)
		i++;

#ifdef CONFIG_USER_FLATFSD_FLATFSD
	if (read_initfile(_PATH_CONFIGTAB) == 0)
		i++;
#endif

	if (i == 0) {
		err("Failed to open " _PATH_INITTAB
#ifdef CONFIG_USER_FLATFSD_FLATFSD
				" or " _PATH_CONFIGTAB
#endif
				"."
				);
	}

	/* if needed, shrink the array using realloc -
	 * must be done here so that we include the results of all init files
	 * when calculating number of commands */
	if ((numcmd + 2) < (inittab_size - NUMCMD)) {
		/* round up from numcmd to the nearest multiple of NUMCMD */
		inittab_size = ((numcmd + 2) / NUMCMD + 1) * NUMCMD;
		inittab = realloc(inittab, inittab_size * sizeof(struct initline));
		if (!inittab) {
			/* failure case - what do you do if init fails? */
			err("malloc failed");
			_exit(1);
		}
	}

	if (numcmd == 0)
		_exit(1);
}


static int
read_initfile(const char *initfile)
{
	struct initline *p;
	FILE *f;
	char *buf = NULL;
	size_t buf_len = 0;
	int i,j,k;
	char *ptr, *getty;
#ifdef SPECIAL_CONSOLE_TERM
	char tty[50];
	struct stat stb;
#endif
			
	i = numcmd;

	if (!(f = fopen(initfile, "r")))
		return 1;

	while(!feof(f)) {
		if (i+2 == inittab_size) {
			/* need to realloc inittab */
			inittab_size += NUMCMD;
			inittab = realloc(inittab, inittab_size * sizeof(struct initline));
			if (!inittab) {
				/* failure case - what do you do if init fails? */
				err("malloc failed");
				_exit(1);
			}
		}
		if (getline(&buf, &buf_len, f) == -1) break;

		for(k = 0; k < buf_len && buf[k]; k++) {
			if(buf[k] == '#') { 
				buf[k] = '\0'; break; 
			}
		}

		if(buf[0] == '\0' || buf[0] == '\n') continue;

		p = inittab + i;
		init_itab(p);
		p->line = strdup(buf);
		p->fullline = strdup(buf);
		if (!p->line || !p->fullline) {
			err("Not memory to allocate dict.dat entry");
			clear_itab(p);
			continue;
		}
		ptr = strtok(p->line, ":");
		if (!ptr) {
			err("Missing seperator 1 field in dict.dat");
			clear_itab(p);
			continue;
		}
		strncpy(p->wrd, ptr, 19);
		//p->tty[9] = '\0';
		ptr = strtok(NULL, "\t\n\r");
		if (!ptr) {
		  err("Missing seperator 2 field in dict.dat");
		  clear_itab(p);
		  continue;
		}
		strncpy(p->mng, ptr, 49);
		//p->termcap[29] = '\0';
		i++;
	}

	if (buf)
		free(buf);
	
	fclose(f);

	numcmd = i;
	return 0;
}

void load_dict(void){
  /* initialize the array of commands */
  inittab = (struct initline *)malloc(NUMCMD * sizeof(struct initline));
  inittab_size = NUMCMD;

  if (!inittab) {
    /* failure case - what do you do if init fails? */
    err("malloc failed");
    _exit(1);
  }

  read_inittab();
}

int lookup(char *wrd,char **mng){
  int 	i;
  for(i = 0; i < numcmd; i++) {
    //char **p = inittab[i].toks;
    //printf("toks= %s %s %s %s\n",p[0], p[1], p[2], p[3]);
    //if(strcmp(argv[1],inittab[i].wrd)==0){
    if(strcmp(wrd,inittab[i].wrd)==0){
#ifdef DEBUGGING
      //printf("wrd=%s", inittab[i].wrd);
      //printf(",mng=%s\n", inittab[i].mng);
      printf("%s: %s\n",wrd,inittab[i].mng);
#endif
      *mng=inittab[i].mng;
      //exit(0);
      return(1);
    }
  }
  //printf("%s not found in dictionary\n",argv[1]);
  return(0);
  /*exit(0);*/
}
