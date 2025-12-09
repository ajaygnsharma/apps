#include <stdio.h>
#include <stdlib.h>
#include "dict.h"

int main(int argc, char *argv[]){
  char **mng=NULL;

  if(argc != 2 ){
    printf("usage: dict <word>\n");
    exit(1);
  }
  
  load_dict();
  if(lookup(argv[1],&mng))
    printf("%s:%s\n",argv[1],mng);
  else
    printf("%s not found in dictionary\n",argv[1]);
}
