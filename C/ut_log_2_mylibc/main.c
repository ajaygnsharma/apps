#include <stdio.h>
#include <stdlib.h>

/*
 * main.c
 *
 *  Created on: Jul 6, 2017
 *      Author: ajay.sharma
 */
/***************************************************
*
*  reduce_expb - The expb routines require an input argument in
* the range[0.0, 0.5].This routine reduces the argument to that range.
*
* Return values:
* - "arg", which is the input argument reduced to that range
* - "two_int_a", which is 2**(int(arg))
* - "adjustment", which is an integer flag set to zero if the
*   fractional part of arg is <=0.5; set to one otherwise
*
* How this range reduction code works:
*  (1) 2**x = 2**a * 2**(x - a)
* and,
*  (2) 2**x = 2**(1/2) * 2**a * 2**(x - a - 1/2)
*
*  Now, this all looks pretty contrived. But the trick is to pick an
* integer "a" such that the term (x - a) is in the range [0.0, 1.0].
* If the result is in the range we use equation 1. If it winds up in
* [0.5, 1.0], use equation (2) which will get it down to our desired
* [0.0, 0.5] range.
*
*  The return value "adjustment" tells the calling function if we
* are using the first or the second equation.
*
***************************************************/
void reduce_expb(double *arg, double *two_int_a, int *adjustment){
 int int_arg;                 // integer part of the input argument
 *adjustment = 0;             // Assume we're using equation (2)
 int_arg =  (int) *arg;
 if((*arg - int_arg) > 0.5)   // if frac(arg) is in [0.5, 1.0]...
   {
       *adjustment = 1;
       *arg = *arg - 0.5;     // ... then change it to [0.0, 0.5]
   }
 *arg = *arg - (double) int_arg;   // arg == just the fractional part
// Now compute 2** (int) arg.
   *two_int_a = 1.0;
   for(; int_arg!=0; int_arg--)*two_int_a = *two_int_a * 2.0;
};

/**************************************************
*
* expb1063 - compute 2**x to 9.85 digits accuracy
*
***************************************************/
double expb1063(double arg){
  const double P00 = + 7.2152891521493;
  const double P01 = + 0.0576900723731;
  const double Q00 = +20.8189237930062;
  const double Q01 = + 1.0;
  const double sqrt2= 1.4142135623730950488; // sqrt(2) for scaling
  double two_int_a;                  // 2**(int(a))
  int    adjustment;                 // set to 1 by reduce_expb
  // ...adjust the answer by sqrt(2)
  double answer;                     // The result
  double Q;                          // Q(x**2)
  double x_P;                        // x*P(x**2)
  int    negative=0;                 // 0 if arg is +; 1 if negative
//  Return an error if the input is too large. "Too large" is
// entirely a function of the range of your floating point
// library and expected inputs used in your application.
  if(abs(arg>100.0)){
     printf("\nYikes! %f is a big number, fella. Aborting.", arg);
     return 0;
  }
//  If the input is negative, invert it. At the end we'll take
// the reciprocal, since n**(-1) = 1/(n**x).
  if(arg<0.0)
  {
     arg = -arg;
     negative = 1;
  }
  reduce_expb(&arg, &two_int_a, &adjustment);// reduce to [0.0, 0.5]
// The format of the polynomial is:
//  answer=(Q(x**2) + x*P(x**2))/(Q(x**2) - x*P(x**2))
//
//  The following computes the polynomial in several steps:
  Q   =        Q00 + Q01 * (arg * arg);
  x_P = arg * (P00 + P01 * (arg * arg));
  answer= (Q + x_P)/(Q - x_P);

// Now correct for the scaling factor of 2**(int(a))
  answer= answer * two_int_a;

// If the result had a fractional part > 0.5, correct for that
  if(adjustment == 1)answer=answer * sqrt2;

// Correct for a negative input
  if(negative == 1) answer = 1.0/answer;
  return(answer);
};

/**************************************************
*
* expd_based_on_expb1063 - compute 10**x to 9.85 digits accuracy
*
*  This is one approach to computing 10**x. Note that:
* 10**x = 2** (x / log_base_10(2)), so
* 10**x = expb(x/log_base_10(2))
*
***************************************************/
double expd_based_on_expb1063(double arg){
   const double log10_2 = 0.30102999566398119521;
   return(expb1063(arg/log10_2));
}

/***************************************************
*
*  reduce_log2 - The log2 routines require an input argument in
* the range[0.5, 1.0].This routine reduces the argument to that range.
*
* Return values:
* - "arg", which is the input argument reduced to that range
* - "n", which is n from the discussion below
* - "adjustment", set to 1 if the input was < 0.5
*
* How this range reduction code works:
*  If we pick an integer n such that x=f x 2**n, and 0.5<= f < 1, and
* assume all "log" functions in this discussion means log(base 2),
* then:
* log(x) = log(f x 2**n)
*        = log(f) + log(2**n)
*        = log(f) + n
*
*  The ugly "for" loop shifts a one through two_to_n while shifting a
* long version of the input argument right, repeating till the long
* version is all zeroes. Thus, two_to_n and the input argument have
* this relationship:
*
*       two_to_n        input argument           n
*        1                0.5 to 1               0
*        2                  1 to 2               1
*        4                  2 to 4               2
*        8                  4 to 8               3
* etc.
*
*  There's a special case for the argument being less than 0.5.
* In this case we use the relation:
*  log(1/x)=log(1)-log(x)
*          =      -log(x)
* That is, we take the reciprocal of x (which will be bigger than
* 0.5) and solve for the above. To tell the caller to do this, set
* "adjustment" = 1.
*
***************************************************/
void reduce_log2(double *arg, double *n, int *adjustment){
  long two_to_n;               // divisor we're looking for: 2^(2k)
  long l_arg;                  // long (32 bit) version of input
  *adjustment=0;
  if(*arg<0.5){                // if small arg use the reciprocal
     *arg=1.0/(*arg);
     *adjustment=1;
  }
  // shift arg to zero while computing two_to_n as described above
  l_arg=(long) *arg;            // l_arg = long version of input
  for(two_to_n=1, *n=0.0; l_arg!=0; l_arg>>=1, two_to_n<<=1, *n+=1.0);
  *arg=*arg / (double) two_to_n;// normalize argument to [0.5, 1]
};

/**************************************************
*
* LOG2_2521 - compute log(base 2)(x) to 4.14 digits accuracy
*
***************************************************/
double log2_2521(double arg){
  const double P00 = -1.45326486;
  const double P01 = +0.951366714;
  const double P02 = +0.501994886;
  const double Q00 = +0.352143751;
  const double Q01 = +1.0;
  double n;                            // used to scale result
  double poly;                         // result from polynomial
  double answer;                       // The result
  int    adjustment;                   // 1 if argument was < 0.5

//  Return an error if the input is <=0 since log is not real at and
// below 0.
  if(arg<=0.0){
     printf("\nHoly smokes! %f is too durn small. Aborting.", arg);
     return 0;
  }
  reduce_log2(&arg, &n, &adjustment);   // reduce  to [0.5, 1.0]
// The format of the polynomial is P(x)/Q(x)
  poly= (P00 + arg * (P01 + arg * P02)) / (Q00 + Q01 * arg);

// Now correct for the scaling factors
  if(adjustment)answer = -n - poly;
  else answer=poly + n;
  return(answer);
};


int main(void){
    double x = 2;
    printf("ln x=%f\n",log2_2521(x));

    x = 10;
    printf("ln x=%f\n",log2_2521(x));

    return 0;
}
