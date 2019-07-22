/*
   Wireless Sensor Networks Laboratory 2019 -- Group 1

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
http://www.lkn.ei.tum.de

copyright (c) 2019 Chair of Communication Networks, TUM

project: SolarPro

 /**
 * @file lightSensorCal.c
 * @author Karthik Sukumar & Johannes Machleid
 * @brief Contains the calibration values for light sensors.
 *
 * The calibration values of each individual sensor are needed to calculate
 * the the lux value. This file is not used in this project.
 */

// Std file includes
#include <stdio.h>

// Private includes
#include "lightSensorCal.h"

/**
* @brief struct alightSensorCal includes the calibration values needed to
* calculate the lux value of the light sensors.
*/
static const lightSensorCal_t lsc[] =
{
    {1.55503, 37.08},
    {1.6683, 41.415},
    {1.6542, 39.749},
    {1.4761, 39.416},
    {1.464, 39.984},
};
