
// Standard C includes:
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "contiki.h"
#include "sys/etimer.h"
#include "broadcast_common.h"

#define BROADCASTRETRANSMITS    3

PROCESS_THREAD (broadcastSendProcess, ev, data)
{
    static struct etimer bcnow;
    static int i = 0;
    PROCESS_BEGIN();
    while(1) {
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_MSG)
        {
            i = 0;
            etimer_set(&bcnow, CLOCK_SECOND/2);
            printf("Setting timer to broadcast\n");

        }

        else if (ev == PROCESS_EVENT_TIMER)
        {
            if (etimer_expired(&bcnow) )
            {
                printf("Timer expired going to broadcast %d now\n",i+2);
                if (i<BROADCASTRETRANSMITS)
                {
                    doBroadCast();
                    etimer_reset(&bcnow);
                    i++;
                }
                else
                {
                    etimer_stop(&bcnow);
                }
            }
        }
    }

    PROCESS_END();
}
