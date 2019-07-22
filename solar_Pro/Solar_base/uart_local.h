/******************************************************************************
   Wireless Sensor Networks Laboratory 2019 -- Group 1

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
http://www.lkn.ei.tum.de

copyright (c) 2019 Chair of Communication Networks, TUM

project: SolarPro

contributors:
 * Karthik Sukumar
 * Johannes Machleid

 *****************************************************************************/

 /**
 * @file uart_local.h
 * @author Karthik Sukumar & Johannes Machleid
 * @brief Definition of UART connection
 *
 */
#ifndef UARTLOCAL_H
#define UARTLOCAL_H

#include "contiki.h"
#include "core/net/linkaddr.h"
#include "cpu/cc2538/dev/uart.h"
//#include "cpu/cc2538/usb/usb-serial.h"	// For UART-like I/O over USB.
#include "dev/serial-line.h"

/*---------------------------------------------------------------------------*/
// Definition of UART special characters.
// 	Note:	Contiki seems to have its own definitions.
// 			Incompatibility may cause unexpected behavior.
// 	For more details, check usb-serial.h and serial-line.h.
#define START_CHAR                  1
#define DEACTIVATION_CHAR           0x0d //25xxx4
#define END_CHAR                    0x0a //255

// Packet type flags used in UART/serial transmission.
#define SERIAL_PACKET_TYPE_ANEMOMETER         	1
#define SERIAL_PACKET_TYPE_NODE_SENSORS       	2
#define SERIAL_PACKET_TYPE_NETWORK_DISCOVERY	3
#define SERIAL_PACKET_TYPE_EMERGENCY			4
#define SERIAL_PACKET_TYPE_SERVO_MANUAL			5
#define SERIAL_PACKET_TYPE_SET_WIND_SPEED_THRS  6

// Tx buffer size in bytes.
#define MAX_USB_PAYLOAD_SIZE	5

#define UART_BUFFER_SIZE        100
#define SENSORCOPYBYTES         15
#define HOPHISTORYBYTES         10

typedef uint8_t uartBuf_t;

void sendUART(char uartBuffer[], int buffersize);
void clearHopHistClear(void);
uartBuf_t * getBuf(void);

#endif
