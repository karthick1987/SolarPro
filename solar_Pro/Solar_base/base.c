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
#include "base.h"

// Common headers
#include "unicast_local.h"
#include "helpers.h"
#include "nodeID.h"
#include "routing.h"
#include "anemometer.h"
#include "base.h"

/*---------------------------------------------------------------------------*/

#define TX_POWER 7
#define UNICASTTRASMITINTERVAL 2*CLOCK_SECOND
#define ACKMODETRASMITINTERVAL 1*CLOCK_SECOND

// int power_options[] =    {255,237,213,197,182,176,161,145,136,114,98 ,88 ,66 ,0};
// int power_dBm[] =        {7  ,5  ,3  ,1  ,0  ,-1 ,-3 ,-5 ,-7 ,-9 ,-11,-13,-15,-24};

/*---------------------------------------------------------------------------*/
#define READ_SENSOR_PERIOD          CLOCK_SECOND
#define ANEMOMETER_THRESHOLD_TICK   13  /**< (value*1.2) = 16 Km/h */
/*---------------------------------------------------------------------------*/

static struct etimer et;

/*---------------------------------------------------------------------------*/
/*  MAIN PROCESS DEFINITION  												 */
/*---------------------------------------------------------------------------*/

PROCESS(windSpeedThread, "Wind Speed Sensor Thread");
PROCESS(broadCastThread, "BroadCast Thread");
PROCESS(uniCastThread, "Unicast Thread");
PROCESS(rxUSB_process, "Receives data from UART/serial (USB).");
AUTOSTART_PROCESSES(&broadCastThread, &uniCastThread, &rxUSB_process);
//&windSpeedThread

/*---------------------------------------------------------------------------*/
    static void
wind_speed_callback(uint16_t value)
{
    /* This checks for instant wind speed values (over a second), the minimum
     * value is 1.2 Km/h (one tick), as the reference is 2.4KM/h per rotation, and
     * the anemometer makes 2 ticks per rotation.  Instant speed is calculated as
     * multiples of this, so if you want to check for 16Km/h, then it would be 13
     * ticks
     */
    printf("*** Wind speed over threshold (%u ticks)\n", value);
}

/*** Wind Speed Sensor THREAD ***/
PROCESS_THREAD (windSpeedThread, ev, data)
{

    PROCESS_BEGIN ();
    static uint16_t wind_speed;
    static uint16_t wind_speed_avg;
    static uint16_t wind_speed_avg_2m;
    static uint16_t wind_speed_max;
    static int uartTxBuffer[MAX_USB_PAYLOAD_SIZE];
    static int i;

    printf("Anemometer test, integration period %u\n",
            ANEMOMETER_AVG_PERIOD);

    /* Register the callback handler when thresholds are met */
    REGISTER_ANEMOMETER_INT(wind_speed_callback);

    /* Enable the sensors, this has to be called before any of the interrupt calls
     * like the ones below
     */
    SENSORS_ACTIVATE(anemometer);

    /* And the upper threshold value to compare and generate an interrupt */
    anemometer.configure(ANEMOMETER_INT_OVER, ANEMOMETER_THRESHOLD_TICK);


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
        uartTxBuffer[2] = wind_speed_avg/1000;
        uartTxBuffer[3] = wind_speed_max/1000;

        uart_write_byte(0,START_CHAR);
        for (i = 0; i < 5; i++)
        {
            uart_write_byte(0,uartTxBuffer[i]);
        }
        uart_write_byte(0,END_CHAR);

        etimer_reset(&et);
    }

    PROCESS_END ();
}

PROCESS_THREAD (broadCastThread, ev, data)
{
    PROCESS_BEGIN();

    // Configure your team's channel (11 - 26).
    NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL,11);
    NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, TX_POWER); //Set up Tx Power
    openBroadcast();
    setUpRtable();
    printf("My RIME ID is %x Node ID is %d\n", getMyRIMEID()->u16, getMyNodeID());

    /* Configure the user button */
    button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL, CLOCK_SECOND);
    static struct etimer et_broadCastOver;
    //etimer_set(&et_broadCastOver, 3*CLOCK_SECOND);

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
                    initNetworkDisc(&broadCastThread, &et_broadCastOver);
                }
            }
        }//end if(ev == sensors_event)

        else if (ev == PROCESS_EVENT_TIMER)
        {
            //process_post(&uniCastThread, PROCESS_EVENT_MSG, ACKMODE);//(((i)%7)+1));
            printf("Timer for Network Discovery expired\n");
            etimer_stop(&et_broadCastOver);
        }

        else if (ev == PROCESS_EVENT_MSG)
        {
            printf("File sent this process a message %d!\n", (int) data);
            // DO the emergency broadcast task

            // Wake up from the emergency task and go to normal polling
        }
    }

    PROCESS_END();
}


