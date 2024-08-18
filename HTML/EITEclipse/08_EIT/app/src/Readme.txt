
/*------------------------------------------------------------------------ 
		Copyright (c) CMX Systems, Inc. 2007. All rights reserved
        ---------------------------------------------------------
					main.c

Description:	Perform the following action
                1. main()
                2. Initialize the Iserver
                3. Open required Sockets
                4. Initialize the main Server loop
                 UART to ethernet and ethernet to UART convertor for the LPC2378.
                 Runs an HTTP server at the same time. No RTOS is used.

                 The app waits for a Telnet connection to start things off.
                 The LPC2378 waits for characters to come in
                 through both the UART and ethernet. Whatever comes in 
                 the the UART is sent to the ethernet Telnet port and 
                 whatever comes in through the
                 ethernet Telnet port is sent out of the UART.
                 Defaults: IP:        192.168.1.200
                           Baud:      9600 8 N 1
                           LocalPort: 2000

Author:         CMX Systems and Ajay Sharma
                Added FLASH driver initialisation for use by SW uploading 
				See the conditional define _POST_FILES  4.9.2008 - M.J.Butcher

Rev:            0

History:        Ver 0:
                ->  NO idea

                Ver 1: 
                -> No Idea

                Ver 2:
                -> Making this project closer to the present IServer.

                Ver 3:
                -> SMTP SUpport added.
                -> Removed the bug of Java Terminal Not Loading.

                2008_03_26:
                -> Added support for Dip switches and restore Defaults
                   for Dip Switch 2
                2008_03_27
                -> Added support for reading, writing RS232/485/GPIO
                   and Forced or Conditional GPIO Settings.
                2008_03_31
                -> Added support for Endcharacter from Serial to Ethernet
                2008_04_01
                -> Added support for end character & fwd enchar from 
                   ethernet to serial, Packet Length Support
                2008_04_02
                -> Added support for Device Query and Response
                2008_04_03 
                -> Added support for setting up device table, getting the
                   table readings.
                2008_04_04 
                -> Added support for Software Flow Control
                   Data ~ 16kb can be sent at 115200 baud.
                   Added support for Hardware Flow Control but the unit 
                   freezes.
                2008_04_06
                -> Added support for UART Configuration at all baud rates
                   and other configurations and to display the read device
                   in a green font heading.
				2008_04_10
				-> Cleaned up webpages, All webpages now have help files 
				   that are as per the statement of work.
				   Increased the RAM to 40KB(added 8KB of USB RAM) See 
				   the project configuration.
				-> Also increased the heap size to 2KB as "atoi" command
				   needs that to work.
				-> Improved the overview.htm webpage.
				-> Added diagnostics.htm file info like TX RX for UART,
				   Internet.
				2008_04_14
				-> Got the RS485 Timer working. Option created to spcify 
				   the timeout value on iopins.htm.
			    -> Added support for Remote Tunneling.
				-> Added support for UDP Bridging. only on the local port
				-> Started to work on Serial Port Configuration.
                2008_04_15
				-> Got the IP Exclusivity Working atleast with One 
				   Connection.
				-> Started to add modbus TCP to the code dont know what
				   is not working...........
			    2008_04_16
				-> Able to set inputs and outputs of forced gpio.
				2008_04_17
				-> forced gpio implemented.
				-> reset counter for the page of diagnostics implemented.
				2008_04_22
				-> implemented serial to ethernet timeout.
				-> implemented inactivity timeout.
                2008_04_23
				-> implemented the serial configuration option at startup
				2008_04_24
				-> implemented show all info at the serial console
				2008_04_25
				-> implemented help and settings on serial port.
				2008_04_28
				-> Implemented part of port2002 configuration.
				2008_04_29
				-> Continued working on the port 2002 configuration.
				2008_05_01 
				-> Finished working on Port2002 Configuration.
				2008_05_05
				-> Working on storing all the values ..
				2008_06_12
				-> Started working on the firmware again.
				was stuck on the bootloader part.
				-> Was able to load the values to the serial.htm page
				2008_06_20
				-> Successfully able to save the multiple remote port and 
				ip to the flash and display and also added the horizontal
				tabs.
				2008_06_24
				-> All the settings on all the webpages can be saved now
				2008_06_26
				-> Divided the webpages into 4 different versions
				Also some part of the code is divided into 4 parts..
				2008_06_30
				-> Added support for UDP based bridging and removed the bug 
				of Ethernet to Serial End char
				2008_07_01
				-> Added support for Multihost Connection. Atleast One connection 
				   can be made. Also a counter is applied to the no of tries that are
				   done while making a connection.
				-> Remote UDP Serial to Ethernet works fine now.
				-> Able to save the conditional GPIO Setings on the webpage.
				2008_07_03
				-> Implemented and tested the Alarms 1-5 of Power Reset,
				   IP Address Changed and iServer Accessed. Email only.
				2008_07_07
				-> Fixed the device Setup page that would not open up in the 
				   internet explorer.
				-> Also increased the timeout for the device quesry and 
				   read_device page to remove the blacking out of the contents
				2008_07_08
				-> the boards arrived today. Finally added firmware for RS232 Full 
				   and RS232 GPIO
                2008_07_16
                -> the serial console was a problem. I fixed the console and evrything 
                on it works fine.
                2008_07_30
                -> Fixed the bug of the packets being spilt into two while responding to the 
                   request from ethernet. Willy Reported this bug.
                2008_08_01
                -> Restore Defaults are converted to Interrupt based rather than polling. 
                   Initially the customer needed to press the Reset Switch while powering up
                   the Unit and then the Unit would restore the defaults. But Now the 
                   if anytime during the operation the customer presses the Reset Switch
                   the Defaults are restored.
                2008_08_04
                -> Fixed a bug in the Internet Console Login. The password would not be accepted
                and was asking again and again for the password.
                2008_08_05
                -> Got the conditional GPIO in place .. Refer to the mn_port.c I am using the 
                timer 0 for ticks. Check every Inputs and drive outputs every 10ms. 
                2008_08_06
                -> Endchar, Buffering Timer remodified as i dont need any type of packaging technique 
                   while sending data from etherent to serial.

                2008_08_07
                -> Port 1000 was added to this source code. Basically It does the same Serial and 
                   Ethernet bridging but closes connection after sending a chunk of data from the 
                   serial side.



               2008_08_08
               -> Fixed a BUG: Unit would not respond to more than 227 readings if any of the 
                  packaging technique is enabled. But that is now fixed.
               -> Fixed a BUG: Serial port password if not entered correctly would freeze the 
                  unit. Removed it.
               2008_08_11
               -> MAC Address was getting reset to default if the unit was reset to defaults,
                  Removed that bug. Also removed the interrupt service routine to process 
                  a request to restore defaults. Moved to polling method in mn_port.c where
                  every 10 ms we poll to see if the reset pin is pressed and if it is then we
                  restore defaults not in the ISR but in mn_application_idle as we 
                  have to disable interrupts while writing to flash.
                2008_08_12
                -> Added support for discovery utlity.
                
                2008_08_18
                -> Minor improvement over the Serial to Ethernet bridge.
                Unit was delaying FIN packet if buffering time was set. Removed that bug
                -> MAC Address was set to 00:00:00:00:00:00 if the unit was 
                reset while booting up.
                
                2008_08_26
                -> SNMP Traps are being sent. SNMP Variables can be set from the browser 
                and the snmp_set.
                
                2008_08_27
                -> A Bug was introduced when we switched from the Single model design to
                   4 model design in the Device Query Page. redesigned the 
                   post_device_query.c file and the bridge in there looks like the 
                   tcp_local_bridge.c file
                2008_08_28
                -> removed the bug where improper settings were getting stored to the flash
                in the device setup page. flash.c is modified. 
                2008_09_11
                -> Implemented Ping from the device to another IP.
                   NOTE : iServer cannot ping itself. 
                   
                   
                2008_09_16
                -> Implemented the Serial Port programming on the RS232 half unit
                   It works fine. But needs to reverse the RX/TX lines. This is not acceptable
                   We have to think of other alternative or drop this feature.
                   
                -> Also the firmware upgrade over the ethernet is working fine. We added 
                   a routine(by Mark Butcher) to erase the flash sector if the flash content
                   does not look good at the download area so that the next download will 
                   make it work.                      
                
                2008_09_22
                -> Due to some reason in the recent past the firmwares got changed and so 
                   the read device page was not taking the readings properly. Fixed it.

---------------------------------------------------------------------------------------------------
 ver 1.1b
---------------------------------------------------------------------------------------------------
                2008_10_06
				-> Added a small note that when U change the network settings, the page prompts 
				   for a reboot. 
				-> Fixed a bug associated with the Discover utility. After discovering the device 
				   the device hangs.

				2008_10_07
				-> Fixed a bug in RS485 two wire. The response was coming with the request.
				-> RS485 2 Wire works if telnet to port 2000.

				2008_10_14
				-> Fixed the bug in hardware flow control where the flow control would not work 
				   properly

---------------------------------------------------------------------------------------------------
 ver 1.1c
---------------------------------------------------------------------------------------------------
		        2008_10_16
				-> Fixed bugs reported by Mak manesh. Completely changed the webpages
				2008_11_07
				-> Fixed the Modbus timer. Now Modbus works fine.

---------------------------------------------------------------------------------------------------
ver 1.1f
---------------------------------------------------------------------------------------------------
		        2008_11_18
				-> Fixed bugs related to remote tunneling. 
				-> Fixed UDP remote bridge.
				-> Fixed Discover Utility again.
				-> Fixed and tested all the alarms.
				-> Diagnostics page was not showing the correct UDP recv and TCP send and ICMP pkt
				   information.
				-> EIT-W-2F is tested and works ok. Except the SNMP MIBS support.
				-> Also this is multiple firmwares still. 