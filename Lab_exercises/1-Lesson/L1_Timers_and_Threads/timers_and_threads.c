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

   LESSON 1: Timers and Threads
*/


// Contiki-specific includes:
#include "contiki.h"
#include "sys/ctimer.h"
#include "dev/leds.h"			// Enables use of LEDs

// Standard C includes:
#include <stdio.h>		// For printf

static struct ctimer ct, ct_blue;
static void callback_function(void *data)
{
    leds_toggle(LEDS_RED);
    /* Reset Timer */
    ctimer_reset(&ct);
    printf("Toggling Red\r\n");
}

static void callback_function_blue(void *data)
{
    leds_toggle(LEDS_BLUE);
    /* Reset Timer */
    ctimer_reset(&ct_blue);
    printf("Toggling Blue\r\n");
}

PROCESS(timers_and_threads_process, "Lesson 1: Timers and Threads");
PROCESS(timers_and_threads_process_blue, "Lesson 1: Timers and Threads Turn on Blue LED");
AUTOSTART_PROCESSES(&timers_and_threads_process,&timers_and_threads_process_blue);

//------------------------ PROCESS' THREAD ------------------------

// Main process:
PROCESS_THREAD(timers_and_threads_process, ev, data) {


    PROCESS_EXITHANDLER( printf("main_process terminated!\n"); )

    PROCESS_BEGIN();

    ctimer_set(&ct, CLOCK_SECOND, callback_function, NULL);
    leds_on(LEDS_RED);

    PROCESS_END();
}

PROCESS_THREAD(timers_and_threads_process_blue, ev, data) {


    PROCESS_EXITHANDLER( printf("main_process terminated!\n"); )

    PROCESS_BEGIN();

    ctimer_set(&ct_blue, CLOCK_SECOND, callback_function_blue, NULL);
    leds_on(LEDS_BLUE);

    PROCESS_END();
}

