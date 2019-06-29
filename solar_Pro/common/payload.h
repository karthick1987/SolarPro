
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

#include "nodeID.h"
#include "contiki.h"

#define BROADCASTMSGSIZE_BYTES  100

typedef enum {
    DISCOVERY,
    EMERGENCY,
    ACK
}pkttype_t;

typedef struct unicastMsg {
    uint16_t temp_mC;
    uint16_t battVolt_mV;
    uint16_t lightSensor;
    uint16_t servoPos_degs;
}unicastMsg_t;

typedef struct broadcastMsg {
    char msg[BROADCASTMSGSIZE_BYTES];
}broadcastMsg_t;

union commType {
    pkttype_t bpkt;
    unicastMsg_t u;
    broadcastMsg_t b;
};

typedef struct payload {
    node_num_t destNode, originNode;
    union commType cType;
}payload_t

#endif
