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

 This c-file is specifically designed for the solar panel nodes.
 */

// Contiki-specific includes:
#include "contiki.h"
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

//project headers
#include "helpers.h"
#include "nodeID.h"
#include "projSensors.h"
#include "servoControl.h"

// Sensor reading frequency in seconds.
#define LIGHT_READ_INTERVAL CLOCK_SECOND*1
#define TEMP_READ_INTERVAL CLOCK_SECOND*1


/*** CONNECTION DEFINITION***/

/*---------------------------------------------------------------------------*/
/* Callback function for received packet processing. 						 */
/*---------------------------------------------------------------------------*/
static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from) {

    leds_on(LEDS_GREEN);

    /* Get packet length */
    uint8_t len = strlen( (char *)packetbuf_dataptr() );
    /* Get packet's RSSI */
    int16_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
    /* Print address of sending node */
    printf("Got RX packet (broadcast) from: 0x%x%x, len: %d, RSSI: %d\r\n",from->u8[0], from->u8[1],len,rssi);

    leds_off(LEDS_GREEN);
}

/* Connection information
*/
static struct broadcast_conn broadcastConn;

/**
 * Assign callback functions to the connection
 */
static const struct broadcast_callbacks broadcast_callbacks = {broadcast_recv};

/*** CONNECTION DEFINITION END ***/




/*---------------------------------------------------------------------------*/
/*  MAIN PROCESS DEFINITION  												 */
/*---------------------------------------------------------------------------*/

PROCESS(gpioTesting, "GPIO Testing");
PROCESS(broadcastingThread, "Broadcasting Thread");
PROCESS(extSensorsThread, "External Sensors Thread");
AUTOSTART_PROCESSES(&broadcastingThread, &gpioTesting, &extSensorsThread);

/*** Broadcasting THREAD ***/
PROCESS_THREAD(broadcastingThread, ev, data) {

    static struct etimer et;

    PROCESS_EXITHANDLER(broadcast_close(&broadcastConn));
    PROCESS_BEGIN();

    /*
     * set your group's channel
     */
    NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL, 11); //Group 1 + 10 = 11
    print_node_IDs();
    printf("My node ID is: %d\n",getMyNodeID( linkaddr_node_addr ));

    /*
     * Open broadcast connection
     */
    broadcast_open(&broadcastConn,129,&broadcast_callbacks);

    etimer_set(&et, CLOCK_SECOND + 0.1*random_rand()/RANDOM_RAND_MAX); //randomize the sending time a little

    while(1){

        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

        leds_on(LEDS_RED);

        /*
         * fill the packet buffer
         */
        packetbuf_copyfrom("Hello You",10); //value 10 is the buffersize of the packet buffer which is filled with "Hello You"

        /*
         * send the message
         */
        broadcast_send(&broadcastConn);

        /*
         * for debugging
         */
        printf("Broadcast message sent\r\n");

        /*
         * reset the timer
         */
        etimer_reset(&et);
        leds_off(LEDS_RED);
    }
    PROCESS_END();
}


/*** GPIO Testing THREAD ***/
PROCESS_THREAD(gpioTesting, ev, data) {
    static struct etimer et1;

    PROCESS_BEGIN();
    /*
     * Open broadcast connection
     */
    uint8_t pins = 1<<6;
    uint32_t port = GPIO_A_BASE;

    gpio_init();
    GPIO_SOFTWARE_CONTROL(port, pins);
    GPIO_SET_OUTPUT(port, pins);

    initServo();
    static int pos = 0;
    etimer_set(&et1, 3*CLOCK_SECOND); // one second timer
    while(1){
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et1));
        etimer_reset(&et1);
        leds_toggle(LEDS_BLUE);
        toggle_GPIO(port,pins);
        printf("Toggled Blue led\n");
        printf("Node ID 0x%x\n", linkaddr_node_addr.u16);

        // Test Servo motor
        setServoPosition(pos);

        if (pos < 180)
            pos += 10;
        else
            pos = 0;
    }
    PROCESS_END();
}

/*** External Sensors THREAD ***/
PROCESS_THREAD (extSensorsThread, ev, data)
{

	/* variables to be used */
	static struct etimer light_reading_timer;

	PROCESS_BEGIN ();


	etimer_set(&light_reading_timer, LIGHT_READ_INTERVAL);

	while (1)
	{

		PROCESS_WAIT_EVENT();  // let process continue
		/* If timer expired, print sensor readings */
	    if(ev == PROCESS_EVENT_TIMER)
	    {
	        uint16_t light_value = getLightSensorValue();
	        printf("Luminosity via ADC1 is %d LUX\n", light_value);

    		etimer_set(&light_reading_timer, LIGHT_READ_INTERVAL);
	    }
    }

	PROCESS_END ();
}

