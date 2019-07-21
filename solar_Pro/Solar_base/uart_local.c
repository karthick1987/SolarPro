#include "contiki.h"
#include "core/net/linkaddr.h"
#include "cpu/cc2538/dev/uart.h"
//#include "cpu/cc2538/usb/usb-serial.h"	// For UART-like I/O over USB.
#include "dev/serial-line.h"
#include "uart_local.h"



void sendUART(int uartBuffer[], int buffersize)
{
  uart_write_byte(0,START_CHAR);
  for (i = 0; i < buffersize; i++)
  {
      uart_write_byte(0,uartBuffer[i]);
  }
  uart_write_byte(0,END_CHAR);
}
