
#include "contiki.h"
#include "net/rime/rime.h"     // Establish connections.
#include "net/netstack.h"      // Wireless-stack definitions
#include "dev/leds.h"          // Use LEDs.

// Private includes
#include "routing.h"
#include "nodeID.h"

// Standard C includes:
#include <stdio.h>
#include <stdint.h>

#define MAXBROADCASTRETRANSMIT  5

// Creates an instance of a unicast connection.
static struct unicast_conn unicast;

// Creates an instance of a broadcast connection.
static struct broadcast_conn broadcast;

// The call backs for a broadcast event
static const struct broadcast_callbacks broadcast_call = {bdct_recv, bdct_send};

// The call backs for a Unicast event
static const struct unicast_callbacks unicast_call = {unict_recv, unict_send};

// Count maximum hops in the case of a broadcast
static int broadcastCount;

// Global Routing Table
static r_table_t rTable;

void initNetworkDisc(void)
{
    printf("Network Discovery Initiated\n");
    // reset routing table
    int i=0;
    for(i=0;i<TOTAL_NODES;i++)
    {
        rTable.dest[i].u16 = nodes[i].rimeID;
        rTable.next_hop[i].u16 = UNINIT;
        rTable.cost[i] = UNINITCOST;
    }
    broadcastCount = 0;

	linkaddr_t *t = getMyRIMEID();
    // initiate controlled flooding
	bdct_send(&broadcast, t);

    return;
}

/**
 * @param message - message to be broadcasted
 */
static void forward_msg(const char * message)
{
    if (broadcastCount < MAXBROADCASTRETRANSMIT)
    {
      //open the connection, if necessary
      broadcast_open(&broadcast, 129, &broadcast_call);

      //send the message
      packetbuf_copyfrom(message,sizeof(message) );
      broadcast_send(&broadcast);
      broadcastCount++;
    }
    else {
      printf("Maximum amount of %d broadcasts reached", MAXBROADCASTRETRANSMIT);
    }
}

void bdct_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  leds_on(LEDS_GREEN);
  printf("Broadcast message received from 0x%x%x: '%s' [RSSI %d]\r\n",
			 from->u8[0], from->u8[1],
			(char *)packetbuf_dataptr(),
			(int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI));

  // return the index of the corresponding rime ID in rTable
  node_num_t rcvdNode = returnIDIndex(from);

  rTable.next_hop[rcvdNode].u16 = from->u16;
  rTable.cost[rcvdNode] = 1;

  printf("Distance Vector of Node %d\n", getMyNodeID());
  int j;
  printf("=========================================\n");
  for (j=0;j<TOTAL_NODES;j++)
  {
      printf("Destination: %x, Next Hop: %x, Cost: %d\n",rTable.dest[j].u16,rTable.next_hop[j].u16,rTable.cost[j]);
  }
  printf("=========================================\n");

  char message[BROADCASTMSGSIZE_BYTES];
  packetbuf_copyto(message);
  forward_msg(message);
  leds_off(LEDS_GREEN);
}

void bdct_send(struct broadcast_conn *c, const linkaddr_t *to)
{
    leds_on(LEDS_BLUE);
    printf("Broadcast message sent from 0x%x%x: '%s'\n",
            to->u8[0], to->u8[1],
            (char *)packetbuf_dataptr());
    leds_off(LEDS_BLUE);
    forward_msg((char *)packetbuf_dataptr());
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
  // unicast_send(&unicast, &rTable.next_hop);
}
