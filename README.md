# DHT22/DHT11 driver for ESP

clone this repository into components folder of ESP IDF project 
and use:
```c

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "DHT22.h"

void app_main(void)
{
    dht_sensor_t sensor;
    dht_init(&sensor,GPIO_NUM_4); //set this actual pin number

    while (1) {
        printf("reading DHT ...\n" );
        
        if (dht_check(dht_refresh(&sensor))) {
              printf("temperature: %.1f\n", sensor.temperature)
              printf("humidity: %.1f\n", sensor.humidity)
          }  
          
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}    
```


