/*
 * uart_hanler.h
 *
 *  Created on: 06-Mar-2024
 *      Author: augustinmathew
 */

#ifndef MAIN_UART_HANDLER_H_
#define MAIN_UART_HANDLER_H_


int sendData(const char *logName, const char* data,uint8_t len );
void atdata(const char *TAG,uint8_t *data,uint8_t rxBytes);
bool compare(uint8_t *dat1,uint8_t len,uint8_t *dat2,int dat2len);
void blink_led();


#endif /* MAIN_UART_HANDLER_H_ */
