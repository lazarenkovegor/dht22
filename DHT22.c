#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include <rom/ets_sys.h>
#include "DHT22.h"



void dht_init(dht_sensor_t *sensor, int pinNum)
{
	sensor->pinNum = pinNum;
	sensor->humidity = -100.;
	sensor->temperature = -100.;

	gpio_set_direction(pinNum, GPIO_MODE_INPUT_OUTPUT_OD);
}


bool dht_check(int response)
{
	static const char* TAG = "DHT";

	switch(response) {
		case DHT_TIMEOUT_ERROR :
			ESP_LOGE( TAG, "Sensor Timeout\n" );
			break;
		case DHT_TIMEOUT_ERROR2 :
			ESP_LOGE( TAG, "Sensor Timeout2\n" );
			break;
		case DHT_CHECKSUM_ERROR:
			ESP_LOGE( TAG, "CheckSum error\n" );
			break;

		case DHT_OK:
			return true;

		default :
			ESP_LOGE( TAG, "Unknown error\n" );
	}
	
	return false;
}

int IRAM_ATTR measureImpulse(int pinNum, int usTimeOut) {
	while( gpio_get_level(pinNum)==0 ) {
		if(--usTimeOut < 0) 
			return -1;
		ets_delay_us(1);		
	}

	int uSec = 0;

	while( gpio_get_level(pinNum)==1) {
		if(uSec > usTimeOut) 
			return -1;
		ets_delay_us(1);	
		uSec++;	
	}


	return uSec;
}


#define responseSize 5

int readDHT(dht_sensor_t *sensor)
{
	uint8_t crc= 0;
	uint8_t response[responseSize];
	memset(&response,0,responseSize);

	gpio_set_level( sensor->pinNum, 0 );
	ets_delay_us( 3000 );			

	gpio_set_level( sensor->pinNum, 1 );
	ets_delay_us( 30 );

	int uSec = measureImpulse(sensor->pinNum,150);
	if( uSec<0 ) return DHT_TIMEOUT_ERROR;

	for( int byteIdex = 0; byteIdex < responseSize; byteIdex++ ) {
		for( int bitIndex = 7; bitIndex >=0; bitIndex--){
			uSec = measureImpulse(sensor->pinNum,150);
			if( uSec<0 ) return DHT_TIMEOUT_ERROR2;
			if (uSec > 27) response[ byteIdex ] |= (1 << bitIndex);
		}

		if (byteIdex == responseSize -1) {
				if (crc != response[byteIdex]) return DHT_CHECKSUM_ERROR;
		} else {
				crc+=response[byteIdex];
		}
	}


	sensor->humidity = (response[0] << 8 | response[1]) /10.  ;
	sensor->temperature = ((response[2] & 0x7F) << 8 | response[3]) * (response[2] & 0x8F?-1:1) /10. ;

	return DHT_OK;
}

int dht_refresh(dht_sensor_t *sensor){
	portDISABLE_INTERRUPTS();
	int res  =  readDHT(sensor);
	portENABLE_INTERRUPTS();

	return res;
}