#pragma once
#include <string>
#include <cstdint>

/* Auto-generated wrappers for tx21MIB. */


int snmp_scalars_tx21ModelNumber_get(std::string &out);
int snmp_scalars_tx21SerialNumber_get(std::string &out);
int snmp_scalars_tx21FirmwareVersion_get(std::string &out);
int snmp_scalars_tx21HardwareVersion_get(std::string &out);

int snmp_tx21_buc_tx_enable_get(buc_e idx, u_long *out);
int snmp_tx21_buc_tx_frequency_get(buc_e idx, u_long *out);
int snmp_tx21_buc_tx_power_up_state_get(buc_e idx, u_long *out);
int snmp_tx21_buc_tx_power_up_delay_get(buc_e idx, u_long *out);
int snmp_tx21_buc_tx_read_power_mode_get(buc_e idx, u_long *out);
int snmp_tx21_buc_tx_burst_thold_mB_get(buc_e idx, u_long *out);
int snmp_tx21_buc_tx_gain_mode_get(buc_e idx, u_long *out);
int snmp_tx21_buc_tx_attenuation_get(buc_e idx, u_long *out);
int snmp_tx21_buc_tx_input_threshold_low_get(buc_e idx, u_long *out);
int snmp_tx21_buc_tx_input_threshold_high_get(buc_e idx, u_long *out);
int snmp_tx21_buc_tx_output_threshold_low_get(buc_e idx, u_long *out);
int snmp_tx21_buc_tx_output_threshold_high_get(buc_e idx, u_long *out);
int snmp_tx21_buc_tx_burst_timeout_get(buc_e idx, u_long *out);
int snmp_tx21_buc_tx_gain_control_get(buc_e idx, u_long *out);
