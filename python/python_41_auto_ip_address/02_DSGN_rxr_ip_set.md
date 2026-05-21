

# RXR IP Set
rxr_ip_set.py is a Python script for configuring the IP settings of the RXR modem. It uses the pyserial library to communicate with the device over a serial connection. The script can read the current IP configuration and set new values for the IP address, subnet mask, and gateway. The RXR has a different command set for IP configuration, and it doesn't require login. The script `rxr_ip_set.py` handles this specific protocol.


Decoded from the RXR log — quite different from the iBUC:



| Difference       | iBUC     | RXR                         |
|------------------|----------|-----------------------------|
| Login required   | Yes      | No                          |
| Prompt           | IBUC>    | RX1+1>                      |
| Read syntax      | cia=C    | cia                         |
| Set syntax       | n/a      | cia=10.10.12.41             |
| Set response     | n/a      | echoes CIA=<value> back     |


## Read current config
```
python3 rxr_ip_set.py --port /dev/ttyUSB0
```

## Set all three
```
python3 rxr_ip_set.py --port /dev/ttyUSB0 --ip 10.10.12.50 --mask 255.255.0.0 --gw 10.10.10.1
```

## Set only IP
```
python3 rxr_ip_set.py --port /dev/ttyUSB0 --ip 10.10.12.50
```

## Different baud
```
python3 rxr_ip_set.py --port /dev/ttyUSB1 --baud 9600 --ip 10.10.12.50
```


```
$ python ./rxr_ip_set.py --port /dev/ttyUSB1 --baud 9600  --ip 10.10.12.41
[*] Opening /dev/ttyUSB1 at 9600 baud...
[*] Waiting for prompt...
[+] Device ready.

Setting parameters:
----------------------------------------
  IP Address    : 10.10.12.41  [OK]
----------------------------------------

Reading back all values after change:

RXR Network Configuration
----------------------------------------
  IP Address    : 10.10.12.41
  Subnet Mask   : 255.255.0.0
  Gateway       : 10.10.10.1
----------------------------------------

[*] Port closed.
```