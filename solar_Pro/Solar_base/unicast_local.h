
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

 This header file is designed for all nodes to define specific Node IDs depending on the RIME ID.
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

#endif
