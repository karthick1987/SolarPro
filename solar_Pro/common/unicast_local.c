
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
#include <stdio.h>
#include "unicast_local.h"
#include "base.h"

static void printPacket(payload_t *p);
static void zeroOut(payload_t *p, pkttype_t type)
{
    int i = 0;
    switch(type)
    {
        case ACK:
            for(i=0;i<TOTAL_NODES;i++)
            {
                (p->a).hopHist[i].u16 = 0xFFFF;
            }
            break;
        case UNICAST:
            p->u.temp_mC = 0;
            p->u.battVolt_mV = 0;
            p->u.lightSensor = 0;
            p->u.servoPos_degs = 0;
            break;
        default:
            break;
    }
}
static void setupPacket(payload_t *p, pkttype_t type, node_num_t n)
{
    switch(type)
    {
        case ACK:
            (p->a).apkt = ACK;
            (p->a).dest.u16 = getRIMEID(n);
            zeroOut(p, type);
            printPacket(p);
            break;
        case UNICAST:
            (p->u).upkt = UNICAST;
            (p->u).originNode = getMyNodeID(); // Base Station
            (p->u).destNode = n; // The destination Node
            zeroOut(p, type);
            printPacket(p);
            break;
        default:
            break;
    }
}

static void printPacket(payload_t *p)
{
    int i;
    switch(p->a.apkt)
    {
        case ACK:
            printf("---ACK PKT---\nDestination Node is %x\n",(p->a).dest.u16);
            for(i=0;i<TOTAL_NODES;i++)
            {
               printf("%d:%d\n",i,(p->a).hopHist[i].u16);
            }
            break;
        case UNICAST:
            printf("---UNICAST PKT---\n");
            printf("Origin: %d, Destination: %d\n",p->u.originNode,p->u.destNode);
            printf("Temp_mC: %d\n",p->u.temp_mC);
            printf("Batt_mV: %d\n",p->u.battVolt_mV);
            printf("LightSensor: %d\n",p->u.lightSensor);
            printf("servoPos_Degs: %d\n",p->u.servoPos_degs);
            break;
    }
}

int doAckMode(node_num_t n, payload_t *p)
{
    setupPacket(p,ACK,n);
    return (n == TOTAL_NODES?0:-1);
}

int doUniCastMode(node_num_t n, payload_t *p)
{
    setupPacket(p,UNICAST,n);
    return (n == TOTAL_NODES?0:-1);
}

