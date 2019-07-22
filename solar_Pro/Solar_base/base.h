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
 * @file base.h
 * @author Karthik Sukumar & Johannes Machleid
 * @brief Main functions of the basestation with a state machine thread
 *
 */

#ifndef BASE_H
#define BASE_H

#include "contiki.h"
#include "core/net/linkaddr.h"

/**
 * \name definition of the base station state machine operation modes
 * @{
 */
enum state_t {
PREPNETDISC = 45,
IDLE        = 46,
INITNETWORKDISC ,
PATHMODE         ,
UNICASTMODE     ,
EMERGENCYSTATE  ,
};
/** @} */
#endif
