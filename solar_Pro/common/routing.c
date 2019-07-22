
#include "contiki.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"
#include "sys/clock.h"
#include "net/rime/rime.h"     // Establish connections.
#include "net/netstack.h"      // Wireless-stack definitions
#include "dev/leds.h"          // Use LEDs.

// Private commom includes
#include "routing.h"
#include "payload.h"
#include "nodeID.h"
#include "broadcast_common.h"
#include "unicast_common.h"
#include "servoControl.h"

// Private local includes
#include "unicast_local.h"

// Standard C includes:
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define RCVTHRESHOLD    -60

// Creates an instance of a unicast connection.
static struct unicast_conn unicast;

// Creates an instance of a broadcast connection.
static struct broadcast_conn broadcast;

// The call backs for a broadcast event
static const struct broadcast_callbacks broadcast_call = {bdct_recv};

// The call backs for a Unicast event
static const struct unicast_callbacks unicast_call = {unict_recv};

// Count maximum hops in the case of a broadcast
static int broadcastCount;

// Global Routing Table
static r_table_t myrTable;

// Global payload to transmit
static payload_t payload_receive, payload_transmit;

// Unicast Receive Packet
static payload_t unicast_rx_packet;

// Callback Timer to toggle red LEDs
static struct ctimer ct_emergency;


static void printRTable2(r_table_t r, const char *text);
static void printRTable(const char *);
static void printFromRTable(payload_t , const char *);

static bool inPrepMode = false;

extern const nodeID_t nodes[];
PROCESS_NAME(broadcastSendProcess);
PROCESS_NAME(stateMachineThread);


static void callbackEmergency(void *ptr)
{
    ctimer_reset(&ct_emergency);
    leds_toggle(LEDS_RED);
}

void setUpRtable(void)
{
    int i;
    broadcastCount = 0;
    for(i=0;i<TOTAL_NODES;i++)
    {
        myrTable.dest[i].u16 = nodes[i].rimeID;
        if(myrTable.dest[i].u16 == getMyRIMEID()->u16 )
        {
            myrTable.next_hop[i].u16 = nodes[i].rimeID;
            myrTable.cost[i] = 0;
        }
        else
        {
            myrTable.next_hop[i].u16 = UNINIT;
            myrTable.cost[i] = UNINITCOST;
        }
    }
    return;
}

bool isValidNextHop(node_num_t node)
{
    // Precondition node should not be greater than TOTAL_NODES
    // if ( node > TOTAL_NODES)
    //     return false;

    // If no entry for next hop then send false
    if(myrTable.next_hop[node-1].u16 == UNINIT)
    {
        return false;
    }
    // else send true
    else
        return true;
}

void initNetworkDisc(void)
{
    printf("Network Discovery Initiated\n");
    // reset routing table
    broadcastCount = 0;
    inPrepMode = false;
    setUpRtable();

    // Copy information to payload
    strncpy(payload_transmit.b.msg,"Init",BROADCASTMSGSIZE_BYTES);
    payload_transmit.b.bpkt = DISCOVERY;
    payload_transmit.b.rTable = myrTable;

    // initiate controlled flooding
    process_post(&broadcastSendProcess, PROCESS_EVENT_MSG,(void *) DISCOVERY);

    return;
}

void prepNetworkDisc(void)
{
    ctimer_stop(&ct_emergency);
    printf("Preparing Network Discovery\n");
    // reset routing table
    broadcastCount = 0;
    inPrepMode = false;
    setUpRtable();

    // Copy information to payload
    strncpy(payload_transmit.b.msg,"Prep",BROADCASTMSGSIZE_BYTES);
    payload_transmit.b.bpkt = PREPDISC;
    printRTable("=======My Table Clearing for NetDiscPrep=======");

    // initiate controlled flooding
    process_post(&broadcastSendProcess, PROCESS_EVENT_MSG, (void *)PREPDISC);

    return;
}

void initEmergency(void)
{
    printf("Emergency initiated\n");
    strncpy(payload_transmit.b.msg,"EMGNCY",BROADCASTMSGSIZE_BYTES);
    payload_transmit.b.bpkt = EMERGENCY;
    broadcastCount = 0;

    // initiate controlled flooding
    process_post(&broadcastSendProcess, PROCESS_EVENT_MSG, (void *)EMERGENCY);
}

