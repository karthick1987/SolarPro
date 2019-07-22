#include "contiki.h"
#include "core/net/linkaddr.h"
#include "cpu/cc2538/dev/uart.h"
#include "dev/serial-line.h"
#include "uart_local.h"

#include<stdio.h>

static uartBuf_t uartBuf[UART_BUFFER_SIZE];

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

void clearHopHistClear(void)
{
    memset(uartBuf,0,UART_BUFFER_SIZE);
    uartBuf[0] = SERIAL_PACKET_TYPE_NETWORK_DISCOVERY;
}

uint8_t * getBuf(void)
{
    return uartBuf;
}

