
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
static struct ctimer ackPathTimer;
static struct ctimer ackSensorTimer, ucInt;

static int transmissionCount = 1;
static bool isPathModeComplete = false;
static bool isPathMode = true;

static void doPathMode(void);
static void doUniCastMode(void);

PROCESS_NAME(stateMachineThread);
PROCESS(unicastSendProcess, "Unicast msg Send Thread");

static void callbackResendPath(void *ptr)
{
    printf("PATH Ctimer callback entered\n");
    printf("Transmission count is :%d\n",transmissionCount);
    process_post(&unicastSendProcess, PROCESS_EVENT_MSG, (void *) PATH);

    if (transmissionCount > UNICASTMAXRETRANSMIT)
    {
        ctimer_stop(&ackPathTimer);
        printf("[PATH] Ctimer CB: stopped\n");
        process_post(&stateMachineThread, PROCESS_EVENT_MSG, (void *) PREPNETDISC);
    }
}

static void callbackResendSensor(void *ptr)
{
  printf("SENSOR UNICAST Ctimer callback entered\n");
  printf("Transmission count is :%d\n",transmissionCount);
  process_post(&unicastSendProcess, PROCESS_EVENT_MSG, (void *) UNICAST);

  if (transmissionCount > UNICASTMAXRETRANSMIT)
  {
      ctimer_stop(&ackSensorTimer);
      printf("[UNI] Ctimer CB: stopped\n");
      process_post(&stateMachineThread, PROCESS_EVENT_MSG, (void *) PREPNETDISC);
  }
}

static void callbackUCInt(void *ptr)
{
    switch((int)ptr)
    {
        case PATH:
            printf("In UCInt Callback of Type PATH\n");
            process_post(&unicastSendProcess, PROCESS_EVENT_MSG, (void *)PATH);
            break;
        case UNICAST:
            printf("In UCInt Callback of Type UNICAST\n");
            process_post(&unicastSendProcess, PROCESS_EVENT_MSG, (void *)UNICAST);
            break;
        default:
            break;
    }
    ctimer_stop(&ucInt);
}

PROCESS_THREAD (unicastSendProcess, ev, data)
{
    static pkttype_t pkt_type;

    PROCESS_EXITHANDLER(
            printf("%s Killed and Unicast Connection Closed\n", PROCESS_CURRENT()->name);
            // closeUnicast();
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
                    doUniCastMode();
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
    transmissionCount = 1;
    isTransSuccess = false;
    stopAllBroadCastTimer();
}

void initPathMode()
{
    next_node = 1;
    transmissionCount = 1;
    isPathModeComplete = false;
    isTransSuccess = false;
    stopAllBroadCastTimer();
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
            {
                current_node = 1;
                isPathModeComplete = true;
            }
        }
        else
        {
            break;
        }
    }
    printf("Current Node is %d\n",current_node);
	return current_node;
}

static void setupPacket(pkttype_t type)
{
    //tx_packet is a static global here
    payload_t *p = &tx_packet;
    printf("[Base/Unicast_Local]: Setting up packet now\n");

    switch(type)
    {
        case PATH:
            zeroOut(p, PATH);
            (p->a).apkt = PATH;
            (p->a).dest.u16 = getRIMEID(next_node);
            addSelfToHist(p);
            break;
        case UNICAST:
            zeroOut(p, UNICAST);
            (p->u).upkt = UNICAST;
            (p->u).originNode = getMyNodeID(); // My Node ID
            (p->u).destNode = next_node; // The destination Node
            // TODO Add Servo Angle manually
            break;
        default:
            printf("[base/unicast_local]: in Default\n");
            break;
    }
    printPacket(p);
}

