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

 This header file is designed for all nodes to read out several sensor values.
 */

#ifndef PROJSENSORS_H
#define PROJSENSORS_H

#include "contiki.h"

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define IDLE 5

#define LIGHTSENSOR_VREF 5

/** \brief Gets the light sensor value in 12 bit format
 *
 * \return \c 12-bit Light sensor value as an unsigned integer
 */
uint16_t getLightSensorValue(void);

//Todo

int getJoystickPosition(void);

/** \brief Gets the internal Temperatute from the Board
 *
 * \return \c Returns the temperature from the board in mC
 */
int getInternalTemperature(void);

/** \brief Gets the Battery voltage in mV
 *
 * \return \c Returns the battery voltage in mV
 */
uint16_t getBatteryVoltage(void);

#endif
