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
 * @file base.c
 * @author Karthik Sukumar & Johannes Machleid
 * @brief Main functions of the basestation with a state machine thread
 *
 *      This file contains the main functionalities of the base station including a
 *      state machine and the anemometer sensor reading.
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
#include <stdbool.h>

// Project headers
#include "base.h"
#include "uart_local.h"

// Common headers
#include "broadcast_common.h"
#include "helpers.h"
#include "nodeID.h"
#include "routing.h"
#include "anemometer.h"
#include "unicast_local.h"

/*---------------------------------------------------------------------------*/
#define UNICASTTRASMITINTERVAL 2*CLOCK_SECOND
#define ACKMODETRASMITINTERVAL 1*CLOCK_SECOND

// int power_options[] =    {255,237,213,197,182,176,161,145,136,114,98 ,88 ,66 ,0};
// int power_dBm[] =        {7  ,5  ,3  ,1  ,0  ,-1 ,-3 ,-5 ,-7 ,-9 ,-11,-13,-15,-24};

/*---------------------------------------------------------------------------*/
#define READ_SENSOR_PERIOD          CLOCK_SECOND
#define ANEMOMETER_THRESHOLD_TICK   13  /**< (value*1.2) = 16 Km/h */
int threshold_tick = 13;
int threshold = 16;
/*---------------------------------------------------------------------------*/

static struct etimer et;
extern struct etimer et_broadCastOver;

/*---------------------------------------------------------------------------*/
/*  MAIN PROCESS DEFINITION  												 */
/*---------------------------------------------------------------------------*/

PROCESS_NAME(broadcastSendProcess);
PROCESS_NAME(unicastSendProcess);

//PROCESS(txUSB_process, "Sending payload");
PROCESS(windSpeedThread, "Wind Speed Sensor Thread");
PROCESS(stateMachineThread, "State Machine Thread");
PROCESS(rxUSB_process, "Receives data from UART/serial (USB).");
AUTOSTART_PROCESSES (&unicastSendProcess, &broadcastSendProcess, &stateMachineThread ,&rxUSB_process, &windSpeedThread);

/*---------------------------------------------------------------------------*/
/**
* @brief Callback function called, when the windspeed passes a defined threshold
*
*/
static void wind_speed_callback(uint16_t value)
{
    /* This checks for instant wind speed values (over a second), the minimum
     * value is 1.2 Km/h (one tick), as the reference is 2.4KM/h per rotation, and
     * the anemometer makes 2 ticks per rotation.  Instant speed is calculated as
     * multiples of this, so if you want to check for 16Km/h, then it would be 13
     * ticks
     */
    printf("*** Wind speed over threshold of %d km/h (%u ticks)\n", threshold, value);
    process_post(&stateMachineThread, PROCESS_EVENT_MSG, (void *)EMERGENCY);
}

/*** Wind Speed Sensor THREAD ***/
/**
* @brief Thread to read out the wind speed sensor
*
*/
PROCESS_THREAD (windSpeedThread, ev, data)
{

    PROCESS_BEGIN ();
    static uint16_t wind_speed;
    static uint16_t wind_speed_avg;
    static uint16_t wind_speed_avg_2m;
    static uint16_t wind_speed_max;
    static char uartTxBuffer[MAX_USB_PAYLOAD_SIZE];

    printf("Anemometer test, integration period %u\n",
            ANEMOMETER_AVG_PERIOD);

    /* Register the callback handler when thresholds are met */
    REGISTER_ANEMOMETER_INT(wind_speed_callback);

    /* Enable the sensors, this has to be called before any of the interrupt calls
     * like the ones below
     */
    SENSORS_ACTIVATE(anemometer);

    /* And the upper threshold value to compare and generate an interrupt */
    anemometer.configure(ANEMOMETER_INT_OVER, threshold_tick);


    etimer_set(&et, READ_SENSOR_PERIOD);

    while (1)
    {

        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        /* If timer expired, print sensor readings */

        wind_speed = anemometer.value(ANEMOMETER);
        wind_speed_avg = anemometer.value(ANEMOMETER_AVG);
        wind_speed_avg_2m = anemometer.value(ANEMOMETER_AVG_X);
        wind_speed_max = anemometer.value(ANEMOMETER_MAX);

        uartTxBuffer[0] = SERIAL_PACKET_TYPE_ANEMOMETER;
        uartTxBuffer[1] = wind_speed/1000;
        uartTxBuffer[2] = wind_speed_avg_2m/1000;
        uartTxBuffer[3] = wind_speed_max/1000;
        uartTxBuffer[4] = threshold;

        sendUART(uartTxBuffer, MAX_USB_PAYLOAD_SIZE);

        etimer_reset(&et);
    }

    PROCESS_END ();
}

