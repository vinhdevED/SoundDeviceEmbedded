/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../Drivers/ssd1306/Inc/ssd1306.h"
#include "../../Drivers/ssd1306/Inc/ssd1306_tests.h"
#include "esp8266wifi.h"
#include "math.h"
#include "DS3231.h"
#include "stdbool.h"
#include  "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
Wifi_t Wifi;
uint8_t init =1;
uint8_t button_resetDevice = 1;
uint8_t actionProtocol =0;

// Global variable to keep track of LED state
volatile uint8_t led_state = 0;
bool stateReset = true;
volatile bool sendFlag = false;

const char *deviceName = "\"ST-ESP-SZ_1-V-AMNH\"";


char message[50];
char jsonData[100];


char response[]="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
	 	 	 "<!DOCTYPE html>\r\n"
	 	 	 "<html lang=\"en\">\r\n"
	 	 	 "<head>\r\n"
	 	 	 "    <title>WiFi Configuration</title>\r\n"
	 	 	 "</head>\r\n"
		     "<body>\r\n"
			 "    <h1>Da dang nhap thanh cong</h1>\r\n"
			 "</body>\r\n"
	 	 	 "</html>\r\n";

char http_response[] =  "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
		 	 	 	 	 "<!DOCTYPE html>\r\n"
		 	 	 	 	 "<html lang=\"en\">\r\n"
		 	 	 	 	 "<head>\r\n"
		 	 	 	 	 "    <title>WiFi Configuration</title>\r\n"
		 	 	 	 	 "</head>\r\n"
		 	 	 	 	 "<body>\r\n"
		 	 	 	 	 "    <h1>WiFi Configuration</h1>\r\n"
		 	 	 	 	 "    <form action=\"/submit\" method=\"post\">\r\n"
		 	 	 	 	 "        <label for=\"ssid\">SSID (WiFi Name):</label><br>\r\n"
		 	 	 	 	 "        <input type=\"text\" id=\"ssid\" name=\"ssid\"><br>\r\n"
		 	 	 	 	 "        <label for=\"password\">Password:</label><br>\r\n"
		 	 	 	 	 "        <input type=\"password\" id=\"password\" name=\"password\"><br><br>\r\n"
		 	 	 	 	 "        <input type=\"submit\" value=\"Submit\">\r\n"
		 	 	 	 	 "    </form>\r\n"
		 	 	 	 	 "</body>\r\n"
		 	 	 	 	 "</html>\r\n";


/*----------------------------------------------TIME RTC-----------------------------------------*/
float rtcTemp;
_RTC rtc ;
/*-----------------------------------------------------------------------------------------------*/
#define sampleWindow 50
uint16_t sample;
volatile uint16_t signalMax = 0;
volatile uint16_t signalMin = 4095;
double volts, first,dBValue=0.0;


const uint16_t levelShift = 676;  // Adjust for your reference voltage
const uint8_t timeIntervalSampling = 30;

static uint16_t audioMin = 4095;
	        static uint16_t audioMax = 0;

uint16_t sampleCounter;
uint16_t audioInRaw;
int audioInRectified;
int audioPeakToPeak;
float audioRMSPrecalc;
float audioRMS;
float audioRMSFiltered;
float audiodBSPL;
float audioVoltagePeakToPeak;
float filteredAudioVoltagePeakToPeak ;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
bool containSubstring(uint8_t *source, int source_len,char *substring) ;
void microphoneADCReading();
void sendDataToServer();
void floatToString2(float number, char* str) ;
void interactiveWithCloudServer(char *data);
float lowPassFilter(float inputValue, float alpha);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */




void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM2) {
    if(init==1){
    	led_state = !led_state;
    	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, led_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }else{
    	sendFlag = true;

    }
  }
  else if(htim->Instance == TIM3){
	  microphoneADCReading();

	  memset(message,0,sizeof(message));
	  floatToString2(audiodBSPL, message);

  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_8){
		stateReset = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);
	}
	else if(GPIO_Pin == RESET_BUTTON_DEVICE_Pin){
		button_resetDevice = HAL_GPIO_ReadPin(RESET_BUTTON_DEVICE_GPIO_Port, RESET_BUTTON_DEVICE_Pin);
	}
	else{
		__NOP();
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if(hadc->Instance == hadc1.Instance){
		audioInRaw = HAL_ADC_GetValue(hadc);
		audioInRectified = abs(audioInRaw - levelShift);


		audioMin = (audioInRectified < audioMin) ? audioInRectified : audioMin;
		audioMax = (audioInRectified > audioMax) ? audioInRectified : audioMax;
	}

}



