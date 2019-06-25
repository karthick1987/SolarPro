/*
   Wireless Sensor Networks Laboratory 2019 -- Group 1

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
http://www.lkn.ei.tum.de

copyright (c) 2019 Chair of Communication Networks, TUM

project: SolarPro

contributors:
 * Karthik Sukumar
 * Johannes Machleid

 This c-file is specifically designed for the base node.
 */

// Contiki-specific includes:
#include "contiki.h"
#include "cpu.h"
#include "sys/etimer.h"
#include "net/rime/rime.h"	// Establish connections.
#include "net/netstack.h"	// Wireless-stack definitions
#include "lib/random.h"
#include "dev/leds.h"		// Use LEDs.
#include "core/net/linkaddr.h"
#include "dev/button-sensor.h" // User Button
#include "dev/adc-zoul.h"      // ADC
#include "dev/zoul-sensors.h"  // Sensor functions
#include "dev/sys-ctrl.h"

// Standard C includes:
#include <stdio.h>
#include <stdint.h>

//project headers
#include "helpers.h"
#include "nodeID.h"
#include "anemometer.h"

/*---------------------------------------------------------------------------*/
#define READ_SENSOR_PERIOD          CLOCK_SECOND
#define ANEMOMETER_THRESHOLD_TICK   13  /**< (value*1.2) = 16 Km/h */
/*---------------------------------------------------------------------------*/

static struct etimer et;

/*---------------------------------------------------------------------------*/
/*  MAIN PROCESS DEFINITION  												 */
/*---------------------------------------------------------------------------*/

PROCESS(windSpeedThread, "Wind Speed Sensor Thread");
AUTOSTART_PROCESSES(&windSpeedThread);


/*---------------------------------------------------------------------------*/
static void
wind_speed_callback(uint16_t value)
{
  /* This checks for instant wind speed values (over a second), the minimum
   * value is 1.2 Km/h (one tick), as the reference is 2.4KM/h per rotation, and
   * the anemometer makes 2 ticks per rotation.  Instant speed is calculated as
   * multiples of this, so if you want to check for 16Km/h, then it would be 13
   * ticks
   */
  printf("*** Wind speed over threshold (%u ticks)\n", value);
}

/*** Wind Speed Sensor THREAD ***/
PROCESS_THREAD (windSpeedThread, ev, data)
{

	PROCESS_BEGIN ();

	static uint16_t wind_speed;
	static uint16_t wind_speed_avg;
	static uint16_t wind_speed_avg_2m;
	static uint16_t wind_speed_max;

	printf("Anemometer test, integration period %u\n",
	         ANEMOMETER_AVG_PERIOD);

	/* Register the callback handler when thresholds are met */
	REGISTER_ANEMOMETER_INT(wind_speed_callback);

	/* Enable the sensors, this has to be called before any of the interrupt calls
	 * like the ones below
	 */
	SENSORS_ACTIVATE(anemometer);

	/* And the upper threshold value to compare and generate an interrupt */
	anemometer.configure(ANEMOMETER_INT_OVER, ANEMOMETER_THRESHOLD_TICK);


	etimer_set(&et, READ_SENSOR_PERIOD);

	while (1)
	{

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		/* If timer expired, print sensor readings */

		wind_speed = anemometer.value(ANEMOMETER);
		wind_speed_avg = anemometer.value(ANEMOMETER_AVG);
		wind_speed_avg_2m = anemometer.value(ANEMOMETER_AVG_X);
		wind_speed_max = anemometer.value(ANEMOMETER_MAX);

		printf("Wind speed: %u.%u km/h ", wind_speed/1000, (wind_speed % 1000)/100);
		printf("(%u.%u km/h avg, %u.%u km/h 2m avg, %u.%u km/h max)\n\n", wind_speed_avg/1000, (wind_speed_avg % 1000)/100,
				wind_speed_avg_2m/1000, (wind_speed_avg_2m % 1000)/100, wind_speed_max/1000, (wind_speed_max % 1000)/100);
		etimer_reset(&et);

    }

	PROCESS_END ();
}
