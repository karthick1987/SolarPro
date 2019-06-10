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

 LESSON 2: Simple packet
 */

// Contiki-specific includes:
#include "contiki.h"
#include "net/rime/rime.h"	// Establish connections.
#include "net/netstack.h"
#include "lib/random.h"
#include "dev/leds.h"

// Standard C includes:
#include <stdio.h>
#include <stdint.h>


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
AUTOSTART_PROCESSES(&broadcastingThread, &gpioTesting);

/*** MAIN THREAD ***/
PROCESS_THREAD(broadcastingThread, ev, data) {

    static struct etimer et;

    PROCESS_EXITHANDLER(broadcast_close(&broadcastConn));
    PROCESS_BEGIN();

    /*
     * set your group's channel
     */
    NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL, 11); //Group 1 + 10 = 11

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

PROCESS_THREAD(gpioTesting, ev, data) {
    static struct etimer et1;

    PROCESS_BEGIN();
    /*
     * Open broadcast connection
     */
    etimer_set(&et1, CLOCK_SECOND); // one second timer
    while(1){

        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et1));
        etimer_reset(&et1);
        leds_toggle(LEDS_BLUE);
        printf("Toggled Blue led\n");
    }
    PROCESS_END();
}
