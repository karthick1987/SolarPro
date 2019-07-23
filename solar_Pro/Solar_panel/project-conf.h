/*
   Wireless Sensor Networks Laboratory 2019 -- Group 1

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
http://www.lkn.ei.tum.de

copyright (c) 2019 Chair of Communication Networks, TUM

project: SolarPro

 /**
 * @file proj-config.h
 * @author Karthik Sukumar & Johannes Machleid
 * @brief Header file with defines of the MAC Layer, Channel and Transmission power
 *
 */
#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_


//// PHY LAYER PARAMETERS
#define CHANNEL 11
#define TX_POWER 7
// int power_options[] =    {255,237,213,197,182,176,161,145,136,114,98 ,88 ,66 ,0};
// int power_dBm[] =        {7  ,5  ,3  ,1  ,0  ,-1 ,-3 ,-5 ,-7 ,-9 ,-11,-13,-15,-24};
//
//// MAC LAYER PARAMETERS
#define NETSTACK_CONF_MAC csma_driver 				//	nullmac_driver or csma_driver
#define NETSTACK_CONF_RDC nullrdc_driver 		//	nullrdc_driver or contikimac_driver
//#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 256 	// 	x^2 (8,16,32,...)
//
//

#endif /* PROJECT_CONF_H_ */
