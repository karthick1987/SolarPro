
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
            p->u.temp_mC = 0;
            p->u.battVolt_mV = 0;
            p->u.lightSensor = 0;
            p->u.servoPos_degs = 0;
            break;
        default:
            break;
    }
}

void addSelfToHist(payload_t *payload)
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


