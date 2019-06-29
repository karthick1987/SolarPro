
#include "routing.h"
#include "contiki.h"
#include "net/rime/rime.h"     // Establish connections.
#include "net/netstack.h"      // Wireless-stack definitions
#include "dev/leds.h"          // Use LEDs.
#include "payload.h"
#include "nodeID.h"

#define MAXBROADCASTRETRANSMIT  5

// Creates an instance of a unicast connection.
static struct unicast_conn unicast;

// Creates an instance of a broadcast connection.
static struct broadcast_conn broadcast;

// The call backs for a broadcast event
static const struct broadcast_callbacks broadcast_call = {broadcast_recv, broadcast_send};

// The call backs for a Unicast event
static const struct unicast_callbacks unicast_call = {unicast_recv, unicast_send};

static int broadcastCount;


extern const nodeID_t nodes[];

void initNetworkDisc(void)
{
    int i=0;
    for(i=0;i<TOTAL_NODES;i++)
    {
        rTable.dest[i] = nodes[i].rimeID;
        rTable.next_hop[i] = UNINIT;
        rTable.cost[i] = UNINITCOST;
    }
    broadcastCount = 0;

    return;
}

/**
 * @param message - message to be broadcasted
 */
void forward_msg(const char * message)
{

    //open the connection, if necessary
    broadcast_open(&broadcast, 129, &broadcast_call);

    //send the message
    packetbuf_copyfrom(message,sizeof(message) );

    broadcast_send(&broadcast);

}

void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
    leds_on(LEDS_GREEN);
    printf("Broadcast message received from 0x%x%x: '%s' [RSSI %d]\r\n",
            from->u8[0], from->u8[1],
            (char *)packetbuf_dataptr(),
            (int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI));

    char message[BROADCASTMSGSIZE_BYTES];
    packetbuf_copyto(message);
    forward_msg(message);
    leds_off(LEDS_GREEN);
}

void broadcast_send(struct broadcast_conn *c, const linkaddr_t *to)
{
    leds_on(LEDS_BLUE);
    printf("Broadcast message sent from 0x%x%x: '%s'\n",
            to->u8[0], to->u8[1],
            (char *)packetbuf_dataptr());
    leds_off(LEDS_BLUE);
    forward_msg((char *)packetbuf_dataptr());
}

// Defines the behavior of a connection upon receiving data.
void unicast_recv(struct unicast_conn *c, const linkaddr_t *from)
{
    packet_t rx_packet;

    packetbuf_copyto(&rx_packet);

    // For Debug purposes
    printf("Unicast message received from 0x%x%x: '%s' [RSSI %d]\n",
            from->u8[0], from->u8[1],
            (char *)packetbuf_dataptr(),
            (int16_t)packetbuf_attr(PACKETBUF_ATTR_RSSI));

}

void unicast_send(packet_t tx_packet)
{
    packetbuf_copyfrom(&tx_packet, sizeof(packet_t));
    unicast_send(&unicast, &next_hop);
}
