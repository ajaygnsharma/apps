Release Notes
==============================

5.10
    • Fixed bus timing between Ethernet and microprocessor.
    • F/W was not detecting new bottom board correctly.
5.11
    • Fixed problem with SNMP GET message not allowing more than 5 OIDs.
5.12
    • TCP/IP abort was not being handled correctly by Telnet.
5.13
    • Serial upgrade utility was not working.

5.14
    * 
5.15
    * 
5.16
    • Added overdrive point for -0009 option

5.17
    • Web page would allow a user to switch to a bad unit in automatic mode.
    • Disable alarms still show in command line and event log.
    • Allow -1508 option to set Buc position on switch manually.
    • Clone link was resetting the major alarm mask on the Rabbit M&C.

5.18
    • Corrected current reading being oﬀ on 5 watt Ka band.
5.19
    • TX1+1 automatic BUC position setting was intermittent.
5.20
    • Fixed “Enable AC 50Hz Correction” resetting when “Tx Cfg” accessed.
5.21
    • Added support for Global Star IBR050052.
    • Overdrive and AGC/ALC out of range alarms did not work.
5.22
    • Fixed TX1+1 webpages not displaying properly when mapped through routers.
    • Fixed problem with binary protocol that caused a runtime error in the BucMon program.
5.23
    • Support for new model numbers
5.24
    • Support for new model numbers

5.25
    • Support for new model numbers
5.26
    • Support for new model numbers

5.27

5.28
    • Not Released.
5.29
    • Added functionality for IBUC3
    • Fixed power out not showing -99.0 when the IBUC is muted.
5.30
    • Fixed bug that sets the model number to Undeﬁned on IBUC3.
5.31
    • Added feature to oﬀset output power reading by +/- 3 dB.
    • Added ability to dynamically change the input min/max value (factory only).
    • Added 500 Watt to power table (factory only).
    • Fixed SNMP responds to empty community string.
5.32
    • Not bug ﬁxes
5.33
    • Fixed attenuator not being set from FSK.
5.35
    • Shutdown PA before reboot.
    • Rework frequency synthesizer values.
    • Add IBG model number.
    • Add PA shutdown sequencing for GaAs units.
5.36
    • Fixed AGC/ALC not starting properly from SNMP.
5.37
    • Fixed intermittent GaN conﬁguration detection.
5.38
    • Maintenance release
5.39
    • Maintenance release
5.40
    • Fixed snmp2c set problem
5.41
    • Maintenance release
5.42
    • Fixed problem with submit button on TxCfg webpage causing transmitter to mute momentarily.
5.43
    • Added driver for new hardware.
5.44
    • Increased the time on startup before the PA is enabled to 30 seconds.
5.45
    • TX1+1 – Powering oﬀ A side then back on causes a double waveguide actuation.
5.46
    • Fixed problem with random packets causing the Ethernet to stop working.
5.47
    • Upgraded networking stack to latest version.
    • Burst mode thresholds were not being restored on reboot.
    • Adjusted timing on SPI bus to ﬁx varying capacitance causing some DACs to not write properly.
5.48
    • Fixed a bus timing issue causing the ethernet to lose connection requiring a reboot to recover.
5.49
    • Added more memory to the network stack for high traﬃc conditions.
5.50
    • Added 60 GaN to conﬁguration.
5.51
    • Added -1505 to the overdrive protection.
5.52
    • Added support for Ka Triband.
5.54
    • Aicox overdrive level and variable IF input level.
5.55o
    • Support for Reverse Power + High Stability 10MHz + TST output correction.
5.58
    • Hot Fix based oﬀ v5.54 to support only High stability 10MHz. Reverse power is not
      yet released to main products. TST Output will be added with it too. It will be added next.
5.59
    • Hot Fix to support 83 in model number wrt Watts
    • Hot Fix to support 801 in model number

5.60
    • Hot Fix to support High Stability 10MHz. This is supposed to be there but v5.59 does not seem to support it. So, recompiling it again and putting it together.

5.61 (Aug 08, 2025)
    Features:

    • Adding support for high stability 10MHz
    • Reverse Output Power Support
    • New model numbers added
    • TST SNMP Output response now matches MIB
    • Dual(commercial) and Tri (Military) band support
    • TML: new command shows the 10MHz state
    • SNMP MIB updated for all features above

5.62 (Sep 18, 2025)
    • Features – Command TML now includes echo. TML-> TML=1 or TML=0

5.63 - Not realeased to production. 
    • Added support for Quad band

5.64 - Not realeased to production.
    • Support for Dual KU band

5.65 - Not realeased to production.
    • Support for Reverse Power for 800 W units

5.66 (Jan 19, 2026) 
    • Merge of Quad Band(5.63) and Reverse Power(5.65) support to main production firmware.

