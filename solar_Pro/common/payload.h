
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

#ifndef PAYLOAD_H
#define PAYLOAD_H

#include "contiki.h"

#include "routing.h"
#include "nodeID.h"

#define BROADCASTMSGSIZE_BYTES  7

typedef enum {
    DISCOVERY = 1,
    EMERGENCY = 2,
    PREPDISC = 7,
    ACK = 3,
    UNICAST = 4,
    PATH = 5,
    MANUAL = 6,
}pkttype_t;

// custom structures
typedef struct {
	linkaddr_t dest[TOTAL_NODES];			// Destination id. Every node should be able to reach every other node plus itself. Thus total entries are equal to total number of nodes.
	linkaddr_t next_hop[TOTAL_NODES];		// Next hop in route to destination.
    uint8_t cost[TOTAL_NODES]; 			    // Number of total hops of the packet route. Maximum 10.
}__attribute__ ((packed)) r_table_t;

typedef struct {
    pkttype_t upkt;
    node_num_t destNode, originNode;
    uint16_t temp_mC;
    uint16_t battVolt_mV;
    uint16_t lightSensor;
    uint16_t servoPos_degs;
}__attribute__ ((packed)) unicastMsg_t;

typedef struct {
    pkttype_t bpkt;
    char msg[BROADCASTMSGSIZE_BYTES];
    r_table_t rTable;
}__attribute__((packed)) broadcastMsg_t;

typedef struct {
    pkttype_t apkt;
    linkaddr_t dest;
    linkaddr_t hopHist[TOTAL_NODES];
}__attribute__((packed)) ackModeMsg_t;

typedef union {
    unicastMsg_t u;
    broadcastMsg_t b;
    ackModeMsg_t a;
}__attribute__((packed)) payload_t;

#endif
