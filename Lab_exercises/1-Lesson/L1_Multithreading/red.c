
#include "contiki.h"
#include "dev/leds.h" // Enables use of LEDs.

// Standard C includes:
#include <stdio.h>    // For printf.

#define PRINTF(...)   printf(__VA_ARGS__)


static struct etimer timerRed,twoSecond;
PROCESS(red, "Red");
PROCESS_THREAD(red, ev, data) {

    PROCESS_BEGIN();
    etimer_set(&timerRed, CLOCK_SECOND);
    etimer_set(&twoSecond, 2*CLOCK_SECOND);
    printf("Timers set!\r\n ");

    while(1) {
        PROCESS_WAIT_EVENT();
        if(etimer_expired(&timerRed)) {
            printf("Timer expired for RED...\r\n");
            leds_toggle(LEDS_RED);
            etimer_reset(&timerRed);
        }
        if(etimer_expired(&twoSecond)) {
            printf("Killing Process Red\n");
            PROCESS_EXIT();
        }
    }
    PROCESS_END();
}

