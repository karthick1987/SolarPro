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
 * @file unicast_common.h
 * @author Karthik Sukumar & Johannes Machleid
 * @brief functions to handle unicast communication
 *
 */

#ifndef UNICASTCOMMON_H
#define UNICASTCOMMON_H

#include "routing.h"
#include "payload.h"

/**
 * \name define the default RIME ID and the unicast channel
 * @{
 */
#define RESETADDR   UNINIT
#define UNICASTCHANNEL          146
/** @} */

void printPacket(payload_t *p);
void zeroOut(payload_t *p, pkttype_t type);
void addSelfToHist(payload_t *payload);

#endif
