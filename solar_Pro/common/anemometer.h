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

 This header file is designed for all nodes to read out wind speed sensor values.
 */

#ifndef ANEMOMETER_H
#define ANEMOMETER_H

#include "lib/sensors.h"
#include "dev/zoul-sensors.h"

/**
 * \name anemometer sensor return and operation values
 * @{
 */
#define ANEMOMETER			0x01
#define ANEMOMETER_AVG		0x02
#define ANEMOMETER_AVG_X	0x03
#define ANEMOMETER_MAX		0x04

/* Period (seconds) to calculate an average */
#ifdef ANEMOMETER_CONF_AVG_PERIOD
#define ANEMOMETER_AVG_PERIOD          ANEMOMETER_CONF_AVG_PERIOD
#else
#define ANEMOMETER_AVG_PERIOD          120
#endif

#define ANEMOMETER_ACTIVE	SENSORS_ACTIVE
#define ANEMOMETER_INT_OVER HW_INT_OVER_THRS
#define ANEMOMETER_INT_DIS  HW_INT_DISABLE

#define ANEMOMETER_SUCCESS              0
#define ANEMOMETER_ERROR                (-1)

/* 2.4Km/h per tick, 2 per rotation */
#define ANEMOMETER_SPEED_1S  (1200)


/* -------------------------------------------------------------------------- */
/**
 * \name Anemometer interrupt callback macro
 * @{
 */
#define REGISTER_ANEMOMETER_INT(ptr) anemometer_int_callback = ptr;
extern void (*anemometer_int_callback)(uint16_t value);
/** @} */
/* -------------------------------------------------------------------------- */

/**
 * \name anemometer default pin, port and interrupt vector
 * @{
 */
#ifdef CONF_ANEMOMETER_PIN
#define ANEMOMETER_SENSOR_PIN   CONF_ANEMOMETER_PIN
#else
#define ANEMOMETER_SENSOR_PIN   2
#endif
#ifdef CONF_ANEMOMETER_PORT
#define ANEMOMETER_SENSOR_PORT  CONF_ANEMOMETER_PORT
#else
#define ANEMOMETER_SENSOR_PORT  GPIO_A_NUM
#endif
#ifdef CONF_ANEMOMETER_VECTOR
#define ANEMOMETER_SENSOR_VECTOR  CONF_ANEMOMETER_VECTOR
#else
#define ANEMOMETER_SENSOR_VECTOR  GPIO_D_IRQn
#endif
/** @} */

/* -------------------------------------------------------------------------- */
#define ANEMOMETER_SENSOR "Amazon Anemometer"
/* -------------------------------------------------------------------------- */
extern const struct sensors_sensor anemometer;
/* -------------------------------------------------------------------------- */


#endif /* ifndef ANEMOMETER_H_ */
/**
 * @}
 */
