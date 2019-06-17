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

#ifndef SENSORS_H
#define SENSORS_H

#include "contiki.h"

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define IDLE 5

static int getLightSensorValue(uint16_t adc_value);
const char * getJoystickPosition(uint16_t x, uint16_t y);

#endif
