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
 * @file routing.h
 * @author Karthik Sukumar & Johannes Machleid
 * @brief functions to handle the routing within the network
 *
 */

#ifndef ROUTING_H
#define ROUTING_H

#include<stdbool.h>

#include "core/net/linkaddr.h"
#include "sys/process.h"

// Private includes
#include "nodeID.h"
#include "payload.h"

/**
 * \name values to fill routing table's next hop and cost column
 * @{
 */
#define UNINIT      0xFFFF
#define UNINITCOST  0xFF
/** @} */

void setUpRtable(void);
bool isValidNextHop(node_num_t node);
void initNetworkDisc(void);
void prepNetworkDisc(void);
void initEmergency(void);
void openBroadcast(void);
void openUnicast(void);
void doBroadCast(void);
void bdct_recv(struct broadcast_conn *c, const linkaddr_t *from);
void bdct_send(struct broadcast_conn *c, const linkaddr_t *to);
void unict_recv(struct unicast_conn *c, const linkaddr_t *from);
void unict_send(payload_t *tx_packet);

#endif /* ROUTING_H */
