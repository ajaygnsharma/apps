// OID .1.3.6.1.4.1.21369.1.6.24
const mib_node_t input_freq_rng_sel =
{
	.oid_frag = {24},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = NULL,
	.snmp_type = SNMP_INT,
	.cb = ibrInputFreqRngSelect
};

// OID .1.3.6.1.4.1.21369.1.6.23
const mib_node_t multiband_sel =
{
	.oid_frag = {23},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &input_freq_rng_sel,
	.snmp_type = SNMP_INT,
	.cb = ibrTxMultiBandSelect
};

// OID .1.3.6.1.4.1.21369.1.6.22
const mib_node_t loadswitch =
{
	.oid_frag = {22},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &multiband_sel,
	.snmp_type = SNMP_INT,
	.cb = ibrTxLoadSwitch
};

// OID .1.3.6.1.4.1.21369.1.6.21
const mib_node_t poutoffset =
{
	.oid_frag = {21},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &loadswitch,
	.snmp_type = SNMP_INT,
	.cb = ibrPoutOffset
};

// OID .1.3.6.1.4.1.21369.1.6.20
const mib_node_t keylinedisable =
{
	.oid_frag = {20},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &poutoffset,
	.snmp_type = SNMP_INT,
	.cb = ibrKeylineDisable
};

// OID .1.3.6.1.4.1.21369.1.6.19.1.2
const mib_node_t statalm =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &statlogentry,
	.child = NULL,
	.sibling = NULL,
	.snmp_type = SNMP_OCT_STR | MIB_TABLE,
	.cb = ibrStatLogEntry
};

// OID .1.3.6.1.4.1.21369.1.6.19.1
const mib_node_t statlogentry =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucEventLog,
	.child = &statalm,
	.sibling = NULL,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.6.19
const mib_node_t ibucEventLog =
{
	.oid_frag = {19},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = &statlogentry,
	.sibling = &keylinedisable,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.6.18.0
const mib_node_t stat_interval =
{
	.oid_frag = {18},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &ibucEventLog,
	.snmp_type = SNMP_INT,
	.cb = ibrStatLogInterval
};

// OID .1.3.6.1.4.1.21369.1.6.17
const mib_node_t clrstatlog =
{
	.oid_frag = {17},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &stat_interval,
	.snmp_type = SNMP_INT,
	.cb = ibrClearStatLog
};

// OID .1.3.6.1.4.1.21369.1.6.16.1.2
const mib_node_t logalm =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &almlogentry,
	.child = NULL,
	.sibling = NULL,
	.snmp_type = SNMP_OCT_STR | MIB_TABLE,
	.cb = ibrAlarmLogAlarm
};

// OID .1.3.6.1.4.1.21369.1.6.16.1
const mib_node_t almlogentry =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucAlarmLog,
	.child = &logalm,
	.sibling = NULL,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.6.16
const mib_node_t ibucAlarmLog =
{
	.oid_frag = {16},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = &almlogentry,
	.sibling = &clrstatlog,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.6.15.0
const mib_node_t txmuted =
{
	.oid_frag = {15},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &ibucAlarmLog,
	.snmp_type = SNMP_INT,
	.cb = ibrTxMutedFault
};

// OID .1.3.6.1.4.1.21369.1.6.14.0
const mib_node_t int10mhz =
{
	.oid_frag = {14},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &txmuted,
	.snmp_type = SNMP_INT,
	.cb = ibrInternal10MHz
};

// OID .1.3.6.1.4.1.21369.1.6.13.0
const mib_node_t voltrange =
{
	.oid_frag = {13},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &int10mhz,
	.snmp_type = SNMP_OCT_STR,
	.cb = ibrVoltageRange
};

// OID .1.3.6.1.4.1.21369.1.6.12.0
const mib_node_t specinv =
{
	.oid_frag = {12},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &voltrange,
	.snmp_type = SNMP_OCT_STR,
	.cb = ibrSpectralInversion
};

// OID .1.3.6.1.4.1.21369.1.6.11.0
const mib_node_t lofreq =
{
	.oid_frag = {11},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &specinv,
	.snmp_type = SNMP_OCT_STR,
	.cb = ibrLOFrequency
};

// OID .1.3.6.1.4.1.21369.1.6.10.0
const mib_node_t ifrange =
{
	.oid_frag = {10},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &lofreq,
	.snmp_type = SNMP_OCT_STR,
	.cb = ibrIFRange
};

// OID .1.3.6.1.4.1.21369.1.6.9.0
const mib_node_t rfrange =
{
	.oid_frag = {9},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &ifrange,
	.snmp_type = SNMP_OCT_STR,
	.cb = ibrRFRange
};

// OID .1.3.6.1.4.1.21369.1.6.8.0
const mib_node_t hwver =
{
	.oid_frag = {8},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &rfrange,
	.snmp_type = SNMP_OCT_STR,
	.cb = ibrHardwareVersion
};

// OID .1.3.6.1.4.1.21369.1.6.7.0
const mib_node_t int10trim =
{
	.oid_frag = {7},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &hwver,
	.snmp_type = SNMP_INT,
	.cb = ibrInternal10MHzTrim
};

// OID .1.3.6.1.4.1.21369.1.6.6.0
const mib_node_t extled =
{
	.oid_frag = {6},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &int10trim,
	.snmp_type = SNMP_INT,
	.cb = ibrEnableExtLED
};

// OID .1.3.6.1.4.1.21369.1.6.5.0
const mib_node_t webtitle =
{
	.oid_frag = {5},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &extled,
	.snmp_type = SNMP_OCT_STR,
	.cb = ibrWebPageTitle
};

// OID .1.3.6.1.4.1.21369.1.6.4.0
const mib_node_t tzoffset =
{
	.oid_frag = {4},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &webtitle,
	.snmp_type = SNMP_INT,
	.cb = ibrTimezoneOffset
};

// OID .1.3.6.1.4.1.21369.1.6.3.0
const mib_node_t ntpaddr =
{
	.oid_frag = {3},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &tzoffset,
	.snmp_type = SNMP_IP_ADDR,
	.cb = ibrNtpIpAddr
};

// OID .1.3.6.1.4.1.21369.1.6.2.0
const mib_node_t rtclk =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &ntpaddr,
	.snmp_type = SNMP_OCT_STR,
	.cb = ibrRealTimeClock
};

