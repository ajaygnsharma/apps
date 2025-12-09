#!/usr/bin/env python3
import asyncio
from pysnmp.hlapi.v3arch.asyncio import (
    SnmpEngine, CommunityData, UdpTransportTarget, ContextData,
    NotificationType, ObjectIdentity, ObjectIdentifier,
    TimeTicks, OctetString, Integer, send_notification
)

async def main():
    snmp_engine = SnmpEngine()

    # v2c target (note: .create() factory in 7.x)
    target = await UdpTransportTarget.create(('127.0.0.1', 10162))

    # Enterprise OID for the notification
    notif = NotificationType(ObjectIdentity('1.3.6.1.4.1.32473.1.0'))

    # Add varbinds using numeric OIDs (no MIB lookup required)
    notif = notif.add_varbinds(
        ('1.3.6.1.2.1.1.3.0', TimeTicks(12345)),                             # sysUpTime.0
        ('1.3.6.1.6.3.1.1.4.1.0', ObjectIdentifier('1.3.6.1.4.1.32473.1.0')),# snmpTrapOID.0
        ('1.3.6.1.4.1.32473.1.1.1.0', OctetString('hello')),
        ('1.3.6.1.4.1.32473.1.1.2.0', Integer(42)),
    )

    errInd, errStat, errIdx, varBinds = await send_notification(
        snmp_engine,
        CommunityData('public', mpModel=1),  # v2c
        target,
        ContextData(),
        'inform',                            # or 'trap'
        notif
    )

    if errInd:
        print("Send error:", errInd)
    elif errStat:
        print("Agent error:", errStat.prettyPrint())
    else:
        print("Inform sent OK")

if __name__ == "__main__":
    asyncio.run(main())
