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
 * @file servoControl.c
 * @author Karthik Sukumar & Johannes Machleid
 * @brief functions to operate the digital servo motor
 *
 */

// Std file includes
#include <stdio.h>

// Private includes
#include "servoControl.h"
#include "dev/gpio.h"
#include "dev/servo.h"          // Servo functionality

static int pos;

/**
* @brief Function to initialize the GPIO port to control the servo
*
*/
int initServo(void)
{
    GPIO_SOFTWARE_CONTROL(SERVOPORT,SERVOPIN);
    GPIO_SET_OUTPUT(SERVOPORT,SERVOPIN);
    return PWM_SUCCESS;
}

/**
* @brief Function to set the servo position
* @param angle is an integer, which defines the servo angle from 0 to 180 degrees
*
* If the passed angle is greater than 180 (out of reach) we assume, that this is
* and emergency call and set the angle to the specified emergency angle
*/
int setServoPosition(int angle)
{
    if(angle>180)
    {
      servo_position(SERVO_CHANNEL_2, SERVOPORT, SERVOPIN, EMERGENCY_ANGLE);
    }
    else
    {
      servo_position(SERVO_CHANNEL_2, SERVOPORT, SERVOPIN, angle);
    }
    pos = angle;
    return PWM_SUCCESS;
}

/**
* @brief Function to get the servo position
* @return pos is the position (angle) in degrees
*
*/
int getServoPosition(void)
{
    return pos;
}