// OID .1.3.6.1.4.1.21369.1.6.1.0
const mib_node_t addrpublic =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucMCv2,
	.child = NULL,
	.sibling = &rtclk,
	.snmp_type = SNMP_IP_ADDR,
	.cb = ibrIPAddressOtherPublic
};

// OID .1.3.6.1.4.1.21369.1.6
const mib_node_t ibucMCv2 =
{
	.oid_frag = {6},
	.oid_len = 1,
	.parent = &ibucMIB,
	.child = &addrpublic,
	.sibling = NULL,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.3.3.9.0
const mib_node_t pwrRev =
{
	.oid_frag = {9},
	.oid_len = 1,
	.parent = &ibucDetector,
	.child = NULL,
	.sibling = NULL,
	.snmp_type = SNMP_INT,
	.cb = ibrReversePowerLevel
};

// OID .1.3.6.1.4.1.21369.1.3.3.8.0
const mib_node_t tenmhzsrc =
{
	.oid_frag = {8},
	.oid_len = 1,
	.parent = &ibucDetector,
	.child = NULL,
	.sibling = &pwrRev,
	.snmp_type = SNMP_INT,
	.cb = ibr10MHzSource
};

// OID .1.3.6.1.4.1.21369.1.3.3.7.0
const mib_node_t pwrout =
{
	.oid_frag = {7},
	.oid_len = 1,
	.parent = &ibucDetector,
	.child = NULL,
	.sibling = &tenmhzsrc,
	.snmp_type = SNMP_INT,
	.cb = ibrOutputPowerLevel
};

// OID .1.3.6.1.4.1.21369.1.3.3.6.0
const mib_node_t pwrin =
{
	.oid_frag = {6},
	.oid_len = 1,
	.parent = &ibucDetector,
	.child = NULL,
	.sibling = &pwrout,
	.snmp_type = SNMP_INT,
	.cb = ibrInputPowerLevel
};

// OID .1.3.6.1.4.1.21369.1.3.3.5.0
const mib_node_t gainctrl =
{
	.oid_frag = {5},
	.oid_len = 1,
	.parent = &ibucDetector,
	.child = NULL,
	.sibling = &pwrin,
	.snmp_type = SNMP_INT,
	.cb = ibrGainControl
};

// OID .1.3.6.1.4.1.21369.1.3.3.4.0
const mib_node_t temperature =
{
	.oid_frag = {4},
	.oid_len = 1,
	.parent = &ibucDetector,
	.child = NULL,
	.sibling = &gainctrl,
	.snmp_type = SNMP_INT,
	.cb = ibrTemperature
};

// OID .1.3.6.1.4.1.21369.1.3.3.3.0
const mib_node_t drovolt =
{
	.oid_frag = {3},
	.oid_len = 1,
	.parent = &ibucDetector,
	.child = NULL,
	.sibling = &temperature,
	.snmp_type = SNMP_INT,
	.cb = ibrDROVoltage
};

// OID .1.3.6.1.4.1.21369.1.3.3.2.0
const mib_node_t current =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &ibucDetector,
	.child = NULL,
	.sibling = &drovolt,
	.snmp_type = SNMP_INT,
	.cb = ibrSupplyCurrent
};

// OID .1.3.6.1.4.1.21369.1.3.3.1.0
const mib_node_t voltage =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucDetector,
	.child = NULL,
	.sibling = &current,
	.snmp_type = SNMP_INT,
	.cb = ibrSupplyVoltage
};

