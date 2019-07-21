
// Standard C includes:
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "contiki.h"
#include "nodeID.h"
#include "routing.h"
#include "base.h"
#include "unicast_common.h"

void printPacket(payload_t *p)
{
    return;
    int i;
    printf("Packet type is %d\n",p->a.apkt);
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

        case ACK:
            if (p->u.destNode > TOTAL_NODES)
            {
                printf("---ACK PATH MODE PKT---\n");
                for(i=0;i<TOTAL_NODES;i++)
                {
                    printf("%d:%d\n",i,(p->a).hopHist[i].u16);
                }
            }
            else
            {
                printf("---ACK UNICAST MODE PKT---\n");
                printf("Origin: %d, Destination: %d\n",p->u.originNode,p->u.destNode);
                printf("Temp_mC: %d\n",p->u.temp_mC);
                printf("Batt_mV: %d\n",p->u.battVolt_mV);
                printf("LightSensor: %d\n",p->u.lightSensor);
                printf("servoPos_Degs: %d\n",p->u.servoPos_degs);
            }
            break;

        default:
            printf("[UnicastCommon]: Random Packet!\n");
            printf("---Default packet---\nPacket Type is %d\nDestination Node is %x\n",(p->a).apkt, (p->a).dest.u16);
            for(i=0;i<TOTAL_NODES;i++)
            {
                printf("%d:%x\n",i,(p->a).hopHist[i].u16);
            }
            break;
    }
}

void zeroOut(payload_t *p, pkttype_t type)
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
            p->u.temp_mC = 1;
            p->u.battVolt_mV = 1;
            p->u.lightSensor = 1;
            p->u.servoPos_degs = 1;
            break;
        default:
            break;
    }
}

void addSelfToHist(payload_t *payload)
{
    int i = 0;
    printf("Adding self to history\n");
    for (i=0;i<TOTAL_NODES;i++)
    {
        if(payload->a.hopHist[i].u16 == RESETADDR)
        {
            printf("i values is %d\n",i);
            break;
        }
    }
    if(i >= 8)
        return;
    else
        payload->a.hopHist[i].u16 = getMyRIMEID()->u16;
    return;
}


