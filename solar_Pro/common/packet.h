
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

#ifndef PACKET_H
#define PACKET_H

#include "nodeid.h"
#include "contiki.h"

typedef struct packet {

    node_num_t nodeNum;
    uint16_t temp_mv;
    uint16_t battVolt_mV;
    uint16_t lightSensor;
}packet_t

#endif