// OID .1.3.6.1.4.1.21369.1.3.3
const mib_node_t ibucDetector =
{
	.oid_frag = {3},
	.oid_len = 1,
	.parent = &ibucStatus,
	.child = &voltage,
	.sibling = NULL,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.3.2.24.0
const mib_node_t revhigh =
{
	.oid_frag = {24},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = NULL,
	.snmp_type = SNMP_INT,
	.cb = ibrReverseThresholdHigh
};

// OID .1.3.6.1.4.1.21369.1.3.2.23.0
const mib_node_t overdrive =
{
	.oid_frag = {23},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &revhigh,
	.snmp_type = SNMP_INT,
	.cb = ibrInputOverdriveFault
};

// OID .1.3.6.1.4.1.21369.1.3.2.22.0
const mib_node_t clonewrite =
{
	.oid_frag = {22},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &overdrive,
	.snmp_type = SNMP_INT,
	.cb = ibrCloneCommFault
};

// OID .1.3.6.1.4.1.21369.1.3.2.21.0
const mib_node_t cloneread =
{
	.oid_frag = {21},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &clonewrite,
	.snmp_type = SNMP_INT,
	.cb = ibrCloneCommFault
};

// OID .1.3.6.1.4.1.21369.1.3.2.20.0
const mib_node_t clonecomm =
{
	.oid_frag = {20},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &cloneread,
	.snmp_type = SNMP_INT,
	.cb = ibrCloneCommFault
};

// OID .1.3.6.1.4.1.21369.1.3.2.19.0
const mib_node_t wgsw =
{
	.oid_frag = {19},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &clonecomm,
	.snmp_type = SNMP_INT,
	.cb = ibrWGSwitchFault
};

// OID .1.3.6.1.4.1.21369.1.3.2.18.0
const mib_node_t fwrestart =
{
	.oid_frag = {18},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &wgsw,
	.snmp_type = SNMP_INT,
	.cb = ibrAlarmNone
};

// OID .1.3.6.1.4.1.21369.1.3.2.17.0
const mib_node_t serdnld =
{
	.oid_frag = {17},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &fwrestart,
	.snmp_type = SNMP_INT,
	.cb = ibrAlarmNone
};

// OID .1.3.6.1.4.1.21369.1.3.2.16.0
const mib_node_t ethdnld =
{
	.oid_frag = {16},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &serdnld,
	.snmp_type = SNMP_INT,
	.cb = ibrAlarmNone
};

// OID .1.3.6.1.4.1.21369.1.3.2.15.0
const mib_node_t cfgdat =
{
	.oid_frag = {15},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &ethdnld,
	.snmp_type = SNMP_INT,
	.cb = ibrAlarmNone
};

// OID .1.3.6.1.4.1.21369.1.3.2.14.0
const mib_node_t logalignerr =
{
	.oid_frag = {14},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &cfgdat,
	.snmp_type = SNMP_INT,
	.cb = ibrAlarmNone
};

// OID .1.3.6.1.4.1.21369.1.3.2.13.0
const mib_node_t currentoor =
{
	.oid_frag = {13},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &logalignerr,
	.snmp_type = SNMP_INT,
	.cb = ibrAlarmNone
};

// OID .1.3.6.1.4.1.21369.1.3.2.12.0
const mib_node_t supplyoor =
{
	.oid_frag = {12},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &currentoor,
	.snmp_type = SNMP_INT,
	.cb = ibrAlarmNone
};

// OID .1.3.6.1.4.1.21369.1.3.2.11.0
const mib_node_t txsim =
{
	.oid_frag = {11},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &supplyoor,
	.snmp_type = SNMP_INT,
	.cb = ibrTxSimulated
};

// OID .1.3.6.1.4.1.21369.1.3.2.10.0
const mib_node_t tempalm =
{
	.oid_frag = {10},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &txsim,
	.snmp_type = SNMP_INT,
	.cb = ibrTemperatureAlarm
};

// OID .1.3.6.1.4.1.21369.1.3.2.9.0
const mib_node_t agcrange =
{
	.oid_frag = {9},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &tempalm,
	.snmp_type = SNMP_INT,
	.cb = ibrAgcAlcRange
};

// OID .1.3.6.1.4.1.21369.1.3.2.8.0
const mib_node_t tenmhz =
{
	.oid_frag = {8},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &agcrange,
	.snmp_type = SNMP_INT,
	.cb = ibr10MHz
};

// OID .1.3.6.1.4.1.21369.1.3.2.7.0
const mib_node_t outhigh =
{
	.oid_frag = {7},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &tenmhz,
	.snmp_type = SNMP_INT,
	.cb = ibrOutputThresholdHigh
};

// OID .1.3.6.1.4.1.21369.1.3.2.6.0
const mib_node_t outlow =
{
	.oid_frag = {6},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &outhigh,
	.snmp_type = SNMP_INT,
	.cb = ibrOutputThresholdLow
};

// OID .1.3.6.1.4.1.21369.1.3.2.5.0
const mib_node_t inhigh =
{
	.oid_frag = {5},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &outlow,
	.snmp_type = SNMP_INT,
	.cb = ibrInputThresholdHigh
};

// OID .1.3.6.1.4.1.21369.1.3.2.4.0
const mib_node_t inlow =
{
	.oid_frag = {4},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &inhigh,
	.snmp_type = SNMP_INT,
	.cb = ibrInputThresholdLow
};

// OID .1.3.6.1.4.1.21369.1.3.2.3.0
const mib_node_t tgtoor =
{
	.oid_frag = {3},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &inlow,
	.snmp_type = SNMP_INT,
	.cb = ibrAgcAlcTargetOOR
};

// OID .1.3.6.1.4.1.21369.1.3.2.2.0
const mib_node_t notsettled =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &tgtoor,
	.snmp_type = SNMP_INT,
	.cb = ibrAlarmNone
};