void openBroadcast(void)
{
    //open the connection, if necessary
    broadcast_open(&broadcast, BROADCASTCHANNEL, &broadcast_call);
}

void openUnicast(void)
{
    // open the connection, if necessary
    unicast_open(&unicast, UNICASTCHANNEL, &unicast_call);
}

void closeUnicast(void)
{
    // close unicast connection
    unicast_close(&unicast);
}
/**
 * @param message - message to be broadcasted
 */
static void forward_msg(const char * message)
{
    //send the message
    printf("Size of payload is %d\n",sizeof(payload_t));
    packetbuf_copyfrom(message,sizeof(payload_t) ); // WARNING: Make sure that the size of message is equal to size of payload_t
    broadcast_send(&broadcast);
    broadcastCount++;
}

static bool compareAndUpdateTable(payload_t p, const linkaddr_t *from)
{
    bool result = false;
    r_table_t *fromrTable = &(p.b.rTable);
    int j = 0;

    for (j=0;j<TOTAL_NODES;j++)
    {
        // First check if the sender of the Broadcast has an entry in our rTable
        if( ((fromrTable->cost)[j] < myrTable.cost[j]) && ( myrTable.cost[j] - (fromrTable->cost)[j] > 1) )
        {
            printf("In Here Network NodeID is %d and result is TRUE\n",j+1);
            myrTable.next_hop[j] = *from;
            myrTable.cost[j] = (fromrTable->cost)[j] + 1;
            result = true;
        }
    }
    return result;
}

void bdct_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
    packetbuf_copyto(&payload_receive);
    bool tableUpdateRequired = false;
    int16_t rssi = (int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI);
    printf("Broadcast message received from 0x%x%x: '%s' [RSSI %d]\r\n",
            from->u8[0], from->u8[1],
            (char *)packetbuf_dataptr(),
            rssi);


    // If the RSSI value is less than a threshold value drop the packet
    if ( rssi <= RCVTHRESHOLD )
    {
        leds_on(LEDS_GREEN);
        printf("Dropping message as %d rssi < %d Threshold\n",rssi, RCVTHRESHOLD);
        leds_off(LEDS_GREEN);
        return ;
    }
    else
    {
        printf("Processing Packet\n");
    }

    leds_on(LEDS_GREEN);

    switch(payload_receive.b.bpkt)
    {
        case EMERGENCY:
            // TODO set servo angle out of reach for emergency angle defined in common/routing.h
            setServoPosition(255);
            // set callback timer to toggle red leds
            ctimer_set(&ct_emergency, CLOCK_SECOND, callbackEmergency, NULL);
            process_post(&broadcastSendProcess, PROCESS_EVENT_MSG, (void *)EMERGENCY);
            break;

        case DISCOVERY:
            strncpy(payload_receive.b.msg, "Disc",BROADCASTMSGSIZE_BYTES);

            printRTable2(payload_receive.b.rTable,"======= Received Payload is =======");
            printRTable("=======My Table before the Update=======");

            // Compare payload with rTable
            tableUpdateRequired = compareAndUpdateTable(payload_receive, from);

            printRTable("=======My Table After the Update=======");

            // if it hasnt changed
            if ( tableUpdateRequired )
            {
                // Set up discovery mode
                payload_transmit.b.bpkt = DISCOVERY;

                // Copy new table into the packet
                payload_transmit.b.rTable = myrTable;

                // Copy new table into the packet
                strncpy(payload_transmit.b.msg, "Disc",BROADCASTMSGSIZE_BYTES);

                //rebroadcast it
                process_post(&broadcastSendProcess, PROCESS_EVENT_MSG, (void *) DISCOVERY);
            }
            break;

        case PREPDISC:
            // Clear own routing table
            ctimer_stop(&ct_emergency);
            leds_off(LEDS_RED);
            setUpRtable();
            printRTable("=======My Table After PREPDISC=======");
            // Set up Prepare Network DISCOVERY
            payload_transmit.b.bpkt = PREPDISC;

            if (!inPrepMode)
            {
                //rebroadcast it
                process_post(&broadcastSendProcess, PROCESS_EVENT_MSG, (void *) PREPDISC);
                inPrepMode = true;
            }
            break;

        default:
            break;
    }
    leds_off(LEDS_GREEN);
}

