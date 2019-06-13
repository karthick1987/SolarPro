/*
   Wireless Sensor Networks Laboratory 2019 -- Group 1

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
http://www.lkn.ei.tum.de

copyright (c) 2019 Chair of Communication Networks, TUM

project: SolarPro

contributors:
 * Karthik Sukumar
 * Johannes Machleid

 This c-file is designed for all nodes to set specific Node IDs depending on the RIME ID.
 */


// Std file includes
#include <stdio.h>

// Contiki includes
#include "core/net/linkaddr.h"

// Private includes
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

node_num_t getMyNodeID( linkaddr_t l )
{
    int i=0;
    for (i=0;i<NETWORKSIZE;i++)
    {
        if ( l.u16 == nodes[i].rimeID )
        {
            return nodes[i].nodeID;
        }
    }
    return -1;
}
