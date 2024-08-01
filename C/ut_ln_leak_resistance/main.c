/*
 * main.c
 *
 *  Created on: Jul 24, 2017
 *      Author: ajay.sharma
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct{
    uint8_t reg_val;
    int32_t ln;
}ln_lut_t;

ln_lut_t ln_lut[] = {
        { 0,   0},
        { 1,-470},
        { 2, 223},
        { 3, 629},
        { 4, 916},
        { 5,1139},
        { 6,1322},
        { 7,1476},
        { 8,1609},
        { 9,1727},
        {10,1833},
        {11,1928},
        {12,2015},
        {13,2095},
        {14,2169},
        {15,2238},
        {16,2303},
        {17,2363},
        {18,2420},
        {19,2474},
        {20,2526},
        {21,2575},
        {22,2621},
        {23,2665},
        {24,2708},
        {25,2749},
        {26,2788},
        {27,2826},
        {28,2862},
        {29,2897},
        {30,2931},
        {31,2964},
        {32,2996},
        {33,3027},
        {34,3056},
        {35,3085},
        {36,3114},
        {37,3141},
        {38,3168},
        {39,3194},
        {40,3219},
        {41,3244},
        {42,3268},
        {43,3291},
        {44,3314},
        {45,3337},
        {46,3359},
        {47,3380},
        {48,3401},
        {49,3422},
        {50,3442},
        {51,3462},
        {52,3481},
        {53,3500},
        {54,3519},
        {55,3537},
        {56,3555},
        {57,3573},
        {58,3590},
        {59,3608},
        {60,3624},
        {61,3641},
        {62,3657},
        {63,3673},
};

int32_t ln_lookup(uint8_t reg_val){
    int32_t ln = 0;
    ln = ln_lut[reg_val].ln;
    return ln;
}

uint32_t calculate_R
(uint16_t T, uint16_t C, uint8_t V_reg_val, uint8_t V1_reg_val){
    uint32_t R;
    int32_t lnV  = ln_lookup(V_reg_val);
    int32_t lnV1 = ln_lookup(V1_reg_val);

    R = T * (1e5) / (C * (lnV - lnV1));
    //printf("1000R=%d\n",R);
    return R;
}
int main(void){
    uint16_t T = 0, C=0;
    T = 90; C = 396;
    printf("T= 90,R=%d\n",calculate_R(T,C,53,45));
    T = 180;
    printf("T=180,R=%d\n",calculate_R(T,C,53,40));
    T = 270;
    printf("T=270,R=%d\n",calculate_R(T,C,53,36));
    T = 360;
    printf("T=360,R=%d\n",calculate_R(T,C,53,33));
    T = 540;
    printf("T=540,R=%d\n",calculate_R(T,C,53,26));
    T = 990;
    printf("T=990,R=%d\n",calculate_R(T,C,53,15));
    T = 1710;
    printf("T=1710,R=%d\n",calculate_R(T,C,53,7));

    return 0;
}