void doBroadCast(void)
{
    // Whats my RIME ID
    linkaddr_t *t = getMyRIMEID();

    // initiate controlled flooding
    bdct_send(&broadcast, t);
}

void bdct_send(struct broadcast_conn *c, const linkaddr_t *from)
{
    leds_on(LEDS_BLUE);
    packetbuf_clear();
    int copiedBytes = packetbuf_copyfrom(&(payload_transmit),sizeof(payload_transmit));
    printf("Copied bytes: %d\n",copiedBytes);
    printf("Broadcast message sent from 0x%x%x: '%s'\n",
            from->u8[0], from->u8[1],
            (char *)packetbuf_dataptr());
    forward_msg((char *)packetbuf_dataptr());
    leds_off(LEDS_BLUE);
}

// Defines the behavior of a connection upon receiving data.
void unict_recv(struct unicast_conn *c, const linkaddr_t *from)
{
    leds_on(LEDS_YELLOW);
    memset(&unicast_rx_packet, 0, sizeof(unicast_rx_packet));

    int copiedBytes = packetbuf_copyto(&unicast_rx_packet);
    printf("Copied bytes: %d\n",copiedBytes);
    printf("Unicast message received from 0x%x%x: '%s' [RSSI %d]\n",
            // For Debug purposes
            from->u8[0], from->u8[1],
            (char *)packetbuf_dataptr(),
            (int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI));
    processUniCast(0, &unicast_rx_packet);
    leds_off(LEDS_YELLOW);
}

static linkaddr_t * getNextHopRIMEID(payload_t tx_packet)
{
    node_num_t destination;
    switch(tx_packet.a.apkt)
    {
        case UNICAST:
            printf("UNI: getting Next Hop\n");
            destination = tx_packet.u.destNode;
            destination = destination - 1;
            break;
        case ACK:
            printf("ACK Mode ");
            if (tx_packet.u.destNode > TOTAL_NODES)
            {
                printf("PATH\n");
                destination = returnIDIndex(&(tx_packet.a.dest));
            }
            else
            {
                printf("UNICAST\n");
                destination = tx_packet.u.destNode;
                destination = destination - 1;
            }
            break;
        case PATH:
            printf("PATH: getting Next Hop\n");
            destination = returnIDIndex(&(tx_packet.a.dest));
            break;
        default:
            destination = 0;
            printf("Shouldnt be here!!!\n");
            break;
    }
    printf("My destination is %d and Dest RIME ID: %x\n",destination+1, myrTable.next_hop[destination].u16);
    return &(myrTable.next_hop[destination]);
}

void unict_send(payload_t *tx_packet)
{
    packetbuf_clear();
    printf("Send Unicast message from %d: to: %x '%s'\n",getMyNodeID(), getNextHopRIMEID(*tx_packet)->u16, (char *)(tx_packet));
    packetbuf_copyfrom(tx_packet, sizeof(payload_t));
    unicast_send(&unicast, getNextHopRIMEID(*tx_packet));
}

// ---------------------------- DEBUG PRINTS ---------------------------


static void printRTable(const char *text)
{
    int j;
    printf("%s\nMy Node ID: %d\n", text, getMyNodeID());
    printf("=========================================\n");
    for (j=0;j<TOTAL_NODES;j++)
    {
        printf("Destination: %x, Next Hop: %x, Cost: %d\n",myrTable.dest[j].u16,myrTable.next_hop[j].u16,myrTable.cost[j]);
    }
    printf("=========================================\n");
}

static void printFromRTable(payload_t p, const char *text)
{
    r_table_t table = p.b.rTable;
    int j;
    printf("%s\nMy Node ID: %d\n", text, getMyNodeID());
    printf("=========================================\n");
    for (j=0;j<TOTAL_NODES;j++)
    {
        printf("Destination: %x, Next Hop: %x, Cost: %d\n",table.dest[j].u16,table.next_hop[j].u16,table.cost[j]);
    }
    printf("=========================================\n");
}

static void printRTable2(r_table_t r, const char *text)
{
    int j;
    printf("%s\nMy Node ID: %d\n", text, getMyNodeID());
    printf("=========================================\n");
    for (j=0;j<TOTAL_NODES;j++)
    {
        printf("Destination: %x, Next Hop: %x, Cost: %d\n",r.dest[j].u16,r.next_hop[j].u16, r.cost[j]);
    }
    printf("=========================================\n");
}