void microphoneADCReading() {

	            audioPeakToPeak = audioMax - audioMin;
	            audioVoltagePeakToPeak = (((float)audioPeakToPeak * 3.3) / 4095.0)*0.707;
	            float alpha = 0.1; // Adjust alpha as needed
	            filteredAudioVoltagePeakToPeak = lowPassFilter(audioVoltagePeakToPeak, alpha);

	            audiodBSPL = 94 + 20 * log10(filteredAudioVoltagePeakToPeak / 0.00631) - 44 - 25;

	            if(audiodBSPL >=58){
	      	  	          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
	            	      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	      	    }else if(audiodBSPL <58){
	      	  	          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
	      	    	      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	      	    }

	            // Reset variables for the next measurement period
	            audioMin = 4095;
	            audioMax = 0;

	        HAL_ADC_Start_IT(&hadc1);
}


float lowPassFilter(float inputValue, float alpha) {
	 static float filteredValue = 0.0;
    // Apply low-pass filter equation: filteredValue = alpha * inputValue + (1 - alpha) * filteredValue
    filteredValue = (float)(alpha * inputValue + (1.0 - alpha) * filteredValue);

    return filteredValue;
}

void floatToString2(float number, char* str) {
    int integerPart = (int)number;
    int decimalPart = (int)((number - integerPart) * 100); // Assuming 2 decimal places

    if (number < 0) {
        	decimalPart*=-1;
    }
    sprintf(str, "%d.%02d", integerPart, decimalPart);
}

bool containSubstring(uint8_t *source, int source_len,char *substring) {
	int sub_len = strlen(substring);

    if (source_len < sub_len) {
        return false;
    }

    for (int i = 0; i <= source_len - sub_len; ++i) {
        if (memcmp(&source[i], substring, sub_len) == 0) {
            return true;
        }
    }

    return false;
}

int is_not_empty(char *str) {
    return (str != NULL && str[0] != '\0');
}

void analyzeBufferTime(char* bufferTime){
	sscanf(bufferTime,"%4d-%2d-%2dT%2d:%2d:%2d",&rtc.Year,&rtc.Month, &rtc.Date, &rtc.Hour, &rtc.Min, &rtc.Sec);
	rtc.Year = rtc.Year%100;
}

bool packageDataSecret(char *data){
	char httpRequest[256];
	sprintf(httpRequest, "POST /endpoint HTTP/1.1\r\nHost: 13.56.232.149\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s\r\n", strlen(data), data);

	//Send Package Data
    if(Wifi_TcpIp_SendDataTcp(0, strlen(httpRequest),(uint8_t*)httpRequest) == false)
		return false;

	return true;

}

bool tryConnectCloudServer(){
	//Connnect to Server IP and Server Port 13.56.232.149
    if(Wifi_TcpIp_CreateTCPConnection("13.56.232.149", 80)==false){
    	    led_state = 0;
			return false;
	}else{
		 led_state = 1;
	}

    return true;
}


void sendDataToServer(){
//	memset(message,0,sizeof(message));
//	floatToString2(audiodBSPL, message);
	sprintf(jsonData,"{\"deviceId\":%s,\"soundLevel\":%s}",deviceName,message);

	if(tryConnectCloudServer()){

		while(packageDataSecret(jsonData)==false);
	}
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, led_state ? GPIO_PIN_SET : GPIO_PIN_RESET);

//	showMonitoring(message);

}

void handleRequestWifiConfig(int linkID){
	if(containSubstring(Wifi.RxBuffer, sizeof(Wifi.RxBuffer), "GET /HOME") == 1){
		if(Wifi_TcpIp_SendDataTcp(linkID, sizeof(http_response),(uint8_t*)http_response ) == false)
		      while(1);
		if(Wifi_TcpIp_Close(linkID) == false)
			while(1);


	}else if(containSubstring(Wifi.RxBuffer, sizeof(Wifi.RxBuffer), "POST /submit") == 1){
		char *body_start =strstr((char*)&Wifi.RxBuffer, "ssid=");
		if(body_start != NULL){
			const char *ssid_start = strstr(body_start, "ssid=")+5;
			const char *password_start = strstr(body_start, "password=")+9;

			// Tìm vị trí kết thúc của ssid (kí tự &)
			const char *ssid_end = strchr(ssid_start, '&');
			// Tìm vị trí kết thúc của password (kí tự null)
			const char *password_end = strchr(password_start, '\0');

			if(ssid_end !=NULL){
				size_t len = ssid_end-ssid_start;
				strncpy(Wifi.SSID_STATION,ssid_start,len);
			}

			size_t len = password_end - password_start;
			strncpy(Wifi.PASSWORD_STATION, password_start, len);
			Wifi.PASSWORD_STATION[len] = '\0';
			Wifi_TxClear();
			if(Wifi_TcpIp_SendDataTcp(linkID, sizeof(response),(uint8_t*)response ) == false)
				while(1);
			if(Wifi_TcpIp_Close(linkID) == false)
				while(1);
		}
	}
}

