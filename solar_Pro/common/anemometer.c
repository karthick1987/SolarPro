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
 * @file anemometer.c
 * @author Karthik Sukumar & Johannes Machleid
 * @brief Functions to read out the windspeed sensor (anemometer) via GPIO.
 *
 * The anemometer function is based on the zolertia zoul example "test-weathermeter-example.c"
 * which comes with the contiki installation in contiki/examples/zolertia/zoul and the
 * "weather-meter.c" file which can be found in contiki/platform/zoul/dev.
 * The anemometer can be considered as a electrical switch opening and closing
 * at a frequency depending on the rotation speed of the anemometer. It sets the connected
 * GPIO Port to HIGH twice a rotation which is recognized by an interrupt handler.
 * These interrupts are counted over a certain amount of time to calculate the actual
 * windspeed in km/h.
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

/**
* @brief struct anemometer_sensors_t includes the values needed for the actual wind speed calculations
*
*/
typedef struct {
  uint16_t ticks; /**< basically a counter to count the GPIO HIGH interrupts */
  uint16_t value; /**< contains the actual wind speed value*/
  uint8_t int_en; /**< logical value if the anemometer is enabled (=1) or not (=0) */
  uint16_t int_thres; /**< threshold wind speed where an error message is sent*/
} anemometer_sensors_t;

/**
* @brief struct anemometer_ext_t includes the values needed to store the maximum wind speed and
* calculate the average wind speed over lifetime and a certain amount of time (default is 120s).
*/
typedef struct {
  uint16_t value_max; /**< the maximum wind speed is saved here. */
  uint64_t ticks_avg; /**< average ticks to calculate average wind speed over lifetime. */
  uint64_t value_avg; /**< contains the average wind speed value over lifetime. */
  uint32_t value_buf_xm; /**< value buffer to calculate the average wind speed over x minutes (2min default). */
  uint16_t value_avg_xm; /**< average wind speed value over x minutes (2min default). */
} anemometer_ext_t;

typedef struct {
  anemometer_sensors_t anemometer;
} anemometer_sensors;

static anemometer_sensors anemo_sensor;
static anemometer_ext_t anemo;

/*---------------------------------------------------------------------------*/
/**
* @brief Callback function that counts the ticks of the anemometer based on an interrupt
* produced by a HIGH input of the respective GPIO (can be set in anemometer.h)
*
*
*/
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

/**
* @brief Anemometer interrupt handler with debounce timer to prevent bounce events
*
*/
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
/**
* @brief return function to deliver the requested anemometer values.
*
* @param type Returns the requested wind speed value, as shown in the table.
* ---------------------+----------------------------------
 *      type                   return value
 *   ANEMOMETER           wind speed *
 *   ANEMOMETER_AVG       average wind speed *
 *   ANEMOMETER_AVG_X     average wind speed (2min) *
 *   ANEMOMETER_MAX       maximum wind speed *
 * --------------------+----------------------------------
*
* @return int: if type is not known: ANEMOMETER_ERROR (-1) or if type is known the requested value.
*/
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
/**
* @brief configure function to enable or disable the anemometer and setup a threshold
* for an error message
*
* @param type Parameter to configure ANEMOMETER_ACTIVE (enable or disable anemometer),
* ANEMOMETER_INT_OVER (to set a threshold), ANEMOMETER_INT_DIS (to disable interrupt).
*
* @param value Parameter to configure the respective type.
*
* @return int: ANEMOMETER_ERROR (-1) or ANEMOMETER_SUCCESS (0)
*/
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
