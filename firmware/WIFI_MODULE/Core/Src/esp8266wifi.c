

#include "esp8266config.h"
#include "esp8266wifi.h"
#include "../../Drivers/ssd1306/Inc/ssd1306_tests.h"
extern Wifi_t Wifi;

//SetUp IP
 char *deviceIP = "192.168.1.69";
 char *gateway = "192.168.1.1";
 char *subnet = "255.255.255.0";

void Wifi_FirstInit(WIFI_MODE mode){

	//char *SSID, char *PASS,
	// Enable WIFI module
	Wifi_Enable();

	switch (mode) {
		case WIFI_MODE_STA:

			/********* AT+CWMODE=1 **********/
			if(Wifi_SetMode(WIFI_MODE_STA) == false)
				while(1);

			/********* AT+CIPMUX **********/
			if(Wifi_TcpIp_SetMultiConnection(false)==false)
				while(1);

			/********* AT+CWJAP="SSID","PASSWD" **********/
			if(Wifi_StationConnectToAp(Wifi.SSID_STATION,Wifi.PASSWORD_STATION,NULL) ==false)
			 	while(1);

//			if(Wifi_Station_SetIp(deviceIP, gateway, subnet)==false)
//				while(1);
//
//			/********* AT+CIFSR **********/
//			if(Wifi_GetIP() == false)
//				while(1);
			break;

		case WIFI_MODE_AP:
			/********* AT **********/
			if(Wifi_Init() == false)
				while(1);

			/********* AT+CWMODE=2 **********/
			if(Wifi_SetMode(WIFI_MODE_AP) == false)
				while(1);

			/********* AT+CIPMUX **********/
			if(Wifi_TcpIp_SetMultiConnection(true)==false)
				while(1);

			/********* AT+CIPSERVER **********/
			if(Wifi_TcpIp_StartTcpConnection(80)==false)
				while(1);

			/********* AT+CWSAP **********/
			if(Wifi_SoftAp_Create("MyESP8266AP","12345678",5, WifiEncryptionType_WPA2_PSK) == false)
				while(1);

			/********* AT+CIFSR **********/
			if(Wifi_GetIP() == false)
				while(1);

			Wifi_RxClear();

			break;

		default:
			break;
	}
}

//bool Wifi_InteractServerNodeJS(){
//
//	if(Wifi_TcpIp_CreateTCPConnection("13.56.232.149", 80)==false)
//		while(1);
//
//}


//#########################################################################################################
int Get_after(const char* buffer) {
    const char* substr = "+IPD,";
    uint8_t ipd_index = 0;

    // Tìm vị trí của chuỗi con "+IPD,"
    char* ipd_pos = strstr(buffer, substr);
    if (ipd_pos != NULL) {
        // Lấy vị trí ký tự đầu tiên sau chuỗi con "+IPD,"
        ipd_index = ipd_pos - buffer + strlen(substr);
        // Trả về giá trị đứng sau chuỗi con "+IPD,"
        return buffer[ipd_index] - '0'; // Chuyển đổi ký tự sang số nguyên
    } else {
        // Trường hợp không tìm thấy chuỗi con "+IPD,"
        // Trả về một giá trị mặc định hoặc thực hiện xử lý phù hợp
        return -1; // Đây chỉ là giá trị mặc định, bạn có thể điều chỉnh tùy ý
    }
}


//#########################################################################################################


//#########################################################################################################
bool Wifi_Init(void)
{
	uint8_t result;
	bool returnVal=false;
	// Clean the variables and start the interruption to work with the UART
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT\r\n");

		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result==2)
			break;
		returnVal = true;
		test((char*)Wifi.TxBuffer,"RUNNING...");
		Wifi_RxClear();
		Wifi_TxClear();
		HAL_UART_Receive_IT(&_WIFI_USART,&Wifi.usartBuff,1);
	}while(0);
	return returnVal;
}

//#########################################################################################################
bool Wifi_SendRaw(uint8_t *data,uint16_t len)
{
	if(len <= _WIFI_TX_SIZE)
	{
		// Send the information in data through the UART of the ESP8266
		memcpy(Wifi.TxBuffer,data,len);
		if(HAL_UART_Transmit(&_WIFI_USART,data,len,900) == HAL_OK)
			return true;
		else
			return false;
	}
	else
		return false;
}