// OID .1.3.6.1.4.1.21369.1.3.2.1.0
const mib_node_t drolock =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucAlarms,
	.child = NULL,
	.sibling = &notsettled,
	.snmp_type = SNMP_INT,
	.cb = ibrDROLock
};

// OID .1.3.6.1.4.1.21369.1.3.2
const mib_node_t ibucAlarms =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &ibucStatus,
	.child = &drolock,
	.sibling = &ibucDetector,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.3.1.7.0
const mib_node_t tottxhrs =
{
	.oid_frag = {7},
	.oid_len = 1,
	.parent = &ibucUnitInfo,
	.child = NULL,
	.sibling = NULL,
	.snmp_type = SNMP_INT,
	.cb = ibrTotalTransmitHours
};

// OID .1.3.6.1.4.1.21369.1.3.1.6.0
const mib_node_t totsyshrs =
{
	.oid_frag = {6},
	.oid_len = 1,
	.parent = &ibucUnitInfo,
	.child = NULL,
	.sibling = &tottxhrs,
	.snmp_type = SNMP_INT,
	.cb = ibrTotalSystemHours
};

// OID .1.3.6.1.4.1.21369.1.3.1.5.0
const mib_node_t freqband =
{
	.oid_frag = {5},
	.oid_len = 1,
	.parent = &ibucUnitInfo,
	.child = NULL,
	.sibling = &totsyshrs,
	.snmp_type = SNMP_INT,
	.cb = ibrFrequencyBand
};

// OID .1.3.6.1.4.1.21369.1.3.1.4.0
const mib_node_t pwrclass =
{
	.oid_frag = {4},
	.oid_len = 1,
	.parent = &ibucUnitInfo,
	.child = NULL,
	.sibling = &freqband,
	.snmp_type = SNMP_INT,
	.cb = ibrPowerClass
};

// OID .1.3.6.1.4.1.21369.1.3.1.3.0
const mib_node_t fwver =
{
	.oid_frag = {3},
	.oid_len = 1,
	.parent = &ibucUnitInfo,
	.child = NULL,
	.sibling = &pwrclass,
	.snmp_type = SNMP_OCT_STR,
	.cb = ibrFirmwareVersion
};

// OID .1.3.6.1.4.1.21369.1.3.1.2.0
const mib_node_t serial =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &ibucUnitInfo,
	.child = NULL,
	.sibling = &fwver,
	.snmp_type = SNMP_OCT_STR,
	.cb = ibrSerialNumber
};

// OID .1.3.6.1.4.1.21369.1.3.1.1.0
const mib_node_t model =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucUnitInfo,
	.child = NULL,
	.sibling = &serial,
	.snmp_type = SNMP_OCT_STR,
	.cb = sysDescr
};

// OID .1.3.6.1.4.1.21369.1.3.1
const mib_node_t ibucUnitInfo =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucStatus,
	.child = &model,
	.sibling = &ibucAlarms,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.3
const mib_node_t ibucStatus =
{
	.oid_frag = {3},
	.oid_len = 1,
	.parent = &ibucMIB,
	.child = &ibucUnitInfo,
	.sibling = &ibucMCv2,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.2.6.0
const mib_node_t fwreboot =
{
	.oid_frag = {6},
	.oid_len = 1,
	.parent = &ibucControl,
	.child = NULL,
	.sibling = NULL,
	.snmp_type = SNMP_INT,
	.cb = ibrFirmwareReboot
};

// OID .1.3.6.1.4.1.21369.1.2.5.0
const mib_node_t rstgainctrl =
{
	.oid_frag = {5},
	.oid_len = 1,
	.parent = &ibucControl,
	.child = NULL,
	.sibling = &fwreboot,
	.snmp_type = SNMP_INT,
	.cb = ibrResetGainControl
};

// OID .1.3.6.1.4.1.21369.1.2.4.0
const mib_node_t txoutput =
{
	.oid_frag = {4},
	.oid_len = 1,
	.parent = &ibucControl,
	.child = NULL,
	.sibling = &rstgainctrl,
	.snmp_type = SNMP_INT,
	.cb = ibrMute
};

// OID .1.3.6.1.4.1.21369.1.2.3.0
const mib_node_t clralmlog =
{
	.oid_frag = {3},
	.oid_len = 1,
	.parent = &ibucControl,
	.child = NULL,
	.sibling = &txoutput,
	.snmp_type = SNMP_INT,
	.cb = ibrClearAlarmLog
};

// OID .1.3.6.1.4.1.21369.1.2.2.0
const mib_node_t simalm =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &ibucControl,
	.child = NULL,
	.sibling = &clralmlog,
	.snmp_type = SNMP_INT,
	.cb = ibrSimulateAlarm
};

// OID .1.3.6.1.4.1.21369.1.2.1.0
const mib_node_t suppalms =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucControl,
	.child = NULL,
	.sibling = &simalm,
	.snmp_type = SNMP_INT,
	.cb = ibrSuppressAlarms
};

