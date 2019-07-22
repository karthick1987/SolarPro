/******************************************************************************
   Wireless Sensor Networks Laboratory 2019 -- Group 1

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
http://www.lkn.ei.tum.de

copyright (c) 2019 Chair of Communication Networks, TUM

project: SolarPro

contributors:
 * Karthik Sukumar
 * Johannes Machleid

 *****************************************************************************/

 /**
 * @file broadcast_common.c
 * @author Karthik Sukumar & Johannes Machleid
 * @brief Functions and Thread to handle the broadcast communication for solar
 * panel and base station
 *
 * The broadcastSendProcess Thread handles the outgoing broadcast communication
 * of the solar panel and the base station.
 */
// Standard C includes:
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "contiki.h"
#include "sys/etimer.h"
#include "broadcast_common.h"
#include "routing.h"
#include "base.h"
#include "payload.h"

/**
 * \name retransmit attempts for network discovery and prepare network discovery
 *  retransmits
 * @{
 */
#define DISCOVERYRETRANSMITS    3
#define PREPDISCRETRANSMITS    5
/** @} */

/**
 * \name Interval timers for broadcast sending interval and transition between
 *  sending states
 * @{
 */
static struct etimer bcinterval, transitiontimer;
/** @} */

PROCESS_NAME(stateMachineThread);
PROCESS(broadcastSendProcess, "Broadcast msg Send Thread");

/**
* @brief Thread to send broadcast messages
*/
PROCESS_THREAD (broadcastSendProcess, ev, data)
{
    static int i = 0;
    static pkttype_t pkt_type;
    static bool bcinterval_flag = false, transition_flag = false;

    PROCESS_BEGIN();
    while(1) {
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_MSG)
        {
            /* check if transition from prepare network discovery to actual
             * network discovery has been made */
            if (transition_flag)
            {
                etimer_reset(&transitiontimer);
            }

            /* convert process data to pkt_type to check for actual type */
            pkt_type = (int) data;

            /* if packet type is for network discovery, retransmit 3 times */
            if(pkt_type == DISCOVERY)
            {
                i = PREPDISCRETRANSMITS - DISCOVERYRETRANSMITS;
                etimer_set(&bcinterval, BROADCASTINTERVAL);
                bcinterval_flag = true;
                printf("Setting timer to BROADCAST\n");
            }
            /* if packet type is for emergency, retransmit 5 times */
            else if (pkt_type == EMERGENCY)
            {
              i = 0;
              etimer_set(&bcinterval,BROADCASTINTERVAL);
              bcinterval_flag = true;
              printf("Setting timer for EMERGENCY\n");
            }
            /* if packet type is for network preparation, retransmit 5 times */
            else if(pkt_type == PREPDISC)
            {
                i = 0;
                etimer_set(&bcinterval,BROADCASTINTERVAL);
                bcinterval_flag = true;
                printf("Setting timer for PREPDISC\n");
            }
        }
        /* if timer for retransmitting or transition expires, do so */
        else if (ev == PROCESS_EVENT_TIMER)
        {
            /* if broadcast interval is reached, retransmit again */
            if (etimer_expired(&bcinterval) && bcinterval_flag)
            {
                if (i<PREPDISCRETRANSMITS)
                {
                    doBroadCast();
                    etimer_reset(&bcinterval);
                    i++;
                    printf("Timer expired going broadcasting try:(%d)\n",i);
                }
                else
                {
                    etimer_stop(&bcinterval);
                    bcinterval_flag = false;
                    transition_flag = true;
                    etimer_set(&transitiontimer, BROADCASTTIMEOUT);
                }
            }
            /* if transition timer expires, shift to next mode */
            if (etimer_expired(&transitiontimer) && transition_flag)
            {
                printf("Broadcast Common: Pkt_Type is %d\n",pkt_type);
                etimer_stop(&transitiontimer);
                transition_flag = false;

                switch(pkt_type)
                {
                    case DISCOVERY:
                        process_post(&stateMachineThread, PROCESS_EVENT_MSG, (void *)PATHMODE);
                        break;
                    case PREPDISC:
                        process_post(&stateMachineThread, PROCESS_EVENT_MSG, (void *)INITNETWORKDISC);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    PROCESS_END();
}

/**
* @brief Function to stop all broadcast timers
*
* Function call is necessary to ensure proper functionality when transitioning
* to other modes, such as unicasting
*/
void stopAllBroadCastTimer(void)
{
    etimer_stop(&transitiontimer);
    etimer_stop(&bcinterval);
}
