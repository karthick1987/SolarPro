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
//
//
//// MAC LAYER PARAMETERS
#define NETSTACK_CONF_MAC csma_driver 				//	nullmac_driver or csma_driver
#define NETSTACK_CONF_RDC contikimac_driver 		//	nullrdc_driver or contikimac_driver
#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 256 	// 	x^2 (8,16,32,...)
//
//


#endif /* PROJECT_CONF_H_ */