// OID .1.3.6.1.4.1.21369.1.2
const mib_node_t ibucControl =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &ibucMIB,
	.child = &suppalms,
	.sibling = &ibucStatus,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.1.6.8.0
const mib_node_t warmstdby =
{
	.oid_frag = {8},
	.oid_len = 1,
	.parent = &ibucRedund,
	.child = NULL,
	.sibling = NULL,
	.snmp_type = SNMP_INT,
	.cb = ibrWarmStandby
};

// OID .1.3.6.1.4.1.21369.1.1.6.7.0
const mib_node_t sysstatus =
{
	.oid_frag = {7},
	.oid_len = 1,
	.parent = &ibucRedund,
	.child = NULL,
	.sibling = &warmstdby,
	.snmp_type = SNMP_OCT_STR,
	.cb = ibrSystemStatus
};

// OID .1.3.6.1.4.1.21369.1.1.6.6.0
const mib_node_t swpos =
{
	.oid_frag = {6},
	.oid_len = 1,
	.parent = &ibucRedund,
	.child = NULL,
	.sibling = &sysstatus,
	.snmp_type = SNMP_INT,
	.cb = ibrSwitchPosition
};

// OID .1.3.6.1.4.1.21369.1.1.6.5.0
const mib_node_t swtype =
{
	.oid_frag = {5},
	.oid_len = 1,
	.parent = &ibucRedund,
	.child = NULL,
	.sibling = &swpos,
	.snmp_type = SNMP_INT,
	.cb = ibrSwitchingType
};

// OID .1.3.6.1.4.1.21369.1.1.6.4.0
const mib_node_t cloning =
{
	.oid_frag = {4},
	.oid_len = 1,
	.parent = &ibucRedund,
	.child = NULL,
	.sibling = &swtype,
	.snmp_type = SNMP_INT,
	.cb = ibrAutoCloning
};

// OID .1.3.6.1.4.1.21369.1.1.6.3.0
const mib_node_t posonsw =
{
	.oid_frag = {3},
	.oid_len = 1,
	.parent = &ibucRedund,
	.child = NULL,
	.sibling = &cloning,
	.snmp_type = SNMP_INT,
	.cb = ibrPositionOnSwitch
};

// OID .1.3.6.1.4.1.21369.1.1.6.2.0
const mib_node_t redmode =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &ibucRedund,
	.child = NULL,
	.sibling = &posonsw,
	.snmp_type = SNMP_INT,
	.cb = ibrRedundancyMode
};

// OID .1.3.6.1.4.1.21369.1.1.6.1.0
const mib_node_t reden =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucRedund,
	.child = NULL,
	.sibling = &redmode,
	.snmp_type = SNMP_INT,
	.cb = ibrRedundancyEnabled
};

