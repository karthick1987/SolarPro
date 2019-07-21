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

 This c-file is designed for all nodes to read out several sensor values.
 */

// Std file includes
#include <stdio.h>

// Contiki includes
#include "dev/adc-zoul.h"      // ADC
#include "dev/zoul-sensors.h"  // Sensor functions
#include "dev/sys-ctrl.h"

// Private includes
#include "projSensors.h"

/*** LIGHT SENSOR FUNCTION ***/
//function for outputting the lux value read from sensor
//@param adc_input: phidget input value. Use ZOUL_SENSORS_ADC1 or ZOUL_SENSORS_ADC3 depending on where the sensor is connected to.
//@return uint16_t : qualitative luminosity value of the sensor (not calculated into lux).
uint16_t getLightSensorValue(void){
	//Configure the ADC ports
	adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC1 );

	//Read ADC1 value. Data is in the 12 MSBs
	uint16_t adc_value = adc_zoul.value(ZOUL_SENSORS_ADC1) >> 4;

	//Read voltage from the phidget interface
	uint16_t sensorValue = adc_value*3.3/4.096;
	//double luxRaw = 1.4761 * sensorValue + 39.416;

	//Return the value of the light with maximum value equal to 1000
	//uint16_t lux = luxRaw;

    // Saturation value
	//if (lux > 1000){
	//	lux = 1000;
	//}
	return sensorValue;
}


/*** JOYSTICK SENSOR FUNCTION ***/
//function for outputting the direction value read from joystick sensor
//@return int: actual direction of the joystick beeing pressed.
int getJoystickPosition(void){
	static uint16_t x, y;
	/* Configure the ADC ports */
	adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC1 | ZOUL_SENSORS_ADC3);

	// Read ADC values. Data is in the 12 MSBs
	x = adc_zoul.value(ZOUL_SENSORS_ADC1) >> 4;
	y = adc_zoul.value(ZOUL_SENSORS_ADC3) >> 4;

	if (x<300){
		return UP;
	}
	else if(x>2000){
		return DOWN;
	}
	else if(y<300){
		return LEFT;
	}
	else if(y>1500){
		return RIGHT;
	}
	else{
		return IDLE;
	}

}


/*** TEMPERATURE SENSOR FUNCTION INTERNAL ***/
//function for outputting the temperature value of the zolertia remote
//@return int: onboard temperature in mC.
int getInternalTemperature(void){

	return cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
}


/*** BATTERY VOLTAGE FUNCTION INTERNAL ***/
//function for outputting the temperature value of the zolertia remote
//@return int: battery voltage in mV
uint16_t getBatteryVoltage(void){

	return vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
}