//#########################################################################################################
void Wifi_RxClear(void)
{
	// Delete all data in the RxBuffer array

	memset(Wifi.RxBuffer,0,_WIFI_RX_SIZE);
	Wifi.RxIndex=0;
	HAL_UART_Receive_IT(&_WIFI_USART,&Wifi.usartBuff,1);
}

//#########################################################################################################
void Wifi_TxClear(void)
{
	memset(Wifi.TxBuffer,0,_WIFI_TX_SIZE);
}

//#########################################################################################################
bool Wifi_SendString(char *data)
{
	return Wifi_SendRaw((uint8_t*)data,strlen(data));
}

//#########################################################################################################
void Wifi_Enable(void){
	// It is necessary to set HIGH both pins of the module to work correctly
	HAL_GPIO_WritePin (_BANK_WIFI_BUTTONS,_BUTTON_ENABLE, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (_BANK_WIFI_BUTTONS,_BUTTON_RST, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin (_BANK_WIFI_BUTTONS,_BUTTON_ENABLE, GPIO_PIN_SET);
	HAL_GPIO_WritePin (_BANK_WIFI_BUTTONS,_BUTTON_RST, GPIO_PIN_SET);
	HAL_Delay(2000);
}

//#########################################################################################################
void Wifi_Disable(void){
	// It is necessary to set LOW both pins of the module to disable correctly
	HAL_GPIO_WritePin (_BANK_WIFI_BUTTONS,_BUTTON_ENABLE, GPIO_PIN_SET);
	HAL_GPIO_WritePin (_BANK_WIFI_BUTTONS,_BUTTON_RST, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin (_BANK_WIFI_BUTTONS,_BUTTON_ENABLE, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (_BANK_WIFI_BUTTONS,_BUTTON_RST, GPIO_PIN_RESET);
}

//#########################################################################################################
bool	Wifi_Restart(void)
{
	// Make a restart of the ESP8266 using the AT Commands

	uint8_t result;
	bool	returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+RST\r\n");
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;			// The timeout was completed and the string was not there
		if(result == 2)		// It was find the "ERROR" String in the receiving information
			break;
		returnVal=true;
		Wifi_RxClear();
		Wifi_TxClear();
	}while(0);
	return returnVal;
}

bool Wifi_FactoryReset(void){

	uint8_t result;
	bool returnVal = false;

	do {
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+RESTORE\r\n");
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;			// The timeout was completed and the string was not there
		if(result == 2)		// It was find the "ERROR" String in the receiving information
			break;
		returnVal=true;
	} while (0);
	return returnVal;
}

//#########################################################################################################

bool Wifi_WaitForString(uint32_t TimeOut_ms,uint8_t *result,uint8_t CountOfParameter,...)
{
	/*
	 * It uses the CountOfParameter and the Parameters after that to compare with the
	 * information that it was received in the UART RX. If the parameter is in the
	 * received string the functions return a true value and the position of the
	 * parameter (according to the position in the function)
	 *
	 * Ex:
	 * Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")
	 *
	 * If the ESP8266 return a AT+OK after the last command, the function is going to
	 * return a true value and the result number would be 1.
	 */

	if(result == NULL)
		return false;
	if(CountOfParameter == 0)
		return false;

	*result=0;

	va_list tag;
		va_start (tag,CountOfParameter);
		char *arg[CountOfParameter];
		for(uint8_t i=0; i<CountOfParameter ; i++)
			arg[i] = va_arg (tag, char *);
	va_end (tag);

	for(uint32_t t=0 ; t<TimeOut_ms ; t+=20)
	{
		HAL_Delay(20);
		for(uint8_t	mx=0 ; mx<CountOfParameter ; mx++)
		{
			if(strstr((char*)Wifi.RxBuffer,arg[mx])!=NULL)
			{
				*result = mx+1;
				return true;
			}
		}
	}
	// timeout
	return false;

}

void Wifi_RxCallBack(void)
{
	Wifi.RxBuffer[Wifi.RxIndex] = Wifi.usartBuff;
	if(Wifi.RxIndex < _WIFI_RX_SIZE)
	  Wifi.RxIndex++;
	HAL_UART_Receive_IT(&_WIFI_USART,&Wifi.usartBuff,1);
}




bool Wifi_SetMode(WIFI_MODE WifiMode_){
	uint8_t result;
	bool returnVal = true;

	do{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CWMODE_CUR=%d\r\n",WifiMode_);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;			// The timeout was completed and the string was not there
		if(result == 2)		// It was find the "ERROR" String in the receiving information
			break;
		Wifi.Mode = WifiMode_;
		returnVal=true;
		test((char*)Wifi.TxBuffer,"RUNNING...");
		Wifi_RxClear();
		Wifi_TxClear();
	}while(0);
	return returnVal;
}

bool Wifi_GetMode(void){
	uint8_t result;
	bool	returnVal=false;

	do {
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CWMODE_CUR?\r\n");
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;			// The timeout was completed and the string was not there
		if(result == 2)		// It was find the "ERROR" String in the receiving information
			break;
		if(Wifi_ReturnInteger((int32_t*)&result, 1, ":"))

			Wifi.Mode = (WIFI_MODE)result;
		else
			Wifi.Mode = WIFI_MODE_ERROR;

		returnVal=true;
	} while (0);
	return returnVal;
}

bool  Wifi_TcpIp_SetEnableTcpServer(uint16_t PortNumber)
{
	uint8_t result;
	bool	returnVal=false;
	do
	{
		Wifi_RxClear();
		if(Wifi.TcpIpMultiConnection==false)
		{
			// it actives the Multiconnection first to make a TCP Server
			sprintf((char*)Wifi.TxBuffer,"AT+CIPMUX=1\r\n");
			if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
				break;
			if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
				break;			// The timeout was completed and the string was not there
			if(result == 2)		// It was find the "ERROR" String in the receiving information
				break;
			Wifi.TcpIpMultiConnection=true;
			Wifi_RxClear();
		}
		else
		  sprintf((char*)Wifi.TxBuffer,"AT+CIPSERVER=1,%d\r\n",PortNumber);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;			// The timeout was completed and the string was not there
		if(result == 2)		// It was find the "ERROR" String in the receiving information
			break;
		returnVal=true;
	}while(0);
	return returnVal;
}

bool Wifi_StationConnectToAp(char *SSID,char *Pass, char *MAC){
	uint8_t result;
	bool returnVal = false;

	do {
		/*
		 * It connects to a WiFi network who has all the parameters correctly
		 */
		Wifi_RxClear();
		if(MAC == NULL)
			sprintf((char*)Wifi.TxBuffer,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",SSID,Pass);
		else
			sprintf((char*)Wifi.TxBuffer,"AT+CWJAP_CUR=\"%s\",\"%s\",\"%s\"\r\n",SSID,Pass,MAC);

		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;

		if(Wifi_WaitForString(_WIFI_WAIT_TIME_VERYHIGH,&result,3,"\r\nOK\r\n","\r\nERROR\r\n","\r\nFAIL\r\n")==false)
			break;

		if(result > 1)		// If the result is higher to 1 is because there were an error
			break;

		returnVal=true;
	} while (0);
	return returnVal;
}

bool Wifi_Station_SetIp(char *IP,char *GateWay,char *NetMask)
{
	uint8_t result;
	bool	returnVal=false;
	do
	{
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CIPSTA=\"%s\",\"%s\",\"%s\"\r\n",IP,GateWay,NetMask);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;			// The timeout was completed and the string was not there
		if(result == 2)		// It was find the "ERROR" String in the receiving information
			break;
		Wifi.StationDhcp=false;
		returnVal=true;
	}while(0);
	return returnVal;
}

bool  Wifi_SoftAp_Create(char *SSID,char *password,uint8_t channel,WifiEncryptionType_t WifiEncryptionType)
{
	uint8_t result;
	bool	returnVal=false;
	do
	{
		/*
		 * It creates the SoftAP (Small WiFi Network) that it is possible with the ESP8266
		 * the data of this network is defined by the inputs of the function
		 */
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n",SSID,password,channel,WifiEncryptionType);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;			// The timeout was completed and the string was not there
		if(result == 2)		// It was find the "ERROR" String in the receiving information
			break;

		returnVal=true;
		Wifi_TxClear();
		test((char*)Wifi.TxBuffer,"RUNNING...");
	}while(0);
	return returnVal;
}

bool Wifi_GetIP(void){
	uint8_t result;
	bool returnVal = false;
	do {
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer, "AT+CIFSR\r\n");
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW, &result, 2,"OK","ERROR") == false)
			break;
		if(result == 2)
			break;
		sscanf((char*)Wifi.RxBuffer,"AT+CIFSR\r\n\n+CIFSR:APIP,\"%[^\"]",Wifi.MyIP);
		sscanf((char*)Wifi.RxBuffer,"AT+CIFSR\r\r\n+CIFSR:STAIP,\"%[^\"]",Wifi.MyIP);
		returnVal = true;
		test((char*)Wifi.TxBuffer,"RUNNING...");
	} while (0);
	return returnVal;
}

