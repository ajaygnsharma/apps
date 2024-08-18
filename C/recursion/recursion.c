/*
 * recursion.c
 *
 *  Created on: Mar 7, 2019
 *      Author: user1
 */

#include <stdio.h>
long int factorial(int n);

int main(){
    int n = 3;
    printf("Factorial of %d = %ld\n", n, factorial(n));
    return 0;
}

long int factorial(int n){
    if (n >= 1)
        return n * factorial(n-1);
    else
        return 1;
}
