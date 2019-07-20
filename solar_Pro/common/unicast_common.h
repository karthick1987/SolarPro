
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

#ifndef UNICASTCOMMON_H
#define UNICASTCOMMON_H

#include "routing.h"
#include "payload.h"

#define RESETADDR   UNINIT

void printPacket(payload_t *p);
void zeroOut(payload_t *p, pkttype_t type);
void addSelfToHist(payload_t *payload);

#endif

