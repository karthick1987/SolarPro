/******************************************************************************
   Wireless Sensor Networks Laboratory 2019 -- Group 1

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
http://www.lkn.ei.tum.de

copyright (c) 2019 Chair of Communication Networks, TUM

project: SolarPro

contributors:
 * Karthik Sukumar
 * Johannes Machleid

 *****************************************************************************/

 /**
 * @file unicast_local.h
 * @author Karthik Sukumar & Johannes Machleid
 * @brief Functions to handle unicast messages specifically in solar Panel
 *
 */

#ifndef UNICASTPANEL_H
#define UNICASTPANEL_H

#include "nodeID.h"
#include "routing.h"

int processUniCast(node_num_t dest, payload_t *);

#endif
