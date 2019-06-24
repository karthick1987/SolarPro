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
#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
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

static anemometer_sensors anemo_sensor;
static anemometer_ext_t anemo;

/*---------------------------------------------------------------------------*/
static void ct_callback(void *ptr)
{
	uint32_t wind_speed;

	/* Disable to make the calculations in an interrupt-safe context */
	  GPIO_DISABLE_INTERRUPT(ANEMOMETER_SENSOR_PORT_BASE,
	                         ANEMOMETER_SENSOR_PIN_MASK);
	  wind_speed = anemo_sensor.anemometer.ticks;
	  wind_speed *= ANEMOMETER_SPEED_1S;
	  anemo_sensor.anemometer.value = (uint16_t)wind_speed;
	  anemo.ticks_avg++;
	  anemo.value_avg += anemo_sensor.anemometer.value;
	  anemo.value_buf_xm += anemo_sensor.anemometer.value;

	  /* Take maximum value */
	  if(anemo_sensor.anemometer.value > anemo.value_max) {
	    anemo.value_max = anemo_sensor.anemometer.value;
	  }

	  /* Calculate the 2 minute average */
	  if(!(anemo.ticks_avg % ANEMOMETER_AVG_PERIOD)) {
	    PRINTF("\nWindspeed: calculate the %u averages ***\n", ANEMOMETER_AVG_PERIOD);

	    if(anemo.value_buf_xm) {
	      anemo.value_avg_xm = anemo.value_buf_xm / ANEMOMETER_AVG_PERIOD;
	      anemo.value_buf_xm = 0;
	    } else {
	      anemo.value_avg_xm = 0;
	    }
	  }

	  /* Check for roll-over */
	  if(!anemo.ticks_avg) {
	    anemo.value_avg = 0;
	  }

	  anemo_sensor.anemometer.ticks = 0;

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

    if((ev == anemometer_int_event) && (anemo_sensor.anemometer.int_en)) {
      if(anemo_sensor.anemometer.ticks >=
         anemo_sensor.anemometer.int_thres) {
        anemometer_int_callback(anemo_sensor.anemometer.ticks);
      }
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

static void
anemometer_interrupt_handler(uint8_t port, uint8_t pin)
{

  /* Prevent bounce events */
  if(!timer_expired(&debouncetimer)) {
    return;
  }

  timer_set(&debouncetimer, DEBOUNCE_DURATION);

  /* We make a process_post() to check in the pollhandler any specific threshold
   * value
   */

  if((port == ANEMOMETER_SENSOR_PORT) && (pin == ANEMOMETER_SENSOR_PIN)) {
    anemo_sensor.anemometer.ticks++;
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
    return anemo_sensor.anemometer.value;

  case ANEMOMETER_AVG:
    if(anemo.value_avg <= 0) {
      return (uint16_t)anemo.value_avg;
    }
    aux = anemo.value_avg / anemo.ticks_avg;
    return (uint16_t)aux;

  case ANEMOMETER_AVG_X:
    return anemo.value_avg_xm;

  case ANEMOMETER_MAX:
    return anemo.value_max;

  default:
    return ANEMOMETER_ERROR;
  }
}
/*---------------------------------------------------------------------------*/
static int configure(int type, int value)
{
  if((type != ANEMOMETER_ACTIVE) &&
     (type != ANEMOMETER_INT_OVER) &&
     (type != ANEMOMETER_INT_DIS)) {
    PRINTF("Anemometer: invalid configuration option\n");
    return ANEMOMETER_ERROR;
  }

  if(type == ANEMOMETER_ACTIVE) {

    anemo.value_avg = 0;
    anemo.ticks_avg = 0;

    anemo_sensor.anemometer.int_en = 0;
    anemo_sensor.anemometer.ticks = 0;
    anemo_sensor.anemometer.value = 0;

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
    anemo_sensor.anemometer.int_en = 1;
    anemo_sensor.anemometer.int_thres = value;
    PRINTF("Anemometer: anemometer threshold %u\n", value);
    break;
  case ANEMOMETER_INT_DIS:
    PRINTF("Anemometer: anemometer int disabled\n");
    anemo_sensor.anemometer.int_en = 0;
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


