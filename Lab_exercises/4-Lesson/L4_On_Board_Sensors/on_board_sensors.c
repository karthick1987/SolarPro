/*
   Wireless Sensor Networks Laboratory

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
   http://www.lkn.ei.tum.de

   copyright (c) 2017 Chair of Communication Networks, TUM

   contributors:
   * Thomas Szyrkowiec
   * Mikhail Vilgelm
   * Octavio Rodríguez Cervantes
   * Angel Corona

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 2.0 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   LESSON 3: Temperature Sensor.
*/

// Contiki-specific includes:
#include "contiki.h"
#include "net/rime/rime.h"     // Establish connections.
#include "net/netstack.h"      // Wireless-stack definitions
#include "dev/leds.h"          // Use LEDs.
#include "dev/adc-zoul.h"      // ADC
#include "dev/zoul-sensors.h"  // Sensor functions
#include "dev/sys-ctrl.h"
// Standard C includes:
#include <stdio.h>      // For printf.

// Reading frequency in seconds.
#define TEMP_READ_INTERVAL CLOCK_SECOND*1

// global variables
int sentFlag = 0;
char sendstr[100];

typedef struct{
	int vdd;
	int temp;
}on_board_sensors;


/*** CONNECTION DEFINITION***/

/**
 * Callback function for received packet processing.
 *
 */
static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from) {

	leds_on(LEDS_GREEN);

	uint8_t len = strlen( (char *)packetbuf_dataptr() );
	int16_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);

	printf("Got RX packet (broadcast) from: 0x%x%x, len: %d, RSSI: %d\r\n",from->u8[0], from->u8[1],len,rssi);

	leds_off(LEDS_GREEN);
}

/**
 * Connection information
 */
static struct broadcast_conn broadcastConn;

/**
 * Assign callback functions to the connection
 */
static const struct broadcast_callbacks broadcast_callbacks = {broadcast_recv};

/*** CONNECTION DEFINITION END ***/

//--------------------- PROCESS CONTROL BLOCK ---------------------
PROCESS (on_board_sensors_process, "Lesson 3: On-Board Sensors");
AUTOSTART_PROCESSES (&on_board_sensors_process);

//------------------------ PROCESS' THREAD ------------------------
PROCESS_THREAD (on_board_sensors_process, ev, data) {

	/* variables to be used */
	static struct etimer temp_reading_timer;
	static on_board_sensors obs;

	PROCESS_EXITHANDLER(broadcast_close(&broadcastConn););
	PROCESS_BEGIN ();

	printf("\r\nZolertia RE-Mote on-board sensors");
	printf("\r\n====================================");

	/*
	 * set your group's channel
	 */
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL,11);

	/*
	 * open the connection
	 */
	broadcast_open(&broadcastConn,129,&broadcast_callbacks);

	etimer_set(&temp_reading_timer, TEMP_READ_INTERVAL);

	while (1) {

		PROCESS_WAIT_EVENT();  // let process continue

		obs.vdd = vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED); //get VDD sensor value in mV
		obs.temp = cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED); // get onboard temperature in mC
		/* If timer expired, print sensor readings */
	    if(ev == PROCESS_EVENT_TIMER) {

	    	leds_on(LEDS_PURPLE);


    		/* Send and print the battery voltage converted in mV alternated*/
    		/* Send and print the temperature of the onboard temperature sensor in °C *10^-3 (mC) alternated*/

    		switch(sentFlag) // initial value is 0
			{
    		case 0:
    			printf("\r\nMy Battery Voltage [VDD] = %d mV", obs.vdd);
    			sprintf(sendstr, "Battery: %d mV", obs.vdd);
    			packetbuf_copyfrom(sendstr, 100);
    			broadcast_send(&broadcastConn);
    			sentFlag = 1;
    			break;

    		case 1:
    			printf("\r\nMy Temperature	  [TEMP] = %d mC", obs.temp);
    			sprintf(sendstr, "Temperature: %d mC", obs.temp);
    			packetbuf_copyfrom(sendstr, 100);
    			broadcast_send(&broadcastConn);
    			sentFlag = 0;
    			break;
    		}


    		leds_off(LEDS_PURPLE);

    		etimer_set(&temp_reading_timer, TEMP_READ_INTERVAL);
	    }

    }

	PROCESS_END ();
}
