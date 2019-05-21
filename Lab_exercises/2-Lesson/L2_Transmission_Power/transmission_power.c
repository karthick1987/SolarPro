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

   LESSON 2: TRansmission Power
*/

// Contiki-specific includes:
#include "contiki.h"
#include "net/rime/rime.h"	// Establish connections.
#include "lib/random.h"
#include "dev/leds.h"
#include "dev/cc2538-rf.h"

// Standard C includes:
#include <stdio.h>
#include <stdint.h>

int i = 0;
int power_options[] = {255,237,213,197,182,176,161,145,136,114,98,88,66,0};
int power_dBm[] = {7,5,3,1,0,-1,-3,-5,-7,-9,-11,-13,-15,-24};

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


/*** MAIN PROCESS DEFINITION ***/
PROCESS(transmission_power_process, "Lesson 2: Transmission Power");
AUTOSTART_PROCESSES(&transmission_power_process);


/*** MAIN THREAD ***/
PROCESS_THREAD(transmission_power_process, ev, data) {

	static struct etimer et;
	static struct timer power_switch;


	PROCESS_EXITHANDLER(broadcast_close(&broadcastConn));
	PROCESS_BEGIN();

	// Define Transmission Arrays


	/*
	 * set your group's channel
	 */
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL, 11);

	/*
	 * Change the transmission power here
	 */

	 //NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, power_dBm[i]); // 5,3,1,-1 ... int value from table

	/*
	 * open broadcast connection
	 */
	broadcast_open(&broadcastConn,129,&broadcast_callbacks);

	etimer_set(&et, CLOCK_SECOND + 0.1*random_rand()/RANDOM_RAND_MAX); //randomize the sending time a little
	timer_set(&power_switch,CLOCK_SECOND*20);

	while(1){

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_TXPOWER, power_dBm[i]); // 5,3,1,-1 ... int value from table

		leds_on(LEDS_RED);

		/*
		 * fill the packet buffer
		 */
		packetbuf_copyfrom("Hello",6);

		/*
		 * send the message
		 */
		broadcast_send(&broadcastConn);

		/*
		 * for debugging
		 */
		printf("Broadcast message sent with power: %d dBm (Index: %d)\r\n", power_dBm[i], i); // or the configured Power

		/*
		 * reset the timer
		 */
		etimer_reset(&et);

		if(timer_expired(&power_switch)){
   		 /* If timer expired, toggle LED*/
			i++;
				if(i==14){
					i = 0;
				}
			/* Reset Timer */
			timer_reset(&power_switch);
		}


		leds_off(LEDS_RED);
	}

	PROCESS_END();
}

