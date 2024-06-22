/*
 * esp8266config.h
 *
 *  Created on: Apr 7, 2024
 *      Author: trand
 */

#ifndef INC_ESP8266CONFIG_H_
#define INC_ESP8266CONFIG_H_

#define 	_WIFI_USART  					huart1
#define		_WIFI_RX_SIZE					2024
#define		_WIFI_RX_FOR_DATA_SIZE			1024
#define		_WIFI_TX_SIZE					1024
#define		_WIFI_TASK_SIZE					512
#define 	_MAX_SEND_BYTES					2048

// Buttons of the ESP8266 Module
#define		_BANK_WIFI_BUTTONS				GPIOA
#define		_BUTTON_RST						GPIO_PIN_2
#define		_BUTTON_ENABLE					GPIO_PIN_11

// The times are defined in ms
#define		_WIFI_WAIT_TIME_LOW				1000
#define		_WIFI_WAIT_TIME_MED				5000
#define		_WIFI_WAIT_TIME_HIGH			15000
#define		_WIFI_WAIT_TIME_VERYHIGH		25000


#endif /* INC_ESP8266CONFIG_H_ */
