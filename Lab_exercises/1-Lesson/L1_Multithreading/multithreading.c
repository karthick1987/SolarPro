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

   LESSON 1: Multithreading
*/

// Contiki-specific includes:
#include "contiki.h"
#include "dev/leds.h" // Enables use of LEDs.

// Standard C includes:
#include <stdio.h>    // For printf.

static struct etimer timerRed, timerGreen, timerBlue;

//--------------------- PROCESS CONTROL BLOCK ---------------------
PROCESS(red, "Red");
PROCESS(green, "Green");
PROCESS(blue, "Blue");
AUTOSTART_PROCESSES(&red, &green, &blue);

//------------------------ PROCESS' THREAD ------------------------

	/*
	 * Set timers
	 */

    PROCESS_THREAD(red, ev, data) {

        PROCESS_BEGIN();
        etimer_set(&timerRed, CLOCK_SECOND);
        printf("Timers set!\r\n ");

        while(1) {
            PROCESS_WAIT_EVENT();
            if(etimer_expired(&timerRed)) {
                printf("Timer expired for RED...\r\n");
                leds_toggle(LEDS_RED);
                etimer_reset(&timerRed);
            }
        }
        PROCESS_END();
    }

    PROCESS_THREAD(green, ev, data) {

        PROCESS_BEGIN();
        etimer_set(&timerGreen, CLOCK_SECOND/2);

        while(1) {
            PROCESS_WAIT_EVENT();
            if(etimer_expired(&timerGreen)) {
                printf("Timer expired for Green...\r\n");
                leds_toggle(LEDS_GREEN);
                etimer_reset(&timerGreen);
            }
        }

        PROCESS_END();
    }

    PROCESS_THREAD(blue, ev, data) {

        PROCESS_BEGIN();
        etimer_set(&timerBlue, CLOCK_SECOND/4);

        while(1) {
            PROCESS_WAIT_EVENT();
            if(etimer_expired(&timerBlue)) {
                printf("Timer expired for Blue...\r\n");
                leds_toggle(LEDS_BLUE);
                etimer_reset(&timerBlue);
            }
        }
        PROCESS_END();
    }


