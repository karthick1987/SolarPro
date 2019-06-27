/*
   Wireless Sensor Networks Laboratory

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
   http://www.lkn.ei.tum.de

   copyright (c) 2018 Chair of Communication Networks, TUM

   contributors:
   * Karthik Sukumar
   * Johannes Machleid
*/

#ifndef ROUTING_H
#define ROUTING_H

#include "core/net/linkaddr.h"
#include "payload.h"

// Standard C includes:
#include <stdint.h>

// the number of nodes present in the network
#ifndef TOTAL_NODES
#define TOTAL_NODES 7
#endif

#define UNINIT  0xFF

// custom structures
typedef struct l_table
{
	linkaddr_t dest[TOTAL_NODES];			// Destination id. Every node should be able to reach every other node plus itself. Thus total entries are equal to total number of nodes.
	linkaddr_t next_hop[TOTAL_NODES];		// Next hop in route to destination.
    uint8_t cost[TOTAL_NODES]; 			    // Number of total hops of the packet route. Maximum 10.
}l_table_t;

typedef struct {
	linkaddr_t dest;
	broadcastMsg_t message;         // Broadcast Packet with destination
}broadcastPacket_t;

#endif /* ROUTING_H */
