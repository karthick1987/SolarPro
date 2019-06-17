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

/*** LIGHT SENSOR FUNCTION ***/
//function for outputting the lux value read from sensor
//@param m: calibration value m inscribed on the back of the sensor
//@param b: calibration value b inscribed on the back of the sensor
//@param adc_input: phidget input value. Use ZOUL_SENSORS_ADC1 or ZOUL_SENSORS_ADC3 depending on where the sensor is connected to.
//@return int : lux value with a max of 1000.
static int getLightSensorValue(uint16_t adc_value){
	//Read voltage from the phidget interface
	double sensorValue = adc_value/4.096;

	//Convert the voltage in lux with the provided formula
	double luxRaw = 1.4761 * sensorValue + 39.416;

	//Return the value of the light with maximum value equal to 1000
	uint16_t lux = luxRaw;

	if (lux > 1000){
		lux = 1000;
	}
	return lux;
}


/*** JOYSTICK SENSOR FUNCTION ***/
//function for outputting the direction value read from joystick sensor
//@param x: ADC value for x-direction tilting
//@param y: ADC value for y-direction tilting
//@return string: actual direction of the joystick beeing pressed.
const char * getJoystickPosition(uint16_t x, uint16_t y){

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
