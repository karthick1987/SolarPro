
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

 This header file is designed for all nodes to read out several sensor values.
 */

#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H

static int servoPos;

int initServo();
int setServoPosition();
int getServoPosition();

#endif
