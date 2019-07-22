
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

// Private includes
#include "servoControl.h"
#include "dev/gpio.h"
#include "dev/servo.h"          // Servo functionality

static int pos;

int initServo(void)
{
    GPIO_SOFTWARE_CONTROL(SERVOPORT,SERVOPIN);
    GPIO_SET_OUTPUT(SERVOPORT,SERVOPIN);
    return PWM_SUCCESS;
}

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

int getServoPosition(void)
{
    return pos;
}
