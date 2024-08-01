/*
 * main.c
 *
 *  Created on: Jul 14, 2017
 *      Author: ajay.sharma
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OPTION_ADC_STUB 0 // For testing enable this to use a ADC software stub
#define SECOND_1        1000 // timer.c resolution is 1ms. So 1 sec is 1000x

/* An extra entry is kept to allow for the delta calculation to happen.
 Example: delta V = V1 - V2. For the last entry, V2=0, but in reality
 it will exist. So we take an extra measurement rather than assume the
 future measurement */
#define TOTAL_RESULTS (20 + 1)

typedef uint8_t timer_handle;

// Reg Addr = 0x24
struct reg_adc_s{
    uint8_t sel    :2;
    uint8_t voltage:6;
};

// Reg Addr = 0x1A
struct reg_regulator_en_s{
    uint8_t pfr_en:1;
    uint8_t ch3_en:1;
    uint8_t ch5_en:1;
    uint8_t ch1_en:1;
    uint8_t ch4_en:1;
    uint8_t ch6_en:1;
    uint8_t ch2_en:1;
    uint8_t low_pwr_mode:1;
};

// Reg Addr = 0x83
struct reg_misc_cfg_s{
    uint8_t i2c_addr_lsb:2;
    uint8_t reserved:2;
    uint8_t soft_start:2;
    uint8_t ch4_cfg:1;
    uint8_t disable_boot_switch:1;
};

// Reg Addr = 0x8F
struct reg_output_voltage_cfg_s{
    uint8_t holdup_cap_voltage:5;
    uint8_t reserved:3;
};

/**
 *  Overall plp component. It contains all the required registers in it.
 *  Read data as a byte and the required structures are filled up since it is a
 *  union of structures.
 */
union pmic_regs_u{
    uint8_t data;
    struct reg_adc_s adc;
    struct reg_regulator_en_s regulator_en;
    struct reg_misc_cfg_s     misc_cfg;
    struct reg_output_voltage_cfg_s output_voltage_cfg;
};

/**
 * PMIC Data structure. Contains all information about the PMIC
 */
struct __attribute__((packed)) pmic_s{
    const uint8_t SLAVE_ADDR;               // PMIC I2C Slave address
    // Addresses of the registers
    const uint8_t REG_REGULATOR_EN;
    const uint8_t REG_ADC;
    const uint8_t REG_MISC_CFG;
    const uint8_t REG_OUTPUT_VOLTAGE_CFG_2;
    // Actual data for the registers.
    union pmic_regs_u pmic_reg;
};

typedef enum{
    TIME_IN_SEC,
    TIME_IN_MSEC
}DELAY_PRECISION;

// cap leak test request structure
typedef struct cap_leak_test_req_data_s {
    uint16_t  loops;
    uint16_t period;
    uint16_t current_threshold;
    uint16_t resistance_threshold;
} cap_leak_test_req_data_t;

struct cap_leak_test_raw_data_s{
    uint16_t time;    // ms
    uint16_t voltage; // V
};

// TODO: For now the response is a full table of the readings with time
// and measurement. Once we are confirm that the results are good. We will
// update the with the screening log requirement for HK6.
typedef struct cap_leak_test_result_s {
    uint32_t                        current;
    uint32_t                        resistance;
    struct cap_leak_test_raw_data_s raw_data[TOTAL_RESULTS];
}cap_leak_test_result_t;

/**
 * Cap leak test structure. Contains information about the test.
 */
struct __attribute__((packed)) cap_leak_test_s {
    // The command line argument gives total time = period * loops
    const uint16_t TOTAL_TIME;
    const uint8_t  CHARGE_VOLTAGE;
    const uint8_t  PERIOD;
    // Thresholds for pass/fail critirea.
    // TODO: This is not used yet. Get reply from Japan
    const uint16_t CURRENT_LIMIT;
    const uint8_t  VOLTAGE_TOLERANCE;
    uint16_t capacitance;             // calculated from cap test
    struct cap_leak_test_raw_data_s present_result;
    uint8_t result_index;
    struct pmic_s pmic;
    timer_handle timer;
    cap_leak_test_result_t *results;
};


/**
 * @fn    result_clear
 *
 * @brief To avoid stale results, clear them
 *
 * @param c - pointer to the present result
 *
 * @return none
 */
static void result_clear(cap_leak_test_result_t *r){
    uint8_t  i = 0;
    cap_leak_test_result_t t = {};
    *r = t;
    for(i = 0; i < TOTAL_RESULTS; i++){
        printf("%3d,%d.%02d\n",\
                r->raw_data[i].time, \
                r->raw_data[i].voltage / 100,\
                r->raw_data[i].voltage % 100);
    }
}

/**
 * @fn    current_calculate
 *
 * @brief calculate the current for every time and voltage reading.
 *
 * @param c - pointer to the cap leak test structure. Its current value is
 *            updated here.
 *
 * @return 0 for now
 */
static uint8_t result_show(struct cap_leak_test_s *c){
    uint8_t  i = 0;

    printf("Time(s),Voltage(V)");

    for(i = 0; i < TOTAL_RESULTS; i++){
        printf("%3d,%d.%02d\n",\
             c->results->raw_data[i].time, \
             c->results->raw_data[i].voltage / 100,\
             c->results->raw_data[i].voltage % 100);
    }
    return 0;
}

/**
 * @fn    pwr_ctrl_cap_leak_test
 * @brief Performs the cap leak test.
 *
 * @param capacitance - Result of cap test
 * @param req         - Incoming command request containing loops and period
 * @param p           - Result pointer. This gets populated during the test.
 *
 * @return 0     if the test passes.
 * @return >0    Error codes for failures. TODO: Japan to figure them out
 */
uint8_t pwr_ctrl_cap_leak_test
(uint16_t capacitance, \
 cap_leak_test_req_data_t req, cap_leak_test_result_t *p){
    struct cap_leak_test_s c = {
            .TOTAL_TIME        = (req.period * req.loops),
            .CHARGE_VOLTAGE    = 33,
            .CURRENT_LIMIT     = 200,
            .PERIOD            = req.period,
            .VOLTAGE_TOLERANCE = 3,
            .capacitance       = capacitance,
            .present_result.time    = 0,
            .present_result.voltage = 0,
            .result_index      = 0,
            .pmic.SLAVE_ADDR               = 0x54,
            .pmic.REG_REGULATOR_EN         = 0x1A,
            .pmic.REG_ADC                  = 0x24,
            .pmic.REG_MISC_CFG             = 0x83,
            .pmic.REG_OUTPUT_VOLTAGE_CFG_2 = 0x8F,
            .timer = 0,
            /* Memory is conserved since 'results' points to the XMODEM
             * response buffer. This is also cleared at test initialization */
            .results = p
    };

    result_clear(c.results);
    result_show(&c);

    return 0;
}


int main(void){
    uint8_t rsp[2000];

    cap_leak_test_req_data_t req;

    req.loops  = 5;
    req.period = 10;
    req.current_threshold    = 4300;
    req.resistance_threshold = 5800;

    cap_leak_test_result_t *cap_leak_test_rsp = \
                           (cap_leak_test_result_t *)rsp;
    pwr_ctrl_cap_leak_test(320, req, cap_leak_test_rsp);

    return 0;
}
