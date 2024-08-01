/*
 * main.c
 *
 *  Created on: Jun 28, 2017
 *      Author: ajay.sharma
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PLP_CAP_LEAK_TEST_TOTAL_RESULTS 20

typedef struct plp_diag_cap_leak_test_req_data_s {
    uint8_t  loops;
    uint16_t period;
} plp_diag_cap_leak_test_req_data_t;

typedef struct plp_diag_cap_leak_test_rsp_data_s {
    uint16_t                        capacitance;
    uint16_t                        charge_voltage;
    struct cap_leak_test_raw_data_s raw_data[PLP_CAP_LEAK_TEST_TOTAL_RESULTS];
} plp_diag_cap_leak_test_rsp_data_t;


typedef struct cap_leak_test_raw_data_s{
    uint16_t time;    // ms
    uint16_t voltage; // V
}cap_leak_test_raw_data_t;

typedef struct leakage_current_table_s{
    uint16_t t;
    uint16_t v;
    uint16_t v2;
    uint16_t v1;
    uint16_t t2;
    uint16_t t1;
    float i;
}cap_leak_test_current_table_t;

typedef struct leakage_resistance_table_s{
    uint16_t t;    // ms
    uint16_t v; // V
    double   r;
}cap_leak_test_resistance_table_t;

cap_leak_test_current_table_t *cap_leak_test_current_init
(cap_leak_test_raw_data_t *raw_data, uint16_t len){
    cap_leak_test_current_table_t *lct =
       malloc(PLP_CAP_LEAK_TEST_TOTAL_RESULTS * sizeof(cap_leak_test_current_table_t));
    uint16_t i;

    for(i = 0; i< len; i++){
        lct[i].t  = raw_data->time;
        lct[i].v  = raw_data->voltage;
        lct[i].v2 = raw_data->voltage;
        lct[i].v1 = (raw_data+1)->voltage;
        lct[i].t2 = (raw_data+1)->time;
        lct[i].t1 = raw_data->time;
        raw_data++;
    }

    return lct;
}

void cap_leak_test_current_delete(cap_leak_test_current_table_t *ptr){
    free(ptr);
}

void cap_leak_test_current_calculate
(cap_leak_test_current_table_t *lct,uint16_t len,uint16_t capacitance){
    uint16_t i;
    uint16_t t2 = 0, t1 = 0, c = 0;
    float v1 = 0, v2 =0;
    cap_leak_test_current_table_t *ptr = lct;
    c = capacitance;

    for(i = 0; i< len; i++){
        v2 = (float)ptr->v2 / 100;
        v1 = (float)ptr->v1 / 100;
        t2 = ptr->t2;
        t1 = ptr->t1;
        if((t2-t1) != 0){
            ptr->i = (float)(c * (v2 - v1) / (t2 - t1));
        }
        ptr++;
    }
}

void cap_leak_test_current_dump
(cap_leak_test_current_table_t *cltct,uint16_t len){
    uint16_t i;

    printf("Time(sec), Voltage(V), Current(uA)\n");
    for(i = 0; i< len; i++){
        printf("%3d, %.02f, %.02f\n", cltct->t,(float)cltct->v/100,cltct->i);
        cltct++;
    }
}


cap_leak_test_resistance_table_t *cap_leak_test_resistance_init
(cap_leak_test_raw_data_t *raw_data, uint16_t len){

    cap_leak_test_resistance_table_t *cltrt = \
      malloc(PLP_CAP_LEAK_TEST_TOTAL_RESULTS * sizeof(cap_leak_test_resistance_table_t));
    uint16_t i;

    for(i = 0; i< len; i++){
        cltrt[i].t    = raw_data->time;
        cltrt[i].v    = raw_data->voltage;
        cltrt[i].r = 0;
        raw_data++;
    }
    return cltrt;
}

void cap_leak_test_resistance_calculate
(cap_leak_test_resistance_table_t *cltrt,uint16_t capacitance,uint16_t v, uint16_t len){
    uint16_t i;
    uint16_t c = capacitance;
    float v1;
    double r;
    uint16_t t = 0;

    cap_leak_test_resistance_table_t *ptr = cltrt;
    for(i = 0; i < len; i++){
        t = ptr->t;
        v1 = (float)ptr->v / 100;
        r = t / ( c * (log(v) - log(v1)) );
        ptr->r = r;
        ptr++;
    }
}

void cap_leak_test_resistance_dump(cap_leak_test_resistance_table_t *t, uint16_t len){
    uint16_t i;
    printf("T(sec), V(V), R(Mohm)\n");
    for(i = 0; i < len; i++){
        printf("%d,%d,%.02f\n",t->t, t->v, t->r);
        t++;
    }
}


void cap_leak_test_resistance_delete(cap_leak_test_resistance_table_t *ptr){
    free(ptr);
}

int main(void){
    static uint16_t data[] = { 318, 33,
            0, 3410,
            30, 3162,
            60, 3038,
            90, 2914,
            120, 2790,
            150, 2666,
            180, 2604,
            210, 2480,
            240, 2418,
            270, 2294,
            300, 2232
    };

    uint16_t len = 10;

    plp_diag_cap_leak_test_rsp_data_t *d = \
            (plp_diag_cap_leak_test_rsp_data_t *)data;

    printf("C=%d uF\nV=%d V\n",d->capacitance,d->charge_voltage);

    cap_leak_test_current_table_t *cltc_ptr = NULL;

    cltc_ptr = cap_leak_test_current_init(d->raw_data, len);
    cap_leak_test_current_calculate(cltc_ptr,len,d->capacitance);
    cap_leak_test_current_dump(cltc_ptr, len);
    cap_leak_test_current_delete(cltc_ptr);

    printf("\n\n");

    cap_leak_test_resistance_table_t *cltr_ptr = NULL;
    cltr_ptr =  cap_leak_test_resistance_init(d->raw_data, len);
    cap_leak_test_resistance_calculate(cltr_ptr, d->capacitance,\
            d->charge_voltage, len);
    cap_leak_test_resistance_dump(cltr_ptr, len);
    cap_leak_test_resistance_delete(cltr_ptr);

    return 0;
}
