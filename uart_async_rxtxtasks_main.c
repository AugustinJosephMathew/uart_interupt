/* UART asynchronous example, that uses separate RX and TX tasks

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "stdint.h"

static const int RX_BUF_SIZE = 256;
uint8_t txBytes;
uint8_t rxBytes;
#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_5)
QueueHandle_t xQueue1;

void init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_DEFAULT,
    };
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

int sendData(const char* logName, const char* data,uint8_t len )
{
//    uint8_t len = strlen(data);
    txBytes = uart_write_bytes(UART_NUM_1, data, len);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}

static void tx_task(void *arg)
{

    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);

    char data =0;
    while(1)
    {
    	static uint8_t n=0;
    	static char f=0;
    	char  ch;
		if(xQueueReceive(xQueue1,&data, (TickType_t)1)){
			printf("%d",data);
			switch(data)
			{
			case 1:
				char disp1[20];
				n++;
				uint8_t len1 = sprintf(disp1,"AT%d\n",n);
				printf("len1 %d",len1);
				sendData(TX_TASK_TAG, disp1,len1);
				f=0;
				break;
			case 2:
				  printf("f=%d",f);
				if(f >= 2)
				{
					n=0;
					f=0;
				}
				f++;

				char disp2[10];
				uint8_t len2 = sprintf(disp2,"AT%d\n",n);
				printf("len2 %d",len2);
			   sendData(TX_TASK_TAG,disp2,len2);
			   break;

			default:
				printf("Error");
				break;
			}
		}

		vTaskDelay(10);
    }


}

bool compare(uint8_t *dat1,uint8_t len,uint8_t *dat2,int dat2len)
{
	uint8_t i=0;

	printf("string lenght %d",len);
    if (len != dat2len)
    {
    	return 1;
    }

	while(i < len)
	{
		if(dat1[i] == dat2[i])
		{
			i++;
		}
		else
		{
			return 1;
		}
	}
	return 0;
}


static void rx_task(void *arg)
{

    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    char q_data =0;
    while (1)
    {
        rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 200 / portTICK_PERIOD_MS);
        if (rxBytes)
        {
        printf("rxBytes %d\n",rxBytes);


        if(compare(data,rxBytes,(uint8_t *)"ok\n",3)==0)
        {
        	printf("okay");

        	q_data = 1;
              xQueueSend(xQueue1, &q_data, 1);

        }


		if(compare(data,rxBytes,(uint8_t *)"error\n",6)==0)
		{

            q_data = 2;
            xQueueSend(xQueue1, &q_data, 1);

		}

        if (rxBytes > 0)
        {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, " %d bytes: OKAY", rxBytes) ;
            ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);
        }

        }
    }
    free(data);
}

void app_main(void)
{
    init();

    printf( "[APP] befour Free memory: %ld bytes\n", esp_get_free_heap_size());
    xQueue1 = xQueueCreate( 4, sizeof(char ) );
    xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(tx_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    printf( "[APP] after Free memory: %ld bytes", esp_get_free_heap_size());
}
