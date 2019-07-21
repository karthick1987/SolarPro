
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

#define DISCOVERYRETRANSMITS    3
#define PREPDISCRETRANSMITS    5

static struct etimer bcinterval, transitiontimer;

PROCESS_NAME(stateMachineThread);
PROCESS(broadcastSendProcess, "Broadcast msg Send Thread");
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
            if (transition_flag)
            {
                etimer_reset(&transitiontimer);
            }
            pkt_type = (int) data;
            if(pkt_type == DISCOVERY)
            {
                i = PREPDISCRETRANSMITS - DISCOVERYRETRANSMITS;
                etimer_set(&bcinterval, BROADCASTINTERVAL);
                bcinterval_flag = true;
                //transition_flag = false;
                printf("Setting timer to BROADCAST\n");
            }
            else if (pkt_type == EMERGENCY)
            {
            }
            else if(pkt_type == PREPDISC)
            {
                i = 0;
                etimer_set(&bcinterval,BROADCASTINTERVAL);
                bcinterval_flag = true;
                printf("Setting timer for PREPDISC\n");
            }
        }

        else if (ev == PROCESS_EVENT_TIMER)
        {
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

void stopAllBroadCastTimer(void)
{
    etimer_stop(&transitiontimer);
    etimer_stop(&bcinterval);
}
