#!/usr/bin/env python3
import asyncio
from pysnmp.hlapi.v3arch.asyncio import SnmpEngine
from pysnmp.entity import config
from pysnmp.entity.rfc3413.ntfrcv import NotificationReceiver
from pysnmp.carrier.asyncio.dgram import udp

async def main():
    snmp_engine = SnmpEngine()

    config.add_v1_system(snmp_engine, 'my-area', 'public')
    config.add_transport(
        snmp_engine, udp.DOMAIN_NAME,
        udp.UdpTransport().open_server_mode(('0.0.0.0', 10162))
    )

    # NOTE: sync callback (not async def)
    def cb(snmp_engine, state_ref, ctx_engine_id, ctx_name, var_binds, cb_ctx):
        print("=== Trap/Inform received ===")
        for oid, val in var_binds:
            print(f"{oid.prettyPrint()} = {val.prettyPrint()}")
        print()

    NotificationReceiver(snmp_engine, cb)

    print("Listening for traps on UDP/10162…")
    await asyncio.Event().wait()

if __name__ == "__main__":
    asyncio.run(main())