bool  Wifi_TcpIp_SetMultiConnection(bool EnableMultiConnections)
{
	uint8_t result;
	bool	returnVal=false;
	do
	{
		// Enable or Disable the multiconnection possibility
		Wifi_RxClear();
		sprintf((char*)Wifi.TxBuffer,"AT+CIPMUX=%d\r\n",EnableMultiConnections);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(result == 2)
			break;
    Wifi.TcpIpMultiConnection=EnableMultiConnections;
		returnVal=true;
		test((char*)Wifi.TxBuffer,"RUNNING...");
	}while(0);
	return returnVal;
}

bool  Wifi_TcpIp_StartTcpConnection(uint16_t RemotePort)
{
	uint8_t result;
	bool	returnVal=false;
	do
	{
		/*
		 * It makes a TCP server and then it creates a TCP Connection according to the
		 * settings in the function. It uses a very high time of waiting because the
		 * ESP8266 takes a lot of time to create a connection with a TCP the first time.
		 */
		Wifi_RxClear();
		if(Wifi.TcpIpMultiConnection==true){
			sprintf((char*)Wifi.TxBuffer,"AT+CIPSERVER=1,%d\r\n",RemotePort);
			if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
				break;
			if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
				break;			// The timeout was completed and the string was not there
			if(result == 2)		// It was find the "ERROR" String in the receiving information
				break;
		}
		//Wifi_RxClear();
//		if(Wifi.TcpIpMultiConnection==false)
//		  sprintf((char*)Wifi.TxBuffer,"AT+CIPSTART=\"TCP\",\"%s\",%d,%d\r\n",RemoteIp,RemotePort,TimeOut);
//		else
//		  sprintf((char*)Wifi.TxBuffer,"AT+CIPSTART=%d,\"TCP\",\"%s\",%d,%d\r\n",LinkId,RemoteIp,RemotePort,TimeOut);
//		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
//			break;
//		if(Wifi_WaitForString(_WIFI_WAIT_TIME_HIGH,&result,3,"OK","CONNECT","ERROR")==false)
//			break;
//		if(result == 3)
//			break;
		returnVal=true;
		test((char*)Wifi.TxBuffer,"RUNNING...");
	}while(0);
	return returnVal;
}

