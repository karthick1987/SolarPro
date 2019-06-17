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

 This c-file is specifically designed for the maintenance node.
 */

// Contiki-specific includes:
#include "contiki.h"
#include "net/rime/rime.h"     // Establish connections.
#include "net/netstack.h"      // Wireless-stack definitions
#include "dev/leds.h"          // Use LEDs.
#include "dev/button-sensor.h" // User Button
#include "dev/adc-zoul.h"      // ADC
#include "dev/zoul-sensors.h"  // Sensor functions
#include "dev/sys-ctrl.h"


// Standard C includes:
#include <stdio.h>
#include <stdint.h>

// project headers
#include "helpers.h"
#include "nodeID.h"
#include "sensors.h"


// Reading frequency in seconds.
#define TEMP_READ_INTERVAL CLOCK_SECOND*1
#define JOYSTICK_POLLING CLOCK_SECOND/10 //100ms


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
PROCESS(joystickThread, "Joystick Thread");
PROCESS(broadcastingThread, "Broadcasting Thread");
AUTOSTART_PROCESSES(&broadcastingThread, &joystickThread);

/*** BROADCASTING THREAD ***/
PROCESS_THREAD(broadcastingThread, ev, data)
{

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

/*** JOYSTICK THREAD ***/
PROCESS_THREAD(joystickThread, ev, data)
{
	// define variables for this thread
	static struct etimer joystick_reading_timer;
	const char * direction;
	static uint16_t adc1_value, adc3_value;


	PROCESS_BEGIN();

	/* Configure the ADC ports */
	adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC1 | ZOUL_SENSORS_ADC3);

	etimer_set(&joystick_reading_timer, JOYSTICK_POLLING);

	while(1)
	{
		PROCESS_WAIT_EVENT();

		if(ev == PROCESS_EVENT_TIMER)
		{
			/*
			 * Read ADC values. Data is in the 12 MSBs and get direction of the Joystick
			 */
			adc1_value = adc_zoul.value(ZOUL_SENSORS_ADC1) >> 4;
			adc3_value = adc_zoul.value(ZOUL_SENSORS_ADC3) >> 4;
			direction = getJoystickPosition(adc1_value, adc3_value);
		}

		etimer_set(&joystick_reading_timer, JOYSTICK_POLLING);

	}







	PROCESS_END();
}
