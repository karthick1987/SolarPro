
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

#ifndef UNICASTPANEL_H
#define UNICASTPANEL_H

#include "nodeID.h"
#include "routing.h"

int doPathMode(node_num_t dest, payload_t *);
int doUniCastMode(node_num_t dest, payload_t *);

#endif
