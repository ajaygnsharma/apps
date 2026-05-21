Release Notes
==============================

v1.01:
  + First release

v1.02: Jan 22 2025
  + Cookies : Session expiration handled more gracefully and tells user to login again.
  + Cloning improvements: Some rare occurrences cause the cloning to skip some parameters.
  This is now ﬁxed and cleaner. Switching is also slightly faster.

v1.03: Jun 26 2025
  + Performance improvements and DDA Panel support

v1.04: July 25 2025
  + Release for replacement of ISATech with minor ﬁxes and related to 10MHz

v1.05: Oct 2 2025
  + Cleanup of Command Line Interface
  + Supply Voltage is secondary side so it is DC not AC even if the supply type is AC

v1.06: Oct 14 2025
  + Adding Ethernet Connection Alarm
  + Adding IF switch status to Web and Command line

v1.07: Feb 24 2026
  + First phase release of SNMP v1/v2c: 
  + Support for Information, Alarm, Sensor and Redundancy

v1.08: Mar 03 2026
  + SNMP v2c: Allowing any network to access the SNMP agent.

v1.09: Mar 12 2026
  + Fixing a conflicting redundant position of Antenna A and B going to spare.
  + SNMP v2c: Updated MIB to support TX Configuration for iBUCs.
  + Controller's 10MHz reference alarm is now default as Minor

v1.10: May 08 2026
  + Fixing IF switch incorrect position when BUC A and BUC B are at fault one after the other.
  + Event log tab added
  + Memory leak fixes related to SNMP long term running
  + SNMP v2c: Updated MIB to support Alarm Configuration
  + Alarm configuration for TXI controller module
