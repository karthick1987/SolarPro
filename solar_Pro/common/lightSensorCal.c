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

 This c-file is designed for all nodes to set specific light sensor calibration values.
 */

// Std file includes
#include <stdio.h>

// Private includes
#include "lightSensorCal.h"

static const lightSensorCal_t lsc[] = 
{
    {1.55503, 37.08},
    {1.6683, 41.415},
    {1.6542, 39.749},
    {1.4761, 39.416},
    {1.464, 39.984},
};