// OID .1.3.6.1.4.1.21369.1.1.6
const mib_node_t ibucRedund =
{
	.oid_frag = {6},
	.oid_len = 1,
	.parent = &ibucCfg,
	.child = &reden,
	.sibling = NULL,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.1.5.20.0
const mib_node_t revthhigh =
{
	.oid_frag = {20},
	.oid_len = 1,
	.parent = &ibucTx,
	.child = NULL,
	.sibling = NULL,
	.snmp_type = SNMP_INT,
	.cb = ibrReverseThresholdHi
};


// OID .1.3.6.1.4.1.21369.1.1.5.19.0
const mib_node_t coupling_factor =
{
	.oid_frag = {19},
	.oid_len = 1,
	.parent = &ibucTx,
	.child = NULL,
	.sibling = &revthhigh,
	.snmp_type = SNMP_INT,
	.cb = ibrCouplingFactor
};

// OID .1.3.6.1.4.1.21369.1.1.5.18.0
const mib_node_t bsttimeout =
{
	.oid_frag = {18},
	.oid_len = 1,
	.parent = &ibucTx,
	.child = NULL,
	.sibling = &coupling_factor,
	.snmp_type = SNMP_INT,
	.cb = ibrBurstTimeout
};

// OID .1.3.6.1.4.1.21369.1.1.5.13.0
const mib_node_t hitempshut =
{
	.oid_frag = {13},
	.oid_len = 1,
	.parent = &ibucTx,
	.child = NULL,
	.sibling = &bsttimeout,
	.snmp_type = SNMP_INT,
	.cb = ibrHighTemperatureShutdown
};

// OID .1.3.6.1.4.1.21369.1.1.5.12.0
const mib_node_t outthhigh =
{
	.oid_frag = {12},
	.oid_len = 1,
	.parent = &ibucTx,
	.child = NULL,
	.sibling = &hitempshut,
	.snmp_type = SNMP_INT,
	.cb = ibrOuputThresholdHi
};

// OID .1.3.6.1.4.1.21369.1.1.5.11.0
const mib_node_t outthlow =
{
	.oid_frag = {11},
	.oid_len = 1,
	.parent = &ibucTx,
	.child = NULL,
	.sibling = &outthhigh,
	.snmp_type = SNMP_INT,
	.cb = ibrOuputThresholdLo
};

// OID .1.3.6.1.4.1.21369.1.1.5.10.0
const mib_node_t inthhigh =
{
	.oid_frag = {10},
	.oid_len = 1,
	.parent = &ibucTx,
	.child = NULL,
	.sibling = &outthlow,
	.snmp_type = SNMP_INT,
	.cb = ibrInputThresholdHi
};

// OID .1.3.6.1.4.1.21369.1.1.5.9.0
const mib_node_t inthlow =
{
	.oid_frag = {9},
	.oid_len = 1,
	.parent = &ibucTx,
	.child = NULL,
	.sibling = &inthhigh,
	.snmp_type = SNMP_INT,
	.cb = ibrInputThresholdLo
};

// OID .1.3.6.1.4.1.21369.1.1.5.8.0
const mib_node_t atten =
{
	.oid_frag = {8},
	.oid_len = 1,
	.parent = &ibucTx,
	.child = NULL,
	.sibling = &inthlow,
	.snmp_type = SNMP_INT,
	.cb = ibrAttenuation
};

// OID .1.3.6.1.4.1.21369.1.1.5.7.0
const mib_node_t gainmode =
{
	.oid_frag = {7},
	.oid_len = 1,
	.parent = &ibucTx,
	.child = NULL,
	.sibling = &atten,
	.snmp_type = SNMP_INT,
	.cb = ibrGainMode
};

// OID .1.3.6.1.4.1.21369.1.1.5.5.0
const mib_node_t bstthold =
{
	.oid_frag = {5},
	.oid_len = 1,
	.parent = &ibucTx,
	.child = NULL,
	.sibling = &gainmode,
	.snmp_type = SNMP_INT,
	.cb = ibrBurstThreshold
};

// OID .1.3.6.1.4.1.21369.1.1.5.4.0
const mib_node_t pwrmode =
{
	.oid_frag = {4},
	.oid_len = 1,
	.parent = &ibucTx,
	.child = NULL,
	.sibling = &bstthold,
	.snmp_type = SNMP_INT,
	.cb = ibrReadPowerMode
};

// OID .1.3.6.1.4.1.21369.1.1.5.3.0
const mib_node_t pupdelay =
{
	.oid_frag = {3},
	.oid_len = 1,
	.parent = &ibucTx,
	.child = NULL,
	.sibling = &pwrmode,
	.snmp_type = SNMP_INT,
	.cb = ibrPowerupDelay
};

// OID .1.3.6.1.4.1.21369.1.1.5.2.0
const mib_node_t pupstate =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &ibucTx,
	.child = NULL,
	.sibling = &pupdelay,
	.snmp_type = SNMP_INT,
	.cb = ibrPowerupState
};

// OID .1.3.6.1.4.1.21369.1.1.5.1.0
const mib_node_t frequency =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucTx,
	.child = NULL,
	.sibling = &pupstate,
	.snmp_type = SNMP_INT,
	.cb = ibrFrequency
};

// OID .1.3.6.1.4.1.21369.1.1.5
const mib_node_t ibucTx =
{
	.oid_frag = {5},
	.oid_len = 1,
	.parent = &ibucCfg,
	.child = &frequency,
	.sibling = &ibucRedund,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.1.4.5.0
const mib_node_t corr50hz =
{
	.oid_frag = {5},
	.oid_len = 1,
	.parent = &ibucSystem,
	.child = NULL,
	.sibling = NULL,
	.snmp_type = SNMP_INT,
	.cb = ibrAC50HzCorrection
};

// OID .1.3.6.1.4.1.21369.1.1.4.4.0
const mib_node_t websrvr =
{
	.oid_frag = {4},
	.oid_len = 1,
	.parent = &ibucSystem,
	.child = NULL,
	.sibling = &corr50hz,
	.snmp_type = SNMP_INT,
	.cb = ibrWebServerRefresh
};

// OID .1.3.6.1.4.1.21369.1.1.4.3.0
const mib_node_t pwtimeout =
{
	.oid_frag = {3},
	.oid_len = 1,
	.parent = &ibucSystem,
	.child = NULL,
	.sibling = &websrvr,
	.snmp_type = SNMP_INT,
	.cb = ibrPasswordTimeOut
};

// OID .1.3.6.1.4.1.21369.1.1.4.2.0
const mib_node_t echo =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &ibucSystem,
	.child = NULL,
	.sibling = &pwtimeout,
	.snmp_type = SNMP_INT,
	.cb = ibrEcho
};

// OID .1.3.6.1.4.1.21369.1.1.4.1.0
const mib_node_t verbmode =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucSystem,
	.child = NULL,
	.sibling = &echo,
	.snmp_type = SNMP_INT,
	.cb = ibrVerboseMode
};