void Devcice_getTimeFromHTTP(){
	char httpRequest[] = "GET /api/timezone/Asia/Ho_Chi_Minh HTTP/1.1\r\nHost: worldtimeapi.org\r\nConnection: close\r\n\r\n";

	while(Wifi_TcpIp_CreateTCPConnection("worldtimeapi.org", 80)==false);
//	if(Wifi_TcpIp_CreateTCPConnection("worldtimeapi.org", 80)==false)
//		 while(1);


	if(Wifi_TcpIp_SendDataTcp(0, strlen(httpRequest),(uint8_t*)httpRequest) == false)
	    while(1);

	do{
		if(containSubstring(Wifi.RxBuffer, sizeof(Wifi.RxBuffer),"\"datetime\":\"") == 1){
				  			const char* timeStart = strstr((char*)Wifi.RxBuffer, "\"datetime\":\"")+12;
				  			const char *timeEnd = strchr(timeStart, ',');
				  			size_t len = timeEnd-timeStart;

				  			if(timeStart != NULL){
				  					actionProtocol =1;
				  					strncpy(message,timeStart,len);
				  					analyzeBufferTime(message);
				  					memset(message,0,sizeof(message));
				  			}
				  		}
	}while(actionProtocol==0);
}

void Device_initializeComplex(){
	// Device started initialized
	HAL_TIM_Base_Start_IT(&htim2);

	//Initialized OLED Srceen
	ssd1306_InitializeDevice();

	/*---------------------------------------SET UP WIFI----------------------------------------*/
	/*------------------------------------------------------------------------------------------*/

	//Set up Device to Access Point
	Wifi_FirstInit(WIFI_MODE_AP);
	test(Wifi.MyIP,"MODE ACCESS POINT");


	//Set up Device to Station
	do{
		  //Waiting for respone from User act to remote ip 192.168.1.4
		  if(Wifi.RxBuffer != NULL){
			  handleRequestWifiConfig(Get_after((char*)Wifi.RxBuffer));
		  }

		  //Wating for receiving SSID and PASSWORD of target Access Point to make it a Station
		  if(is_not_empty(Wifi.SSID_STATION)&& is_not_empty(Wifi.PASSWORD_STATION)){
		  		Wifi_FirstInit(WIFI_MODE_STA);

		  		memset(Wifi.SSID_STATION,0,strlen(Wifi.SSID_STATION));
		  		memset(Wifi.PASSWORD_STATION,0,strlen(Wifi.PASSWORD_STATION));
		  		test(Wifi.MyIP,"-MODE STATION-");
		  		test("","WIFI CONNECTED");
		  		HAL_Delay(500);
		  }
	}while(Wifi.Mode == WIFI_MODE_AP);

	/*------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------------------------------------*/
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */


  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */


  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */


  HAL_TIM_Base_Start_IT(&htim3);
  if (HAL_ADC_Start_IT(&hadc1) != HAL_OK) {

  }

  DS3231_Init(&hi2c1);
  Device_initializeComplex();
  Devcice_getTimeFromHTTP();

  HAL_Delay(500);
  DS3231_SetTime(&rtc);

  init=0;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    DS3231_GetTime(&rtc);


	showMonitoring(message);

	if (sendFlag){
	    	   sendFlag = false;
	    	   sendDataToServer();
	}






  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 249;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 63999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 15;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 49999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, _BUTTON_RST_Pin|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_10|GPIO_PIN_11|I2C_Led_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : _BUTTON_RST_Pin PA11 */
  GPIO_InitStruct.Pin = _BUTTON_RST_Pin|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB10 PB11 I2C_Led_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_10|GPIO_PIN_11|I2C_Led_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : RESET_BUTTON_Pin RESET_BUTTON_DEVICE_Pin */
  GPIO_InitStruct.Pin = RESET_BUTTON_Pin|RESET_BUTTON_DEVICE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON_DOWN_Pin BUTTON_UP_Pin */
  GPIO_InitStruct.Pin = BUTTON_DOWN_Pin|BUTTON_UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
