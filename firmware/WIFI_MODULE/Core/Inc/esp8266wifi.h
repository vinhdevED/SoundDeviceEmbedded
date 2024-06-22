/*
 * esp8266wifi.h
 *
 *  Created on: Apr 3, 2024
 *      Author: trand
 */

#ifndef INC_ESP8266WIFI_H_
#define INC_ESP8266WIFI_H_

#include "esp8266config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "stm32f1xx_hal.h"

typedef enum{
	false = 0,
	true = 1
}bool;

extern UART_HandleTypeDef 	_WIFI_USART;


//#########################################################################################
typedef enum {								// WiFi mode for AT+CWMODE
	WIFI_MODE_ERROR =0,
	WIFI_MODE_STA = 1,						// Station
	WIFI_MODE_AP = 2,						// Access point
	WIFI_MODE_APSTA = 3						// Both
} WIFI_MODE;

// IP connection status
typedef enum {								// Return enumeration from AT+CIPSTATUS
	WIFI_STATUS_ERROR,
	WIFI_STATUS_GOTIP,
	WIFI_STATUS_CONN,
	WIFI_STATUS_DISCONN,
	WIFI_STATUS_CONNFAIL,
} WIFI_STATUS;

typedef enum
{
  WifiEncryptionType_Open                 =     0,
  WifiEncryptionType_WPA_PSK              =     2,
  WifiEncryptionType_WPA2_PSK             =     3,
  WifiEncryptionType_WPA_WPA2_PSK         =     4,
}WifiEncryptionType_t;

typedef struct
{
  WIFI_STATUS      			  status;
  uint8_t                     LinkId;
  char                        Type[4];
  char                        RemoteIp[17];
  uint16_t                    RemotePort;
  uint16_t                    LocalPort;
  bool                        RunAsServer;
}WIFI_CONNECTION_t;


//###################################################################################
typedef struct
{
	//----------------Testing Parameter
	bool 						  status;
	uint8_t						      linkId;
	//----------------Usart	Paremeter
	uint8_t                       usartBuff;
	uint8_t                       RxBuffer[_WIFI_RX_SIZE];
	uint8_t                       TxBuffer[_WIFI_TX_SIZE];
	uint16_t                      RxIndex;
	bool                          RxIsData;
	//----------------General	Parameter
	WIFI_MODE                     Mode;
	char                          MyIP[16];
	char                          MyGateWay[16];
	//----------------Wifi SSID	Parameter
	char                          SSID_STATION[16];
	char                          PASSWORD_STATION[16];
	//----------------Station	Parameter
	char						  SSID_Connected[20];
	bool                          StationDhcp;
	char                          StationIp[16];
	//----------------SoftAp Parameter
	bool                          SoftApDhcp;
	char                          SoftApConnectedDevicesIp[6][16];
	char                          SoftApConnectedDevicesMac[6][18];
	//----------------TcpIp Parameter
	bool                          TcpIpMultiConnection;
	uint16_t                      TcpIpPingAnswer;
	WIFI_CONNECTION_t             TcpIpConnections[5];
	//----------------
}Wifi_t;




// Basic functions of ESP8266
bool Wifi_Init(void);
void Wifi_Enable(void);
void Wifi_Disable(void);
bool Wifi_Restart(void);
bool Wifi_FactoryReset(void);
void Wifi_RxCallBack(void);
bool Wifi_SendRaw(uint8_t *data,uint16_t len);
bool Wifi_SetMode(WIFI_MODE WifiMode_);
void Wifi_RxClear(void);
void Wifi_TxClear(void);
bool Wifi_GetMode(void);
bool Wifi_GetIP(void);
void Wifi_FirstInit(WIFI_MODE mode);
bool Wifi_StationConnectToAp(char *SSID,char *Pass, char *MAC);
bool  Wifi_SoftAp_Create(char *SSID,char *password,uint8_t channel,WifiEncryptionType_t WifiEncryptionType);
bool  Wifi_TcpIp_SetMultiConnection(bool EnableMultiConnections);
bool  Wifi_TcpIp_StartTcpConnection(uint16_t RemotePort);
bool Wifi_TcpIp_StartUdpConnection(uint8_t LinkId,char *RemoteIp,uint16_t RemotePort,uint16_t LocalPort);
int Get_after(const char* buffer);
bool  Wifi_TcpIp_SetEnableTcpServer(uint16_t PortNumber);
bool  Wifi_TcpIp_SendDataTcp(uint8_t LinkId,uint16_t dataLen,uint8_t *data);
bool Wifi_TcpIp_Close(uint8_t LinkId);
bool Wifi_SendString(char *data);
bool Wifi_ReturnString(char *result, uint8_t WantWhichOne, char *SplitChars);
bool Wifi_ReturnInteger(int32_t *result, uint8_t WantWhichOne, char *SplitChars);
bool Wifi_WaitForString(uint32_t TimeOut_ms,uint8_t *result,uint8_t CountOfParameter,...);
bool ESP8266_WaitForString(uint32_t Timeout_ms, char *target_str);
bool Wifi_TcpIp_CreateTCPConnection(char *RemoteIp,uint16_t RemotePort);
bool Wifi_Station_SetIp(char *IP,char *GateWay,char *NetMask);
bool Wifi_InteractServerNodeJS(void);

#endif /* INC_ESP8266WIFI_H_ */