static void doPathMode(void)
{
    // printf("Before: Sending to NextNode: %d Attempt: %d\n",next_node, transmissionCount);

    // Determine next node
    next_node = nextNode(next_node);

    // printf("After:  Sending to NextNode: %d Attempt: %d\n",next_node, transmissionCount);
    // printf("isPathModeComplete Flag is %d\n", isPathModeComplete);

    // setupPacket can be in dopathMode
    setupPacket(PATH);

    // check transflag==true then stop timer and increment the pathCounter variable
    if (isTransSuccess)
    {
        ctimer_stop(&ackPathTimer);
        //printf("dPM: ctimer stopped\n");
        printf("dPM: Transmission count is :%d\n",transmissionCount);
        transmissionCount = 1;

        // TODO send GUI the rx_data
        isTransSuccess = false;

        //printf("Next node is %d\n",next_node);

        // If all nodes are done
        if (isPathModeComplete)
        {
            printf("PATH MODE COMPLETE\n");
            ctimer_stop(&ackPathTimer);
            process_post(&stateMachineThread, PROCESS_EVENT_MSG, (void *)UNICASTMODE);
        }
        else
        {
            printf("Node [%d] SUCCESS Going to next Node\n",next_node);
            // go to next_node
            next_node++;
            process_post(&unicastSendProcess, PROCESS_EVENT_MSG, (void *)PATH);
        }
    }
    else
    {
        if(isPathModeComplete)
        {
            ctimer_stop(&ackPathTimer);
            printf("PATH MODE COMPLETE\n");
            process_post(&stateMachineThread, PROCESS_EVENT_MSG, (void *)UNICASTMODE);
        }
        else
        {
            // Send packet
            unict_send(&tx_packet);

            // Set acknowledge timer
            ctimer_set(&ackPathTimer, UNICASTINTERVAL, &callbackResendPath, NULL);
            //printf("Callback Timer set to expire in a little while\n");
            transmissionCount++;
        }
    }
}

static void doUniCastMode(void)
{
    printf("Now starting to collect sensor values....\n");
    printf("Before: Sending to NextNode: %d Attempt: %d\n",next_node, transmissionCount);

    // Determine next node
    next_node = nextNode(next_node);

    printf("After:  Sending to NextNode: %d Attempt: %d\n",next_node, transmissionCount);
    printf("isTransSuccess Flag is %d\n", isTransSuccess);

    // setupPacket can be in dopathMode
    setupPacket(UNICAST);

    // check transflag==true then stop timer and increment the pathCounter variable
    if (isTransSuccess)
    {
        ctimer_stop(&ackSensorTimer);
        printf("dUM: ctimer stopped\n");
        printf("dUM: Transmission count is :%d\n",transmissionCount);
        transmissionCount = 1;

        // TODO send GUI the rx_data
        isTransSuccess = false;

        printf("Next node is %d\n",next_node);

        printf("Going to next Node\n");
        // go to next_node
        next_node++;
        process_post(&unicastSendProcess, PROCESS_EVENT_MSG, (void *)UNICAST);

    }
    else
    {
        // Send packet
        unict_send(&tx_packet);

        // Set acknowledge timer
        ctimer_set(&ackSensorTimer, UNICASTINTERVAL, &callbackResendSensor, NULL);
        printf("Callback Timer set to expire in a little while\n");
        transmissionCount++;
    }
}

int processUniCast(node_num_t dest, payload_t *rx_packet)
{
    //read out byte 1 if PATH or UNICAST
    switch(rx_packet->a.apkt)
    {
        case ACK:
            // Do the compare with the sent message and node and send Hop History to GUI
            // ACK with hop history
            if (rx_packet->u.destNode > TOTAL_NODES)
            {
                //printf("Base processing ACK PATH MODE PKT\n");
                int i = 0;
                for (i=0;i<TOTAL_NODES;i++)
                {
                    if(rx_packet->a.hopHist[i].u16 == RESETADDR)
                    {
                        //printf("i value is %d\n",i);
                        break;
                    }
                }
                if ( getRIMEID(next_node) == rx_packet->a.hopHist[i-1].u16 )
                {
                    printf("REACHED DESTINATION\n");
                    isTransSuccess = true;
                    ctimer_stop(&ackPathTimer);
                    // start timer to Post msg to thread
                    ctimer_set(&ucInt, UNICASTINTERVAL, &callbackUCInt, (void *)PATH);

                }
            }
            // ACK Unicast with sensor values
            else
            {
                printf("Base processing ACK UNICAST SENSOR MODE PKT\n");
                if ( next_node == rx_packet->u.originNode )
                {
                    printf("Setting isTransSuccess to TRUE\n");
                    isTransSuccess = true;
                    ctimer_stop(&ackSensorTimer);
                    ctimer_set(&ucInt, UNICASTINTERVAL, &callbackUCInt, (void *)UNICAST);
                }
            }
            break;

        case UNICAST:
            printf("Received packet is UNICAST type Shouldnt be here!!!\n");
            break;

        case PATH:
            printf("Received packet is PATH type Shouldnt be here!!!\n");
            break;

        default:
            break;

    }
    return 0;
}
