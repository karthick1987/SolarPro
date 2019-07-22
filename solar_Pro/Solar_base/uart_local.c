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
 * @file uart_local.c
 * @author Karthik Sukumar & Johannes Machleid
 * @brief Functions to handle UART communication between the base station and GUI
 *
 */
#include "contiki.h"
#include "core/net/linkaddr.h"
#include "cpu/cc2538/dev/uart.h"
#include "dev/serial-line.h"
#include "uart_local.h"

#include<stdio.h>

static uartBuf_t uartBuf[UART_BUFFER_SIZE];


/**
* @brief Function to write payload on UART
* @param uartBuffer[] is an array of bytes to send over serial
* @param buffersize defines the amount of bytes to be sent
*/
void sendUART(char uartBuffer[], int buffersize)
{
    int i;
    uart_write_byte(0,START_CHAR);
    for (i = 0; i < buffersize; i++)
    {
        uart_write_byte(0,uartBuffer[i]);
    }
    uart_write_byte(0,END_CHAR);
    return;
}

/**
* @brief Function sends a clearing package to GUI during network discovery
*/
void clearHopHistClear(void)
{
    memset(uartBuf,0,UART_BUFFER_SIZE);
    uartBuf[0] = SERIAL_PACKET_TYPE_NETWORK_DISCOVERY;
}

/**
* @brief Function to return the UART Buffer
*/
uint8_t * getBuf(void)
{
    return uartBuf;
}
