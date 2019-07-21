
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


#include "sys/clock.h"
#include "dev/leds.h"          // Use LEDs.

#include "servoControl.h"
#include "projSensors.h"
#include "unicast_local.h"
#include "unicast_common.h"
#include "broadcast_common.h"

// Local global variable static to this file
static payload_t tx_packet;

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
            break;
        case UNICAST:
            zeroOut(p, UNICAST);
            (p->u).upkt = ACK;
            (p->u).originNode = getMyNodeID(); // My Node ID
            (p->u).destNode = rx_packet->u.originNode; // The destination Node

            // Set up the return packet
            p->u.temp_mC = getInternalTemperature();
            p->u.battVolt_mV = getBatteryVoltage();
            p->u.lightSensor = 55;
            p->u.servoPos_degs = 77;
            //p->u.lightSensor = getLightSensorValue();
            //p->u.servoPos_degs = getServoPosition();
            break;
        default:
            break;
    }
    printPacket(p);
}

int processUniCast(node_num_t dest, payload_t *rx_packet)
{
    // Stop all broadcast timers!
    stopAllBroadCastTimer();
    // printf("Stopping all broadcast timers!\n");

    // dest is not used here for the solar Panel
    // check if destination byte 2
    (void) dest;
    //read out byte 1 if PATH or UNICAST
    switch(rx_packet->a.apkt)
    {
        case ACK:
            // always forward to destination(BASE STATION)
            printf("Received packet is ACK type\n");
            unict_send(rx_packet);
            break;

        case UNICAST:
            printf("Received packet is UNICAST type\n");
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
            printf("Received packet is PATH type\n");
            //check if Destination
            if(rx_packet->a.dest.u16 == getMyRIMEID()->u16){
                //add own Node ID to the first entry of hophistory with 0xFFFF
                addSelfToHist(rx_packet);
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
            printf("Unicast_local: In default!!! Value of pkttype is %d\n",rx_packet->a.apkt);
            printPacket(rx_packet);
            break;

    }
    return 0;
}
