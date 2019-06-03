
/*
   Wireless Sensor Networks Laboratory

   Technische Universität München
   Lehrstuhl für Kommunikationsnetze
   http://www.lkn.ei.tum.de

   copyright (c) 2017 Chair of Communication Networks, TUM

   contributors:
   * Thomas Szyrkowiec
   * Mikhail Vilgelm
   * Octavio Rodríguez Cervantes
   * Angel Corona

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 2.0 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   LESSON 3: External Sensors.
*/

// Contiki-specific includes:
#include "contiki.h"
#include "net/rime/rime.h"     // Establish connections.
#include "net/netstack.h"      // Wireless-stack definitions
#include "dev/leds.h"          // Use LEDs.
#include "dev/button-sensor.h" // User Button
#include "dev/adc-zoul.h"      // ADC
#include "dev/zoul-sensors.h"  // Sensor functions
#include "dev/sys-ctrl.h"
// Standard C includes:
#include <stdio.h>      // For printf.

// Reading frequency in seconds.
#define TEMP_READ_INTERVAL CLOCK_SECOND*1
#define JOYSTICK_POLLING CLOCK_SECOND/100 //10ms

/*** CONNECTION DEFINITION***/

/**
 * Callback function for received packet processing.
 *
 */
static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from) {

	leds_on(LEDS_GREEN);

	uint8_t len = strlen( (char *)packetbuf_dataptr() );
	int16_t rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);

	printf("Got RX packet (broadcast) from: 0x%x%x, len: %d, RSSI: %d\r\n",from->u8[0], from->u8[1],len,rssi);

	leds_off(LEDS_GREEN);
}

/**
 * Connection information
 */
static struct broadcast_conn broadcastConn;

/**
 * Assign callback functions to the connection
 */
static const struct broadcast_callbacks broadcast_callbacks = {broadcast_recv};

/*** CONNECTION DEFINITION END ***/

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

const char * getJoystickPosition(uint16_t x, uint16_t y){
	// return values can be interpreted as followed:

	if (x<300){
		return "UP";
	}
	else if(x>2000){
		return "DOWN";
	}
	else if(y<300){
		return "LEFT";
	}
	else if(y>1500){
		return "RIGHT";
	}
	else{
		return "IDLE";
	}

}

//--------------------- PROCESS CONTROL BLOCK ---------------------
PROCESS (ext_sensors_process, "External Sensors process");
PROCESS (ext_sensors_joystick_process, "External Sensors Joystick process");
AUTOSTART_PROCESSES (&ext_sensors_process, &ext_sensors_joystick_process);


//------------------------ PROCESS' EXTERNAL JOYSTICK THREAD ------------------------
PROCESS_THREAD (ext_sensors_joystick_process, ev, data){
	static struct etimer joystick_reading_timer;
	const char * direction;
	static uint16_t adc1_value, adc3_value;

	PROCESS_BEGIN();

	/*
	 * set your group's channel
	 */
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL,11);

	/*
	 * open the connection
	 */
	broadcast_open(&broadcastConn,129,&broadcast_callbacks);


	/* Configure the ADC ports */
	adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC1 | ZOUL_SENSORS_ADC3);


	etimer_set(&joystick_reading_timer, JOYSTICK_POLLING);

	while(1){

		PROCESS_WAIT_EVENT();

		if(ev == PROCESS_EVENT_TIMER) {


			/*
			 * Read ADC values. Data is in the 12 MSBs
			 */
			adc1_value = adc_zoul.value(ZOUL_SENSORS_ADC1) >> 4;
			adc3_value = adc_zoul.value(ZOUL_SENSORS_ADC3) >> 4;
			direction = getJoystickPosition(adc1_value, adc3_value);

			if (direction != "IDLE"){
			/*
			 * fill the packet buffer
			 */
			leds_on(LEDS_GREEN);
			packetbuf_copyfrom(direction,10);

			/*
			 * send the message
			 */
			broadcast_send(&broadcastConn);

			leds_off(LEDS_GREEN);
			}

			etimer_set(&joystick_reading_timer, JOYSTICK_POLLING);
		}
	}

	PROCESS_END();

}


//------------------------ PROCESS' EXTERNAL SENSORS THREAD ------------------------
PROCESS_THREAD (ext_sensors_process, ev, data) {

	/* variables to be used */
	static struct etimer temp_reading_timer;
	static uint16_t adc1_value, adc3_value;

	PROCESS_BEGIN ();


	printf("\r\nZolertia RE-Mote external sensors");
	printf("\r\n====================================");

	/*
	 * set your group's channel
	 */
	NETSTACK_CONF_RADIO.set_value(RADIO_PARAM_CHANNEL,11);

	/*
	 * open the connection
	 */
	broadcast_open(&broadcastConn,129,&broadcast_callbacks);


	/* Configure the ADC ports */
	adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC1 | ZOUL_SENSORS_ADC3);


	etimer_set(&temp_reading_timer, TEMP_READ_INTERVAL);

	while (1) {

		PROCESS_WAIT_EVENT();  // let process continue
		/* If timer expired, print sensor readings */
	    if(ev == PROCESS_EVENT_TIMER) {

	    	leds_on(LEDS_PURPLE);

	    	/*
	    	 * Read ADC values. Data is in the 12 MSBs
	    	 */
	    	adc1_value = adc_zoul.value(ZOUL_SENSORS_ADC1) >> 4;
	    	adc3_value = adc_zoul.value(ZOUL_SENSORS_ADC3) >> 4;

	    	/*
	    	 * Print Raw values
	    	 */

	    	printf("\r\nADC1 value [Raw] = %d", adc1_value);
	        printf("\r\nADC3 value [Raw] = %d", adc3_value);

	        //uint16_t light_value1 = getLightSensorValue(adc1_value);
	        //uint16_t light_value3 = getLightSensorValue(adc3_value);
	        //printf("\r\nLuminosity via ADC1 is %d LUX", light_value1);
	        //printf("\r\nLuminosity via ADC3 is %d LUX", light_value3);


    		leds_off(LEDS_PURPLE);

    		etimer_set(&temp_reading_timer, TEMP_READ_INTERVAL);
	    }
    }

	PROCESS_END ();
}

