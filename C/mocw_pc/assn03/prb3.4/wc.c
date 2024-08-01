#include <stdio.h>
#include <stdlib.h>
int main(int argc,char* argv[])
{
    FILE* fp=NULL;
    int   nfiles  =--argc; /*ignore the name of the program itself*/
    int   argidx  =1;      /*ignore the name of the program itself*/
    char* currfile="";
    signed char c;         /* on ARM mcu: declared as signed */
    /*count of words,lines,characters*/
    unsigned long nw=0,nl=0,nc=0;
    int lc,rc,cc,i;

    if(nfiles==0) 
    {
        fp=stdin; /*standard input*/
        nfiles++; 
    }
    else /*set to first*/
    {
        currfile=argv[argidx++];
        fp=fopen(currfile,"r");
    }
    while(nfiles>0) /*files left >0*/
    {
       if(fp==NULL)
        {
            fprintf(stderr,"Unable to open input\n");
            exit(-1);
        }
        nc=nw=nl=0;
        lc=rc=cc=i=0;
        while((c=getc(fp))!=EOF)
        {
	        switch(i){
	        case 0: lc=c; i++; break;
	        case 1: cc=c; i++; break;
	        case 2: rc=c; i++; break;
	        case 3: i=0;       break;
	        }

	        if(cc==0x20){
	          if((lc!=0x20)&&(rc!=0x20)){
	            nw++; /* count transitions between non white spaces to
	                     white spaces*/
	          }
	        }

        	if((c!=0x0d)&&(c!=0x0a))
	        	nc++;
	        if((c==0x0d)||(c==0x0a))
	        	nl++;
        }
        if((nc!=0)&&(nw==0))
        	nw++;
        printf("%ld %ld %ld %s\n",nl,nw,nc,currfile);
        /*next file if exists*/
        nfiles--;
        if(nfiles>0)
        {
            currfile=argv[argidx++];
            fp     =fopen(currfile,"r");
        }
    }
	return 0;
}