/**
* @brief State machine thread of the basestation
*
*       The state machine enables the base station to operate in different modes.
*       1. Prepare Network Discovery: triggers all motes in the network to clear its
*          routing table through broadcasting.
*       2. Network Discovery: triggers all motes in the network to build their routing
*          tables by exchanging information with their neighbors.
*       3. Unicast Mode: base station first collects the hop history to each mote and
*          then starts polling the sensor values.
*       4. Emergency State: triggers all motes in the network to go into an emergency
*          state through broadcasting.
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
    printf("This is the BASE STATION\n");
    printf("BASE STATION ID is %x Node ID is %d\n", getMyRIMEID()->u16, getMyNodeID());

    /* Configure the user button */
    button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL, CLOCK_SECOND);
    static enum state_t state = IDLE;

    process_post(&stateMachineThread, PROCESS_EVENT_MSG, (void *)IDLE);


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
                    process_post(&stateMachineThread, PROCESS_EVENT_MSG, (void *)PREPNETDISC);
                }
            }
        }

        else if (ev == PROCESS_EVENT_TIMER)
        {
        }

        else if (ev == PROCESS_EVENT_MSG)
        {
            char uartTxBuffer[MAX_USB_PAYLOAD_SIZE];
            state = (enum state_t) (data);
            printf("STATE is %d\n",state);
            //printf("File sent this process a message %d!\n", (int) data);
            switch(state)
            {
                case IDLE:
                    break;

                case PREPNETDISC:
                    // transmit serial message to GUI
                    uartTxBuffer[0] = SERIAL_PACKET_TYPE_NETWORK_DISCOVERY;
                    uartTxBuffer[1] = 0xFF;
                    uartTxBuffer[2] = 0xFF;
                    uartTxBuffer[3] = 0xFF;
                    sendUART(uartTxBuffer, MAX_USB_PAYLOAD_SIZE);
                    // kill unicast process
                    process_exit(&unicastSendProcess);
                    prepNetworkDisc();
                    break;

                case INITNETWORKDISC:
                    initNetworkDisc();
                    break;

                case PATHMODE:
                    // start unicast_process
                    process_start(&unicastSendProcess,NULL);
                    initPathMode();
                    process_post(&unicastSendProcess, PROCESS_EVENT_MSG, (void *)PATH);
                    break;

                case UNICASTMODE:
                    initUnicastMode();
                    process_post(&unicastSendProcess, PROCESS_EVENT_MSG, (void *)UNICAST);
                    break;

                case EMERGENCYSTATE:
                    process_exit(&unicastSendProcess);
                    initEmergency();
                    break;

                default:
                    printf("[Base.c]: In Default???\n");
                    break;
            }
        }
    }

    PROCESS_END();
}

/**
* @brief Listens for data coming from the USB connection (UART0) and
*     processes it according to its type
*
*/
PROCESS_THREAD(rxUSB_process, ev, data) {
    PROCESS_BEGIN();

    uint8_t *uartRxBuffer;


    /* whenever data is received from UART0, the
     * default handler (i.e. serial_line_input_byte) is called.
     *
     * The handler runs in a process that puts all incoming data
     * in a buffer until it gets a newline or END character
     * ('\n' and 0x0A are equivalent). Also good to know is that
     * there is an IGNORE character: 0x0D.
     *
     * Once a newline or END is detected, a termination char ('\0')
     * is appended to the buffer and the process broadcasts a
     * "serial_line_event_message" along with the buffer contents.
     *
     * For more details, check contiki/core/dev/serial-line.c
     * For an "example", check contiki/apps/serial-shell/serial-shell.c
     */
    while(1){
        PROCESS_YIELD();

        if (ev == serial_line_event_message)
        {

            leds_on(LEDS_RED);

            uartRxBuffer = (uint8_t*)data;

            switch (uartRxBuffer[0]) {
              case SERIAL_PACKET_TYPE_NETWORK_DISCOVERY:
                // Signal that Network Disc has been inited
                process_post(&stateMachineThread, PROCESS_EVENT_MSG, (void *)PREPNETDISC);
                break;

              case SERIAL_PACKET_TYPE_EMERGENCY:
                //run emergency mode
                printf("Received EMERGENCY cmd\n");
                process_post(&stateMachineThread, PROCESS_EVENT_MSG, (void *)EMERGENCYSTATE);
                break;

              case SERIAL_PACKET_TYPE_SET_WIND_SPEED_THRS:
                threshold = uartRxBuffer[1];
                threshold_tick = threshold/1.2;
                printf("Set Windspeed Threshold to %d km/h",uartRxBuffer[1]);
                break;

              case SERIAL_PACKET_TYPE_ANEMOMETER:
                break;

              case SERIAL_PACKET_TYPE_SERVO_MANUAL:
                //set servo angle manually
                break;

              case SERIAL_PACKET_TYPE_NODE_SENSORS:
                break;

              default:
                printf("Unknown UART command");
                break;
            }
            leds_off(LEDS_RED);
        }
    }
    PROCESS_END();
}
