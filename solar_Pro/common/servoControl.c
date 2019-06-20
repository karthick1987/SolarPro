
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
#include "dev/pwm.h"

int initServo(void)
{
    GPIO_SOFTWARE_CONTROL(SERVOPORT,SERVOPIN);
    GPIO_SET_OUTPUT(SERVOPORT,SERVOPIN);
    return PWM_SUCCESS;
}

int setServoPosition(int angle)
{
    if (pwm_enable(50,0,20,PWM_TIMER_1,PWM_TIMER_A) == PWM_SUCCESS)
        printf("PWM sucessfully inited\n");
    else
        printf("PWM init FAILED\n");

    pwm_start(PWM_TIMER_1, PWM_TIMER_A, SERVOPORT, SERVOPIN);

    //clock_delay(350000);

    //if (pwm_enable(50,10,20,PWM_TIMER_1,PWM_TIMER_A) == PWM_SUCCESS)
    //pwm_start(PWM_TIMER_1, PWM_TIMER_A, GPIO_B_NUM, 0);

    return PWM_SUCCESS;
}

int getServoPosition(void)
{
    return PWM_SUCCESS;
}

static void calculatePWMFactor(int angle)
{

}

