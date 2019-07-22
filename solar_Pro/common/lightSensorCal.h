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
 *      The calibration values of each individual sensor are needed to calculate
 *      the the lux value. This file is not used in this project.
 */

#ifndef LIGHTSENSORCAL_H
#define LIGHTSENSORCAL_H

/**
* @brief struct alightSensorCal includes the calibration values needed to
* calculate the lux value of the light sensors.
*/
typedef struct lightSensorCal{
    float mVal,bVal;
}lightSensorCal_t;

#endif