bool Wifi_TcpIp_CreateTCPConnection(char *RemoteIp,uint16_t RemotePort){
	uint8_t result;
		bool	returnVal=false;
	do
		{
	    Wifi_RxClear();
		if(Wifi.TcpIpMultiConnection==false)
		  sprintf((char*)Wifi.TxBuffer,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",RemoteIp,RemotePort);
//		else
//		  sprintf((char*)Wifi.TxBuffer,"AT+CIPSTART=%d,\"TCP\",\"%s\",%d,%d\r\n",LinkId,RemoteIp,RemotePort,TimeOut);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_HIGH,&result,3,"OK","CONNECT","ERROR")==false)
			break;
		if(result == 3)
			break;
		returnVal=true;
		}while(0);
	return returnVal;
}

bool  Wifi_TcpIp_SendDataTcp(uint8_t LinkId,uint16_t dataLen,uint8_t *data)
{
	uint8_t result;
	bool	returnVal=false;
	do
	{
		Wifi_RxClear();
		if(Wifi.TcpIpMultiConnection==false)
			sprintf((char*)Wifi.TxBuffer,"AT+CIPSEND=%d\r\n",dataLen);
		else
			sprintf((char*)Wifi.TxBuffer,"AT+CIPSEND=%d,%d\r\n",LinkId,dataLen);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,3,">","ERROR","busy")==false)
			break;
		if(result > 1)
			break;
		Wifi_RxClear();
//		//Wifi_SendString(data);
		Wifi_TxClear();
		Wifi_SendRaw(data,dataLen);
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;
		returnVal=true;
	}while(0);
	return returnVal;
}

