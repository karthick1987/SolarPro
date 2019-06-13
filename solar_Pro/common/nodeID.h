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

#ifndef NODEID_H
#define NODEID_H

#include "contiki.h"
#include "core/net/linkaddr.h"

typedef uint16_t node_num_t;
typedef struct nodeID{

    node_num_t nodeID;
    uint16_t serialNumber;
    uint16_t rimeID;
}nodeID_t;

void print_node_IDs( void );
node_num_t getMyNodeID( linkaddr_t l );
#endif
