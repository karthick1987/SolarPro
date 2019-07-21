/*
   Wireless Sensor Networks Laboratory 2019 -- Group 1

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
http://www.lkn.ei.tum.de

copyright (c) 2019 Chair of Communication Networks, TUM

project: SolarPro

contributors:
 * Karthik Sukumar
 * Johannes Machleid

 This header file is specifically designed for the base node.
 */

#ifndef BASE_H
#define BASE_H

#include "contiki.h"
#include "core/net/linkaddr.h"
#include "cpu/cc2538/dev/uart.h"
//#include "cpu/cc2538/usb/usb-serial.h"	// For UART-like I/O over USB.
#include "dev/serial-line.h"			// For UART-like I/O over USB.

/*---------------------------------------------------------------------------*/
// Definition of UART special characters.
// 	Note:	Contiki seems to have its own definitions.
// 			Incompatibility may cause unexpected behavior.
// 	For more details, check usb-serial.h and serial-line.h.
#define START_CHAR                  1
#define DEACTIVATION_CHAR           0x0d //25xxx4
#define END_CHAR                    0x0a //255

// Packet type flags used in UART/serial transmission.
#define SERIAL_PACKET_TYPE_ANEMOMETER         	0
#define SERIAL_PACKET_TYPE_NODE_SENSORS       	1
#define SERIAL_PACKET_TYPE_HOP_HISTORY	        2
#define SERIAL_PACKET_TYPE_EMERGENCY			3
#define SERIAL_PACKET_TYPE_SERVO_MANUAL			4
#define SERIAL_PACKET_TYPE_SET_WIND_SPEED_THRS  5

// Tx buffer size in bytes.
#define MAX_USB_PAYLOAD_SIZE	5

enum state_t {
PREPNETDISC = 45,
IDLE        = 46,
INITNETWORKDISC ,
PATHMODE         ,
UNICASTMODE     ,
EMERGENCYSTATE  ,
};
#endif
