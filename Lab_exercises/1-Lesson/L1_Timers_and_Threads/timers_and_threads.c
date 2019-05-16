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
#include "sys/etimer.h"
#include "dev/leds.h"			// Enables use of LEDs

// Standard C includes:
#include <stdio.h>		// For printf

#define PRINTF(...)   printf(__VA_ARGS__)

/*
 * \brief   Function to print the processes running currently
 */

void print_active_procs(void)
{
    uint8_t ps=process_nevents();
    struct process *p;
    PRINTF("there are %u events in the queue\n\n", ps);
    PRINTF("Processes:\n");
    for(p = PROCESS_LIST(); p != NULL; p = p->next)
    {
        char namebuf[30];
        strncpy(namebuf, PROCESS_NAME_STRING(p), sizeof(namebuf));
        PRINTF("--%s--\n", namebuf);
    }
    PRINTF("\n\n");
}

// The ct_kill_proc timer is to kill the process after 
static struct ctimer ct, ct_kill_proc;
static struct etimer et_blue;

// Define the 2 processes first
PROCESS(timers_and_threads_process, "Red_led");
PROCESS(timers_and_threads_process_blue, "Blue_led");

/*
 * Function to toggle the Red LED
 */

static void callback_function(void *data)
{
    leds_toggle(LEDS_RED);
    /* Reset Timer */
    ctimer_reset(&ct);
    printf("Toggling Red\r\n");
}

/*
 * Callback function for the 5s time to stop the 
 */
static void kill_proc(void *data)
{
    printf("entered kill_proc()\n");

    // Stop both the 5s timer as well as the RED LED toggle timer
    ctimer_stop(&ct_kill_proc);
    ctimer_stop(&ct);
    leds_off(LEDS_RED);

    // kill the process as well
    process_exit(&timers_and_threads_process);
    // PROCESS_EXIT();
}

// Add the list of processes to auto start here in this macro because
// #define AUTOSTART_PROCESSES(...)
// struct process * const autostart_processes[] = {__VA_ARGS__, NULL}

AUTOSTART_PROCESSES(&timers_and_threads_process,&timers_and_threads_process_blue);

//------------------------ PROCESS' THREAD 1------------------------

// RED LED process:
PROCESS_THREAD(timers_and_threads_process, ev, data) {


    PROCESS_EXITHANDLER( printf("Red_led terminated!\n"); )

    PROCESS_BEGIN();
    // callback timer to toggle led every second
    ctimer_set(&ct, CLOCK_SECOND, callback_function, NULL);
    // callback timer to kill proc
    // ctimer_set(&ct_kill_proc, 5*CLOCK_SECOND, kill_proc, NULL);

    leds_on(LEDS_RED);

    PROCESS_END();
}

//------------------------ PROCESS' THREAD 2------------------------

// BLUE LED process:
PROCESS_THREAD(timers_and_threads_process_blue, ev, data) {


    PROCESS_EXITHANDLER( printf("Blue_led terminated!\n"); )

    PROCESS_BEGIN();
    leds_on(LEDS_BLUE);

    /* Delay 1 second */
    etimer_set(&et_blue, CLOCK_SECOND);

    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et_blue));
        leds_toggle(LEDS_BLUE);
        etimer_reset(&et_blue);
        printf("Toggling Blue\r\n");print_active_procs();
    }

    PROCESS_END();
}

