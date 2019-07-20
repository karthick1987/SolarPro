
// Standard C includes:
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "contiki.h"
#include "sys/ctimer.h"

#include "unicast_common.h"
#include "broadcast_common.h"
#include "routing.h"
#include "base.h"
#include "payload.h"
#include "unicast_local.h"

// for the next node calculation
static node_num_t next_node = 1;

// Global for transmission success
bool isTransSuccess = false;

// Local global variable static to this file
static payload_t tx_packet;

// Ctimer for receiving ack
static struct ctimer ackTimer;

static int transmissionCount = 1;
static int pathCounter = 1;

static void doPathMode(void);

PROCESS_NAME(stateMachineThread);

static void callbackResendPath(void *ptr)
{
    doPathMode();
    if (transmissionCount == UNICASTMAXRETRANSMIT)
    {
        ctimer_stop(&ackTimer);
        process_post(&stateMachineThread, PROCESS_EVENT_MSG, (void *) PREPNETDISC);
    }
    else
    {
        transmissionCount++;
    }
}

PROCESS(unicastSendProcess, "Unicast msg Send Thread");
PROCESS_THREAD (unicastSendProcess, ev, data)
{
    static pkttype_t pkt_type;

    PROCESS_EXITHANDLER(
            printf("%s Killed\n", PROCESS_CURRENT()->name);
            )

    PROCESS_BEGIN();

    printf("%s Started\n", PROCESS_CURRENT()->name);
    while(1) {
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_MSG)
        {
            pkt_type = (int) data;
            switch(pkt_type)
            {
                case PATH:
                    doPathMode();
                    break;
                case UNICAST:
                    //doUnicastMode();
                    printf("Setting timer for UNICAST\n");
                    break;
                default:
                    // Cancel all unicast msgs
                    break;
            }
        }

        else if (ev == PROCESS_EVENT_TIMER)
        {
        }
    }

    PROCESS_END();
}

void initUnicastMode(void)
{
    next_node = 1;
    pathCounter = 1;
    isTransSuccess = false;
}

void initPathMode()
{
    next_node = 1;
    pathCounter = 1;
    transmissionCount = 1;
    isTransSuccess = false;
}

static node_num_t nextNode(node_num_t current_node)
{

    int i = 0;
    for(i = 0;i<TOTAL_NODES;i++)
    {
        if (current_node == getMyNodeID() || !(isValidNextHop(current_node)))
        {
            printf("Skipped node %d as its either me or is not in Network\n",i+1);
            current_node++;
            if (current_node > TOTAL_NODES)
                current_node = 1;
        }
        else
        {
            break;
        }
    }
	return current_node;
}

static void setupPacket(pkttype_t type) // , pkttype_t type, node_num_t dest)
{
    //tx_packet is a static global here
    payload_t *p = &tx_packet;

    switch(type)
    {
        case PATH:
            zeroOut(p, PATH);
            (p->a).apkt = PATH;
            (p->a).dest.u16 = getRIMEID(next_node);
            addSelfToHist(p);
            printPacket(p);
            break;
        case UNICAST:
            zeroOut(p, UNICAST);
            (p->u).upkt = UNICAST;
            (p->u).originNode = getMyNodeID(); // My Node ID
            (p->u).destNode = next_node; // The destination Node
            // TODO Add Servo Angle manually
            printPacket(p);
            break;
        default:
            printf("[base/unicast_local]: in Default\n");
            printPacket(p);
            break;
    }
}

static void doPathMode(void)
{

    printf("Sending to NextNode: %d Attempt: %d\n",next_node, transmissionCount);

    // Determine next node
    next_node = nextNode(next_node);

    // setupPacket can be in dopathMode
    setupPacket(PATH);

    // check transflag==true then stop timer and increment the pathCounter variable
    if (isTransSuccess)
    {
        ctimer_stop(&ackTimer);
        transmissionCount = 1;
        pathCounter++;

        // TODO send GUI the rx_data
        isTransSuccess = false;

        // If all nodes are done
        if (next_node >= TOTAL_NODES)
        {
            process_post(&stateMachineThread, PROCESS_EVENT_MSG, (void *)UNICASTMODE);
        }
        else
        {
            // go to next_node
            next_node++;
            process_post(&unicastSendProcess, PROCESS_EVENT_MSG, (void *)UNICAST);
        }
    }
    else
    {
        // Send packet
        unict_send(&tx_packet);

        // Set acknowledge timer
        ctimer_set(&ackTimer, UNICASTINTERVAL, &callbackResendPath, NULL);

        printf("Callback Timer set to expire in a little while\n");

    }
}

int doUniCastMode(node_num_t dest, payload_t *rx_packet)
{
    //check if destination byte 2

    // start unicast_process
    // node = nextNodeis(node);
    // setupPacket can be in dopathMode
    // Send and wait verfahren x3 MAXIMAL
    // AckCounter++

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
                setupPacket(UNICAST);
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
                setupPacket(PATH);
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
    return 0;
}

