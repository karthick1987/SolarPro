
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

 This header file is designed for all nodes to define specific Node IDs depending on the RIME ID.
 */

#ifndef BROADCASTCOMMON_H
#define BROADCASTCOMMON_H

#define BROADCASTTIMEOUT        5*CLOCK_SECOND
#define BROADCASTINTERVAL       CLOCK_SECOND/2
#define BROADCASTCHANNEL        129

/**
* @brief Function to stop all broadcast timers
*
* Function call is necessary to ensure proper functionality when transitioning
* to other modes, such as unicasting
*/
void stopAllBroadCastTimer(void);

#endif
