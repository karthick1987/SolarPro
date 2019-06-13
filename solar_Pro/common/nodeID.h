#ifndef NODEID_H
#define NODEID_H

#include "contiki.h"
#include "core/net/linkaddr.h"

typedef uint16_t node_num_t;
typedef struct nodeID{

    node_num_t nodeID;
    uint16_t serialNumber;
    uint16_t rimeID;
}nodeID_t;

void print_node_IDs( void );
node_num_t getMyNodeID( linkaddr_t l );
#endif
