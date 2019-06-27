/*
   Wireless Sensor Networks Laboratory

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
   http://www.lkn.ei.tum.de

   copyright (c) 2018 Chair of Communication Networks, TUM

   contributors:
   * Karthik Sukumar
   * Johannes Machleid

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 2.0 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

#define UNINIT      0xFFFF
#define UNINITCOST  0xFF

// custom structures
typedef struct r_table
{
	linkaddr_t dest[TOTAL_NODES];			// Destination id. Every node should be able to reach every other node plus itself. Thus total entries are equal to total number of nodes.
	linkaddr_t next_hop[TOTAL_NODES];		// Next hop in route to destination.
    uint8_t cost[TOTAL_NODES]; 			    // Number of total hops of the packet route. Maximum 10.
}r_table_t;

typedef struct {
	linkaddr_t dest;
	broadcastMsg_t message;         // Broadcast Packet with destination
}broadcastPacket_t;

static r_table_t rTable;

void initNetworkDisc(void);
int broadcast_recv(void);
int broadcast_send(void);
int unicast_recv(void);
int unicast_send(void);

#endif /* ROUTING_H */