bool  Wifi_TcpIp_StartUdpConnection(uint8_t LinkId,char *RemoteIp,uint16_t RemotePort,uint16_t LocalPort)
{
	uint8_t result;
	bool	returnVal=false;
	do
	{
		Wifi_RxClear();
		if(Wifi.TcpIpMultiConnection==false)
		  sprintf((char*)Wifi.TxBuffer,"AT+CIPSTART=\"UDP\",\"%s\",%d,%d,2\r\n",RemoteIp,RemotePort,LocalPort);
		else
		  sprintf((char*)Wifi.TxBuffer,"AT+CIPSTART=%d,\"UDP\",\"%s\",%d,%d,2\r\n",LinkId,RemoteIp,RemotePort,LocalPort);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_HIGH,&result,3,"OK","ALREADY","ERROR")==false)
			break;
		if(result == 3)		// If the RX String returns a ERROR the function breaks and
			break;			// send a false
		returnVal=true;
//		Wifi_TxClear();
//		Wifi_RxClear();
	}while(0);
	return returnVal;
}

bool  Wifi_TcpIp_Close(uint8_t LinkId)
{
	uint8_t result;
	bool	returnVal=false;
	do
	{
		Wifi_RxClear();
		if(Wifi.TcpIpMultiConnection==false)
		  sprintf((char*)Wifi.TxBuffer,"AT+CIPCLOSE\r\n");
		else
		  sprintf((char*)Wifi.TxBuffer,"AT+CIPCLOSE=%d\r\n",LinkId);
		if(Wifi_SendString((char*)Wifi.TxBuffer)==false)
			break;
		if(Wifi_WaitForString(_WIFI_WAIT_TIME_LOW,&result,2,"OK","ERROR")==false)
			break;			// The timeout was completed and the string was not there
		if(result == 2)		// It was find the "ERROR" String in the receiving information
			break;
		returnVal=true;
		Wifi_RxClear();
		Wifi_TxClear();

	}while(0);
	return returnVal;
}


bool Wifi_ReturnString(char *result, uint8_t WantWhichOne, char *SplitChars){
	/*
	 * FUNCTION - strtok()
	 * In the first call, strtok() takes the first parameter of the string to be split,
	 * from the second call onwards, strtok takes the first parameter which is a NULL pointer.
	 */
	if(result == NULL)
		return false;

	if(WantWhichOne == 0)
		return false;

	char *str = (char *)Wifi.RxBuffer;

	str = strtok(str,SplitChars);

	while(str != NULL){
		str = strtok(NULL,SplitChars);
		if(str != NULL){
			WantWhichOne--;
		}
		if(WantWhichOne ==0){
			strcpy(result, str);
			return true;
		}
	}

	strcpy(result,"");
	return false;
}

bool Wifi_ReturnInteger(int32_t *result, uint8_t WantWhichOne, char *SplitChars){

	if((char*)Wifi.RxBuffer == NULL)
		return false;
	if(Wifi_ReturnString((char*)Wifi.RxBuffer, WantWhichOne, SplitChars) == false)
		return false;
	*result = atoi((char*)Wifi.RxBuffer);
	return true;
}

