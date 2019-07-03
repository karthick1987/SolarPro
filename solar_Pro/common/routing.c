
#include "contiki.h"
#include "sys/timer.h"
#include "sys/process.h"
#include "sys/clock.h"
#include "net/rime/rime.h"     // Establish connections.
#include "net/netstack.h"      // Wireless-stack definitions
#include "dev/leds.h"          // Use LEDs.

// Private includes
#include "routing.h"
#include "nodeID.h"

// Standard C includes:
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define MAXBROADCASTRETRANSMIT  5
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
static payload_t payload;

static void printRTable2(r_table_t r, const char *text);
static void printRTable(const char *);
static void printFromRTable(payload_t , const char *);

extern const nodeID_t nodes[];

void setUpRtable(void)
{
    int i;
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

// NOTE: If ms>500 then this will trigger the watchdog!!!
static void delay_ms(uint16_t ms)
{
    for(int i = 0;i<ms;i++)
    {
        clock_delay_usec(1000);
    }
    printf("Delayed for %dms\n",ms);
    return;
}

void initNetworkDisc(void)
{
    printf("Network Discovery Initiated\n");
    // reset routing table
    int i = 0;
    broadcastCount = 0;

    setUpRtable();

    // Copy information to payload
    strncpy(payload.b.msg,"HelloWorld",BROADCASTMSGSIZE_BYTES);
    payload.b.bpkt = DISCOVERY;
    payload.b.rTable = myrTable;

    // Whats my RIME ID
    linkaddr_t *t = getMyRIMEID();

    // initiate controlled flooding
    bdct_send(&broadcast, t);

    return;
}

void openBroadcast(void)
{
    //open the connection, if necessary
    broadcast_open(&broadcast, 129, &broadcast_call);
}

/**
 * @param message - message to be broadcasted
 */
static void forward_msg(const char * message)
{
    if (broadcastCount < MAXBROADCASTRETRANSMIT)
    {
        //send the message
        printf("Size of payload is %d\n",sizeof(payload_t));
        packetbuf_copyfrom(message,sizeof(payload_t) ); // WARNING: Make sure that the size of message is equal to size of payload_t
        broadcast_send(&broadcast);
        delay_ms(200);
        broadcastCount++;
    }
    else {
        printf("Maximum amount of %d broadcasts reached\n", MAXBROADCASTRETRANSMIT);
    }
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
            broadcastCount = 0;
        }
    }
    return result;
}

void bdct_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
    bool tableUpdateRequired = false;
    int16_t rssi = (int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI);
    printf("Broadcast message received from 0x%x%x: '%s' [RSSI %d]\r\n",
            from->u8[0], from->u8[1],
            (char *)packetbuf_dataptr(),
            rssi);


    // return the index of the corresponding rime ID in myrTable
    node_num_t rcvdNode = returnIDIndex(from);

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
    // myrTable.next_hop[rcvdNode].u16 = from->u16;
    // myrTable.cost[rcvdNode] = 1;

    packetbuf_copyto(&(payload.b));

    strncpy(payload.b.msg, "Hello You",BROADCASTMSGSIZE_BYTES);
    //printf("The beginning of broadcastMsg_t is %d %c\n",(uint16_t)(payload.b.rTable), payload.b.msg[0]);

    printRTable2(payload.b.rTable,"======= Received Payload is =======");

    printRTable("=======My Table before the Update=======");

    // Compare payload with rTable
    tableUpdateRequired = compareAndUpdateTable(payload, from);

    printRTable("=======My Table After the Update=======");

    // if it hasnt changed 
    if ( tableUpdateRequired )
    {

        // Set up discovery mode
        payload.b.bpkt = DISCOVERY;

        // Copy new table into the packet
        payload.b.rTable = myrTable;

        // Copy new table into the packet
        strncpy(payload.b.msg, "Hello You",BROADCASTMSGSIZE_BYTES);

        //rebroadcast it
        forward_msg((const char *)&payload);
    }

    // Else do nothing and dont Broadcast again
    leds_off(LEDS_GREEN);
}

void bdct_send(struct broadcast_conn *c, const linkaddr_t *from)
{
    leds_on(LEDS_BLUE);
    packetbuf_clear();
    int copiedBytes = packetbuf_copyfrom(&(payload),sizeof(payload));
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
    payload_t rx_packet;

    packetbuf_copyto(&rx_packet);

    // For Debug purposes
    printf("Unicast message received from 0x%x%x: '%s' [RSSI %d]\n",
            from->u8[0], from->u8[1],
            (char *)packetbuf_dataptr(),
            (int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI));
}

void unict_send(payload_t tx_packet)
{
    packetbuf_copyfrom(&tx_packet, sizeof(tx_packet));
    // unicast_send(&unicast, &myrTable.next_hop);
}

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
