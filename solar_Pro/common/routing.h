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
#include "sys/process.h"

// Private includes
#include "nodeID.h"
#include "payload.h"

#define UNINIT      0xFFFF
#define UNINITCOST  0xFF

void setUpRtable(void);
void initNetworkDisc(struct process *p);
void openBroadcast(void);
void bdct_recv(struct broadcast_conn *c, const linkaddr_t *from);
void bdct_send(struct broadcast_conn *c, const linkaddr_t *to);
void unict_recv(struct unicast_conn *c, const linkaddr_t *from);
void unict_send(payload_t tx_packet);

#endif /* ROUTING_H */
