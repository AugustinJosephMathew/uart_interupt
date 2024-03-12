
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "uart_handler.h"
#include "stdint.h"
#include "global.h"



#define EX_UART_NUM UART_NUM_1
uint8_t txBytes;

int sendData(const char *logName, const char* data,uint8_t len )
{

    txBytes = uart_write_bytes(UART_NUM_1, data, len);
    ESP_LOGI(logName, "Wrote %d bytes", txBytes);
    return txBytes;
}

void atdata( const char* TAG,uint8_t *data,uint8_t rxBytes)
{
	static uint8_t n=0;
	  static char f=0;
	  printf("data is %s",data);
	  printf("rxBytes %d\n",rxBytes);
	  char q_data =0;
switch(rxBytes)
        {


	case 3:
		if(compare(data,rxBytes,(uint8_t *)"ok\n",3)==0)
		{
			q_data = 1;
		              xQueueSend(xqueue2, &q_data, 1);
		}
		break;
	case 6:
			if(compare(data,rxBytes,(uint8_t *)"error\n",6)==0)
			{
				q_data = 2;
				xQueueSend(xqueue2, &q_data, 1);
				break;
			}
			break;

	case 11:
        	{
//        		int j=0;
//        			char s[15];
//        			for (int i=0;i<n;i++)
//        			{
//        				if(s[i]<'A' && s[i]>'Z')
//        				{
//        					s[j]=data[i];
//        					j++;
//        				}
//        			}



        		if(compare( data,rxBytes,(uint8_t*)"\r\nPB DONE\r\n",11)==0)

        		{

        			printf("okay");
        			q_data=3;
        			xQueueSend(xqueue2,( void * ) &q_data, 1);
        			printf("data sent ,led blink\n");
        			blink_led();
        			printf("Done");
        		}
        	}
        	break;



	default:
			break;

     }
}

bool compare(uint8_t *dat1,uint8_t len,uint8_t *dat2,int dat2len)
{
	int i=0;
	printf("comparing");
	printf("\nstring lenght %d and data lenth %d\n",len,dat2len);
    if (len != dat2len)
    {
    	return 1;
    }

	while(i < len)
	{
        printf("dat %c,%c\n",dat1[i],dat2[i]);
		if(dat1[i] == dat2[i])
		{

			i++;
		}
		else
		{
			return 1;
		}
	}
    printf("compare succes\n");
	return 0;
}

