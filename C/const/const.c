/*
 * const.c
 *
 *  Created on: Apr 29, 2013
 *      Author: sharmaj
 */
#define PROBLEM_1 0
#define PROBLEM_1_SOL_1 0
#define PROBLEM_1_SOL_2 1

const int arr1_sz=250;

enum arr_sz_enum{ arr1_sz_e = 250 };

#if(PROBLEM_1)
char arr1[arr1_sz];
#endif


#if(PROBLEM_1_SOL_2)
char arr1[arr1_sz_e];
#endif


int main(){
#if(PROBLEM_1_SOL_1)
  char arr1[arr1_sz];
#endif

  int i = 0;
  for(i=0; i < arr1_sz; i++)
    printf("arr1[%d]=%d\n", i, arr1[i]);

  return 0;
}

