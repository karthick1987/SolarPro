
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
#include "unicast_panel.h"

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
static void setupPacket(payload_t *p, pkttype_t type, node_num_t dest)
{
    switch(type)
    {
        case PATH:
            (p->a).apkt = PATH;
            (p->a).dest.u16 = getRIMEID(n);
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
    }
}

int doPathMode(node_num_t dest, payload_t *p)
{
    setupPacket(p,PATH,dest);
    return (n == TOTAL_NODES?0:-1);
}



int doUniCast(payload_t *rx_packet)
{
  //check if destination byte 2
  //read out byte 1 if PATH or UNICAST
  switch(rx_packet->a.apkt)
  {
      case ACK:
          // always forward to basestation
          unict_send(rx_packet);
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
            unicst_send(tx_packet);
          }
          else
          {
            unict_send(rx_packet);
          }
          break;

        case PATH:
          //check if Destination
          if(rx_packet->a.dest == getMyRIMEID()){
            // send unicast with PATH header and hophistory back to base station
            // TODO and add own ID as last entry in Hophistory
            tx_packet.a.apkt = PATH;
            tx_packet.a.dest = getRIMEID(rx_packet->u.originNode);
            tx_packet.a.hopHist = rx_packet.a.hopHist;
          }
          else{
            //add own Node ID to the first entry of hophistory with 0xFF
            //forward message to destination
            unict_send(rx_packet);
          }
          break;

        default:
          break;


  }
    setupPacket(p,UNICAST,n);
    return (n == TOTAL_NODES?0:-1);
}
