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
static int value(int type)
{
  uint64_t aux;

  if((type != ANEMOMETER) &&
     (type != ANEMOMETER_AVG) &&
     (type != ANEMOMETER_AVG_X) &&
     (type != ANEMOMETER_MAX)) {
    PRINTF("Anemometer: requested an invalid sensor value\n");
    return ANEMOMETER_ERROR;
  }

  if(!enabled) {
    PRINTF("Anemometer: module is not configured\n");
    return ANEMOMETER_ERROR;
  }

  switch(type) {

  case ANEMOMETER:
    return anemometer_sensors.anemometer.value;

  case ANEMOMETER_AVG:
    if(anemometer.value_avg <= 0) {
      return (uint16_t)anemometer.value_avg;
    }
    aux = anemometer.value_avg / anemometer.ticks_avg;
    return (uint16_t)aux;

  case ANEMOMETER_AVG_X:
    return anemometer.value_avg_xm;

  case ANEMOMETER_MAX:
    return anemometer.value_max;

  default:
    return ANEMOMETER_ERROR;
  }
}
/*---------------------------------------------------------------------------*/
static int configure(int type, int value)
{
  if((type != ANEMOMETER_ACTIVE) &&
     (type != ANEMOMETER_INT_OVER) &&
     (type != NEMOMETER_INT_DIS) &&
    PRINTF("Anemometer: invalid configuration option\n");
    return ANEMOMETER_ERROR;
  }

  if(type == ANEMOMETER_ACTIVE) {

    anemometer.value_avg = 0;
    anemometer.ticks_avg = 0;

    anemometer_sensors.anemometer.int_en = 0;
    anemometer_sensors.anemometer.ticks = 0;
    anemometer_sensors.anemometer.value = 0;

    if(!value) {
      anemometer_int_callback = NULL;
      GPIO_DISABLE_INTERRUPT(ANEMOMETER_SENSOR_PORT_BASE,
                             ANEMOMETER_SENSOR_PIN_MASK);
      process_exit(&anemometer_int_process);
      enabled = 0;
      PRINTF("Anemometer: disabled\n");
      return ANEMOMETER_SUCCESS;
    }

    /* Configure anemometer interruption */
    GPIO_SOFTWARE_CONTROL(ANEMOMETER_SENSOR_PORT_BASE, ANEMOMETER_SENSOR_PIN_MASK);
    GPIO_SET_INPUT(ANEMOMETER_SENSOR_PORT_BASE, ANEMOMETER_SENSOR_PIN_MASK);
    GPIO_DETECT_RISING(ANEMOMETER_SENSOR_PORT_BASE, ANEMOMETER_SENSOR_PIN_MASK);
    GPIO_TRIGGER_SINGLE_EDGE(ANEMOMETER_SENSOR_PORT_BASE,
                             ANEMOMETER_SENSOR_PIN_MASK);
    ioc_set_over(ANEMOMETER_SENSOR_PORT, ANEMOMETER_SENSOR_PIN, IOC_OVERRIDE_DIS);
    gpio_register_callback(anemometer_interrupt_handler, ANEMOMETER_SENSOR_PORT,
                           ANEMOMETER_SENSOR_PIN);

    process_start(&anemometer_int_process, NULL);

    ctimer_set(&ct, CLOCK_SECOND, ct_callback, NULL);

    GPIO_ENABLE_INTERRUPT(ANEMOMETER_SENSOR_PORT_BASE, ANEMOMETER_SENSOR_PIN_MASK);
    NVIC_EnableIRQ(ANEMOMETER_SENSOR_VECTOR);

    enabled = 1;
    PRINTF("Anemometer: started\n");
    return ANEMOMETER_SUCCESS;
  }

  switch(type) {
  case ANEMOMETER_INT_OVER:
    anemometer_sensors.anemometer.int_en = 1;
    anemometer_sensors.anemometer.int_thres = value;
    PRINTF("Anemometer: anemometer threshold %u\n", value);
    break;
  case ANEMOMETER_INT_DIS:
    PRINTF("Anemometer: anemometer int disabled\n");
    anemometer_sensors.anemometer.int_en = 0;
    break;
  default:
    return ANEMOMETER_ERROR;
  }

  return ANEMOMETER_SUCCESS;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(anemometer, ANEMOMETER_SENSOR, value, configure, NULL);
/*---------------------------------------------------------------------------*/
/** @} */


