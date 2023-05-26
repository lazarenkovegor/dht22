/* 
	DHT22 temperature sensor driver
*/

#ifndef DHT22_H_  
#define DHT22_H_

#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2
#define DHT_TIMEOUT_ERROR2 -3

typedef struct {
	int   pinNum;				
	float humidity;
	float temperature;
} dht_sensor_t;


void 	dht_init(dht_sensor_t *sensor, int pinNum);
int 	dht_refresh(dht_sensor_t *sensor);
bool 	dht_check(int response);
#endif