PROCESS_THREAD (uniCastThread, ev, data)
{
    PROCESS_BEGIN();

    // Configure your team's channel (11 - 26).
    NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL,11);
    NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, TX_POWER); //Set up Tx Power

    static struct etimer et_unicastTransmit, et_ackModeTransmit;
    static int status = 0;
    static node_num_t node = 1;
    static enum unicast_state state = ACKMODE;

    while(1) {
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_TIMER)
        {
            // In case the unicast timer 
            if (etimer_expired(&et_unicastTransmit) && (state == UNICASTMODE))
            {
                //etimer_reset(&et_unicastTransmit);
                printf("Restarted Unicast Timer\n");
                process_post(&uniCastThread, PROCESS_EVENT_MSG, UNICASTMODE);
                leds_toggle(LEDS_BLUE);
                //print_active_procs();
            }
            if (etimer_expired(&et_ackModeTransmit) && (state == ACKMODE))
            {
                etimer_reset(&et_ackModeTransmit);
                printf("Restarted ackMode Timer\n");
                process_post(&uniCastThread, PROCESS_EVENT_MSG, ACKMODE);
                // Start polling the devices one by one
                leds_toggle(LEDS_RED);
                //print_active_procs();
            }
        }
        else if (ev == PROCESS_EVENT_MSG)
        {
            state = (enum unicast_state) (data);
            printf("STATE is %d\n",state);
            payload_t p;
            switch((int) data)
            {
                case ACKMODE:
                    // Send unicast and expect payload as hop history
                    status = doAckMode(node,&p);

                    printf("Here in ACKMODE to node %d, Status is %d\n",node,status);

                    // Set up the ACKMODE Packet and send data in round robin fashion
                    node++;

                    // Set the etimer to expire again
                    etimer_set(&et_ackModeTransmit, ACKMODETRASMITINTERVAL);
                    if ( status == 0 )
                    {
                        etimer_stop(&et_ackModeTransmit);
                        process_post(&uniCastThread, PROCESS_EVENT_MSG, UNICASTMODE);
                        node = 1;
                    }
                    break;

                case UNICASTMODE:
                    // Collect data from the motes
                    status = doUniCastMode(node);

                    printf("Here in UNICASTMODE to node %d, Status is %d\n",node,status);

                    // Go to the next mote
                    node++;

                    // Set the timer for Unicast interval
                    etimer_set(&et_unicastTransmit, UNICASTTRASMITINTERVAL);
                    if ( status == 0 )
                    {
                        // start again from Node 1 in a round robin manner
                        node = 1;
                        process_post(&uniCastThread, PROCESS_EVENT_MSG, UNICASTMODE);
                    }
                    // start unicast polling one by one
                    break;

                case STARTBROADCAST:
                    // stop unicast polling and go to Broadcast mode
                    printf("Here in STARTBROADCAST\n");
                    break;
                case STOPBROADCAST:
                    // stop unicast polling and go to Broadcast mode
                    printf("Here in STOPBROADCAST\n");
                    break;
                case STARTEMERGENCY:
                    // stop unicast polling and go to Broadcast mode
                    printf("Here in STARTEMERGENCY\n");
                    break;
                case STOPEMERGENCY:
                    // EMERGENCY is done so start polling as normal again
                    printf("Here in STOPUNICAST\n");
                    break;
            }
        }
    }//end while(1)

    PROCESS_END();
}

// Listens for data coming from the USB connection (UART0)
// and prints it.
PROCESS_THREAD(rxUSB_process, ev, data) {
    PROCESS_BEGIN();

    uint8_t *uartRxBuffer;


    /* In this example, whenever data is received from UART0, the
     * default handler (i.e. serial_line_input_byte) is called.
     * But how does it work?
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

            leds_toggle(LEDS_RED);

            uartRxBuffer = (uint8_t*)data;

            // If the received packet is a packet to start network discovery
            if(uartRxBuffer[0] == SERIAL_PACKET_TYPE_NETWORK_DISCOVERY) {

                // Do network discovery

            }
            else if(uartRxBuffer[0] == SERIAL_PACKET_TYPE_EMERGENCY) {

                // Do Emergency
            }
            else if(uartRxBuffer[0] == SERIAL_PACKET_TYPE_SERVO_MANUAL){

                // Do Servo Manual operation
            }
            //leds_off(LEDS_BLUE);
        }
    }
    PROCESS_END();
}
