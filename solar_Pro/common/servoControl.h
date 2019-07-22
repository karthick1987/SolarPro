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
 * @file servoControl.h
 * @author Karthik Sukumar & Johannes Machleid
 * @brief functions to operate the digital servo motor
 *
 */

#ifndef SERVOCONTROL_H
#define SERVOCONTROL_H

/**
 * \name define the GPIO port of the digital servo and the emergency angle
 * @{
 */
#define SERVOPORT   GPIO_A_NUM
#define SERVOPIN    2

#define EMERGENCY_ANGLE 90
/** @} */


int initServo(void);
int setServoPosition(int angle);
int getServoPosition(void);

#endif
