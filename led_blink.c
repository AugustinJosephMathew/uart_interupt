#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "uart_handler.h"
#include "driver/gpio.h"
void blink_led()
{
	uint8_t i=0;
  gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);  // Built-in LED on GPIO 0
    while(i<100)
		{
        gpio_set_level(GPIO_NUM_25,0);
        vTaskDelay(10);
        gpio_set_level(GPIO_NUM_25,1);
        vTaskDelay(10);
        i++;
		}
}
