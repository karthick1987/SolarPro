
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
#include "servoControl.h"
#include "projSensors.h"
#include "unicast_local.h"

#define RESETADDR   0xFFFF

static payload_t tx_packet;
static void printPacket(payload_t *p);
static void zeroOut(payload_t *p, pkttype_t type)
{
    int i = 0;
    switch(type)
    {
        case PATH:
            for(i=0;i<TOTAL_NODES;i++)
            {
                (p->a).hopHist[i].u16 = RESETADDR;
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
static void setupPacket(payload_t *p, pkttype_t type, node_num_t dest)
{
    switch(type)
    {
        case PATH:
            (p->a).apkt = PATH;
            (p->a).dest.u16 = getRIMEID(dest);
            zeroOut(p, type);
            printPacket(p);
            break;
        case UNICAST:
            (p->u).upkt = UNICAST;
            (p->u).originNode = getMyNodeID(); // Base Station
            (p->u).destNode = dest; // The destination Node
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
        case PATH:
            printf("---PATH PKT---\nDestination Node is %x\n",(p->a).dest.u16);
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
        default:
            break;
    }
}

int doPathMode(node_num_t dest, payload_t *p)
{
    setupPacket(p,PATH,dest);
    return (dest == TOTAL_NODES?0:-1);
}

static void addSelfToHist(payload_t *rx_packet)
{
    int i = 0;
    for (i=0;i<TOTAL_NODES;i++)
    {
        if(rx_packet->a.hopHist[i].u16 == RESETADDR)
            break;
    }
    if(i >= 8)
        return;
    else
        rx_packet->a.hopHist[i].u16 = getMyRIMEID()->u16;
    return;
}


int doUniCast(payload_t *rx_packet)
{
    //check if destination byte 2
    //read out byte 1 if PATH or UNICAST
    switch(rx_packet->a.apkt)
    {
        case ACK:
            // always forward to basestation
            unict_send(*rx_packet);
            break;

        case UNICAST:
            //check if Destination
            if(rx_packet->u.destNode == getMyNodeID()){
                // setup packet with sensor values and new destination (basestation)
                tx_packet.u.upkt = ACK;
                tx_packet.u.destNode = rx_packet->u.originNode;
                tx_packet.u.temp_mC = getInternalTemperature();
                tx_packet.u.battVolt_mV = getBatteryVoltage();
                tx_packet.u.lightSensor = getLightSensorValue();
                tx_packet.u.servoPos_degs = getServoPosition();

                // send back unicast back to basestation
                unict_send(tx_packet);
            }
            else
            {
                unict_send(*rx_packet);
            }
            break;

        case PATH:
            //check if Destination
            if(rx_packet->a.dest.u16 == getMyRIMEID()->u16){
                // send unicast with PATH header and hophistory back to base station
                setupPacket(&tx_packet, PATH, returnIDIndex(&(rx_packet->a.dest)) );
                tx_packet.a.apkt = PATH;
                tx_packet.a.dest.u16 = rx_packet->a.hopHist[0].u16;
                tx_packet.a.hopHist[0].u16 = getMyRIMEID()->u16;
                // Lets Keep it Simple
                // What happends here is that if the destination has been reached
                // a new transmit packet is created and it zeroed out 
                // with the 1st entry of the hopHist as the nodes RIMEID and send it across
            }
            else{
                //add own Node ID to the first entry of hophistory with 0xFFFF
                addSelfToHist(rx_packet);
                //forward message to destination
                unict_send(*rx_packet);
            }
            break;

        default:
            break;

    }
    //return (n == TOTAL_NODES?0:-1);
    return 0;
}