// OID .1.3.6.1.4.1.21369.1.1.4
const mib_node_t ibucSystem =
{
	.oid_frag = {4},
	.oid_len = 1,
	.parent = &ibucCfg,
	.child = &verbmode,
	.sibling = &ibucTx,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.1.3.12.0
const mib_node_t revhisup =
{
	.oid_frag = {12},
	.oid_len = 1,
	.parent = &ibucAlarm,
	.child = NULL,
	.sibling = NULL,
	.snmp_type = SNMP_INT,
	.cb = ibrReverseHighThresholdSuppressible
};

// OID .1.3.6.1.4.1.21369.1.1.3.11.0
const mib_node_t revhilev =
{
	.oid_frag = {11},
	.oid_len = 1,
	.parent = &ibucAlarm,
	.child = NULL,
	.sibling = &revhisup,
	.snmp_type = SNMP_INT,
	.cb = ibrReverseHighThresholdAlarmLevel
};

// OID .1.3.6.1.4.1.21369.1.1.3.10.0
const mib_node_t outhilev =
{
	.oid_frag = {10},
	.oid_len = 1,
	.parent = &ibucAlarm,
	.child = NULL,
	.sibling = &revhilev,
	.snmp_type = SNMP_INT,
	.cb = ibrOutputHighThresholdAlarmLevel
};

// OID .1.3.6.1.4.1.21369.1.1.3.9.0
const mib_node_t outlolev =
{
	.oid_frag = {9},
	.oid_len = 1,
	.parent = &ibucAlarm,
	.child = NULL,
	.sibling = &outhilev,
	.snmp_type = SNMP_INT,
	.cb = ibrOutputLowThresholdAlarmLevel
};

// OID .1.3.6.1.4.1.21369.1.1.3.8.0
const mib_node_t inhilev =
{
	.oid_frag = {8},
	.oid_len = 1,
	.parent = &ibucAlarm,
	.child = NULL,
	.sibling = &outlolev,
	.snmp_type = SNMP_INT,
	.cb = ibrInputHighThresholdAlarmLevel
};

// OID .1.3.6.1.4.1.21369.1.1.3.7.0
const mib_node_t inlolev =
{
	.oid_frag = {7},
	.oid_len = 1,
	.parent = &ibucAlarm,
	.child = NULL,
	.sibling = &inhilev,
	.snmp_type = SNMP_INT,
	.cb = ibrInputLowThresholdAlarmLevel
};

// OID .1.3.6.1.4.1.21369.1.1.3.6.0
const mib_node_t templev =
{
	.oid_frag = {6},
	.oid_len = 1,
	.parent = &ibucAlarm,
	.child = NULL,
	.sibling = &inlolev,
	.snmp_type = SNMP_INT,
	.cb = ibrTemperatureAlarmLevel
};

// OID .1.3.6.1.4.1.21369.1.1.3.5.0
const mib_node_t outhisup =
{
	.oid_frag = {5},
	.oid_len = 1,
	.parent = &ibucAlarm,
	.child = NULL,
	.sibling = &templev,
	.snmp_type = SNMP_INT,
	.cb = ibrOuputHighThresholdSuppressible
};

// OID .1.3.6.1.4.1.21369.1.1.3.4.0
const mib_node_t outlosup =
{
	.oid_frag = {4},
	.oid_len = 1,
	.parent = &ibucAlarm,
	.child = NULL,
	.sibling = &outhisup,
	.snmp_type = SNMP_INT,
	.cb = ibrOuputLowThresholdSuppressible
};

// OID .1.3.6.1.4.1.21369.1.1.3.3.0
const mib_node_t inhisup =
{
	.oid_frag = {3},
	.oid_len = 1,
	.parent = &ibucAlarm,
	.child = NULL,
	.sibling = &outlosup,
	.snmp_type = SNMP_INT,
	.cb = ibrInputHighThresholdSuppressible
};

// OID .1.3.6.1.4.1.21369.1.1.3.2.0
const mib_node_t inlosup =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &ibucAlarm,
	.child = NULL,
	.sibling = &inhisup,
	.snmp_type = SNMP_INT,
	.cb = ibrInputLowThresholdSuppressible
};

// OID .1.3.6.1.4.1.21369.1.1.3.1.0
const mib_node_t tempsup =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucAlarm,
	.child = NULL,
	.sibling = &inlosup,
	.snmp_type = SNMP_INT,
	.cb = ibrTemperatureSuppressible
};

// OID .1.3.6.1.4.1.21369.1.1.3
const mib_node_t ibucAlarm =
{
	.oid_frag = {3},
	.oid_len = 1,
	.parent = &ibucCfg,
	.child = &tempsup,
	.sibling = &ibucSystem,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.1.2.4.0
const mib_node_t delay485 =
{
	.oid_frag = {4},
	.oid_len = 1,
	.parent = &ibucRS485,
	.child = NULL,
	.sibling = NULL,
	.snmp_type = SNMP_INT,
	.cb = ibrRS485Delay
};

// OID .1.3.6.1.4.1.21369.1.1.2.3.0
const mib_node_t mode485 =
{
	.oid_frag = {3},
	.oid_len = 1,
	.parent = &ibucRS485,
	.child = NULL,
	.sibling = &delay485,
	.snmp_type = SNMP_INT,
	.cb = ibrRS485Mode
};

// OID .1.3.6.1.4.1.21369.1.1.2.2.0
const mib_node_t baud485 =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &ibucRS485,
	.child = NULL,
	.sibling = &mode485,
	.snmp_type = SNMP_INT,
	.cb = ibrRS485Baudrate
};

