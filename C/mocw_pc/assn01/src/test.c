/*
 * test.c
 *
 *  Created on: Mar 22, 2012
 *      Author: user1
 */
#include <stdio.h>

/* call the correct function */
void test_01(){
	double ans=0;
	prog_1_1_c(&ans);
	printf("%f\n",ans);
}

/* call the error function */
void test_02(){
	double ans=0;
	prog_1_1_c_err(&ans);
	printf("%f\n",ans);
}

void test_03(){
	double ans=0;
	prog_1_2_1(&ans);
	printf("%f\n",ans);
}

void test_04(){
	double ans=0;
	prog_1_2_2(&ans);
	printf("%f\n",ans);
}

void test_05(){
	double ans=0;
	prog_1_2_3(&ans);
	printf("%f\n",ans);
}

void test_06(){
	double ans=0;
	prog_1_2_4(&ans);
	printf("%f\n",ans);
}

void test_07(){
	prog_1_4();
}

void test_08(){
	printf("%d\n",prog_1_5(3));
}

void test_09(){
	prog_1_6();
}
