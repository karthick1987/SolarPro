
#include "routing.h"

void initNetworkDisc(void)
{
    int i=0;
    for(i=0;i<TOTAL_NODES;i++)
    {
        rTable.dest[i] = UNINIT;
        rTable.next_hop[i] = UNINIT;
        rTable.cost[i] = UNINITCOST;
    }

    return;
}

int broadcast_recv(void)
{

}

int broadcast_send(void)
{

}

int unicast_recv(void)
{

}

int unicast_send(void)
{

}

