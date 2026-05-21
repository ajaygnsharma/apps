#include <string>
#include <cstring>
#include <cstdint>

#include "tx21MIB.h"
#include <lib_cfg_cntrlr_factory.h>
#include <lib_json.h>
#include <lib_error.h>

/* Auto-generated. Do not edit by hand. */

int snmp_scalars_tx21ModelNumber_get(std::string &out)
{
    cfg_cntrlr_factory_s fcfg{};
    Err err_read = j_read_fcfg_cntrlr(fcfg);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    size_t n = strnlen(fcfg.model_number, MAX_LEN);
    out.assign(fcfg.model_number, n);
    return 0;
}


int snmp_scalars_tx21SerialNumber_get(std::string &out)
{
    cfg_cntrlr_factory_s fcfg{};
    Err err_read = j_read_fcfg_cntrlr(fcfg);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    size_t n = strnlen(fcfg.serial_number, MAX_LEN);
    out.assign(fcfg.serial_number, n);
    return 0;
}


int snmp_scalars_tx21FirmwareVersion_get(std::string &out)
{
    cfg_cntrlr_s fcfg{};
    Err err_read = j_read_cfg_cntrlr(fcfg);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    size_t n = strnlen(fcfg.firmware_version, MAX_LEN);
    out.assign(fcfg.firmware_version, n);
    return 0;
}


int snmp_scalars_tx21HardwareVersion_get(std::string &out)
{
    cfg_cntrlr_factory_s fcfg{};
    Err err_read = j_read_fcfg_cntrlr(fcfg);
    if (err_read != Err::SUCCESS) {
        return -1;
    }
    size_t n = strnlen(fcfg.hardware_revision, MAX_LEN);
    out.assign(fcfg.hardware_revision, n);
    return 0;
}


