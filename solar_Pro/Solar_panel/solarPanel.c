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
 * @file solarPanel.c
 * @author Karthik Sukumar & Johannes Machleid
 * @brief Setting up the solar panel motes with RF connection and running a
 * state machine thread for the solar panel
 *
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

// Standard C includes:
#include <stdio.h>
#include <stdint.h>

// Project headers
#include "project-conf.h"

// Common headers
#include "unicast_local.h"
#include "broadcast_common.h"
#include "helpers.h"
#include "nodeID.h"
#include "routing.h"
#include "servoControl.h"
#include "solarPanel.h"
/*---------------------------------------------------------------------------*/

#define SERVO_REFRESH 2*CLOCK_SECOND

static uint16_t myNodeID;

static struct ctimer ctServo;
static int angle = 0;

void setAngle(int ang)
{
    angle = ang;
}

static void callback_Servo(void *ptr)
{
    setServoPosition(angle);
    angle+=3;
    if(angle > 170)
    {
        angle = 0;
    }
    ctimer_reset(&ctServo);
}

void setAngle(int angle);

void stopServoCallback(void)
{
  ctimer_stop(&ctServo);
}

void resetServoCallback(void)
{
  ctimer_reset(&ctServo);
}

/*---------------------------------------------------------------------------*/
/*  MAIN PROCESS DEFINITION  												 */
/*---------------------------------------------------------------------------*/
// The process is defined in another file
PROCESS_NAME(broadcastSendProcess);

PROCESS(stateMachineThread, "Main Thread");
AUTOSTART_PROCESSES(&stateMachineThread, &broadcastSendProcess);

/**
* @brief Thread runs the state machine of the solar panel
*
*       The solar panel mote waits in idle until it receives messages or a
*       network discovery is initiated by pressing the USER Button on the mote.
*
*/
PROCESS_THREAD (stateMachineThread, ev, data)
{
    PROCESS_BEGIN();

    // Configure your team's channel (11 - 26).
    NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL, CHANNEL);
    NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, TX_POWER); //Set up Tx Power
    openBroadcast();
    openUnicast();
    setUpRtable();
    initServo();
    angle = 0;

    ctimer_set(&ctServo, SERVO_REFRESH, callback_Servo, NULL);

    myNodeID = getMyNodeID();
    printf("This is the Mote\n");
    printf("RIME ID is %x Node ID is %d\n", getMyRIMEID()->u16, myNodeID);

    /* Configure the user button */
    button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL, CLOCK_SECOND);

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
                    prepNetworkDisc();
                }
            }
        }

        else if (ev == PROCESS_EVENT_TIMER)
        {
        }
    }

    PROCESS_END();
}
