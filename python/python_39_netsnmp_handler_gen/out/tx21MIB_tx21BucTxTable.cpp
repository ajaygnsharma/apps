#include <string>
#include <cstring>
#include <cstdint>

#include "tx21MIB.h"
#include <lib_cfg_cntrlr_factory.h>
#include <lib_json.h>
#include <lib_error.h>

/* Auto-generated. Do not edit by hand. */

/* Wrappers for table: tx21BucTxTable */

int snmp_tx21_buc_tx_enable_get(buc_e idx, u_long *out)
{
    if (!out) return -1;
    ibuc_cfg_s fcfg{};
    Err err_read = j_read_cfg_ibuc(fcfg, idx, false);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    *out = (u_long)fcfg.tx_enable;
    return 0;
}


int snmp_tx21_buc_tx_frequency_get(buc_e idx, u_long *out)
{
    if (!out) return -1;
    ibuc_cfg_s fcfg{};
    Err err_read = j_read_cfg_ibuc(fcfg, idx, false);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    *out = (u_long)fcfg.monitor_freq;
    return 0;
}


int snmp_tx21_buc_tx_power_up_state_get(buc_e idx, u_long *out)
{
    if (!out) return -1;
    ibuc_cfg_s fcfg{};
    Err err_read = j_read_cfg_ibuc(fcfg, idx, false);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    *out = (u_long)fcfg.tx_pup_state;
    return 0;
}


int snmp_tx21_buc_tx_power_up_delay_get(buc_e idx, u_long *out)
{
    if (!out) return -1;
    ibuc_cfg_s fcfg{};
    Err err_read = j_read_cfg_ibuc(fcfg, idx, false);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    *out = (u_long)fcfg.tx_pup_delay;
    return 0;
}


int snmp_tx21_buc_tx_read_power_mode_get(buc_e idx, u_long *out)
{
    if (!out) return -1;
    ibuc_cfg_s fcfg{};
    Err err_read = j_read_cfg_ibuc(fcfg, idx, false);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    *out = (u_long)fcfg.power_monitor_mode;
    return 0;
}


int snmp_tx21_buc_tx_burst_thold_mB_get(buc_e idx, u_long *out)
{
    if (!out) return -1;
    ibuc_cfg_s fcfg{};
    Err err_read = j_read_cfg_ibuc(fcfg, idx, false);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    *out = (u_long)fcfg.burst_thold_mB;
    return 0;
}


int snmp_tx21_buc_tx_gain_mode_get(buc_e idx, u_long *out)
{
    if (!out) return -1;
    ibuc_cfg_s fcfg{};
    Err err_read = j_read_cfg_ibuc(fcfg, idx, false);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    *out = (u_long)fcfg.automatic_mode;
    return 0;
}


int snmp_tx21_buc_tx_attenuation_get(buc_e idx, u_long *out)
{
    if (!out) return -1;
    ibuc_cfg_s fcfg{};
    Err err_read = j_read_cfg_ibuc(fcfg, idx, false);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    *out = (u_long)fcfg.attenuation_mB;
    return 0;
}


int snmp_tx21_buc_tx_input_threshold_low_get(buc_e idx, u_long *out)
{
    if (!out) return -1;
    ibuc_cfg_s fcfg{};
    Err err_read = j_read_cfg_ibuc(fcfg, idx, false);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    *out = (u_long)fcfg.in_low_thold_mB;
    return 0;
}


int snmp_tx21_buc_tx_input_threshold_high_get(buc_e idx, u_long *out)
{
    if (!out) return -1;
    ibuc_cfg_s fcfg{};
    Err err_read = j_read_cfg_ibuc(fcfg, idx, false);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    *out = (u_long)fcfg.in_high_thold_mB;
    return 0;
}


int snmp_tx21_buc_tx_output_threshold_low_get(buc_e idx, u_long *out)
{
    if (!out) return -1;
    ibuc_cfg_s fcfg{};
    Err err_read = j_read_cfg_ibuc(fcfg, idx, false);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    *out = (u_long)fcfg.out_low_thold_mB;
    return 0;
}


int snmp_tx21_buc_tx_output_threshold_high_get(buc_e idx, u_long *out)
{
    if (!out) return -1;
    ibuc_cfg_s fcfg{};
    Err err_read = j_read_cfg_ibuc(fcfg, idx, false);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    *out = (u_long)fcfg.out_high_thold_mB;
    return 0;
}


int snmp_tx21_buc_tx_burst_timeout_get(buc_e idx, u_long *out)
{
    if (!out) return -1;
    ibuc_cfg_s fcfg{};
    Err err_read = j_read_cfg_ibuc(fcfg, idx, false);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    *out = (u_long)fcfg.burst_timeout;
    return 0;
}


int snmp_tx21_buc_tx_gain_control_get(buc_e idx, u_long *out)
{
    if (!out) return -1;
    status_s fcfg{};
    Err err_read = read(fcfg, idx, false);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    *out = (u_long)fcfg.gain_control_mB;
    return 0;
}


