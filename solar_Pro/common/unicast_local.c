
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

 This c-file is specifically designed for the base node.
 */

// Contiki-specific includes:
#include "unicast_local.h"

static void setupAckModePacket(payload_t *p)
{
    (p->a).apkt = ACK;
}

int doAckMode(node_num_t n, payload_t *p)
{
    setupAckModePacket(p);
    return (n == TOTAL_NODES?0:-1);
}

int doUniCastMode(node_num_t n)
{
    return (n == TOTAL_NODES?0:-1);
}
