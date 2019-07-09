
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

#ifndef UNICASTLOCAL_H
#define UNICASTLOCAL_H

#include "nodeID.h"

enum unicast_state {
UNICASTMODE    = 1,
STARTBROADCAST ,
STOPUNICAST    ,
STOPBROADCAST  ,
ACKMODE        ,
STARTEMERGENCY ,
STOPEMERGENCY  ,
};

int doAckMode(node_num_t n);
int doUniCastMode(node_num_t n);

#endif
