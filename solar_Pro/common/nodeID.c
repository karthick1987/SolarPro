
#include <stdio.h>
#include "nodeID.h"

#define NETWORKSIZE 7

static nodeID_t nodes[NETWORKSIZE] = 
{
    {0,2048,0xBFED},
    {1,2215,0xF2F3},
    {2,2162,0x60EF},
    {3,2031,0xE7F3},
    {4,1982,0xDCF3},
    {5,2053,0xDDED},
    {6,2207,0xB0EE}
};

void print_node_IDs( void )
{
    int i=0;
    for (i=0;i<NETWORKSIZE;i++)
    {
        printf("Node ID: %d, Serial No: %d, Rime ID: %x\n",nodes[i].nodeID,nodes[i].serialNumber,nodes[i].rimeID);
    }
}
