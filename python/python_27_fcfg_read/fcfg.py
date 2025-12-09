from dataclasses import dataclass

MAX_STR_LEN  = 51

@dataclass
class FactoryCntrlrData:
    serial_number: str
    model_number: str
    private_label: str
    hardware_revision: str
    total_sys_hours: int
    internal_10MHz: int
    supply_voltage_min: int
    supply_voltage_max: int

mnm = { "prfx"      : "TXI",
        "rdd_opt"   : [1, 2],
        "ten_MHz_opt": [0, 1, 2],
        "intf_opt"  : ['N', 'F', 'K'],
        "supply_opt": ['A', 'D'],
        "AC_range"  : [ 100, 240 ],
        "DC_range"  : [ 30,  60  ],
}

def configure_fcfg(fcfg: FactoryCntrlrData) -> FactoryCntrlrData:
    def set_AC_def():
        fcfg.internal_10MHz = 0
        fcfg.supply_voltage_min = mnm["AC_range"][0]
        fcfg.supply_voltage_max = mnm["AC_range"][1]


    if fcfg.model_number == "Default":
        set_AC_def()

    elif len(fcfg.model_number) <= MAX_STR_LEN:
        prfx        = fcfg.model_number[:3]
        rdd_opt     = int(fcfg.model_number[4])
        ten_MHz_opt = int(fcfg.model_number[5])
        intf_opt    = fcfg.model_number[6]
        supply_opt  = fcfg.model_number[7]


        if prfx        == mnm["prfx"]        and\
           rdd_opt     in mnm["rdd_opt"]     and\
           ten_MHz_opt in mnm["ten_MHz_opt"] and\
           supply_opt  in mnm["supply_opt"]  and\
           intf_opt    in mnm["intf_opt"]:

            fcfg.internal_10MHz = mnm["ten_MHz_opt"][ten_MHz_opt]
            if supply_opt == 'A':
                fcfg.supply_voltage_min = mnm["AC_range"][0]
                fcfg.supply_voltage_max = mnm["AC_range"][1]
            else:
                fcfg.supply_voltage_min = mnm["DC_range"][0]
                fcfg.supply_voltage_max = mnm["DC_range"][1]
        else:
            set_AC_def()

    else:
        set_AC_def()

    return fcfg

