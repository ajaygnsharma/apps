/*
 * taylor_series_log.c
 *
 *  Created on: Jul 6, 2017
 *      Author: ajay.sharma
 */

#include <stdio.h>

double term(double x, int p){
    double res = 1, t, i;
    t = (x - 1) / (x + 1);

    for (i = 0; i < p; i++){
        res = res * t;
    }

    res = res/p;
    //printf("res=%0.4f\n",res);

    return res;
}

double ln(double x){
    double res = 0;

    double t  = term(x, 1) + term(x, 3) + term(x, 5) + term(x, 7) + term(x, 9);
    //printf("t=%f\n",t);

    res = 2 *  t;
    return res;
}

int main(void){
    double x = 2;
    printf("ln x=%f\n",ln(x));

    x = 10;
    printf("ln x=%f\n",ln(x));

    x = 33.48;
    printf("ln x=%f\n",ln(x));

    x = 31.62;
    printf("ln %f=%f\n",x,ln(x));

    x = 29.76;
    printf("ln %f=%f\n",x,ln(x));

    x = 29.14;
    printf("ln %f=%f\n",x,ln(x));

    return 0;
}

