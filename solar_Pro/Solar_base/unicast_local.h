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
 * @file unicast_local.h
 * @author Karthik Sukumar & Johannes Machleid
 * @brief Sets the unicast transmission interval and maximum amounts of retransmissions
 *
 */

#ifndef UNICASTLOC_H
#define UNICASTLOC_H

#include "nodeID.h"
#include "payload.h"
#include "uart_local.h"

#define UNICASTINTERVAL         2*CLOCK_SECOND
#define UNICASTMAXRETRANSMIT    5

int processUniCast(node_num_t dest, payload_t *rx_packet);
void initUnicastMode(void);
void initPathMode(void);
void stopAllUnicastTimers(void);

#endif
