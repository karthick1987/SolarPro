#ifndef NODEID_H
#define NODEID_H

#include "contiki.h"

typedef struct nodeID{

    uint16_t nodeID;
    uint16_t serialNumber;
    uint16_t rimeID;
}nodeID_t;

void print_node_IDs( void );
#endif
