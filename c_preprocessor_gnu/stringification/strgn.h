/*
 * strgn.h
 *
 *  Created on: Jun 8, 2012
 *      Author: leonyuwin7
 */

#ifndef STRGN_H_
#define STRGN_H_

#define XSTRING(x)      #x
#define STRING(x)       XSTRING(x)

#define CISCO_SOME_VERSION "0.0.0.4"
#define CISCO_UBVER          STRING(v1.1.10)
#define CISCO_UBDATE         STRING(__DATE__)

#define EXTRA_ENV_SETTINGS "tty=ttyS0\n"

#define	CONFIG_EXTRA_ENV_SETTINGS EXTRA_ENV_SETTINGS	\
   "cisco_ubver="CISCO_UBVER"\n"							\
   "cisco_ubdate="CISCO_UBDATE"\0"                       \
   "netdev=eth0\0"										\
   "consoledev=ttyS0\0"									\
   "br=115200\0"										\
   "default=0\0"

#endif /* STRGN_H_ */
