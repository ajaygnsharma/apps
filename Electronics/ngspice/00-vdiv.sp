******************************************************************************
*          Electronic circuit simulation file generated by gSpiceUI          *
*                        Version 1.2.87 (2020-09-18)                         *
******************************************************************************

* voltage divider netlist

********** Component Definitions **********

R1 in out 1k
R2 out 0 2k
V1 in 0 1

********** NG-Spice Simulation Commands **********

.OPTIONS NOPAGE WIDTH=56
.PRINT DC V(in)
.DC TEMP 0.00 100.00 10.00

.END
