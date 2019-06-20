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

 This c-file is designed for all nodes to read out wind speed sensor sensor values.
 */

/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include "contiki.h"
#include "dev/adc-zoul.h"
#include "anemometer.h"
#include "dev/zoul-sensors.h"
#include "lib/sensors.h"
#include "dev/sys-ctrl.h"
#include "dev/gpio.h"
#include "dev/ioc.h"
#include "sys/timer.h"
#include "sys/ctimer.h"
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#define DEBOUNCE_DURATION  (CLOCK_SECOND >> 6)
/*---------------------------------------------------------------------------*/
#define ANEMOMETER_SENSOR_PORT_BASE  GPIO_PORT_TO_BASE(ANEMOMETER_SENSOR_PORT)
#define ANEMOMETER_SENSOR_PIN_MASK   GPIO_PIN_MASK(ANEMOMETER_SENSOR_PIN)

void (*anemometer_int_callback)(uint16_t value);

static uint8_t enabled;

/*---------------------------------------------------------------------------*/
process_event_t anemometer_int_event;

/*---------------------------------------------------------------------------*/
static struct ctimer ct;
static struct timer debouncetimer;

typedef struct {
  uint16_t ticks;
  uint16_t value;
  uint8_t int_en;
  uint16_t int_thres;
} anemometer_sensors_t;

typedef struct {
  uint16_t value_max;
  uint64_t ticks_avg;
  uint64_t value_avg;
  uint32_t value_buf_xm;
  uint16_t value_avg_xm;
} anemometer_ext_t;

typedef struct {
  anemometer_sensors_t anemometer;
} anemometer_sensors;

static anemometer_sensors anemometer_sensors;
static anemometer_ext_t anemometer;

/*---------------------------------------------------------------------------*/
static void ct_callback(void *ptr)
{
	uint32_t wind_speed;

	/* Disable to make the calculations in an interrupt-safe context */
	  GPIO_DISABLE_INTERRUPT(ANEMOMETER_SENSOR_PORT_BASE,
	                         ANEMOMETER_SENSOR_PIN_MASK);
	  wind_speed = anemometer_sensors.anemometer.ticks;
	  wind_speed *= ANEMOMETER_SPEED_1S;
	  anemometer_sensors.anemometer.value = (uint16_t)wind_speed;
	  anemometer.ticks_avg++;
	  anemometer.value_avg += anemometer_sensors.anemometer.value;
	  anemometer.value_buf_xm += anemometer_sensors.anemometer.value;

	  /* Take maximum value */
	  if(anemometer_sensors.anemometer.value > anemometer.value_max) {
	    anemometer.value_max = anemometer_sensors.anemometer.value;
	  }

	  /* Calculate the 2 minute average */
	  if(!(anemometer.ticks_avg % ANEMOMETER_AVG_PERIOD)) {
	    PRINTF("\nWindspeed: calculate the %u averages ***\n", ANEMOMETER_AVG_PERIOD);

	    if(anemometer.value_buf_xm) {
	      anemometer.value_avg_xm = anemometer.value_buf_xm / ANEMOMETER_AVG_PERIOD;
	      anemometer.value_buf_xm = 0;
	    } else {
	      anemometer.value_avg_xm = 0;
	    }
	  }

	  /* Check for roll-over */
	  if(!anemometer.ticks_avg) {
	    anemometer.value_avg = 0;
	  }

	  anemometer_sensors.anemometer.ticks = 0;

	  /* Enable the interrupt again */
	  GPIO_ENABLE_INTERRUPT(ANEMOMETER_SENSOR_PORT_BASE,
	                        ANEMOMETER_SENSOR_PIN_MASK);

	  ctimer_set(&ct, CLOCK_SECOND, ct_callback, NULL);

}


/*---------------------------------------------------------------------------*/
PROCESS(anemometer_int_process, "Anemometer interrupt process handler");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(anemometer_int_process, ev, data)
{
  PROCESS_EXITHANDLER();
  PROCESS_BEGIN();

  while(1) {
    PROCESS_YIELD();

    if((ev == anemometer_int_event) && (anemometer_sensors.anemometer.int_en)) {
      if(anemometer_sensors.anemometer.ticks >=
         anemometer_sensors.anemometer.int_thres) {
        anemometer_int_callback(anemometer_sensors.anemometer.ticks);
      }
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

static void
anemometer_interrupt_handler(uint8_t port, uint8_t pin)
{
  uint32_t aux;

  /* Prevent bounce events */
  if(!timer_expired(&debouncetimer)) {
    return;
  }

  timer_set(&debouncetimer, DEBOUNCE_DURATION);

  /* We make a process_post() to check in the pollhandler any specific threshold
   * value
   */

  if((port == ANEMOMETER_SENSOR_PORT) && (pin == ANEMOMETER_SENSOR_PIN)) {
    anemometer_sensors.anemometer.ticks++;
    process_post(&anemometer_int_process, anemometer_int_event, NULL);
  }
}
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
static int
value(int type)
{
  uint64_t aux;

  if((type != WEATHER_METER_ANEMOMETER) &&
     (type != WEATHER_METER_RAIN_GAUGE) &&
     (type != WEATHER_METER_WIND_VANE) &&
     (type != WEATHER_METER_WIND_VANE_AVG_X) &&
     (type != WEATHER_METER_ANEMOMETER_AVG) &&
     (type != WEATHER_METER_ANEMOMETER_AVG_X) &&
     (type != WEATHER_METER_ANEMOMETER_MAX)) {
    PRINTF("Weather: requested an invalid sensor value\n");
    return WEATHER_METER_ERROR;
  }

  if(!enabled) {
    PRINTF("Weather: module is not configured\n");
    return WEATHER_METER_ERROR;
  }

  switch(type) {
  case WEATHER_METER_WIND_VANE:
    return weather_meter_get_wind_dir();

  case WEATHER_METER_WIND_VANE_AVG_X:
    return wind_vane.value_avg_xm;

  case WEATHER_METER_ANEMOMETER:
    return weather_sensors.anemometer.value;

  case WEATHER_METER_ANEMOMETER_AVG:
    if(anemometer.value_avg <= 0) {
      return (uint16_t)anemometer.value_avg;
    }
    aux = anemometer.value_avg / anemometer.ticks_avg;
    return (uint16_t)aux;

  case WEATHER_METER_ANEMOMETER_AVG_X:
    return anemometer.value_avg_xm;

  case WEATHER_METER_ANEMOMETER_MAX:
    return anemometer.value_max;

  /* as the default return type is int, we have a lower resolution if returning
   * the calculated value as it is truncated, an alternative is returning the
   * ticks and calculating on your own with WEATHER_METER_AUX_RAIN_MM
   */
  case WEATHER_METER_RAIN_GAUGE:
#if WEATHER_METER_RAIN_RETURN_TICKS
    return weather_sensors.rain_gauge.ticks;
#else
    return weather_sensors.rain_gauge.value;
#endif

  default:
    return WEATHER_METER_ERROR;
  }
}
/*---------------------------------------------------------------------------*/

