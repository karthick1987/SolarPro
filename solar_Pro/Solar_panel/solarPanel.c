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

 This c-file is specifically designed for the base node.
 */

// Contiki-specific includes:
#include "contiki.h"
#include "cpu.h"
#include "sys/etimer.h"
#include "net/rime/rime.h"	// Establish connections.
#include "net/netstack.h"	// Wireless-stack definitions
#include "lib/random.h"
#include "dev/leds.h"		// Use LEDs.
#include "core/net/linkaddr.h"
#include "dev/button-sensor.h" // User Button
#include "dev/adc-zoul.h"      // ADC
#include "dev/zoul-sensors.h"  // Sensor functions
#include "dev/sys-ctrl.h"
#include "cpu/cc2538/dev/uart.h"
//#include "cpu/cc2538/usb/usb-serial.h"	// For UART-like I/O over USB.
#include "dev/serial-line.h"			// For UART-like I/O over USB.


// Standard C includes:
#include <stdio.h>
#include <stdint.h>

// Project headers
#include "project-conf.h"

// Common headers
#include "unicast_local.h"
#include "broadcast_local.h"
#include "helpers.h"
#include "nodeID.h"
#include "routing.h"

/*---------------------------------------------------------------------------*/
#define UNICASTTRASMITINTERVAL 2*CLOCK_SECOND
#define ACKMODETRASMITINTERVAL 1*CLOCK_SECOND

extern struct etimer et_broadCastOver;
static uint16_t myNodeID;

extern struct etimer et_broadCastOver;

/*---------------------------------------------------------------------------*/
/*  MAIN PROCESS DEFINITION  												 */
/*---------------------------------------------------------------------------*/

PROCESS(mainThread, "Main Thread");
AUTOSTART_PROCESSES(&mainThread);

PROCESS_THREAD (mainThread, ev, data)
{
    PROCESS_BEGIN();

    // Configure your team's channel (11 - 26).
    NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL, CHANNEL);
    NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, TX_POWER); //Set up Tx Power
    openBroadcast();
    setUpRtable();
    myNodeID = getMyNodeID();
    printf("This is the Mote\n");
    printf("RIME ID is %x Node ID is %d\n", getMyRIMEID()->u16, myNodeID);

    /* Configure the user button */
    button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL, CLOCK_SECOND);

    //payload_t p; // For setting up payload for ackmode and Unicast modes

    while(1) {
        PROCESS_WAIT_EVENT();
        // check if button was pressed
        if(ev == sensors_event)
        {
            if(data == &button_sensor)
            {
                if( button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) ==
                        BUTTON_SENSOR_PRESSED_LEVEL )
                {
                    // Signal that Network Disc has been inited
                    // process_post(&mainThread, PROCESS_EVENT_MSG, INITNETWORKDISC);
                    initNetworkDisc(&mainThread);
                }
            }
        }//end if(ev == sensors_event)

        else if (ev == PROCESS_EVENT_TIMER)
        {
            if ((etimer_expired(&et_broadCastOver)))
            {
                printf("NETWORKDISC expired in: %d\n",BROADCASTTIMEOUT/CLOCK_SECOND);
                etimer_stop(&et_broadCastOver);
            }

            // Wake up from the emergency task and go to normal polling
        }
    }

    PROCESS_END();
}
