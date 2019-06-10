/*
   Wireless Sensor Networks Laboratory

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
   http://www.lkn.ei.tum.de

   copyright (c) 2018 Chair of Communication Networks, TUM

   contributors:
   * Thomas Szyrkowiec
   * Mikhail Vilgelm
   * Octavio Rodríguez Cervantes
   * Angel Corona
   * Donjeta Elshani
   * Onur Ayan
   * Benedikt Hess

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 2.0 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   LESSON 5: Routing
*/

// turns an LED on and increases the number of packets of this color

// C includes
#include <stdio.h>

// Contiki includes
#include "contiki.h"
#include "dev/leds.h"
#include "dev/gpio.h"
#include "net/netstack.h"

//project headers
#include "helpers.h"

static uint8_t count_on[3] = { 0 }; // counts how many packets with the LED
                                    // colors red green and blue are currently
                                    // present


void turn_on(uint8_t color) {
	leds_on(color);

	if ( color == LEDS_BLUE ) {
		count_on[0]++;
	} else if ( color == LEDS_GREEN ) {
		count_on[1]++;
	} else if ( color == LEDS_RED ) {
		count_on[2]++;
	}
}


// decreases the the number of color packets and turns the LED of i 0
void turn_off(uint8_t color) {
	uint8_t index;
	if ( color == LEDS_BLUE ) {
		index = 0;
	} else if ( color == LEDS_GREEN ) {
		index = 1;
	} else if ( color == LEDS_RED ) {
		index = 2;
	} else {
		leds_off(color);
		return;
	}
	count_on[index]--;
	// check if no packets of this color are present any more and turn the LED
	// of if so
	if ( count_on[index] == 0 ) {
		leds_off(color);
	}
}

// Define a led color to indicate packets send by this RE-Mote
uint8_t get_led_color(uint8_t id) {
	uint8_t color;
	printf("\nThis node will forward packets with");

	switch	(id)	{
	case 1:	{
		color = LEDS_BLUE;
		printf(" blue color.\n\n");
		break;
	}
	case 2:	{
		color = LEDS_GREEN;
		printf(" green color.\n\n");
		break;
	}
	case 3:
		color = LEDS_RED;
		printf(" yellow color.\n\n");
		break;
	default:
		/*
		 * All other cases.
		 * */
		color = LEDS_ALL;
		printf(" default color.\n\n");
	}
	return color;
}


// Prints the current settings.
void print_settings(void){
	radio_value_t channel;

	NETSTACK_CONF_RADIO.get_value(RADIO_PARAM_CHANNEL,&channel);

	printf("\n-------------------------------------\n");
	printf("RIME addr = \t0x%x%x\n",
			linkaddr_node_addr.u8[0],
			linkaddr_node_addr.u8[1]);
	printf("Using radio channel %d\n", channel);
	printf("---------------------------------------\n");
}

uint8_t calculate_destination(uint8_t id, uint8_t network_size) {
	return ((id + network_size - 2) % network_size) + 1;
}

void toggle_GPIO(uint32_t port, uint8_t pins)
{
    const uint8_t ALLPINS = 0xFF;
    uint8_t readpins;
    readpins = GPIO_READ_PIN(port, ALLPINS);
    uint8_t clearBitmask = (readpins & (~pins));
    uint8_t tog = ((~readpins) & pins);
    tog |= clearBitmask;
    GPIO_WRITE_PIN(port, pins, tog);
}
