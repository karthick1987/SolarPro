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


uint16_t getLightSensorValue(void);
int getJoystickPosition(void);
int getInternalTemperature(void);
int getBatteryVoltage(void);

#endif
