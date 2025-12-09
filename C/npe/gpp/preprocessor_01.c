/*
 * preprocessor_01.c
 *
 *  Created on: Mar 5, 2012
 *      Author: user1
 */
//#include <stdio.h>

#define MSG_LOAD 8
#define lcd_write(MSG) i16WritePCF8562(0,0,0,0,0,MSG,0)

int i16WritePCF8562(int a,int b,int c,int d,int e,int msg, int f);

int main(void){
	lcd_write(MSG_LOAD);
}

int i16WritePCF8562(int a,int b,int c,int d,int e,int msg, int f){
	if(msg!=MSG_LOAD)
		return -1;
	//printf("msg=%d",msg);
	return 0;
}

/*
I wanted to see the preprocessor output of the program. Compare above with below
Giving the -E flag does the pre-processing and outputs the result. Code is not compiled.
user1@debian:~/workspace/pwc$ gcc -E preprocessor_01.c
# 1 "preprocessor_01.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "preprocessor_01.c"
# 12 "preprocessor_01.c"
int i16WritePCF8562(int a,int b,int c,int d,int e,int msg, int f);

int main(void){
 i16WritePCF8562(0,0,0,0,0,8,0);
}

int i16WritePCF8562(int a,int b,int c,int d,int e,int msg, int f){
 if(msg!=8)
  return -1;

 return 0;
}
*/
