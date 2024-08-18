#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
	FILE* fin;
	FILE* fout;
	char c;

	if(argc<3 || !strcmp(argv[1],"--help"))
	{
		printf("Usage: myHTMLshrinker [input file name] [output file name]\n");
		return -1;
	}
	
	fin=fopen(argv[1],"r");
	if(fin==NULL)
	{
		printf("Open input file: %s failed\n",argv[1]);
		return -2;
	}
	
	fout=fopen(argv[2],"w");
	if(fout==NULL)
	{
		printf("Open output file: %s failed\n",argv[2]);
		return -3;
	}

	c=fgetc(fin);
	while(c>0)
	{
		if(c!='\n')
			fputc(c,fout);
		c=fgetc(fin);
	}

	printf("Done\n");
	return 0;
}