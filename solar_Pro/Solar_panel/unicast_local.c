
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
#include "dev/leds.h"		// Use LEDs.

#define RESETADDR   0xFFFF

// Local global variable static to this file
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
static void setupPacket(payload_t *rx_packet) // , pkttype_t type, node_num_t dest)
{
    //tx_packet is a static global here
    payload_t *p = &tx_packet;

    switch(rx_packet->a.apkt)
    {
        case PATH:
            (p->a).apkt = ACK;
            (p->a).dest.u16 = (rx_packet->a).hopHist[0].u16;
            int i = 0;
            for(i=0;i<TOTAL_NODES;i++)
            {
                p->a.hopHist[i] = rx_packet->a.hopHist[i];
            }
            printPacket(p);
            break;
        case UNICAST:
            zeroOut(p, UNICAST);
            (p->u).upkt = ACK;
            (p->u).originNode = getMyNodeID(); // My Node ID
            (p->u).destNode = rx_packet->u.originNode; // The destination Node

            // Set up the return packet
            p->u.temp_mC = getInternalTemperature();
            p->u.battVolt_mV = getBatteryVoltage();
            p->u.lightSensor = getLightSensorValue();
            p->u.servoPos_degs = getServoPosition();
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

static void addSelfToHist(payload_t *payload)
{
    int i = 0;
    for (i=0;i<TOTAL_NODES;i++)
    {
        if(payload->a.hopHist[i].u16 == RESETADDR)
            break;
    }
    if(i >= 8)
        return;
    else
        payload->a.hopHist[i].u16 = getMyRIMEID()->u16;
    return;
}

int doUniCastMode(node_num_t dest, payload_t *rx_packet)
{
    leds_on(LEDS_YELLOW);
    // dest is not used here for the solar Panel
    //check if destination byte 2
    (void) dest;
    //read out byte 1 if PATH or UNICAST
    switch(rx_packet->a.apkt)
    {
        case ACK:
            // always forward to destination(BASE STATION)
            unict_send(rx_packet);
            break;

        case UNICAST:
            //check if Destination
            if(rx_packet->u.destNode == getMyNodeID()){
                // setup packet with sensor values and new destination (basestation)
                setupPacket(rx_packet);
                // send back unicast back to basestation
                unict_send(&tx_packet);
            }
            else
            {
                // If not at destination then forward
                unict_send(rx_packet);
            }
            break;

        case PATH:
            //check if Destination
            if(rx_packet->a.dest.u16 == getMyRIMEID()->u16){
                // send unicast with PATH header and hophistory back to base station
                setupPacket(rx_packet);
                // Send packet
                unict_send(&tx_packet);
            }
            else{
                memcpy(&tx_packet,rx_packet,sizeof(payload_t));
                //add own Node ID to the first entry of hophistory with 0xFFFF
                addSelfToHist(&tx_packet);
                //forward message to destination
                unict_send(&tx_packet);
            }
            break;

        default:
            break;

    }
    leds_off(LEDS_YELLOW);
    return 0;
}
