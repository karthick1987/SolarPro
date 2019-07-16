#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_


//// PHY LAYER PARAMETERS
#define CHANNEL 11
#define TX_POWER 7
//// int power_options[] =    {255,237,213,197,182,176,161,145,136,114,98 ,88 ,66 ,0};
//// int power_dBm[] =        {7  ,5  ,3  ,1  ,0  ,-1 ,-3 ,-5 ,-7 ,-9 ,-11,-13,-15,-24};
//
//// MAC LAYER PARAMETERS
#define NETSTACK_CONF_MAC csma_driver 				//	nullmac_driver or csma_driver
#define NETSTACK_CONF_RDC contikimac_driver 		//	nullrdc_driver or contikimac_driver
//#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 8 	// 	x^2 (8,16,32,...)
//
//
//// MEASUREMENT PARAMETERS
//#define INTER_PACKET_TIME	32		//  Inter-Packet Arrival Time 128 Ticks = 1 second.
//#define MAX_MESSAGE_LENGTH 	12		//  Message length (values btw 8-100)
//#define TOTAL_TX_PACKETS	10		//  Number of total transmit packets.
//

#endif /* PROJECT_CONF_H_ */
