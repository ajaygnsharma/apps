/*
 * repeat.c
 *
 *  Created on: Nov 15, 2012
 *      Author: ajsharm2
 */
int check_repeat(char input[]){
  int length = 4;
  int i,j;
  for(i = 0; i < length; i++) {
    for(j = i + 1; j < length; j++) {
        if(input[i] == input[j]) return (input[i]);
    }
  }
  return 0;
}

int main(void){
  char arr[4] = { 1, 2, 2, 4 };
  int repeat=0;
  repeat=check_repeat(arr);
  if(repeat)
	  printf("repeated %d\n",repeat);
}
