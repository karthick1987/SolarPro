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

 This header file is specifically designed for the maintenance node.
 */


#ifndef MAINTENANCE_H
#define MAINTENANCE_H

#include "core/net/linkaddr.h"
#include "nodeID.h"

/**
* @brief struct to maintain a list of neighbor nodes for maintenance worker to control
* the node with the smallest RSSI manually via unicast.
*
*/
typedef struct neighbor_table{
  linkaddr_t dest[TOTAL_NODES];
  int16_t rssi;
}neighbor_table_t;


#endif /* MAINTENANCE_H */