// OID .1.3.6.1.4.1.21369.1.1.2.1.0
const mib_node_t addr485 =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucRS485,
	.child = NULL,
	.sibling = &baud485,
	.snmp_type = SNMP_INT,
	.cb = ibrRS485Address
};

// OID .1.3.6.1.4.1.21369.1.1.2
const mib_node_t ibucRS485 =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &ibucCfg,
	.child = &addr485,
	.sibling = &ibucAlarm,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.1.1.5.0
const mib_node_t telnetport =
{
	.oid_frag = {5},
	.oid_len = 1,
	.parent = &ibucIP,
	.child = NULL,
	.sibling = NULL,
	.snmp_type = SNMP_INT,
	.cb = ibrTelnetPort
};

// OID .1.3.6.1.4.1.21369.1.1.1.4.0
const mib_node_t addrother =
{
	.oid_frag = {4},
	.oid_len = 1,
	.parent = &ibucIP,
	.child = NULL,
	.sibling = &telnetport,
	.snmp_type = SNMP_IP_ADDR,
	.cb = ibrIPAddressOther
};

// OID .1.3.6.1.4.1.21369.1.1.1.3.0
const mib_node_t ipsubnet =
{
	.oid_frag = {3},
	.oid_len = 1,
	.parent = &ibucIP,
	.child = NULL,
	.sibling = &addrother,
	.snmp_type = SNMP_INT,
	.cb = ibrIPSubnet
};

// OID .1.3.6.1.4.1.21369.1.1.1.2.0
const mib_node_t ipgw =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &ibucIP,
	.child = NULL,
	.sibling = &ipsubnet,
	.snmp_type = SNMP_IP_ADDR,
	.cb = ibrIPGateway
};

// OID .1.3.6.1.4.1.21369.1.1.1.1.0
const mib_node_t ipaddr =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucIP,
	.child = NULL,
	.sibling = &ipgw,
	.snmp_type = SNMP_IP_ADDR,
	.cb = ibrIPAddress
};

// OID .1.3.6.1.4.1.21369.1.1.1
const mib_node_t ibucIP =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucCfg,
	.child = &ipaddr,
	.sibling = &ibucRS485,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1.1
const mib_node_t ibucCfg =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &ibucMIB,
	.child = &ibucIP,
	.sibling = &ibucControl,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.4.1.21369.1
const mib_node_t ibucMIB =
{
	.oid_frag = {4,1,21369,1},
	.oid_len = 4,
	.parent = &internet,
	.child = &ibucCfg,
	.sibling = NULL,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1.2.1.1.7.0
const mib_node_t sysservices =
{
	.oid_frag = {7},
	.oid_len = 1,
	.parent = &sys,
	.child = NULL,
	.sibling = NULL,
	.snmp_type = SNMP_INT,
	.cb = sysServices
};

// OID .1.3.6.1.2.1.1.6.0
const mib_node_t syslocation =
{
	.oid_frag = {6},
	.oid_len = 1,
	.parent = &sys,
	.child = NULL,
	.sibling = &sysservices,
	.snmp_type = SNMP_OCT_STR,
	.cb = sysLocation
};

// OID .1.3.6.1.2.1.1.5.0
const mib_node_t sysname =
{
	.oid_frag = {5},
	.oid_len = 1,
	.parent = &sys,
	.child = NULL,
	.sibling = &syslocation,
	.snmp_type = SNMP_OCT_STR,
	.cb = sysName
};

// OID .1.3.6.1.2.1.1.4.0
const mib_node_t syscontact =
{
	.oid_frag = {4},
	.oid_len = 1,
	.parent = &sys,
	.child = NULL,
	.sibling = &sysname,
	.snmp_type = SNMP_OCT_STR,
	.cb = sysContact
};

// OID .1.3.6.1.2.1.1.3.0
const mib_node_t sysuptime =
{
	.oid_frag = {3},
	.oid_len = 1,
	.parent = &sys,
	.child = NULL,
	.sibling = &syscontact,
	.snmp_type = SNMP_TIME_TICK,
	.cb = sysUpTime
};

// OID .1.3.6.1.2.1.1.2.0
const mib_node_t sysobjid =
{
	.oid_frag = {2},
	.oid_len = 1,
	.parent = &sys,
	.child = NULL,
	.sibling = &sysuptime,
	.snmp_type = SNMP_OID_TYPE,
	.cb = sysObjectID
};

// OID .1.3.6.1.2.1.1.1.0
const mib_node_t sysdescr =
{
	.oid_frag = {1},
	.oid_len = 1,
	.parent = &sys,
	.child = NULL,
	.sibling = &sysobjid,
	.snmp_type = SNMP_OCT_STR,
	.cb = sysDescr
};

// OID .1.3.6.1.2.1.1
const mib_node_t sys =
{
	.oid_frag = {2,1,1},
	.oid_len = 3,
	.parent = &internet,
	.child = &sysdescr,
	.sibling = &ibucMIB,
	.snmp_type = 0,
	.cb = NULL
};

// OID .1.3.6.1
const mib_node_t internet =
{
	.oid_frag = {1,3,6,1},
	.oid_len = 4,
	.parent = NULL,
	.child = &sys,
	.sibling = NULL,
	.snmp_type = 0,
	.cb = NULL
};
