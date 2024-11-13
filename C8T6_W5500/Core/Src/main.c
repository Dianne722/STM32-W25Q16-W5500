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
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "w5500_spi.h"
#include "socket.h"
#include "loopback.h"
#include <string.h>  // memcmp
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE BEGIN PTD */
uint8_t destination_ip[]={192,168,1,2}; //远程主机地址
uint16_t destination_port = 5000;        //远程主机端口

#define LISTEN_PORT 5000 //本地主机端口
#define RECEIVE_BUFF_SIZE 128

wiz_NetInfo gWIZNETINFO = {
  .mac = { 0x80, 0x80, 0x80,0x80,0x80,0x80 },//MSB - LSB
  .ip  = { 192, 168, 1, 10 },  //IP地址
  .sn  = { 255, 255, 255, 0 }, //子网掩码
  .gw  = { 192, 168, 1, 1 },   //网关参数
  .dns  = { 8, 8, 8, 8 },      //DNS
  .dhcp = NETINFO_STATIC };    //DHCP关闭
uint8_t receive_buff[RECEIVE_BUFF_SIZE];
/* USER CODE END PTD */

  
static void PHYStatusCheck(void);
//static void PrintPHYConf(void);
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
 //printf("A simple TCP Server Application using W5500!\r\n");
  W5500Init();                                     //W5500初始化
	
 /* USER CODE BEGIN 2 */
  //printf("A simple TCP Client Application using W5500!\r\n");
  ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO); //配置网络参数
  
  wiz_PhyConf phyconf;
  phyconf.by = PHY_CONFBY_SW;
  phyconf.duplex = PHY_DUPLEX_FULL;
  phyconf.speed = PHY_SPEED_10;
  phyconf.mode = PHY_MODE_AUTONEGO;
  ctlwizchip(CW_SET_PHYCONF, (void*)&phyconf);     //配置PHY参数
  
  PHYStatusCheck();                                //检查网络连接状态
  //PrintPHYConf();                                  //打印PHY配置信息
 // printf("Simple TCP Client Application\r\n");
  /* USER CODE END 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    
    //The return value of socket() call is the socket number if success
    //we are using socket number 1 so it should return 1 for success
    //The 3rd argument is the local port, when a client app is written
    //better give 0 here, that will use a random available port
    if(socket(1, Sn_MR_TCP, 0, 0)==1) //创建Socket=1
    {
      //printf("\r\nSocket Created Successfully");
    }
    else
    {
      //printf("\r\nCannot create socket");
      while(1);
    }

    //printf("\r\nConnecting to server: %d.%d.%d.%d @ TCP Port: %d",destination_ip[0],destination_ip[1],destination_ip[2],destination_ip[3],destination_port);
    if(connect(1, destination_ip, destination_port)==SOCK_OK) //连接远程主机
    {
      //printf("\r\nConnected with server.");
    }
    else
    {
      //failed
      //printf("\r\nCannot connect with server!");
      while(1);
    }

    while (1)
    {
      //Return value of the send() function is the amount of data sent
      if(send(1, "I am role_2099!\r\n", 16)<=SOCK_ERROR) //向服务器发送数据I am role_2099
      {
        //printf("\r\nSending Failed!");
        while(1);

      }
      else
      {
        //printf("\r\nSending Success!");
      }

      HAL_Delay(1000);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void PHYStatusCheck(void)
{
	uint8_t tmp;

	do
	{
		//printf("\r\nChecking Ethernet Cable Presence ...");
		ctlwizchip(CW_GET_PHYLINK, (void*) &tmp);

		if(tmp == PHY_LINK_OFF)
		{
			//printf("NO Cable Connected!");
			HAL_Delay(1500);
		}
	}while(tmp == PHY_LINK_OFF);

	//printf("Good! Cable got connected!");

}

//void PrintPHYConf(void)
//{
//	wiz_PhyConf phyconf;

//	ctlwizchip(CW_GET_PHYCONF, (void*) &phyconf);

//	if(phyconf.by==PHY_CONFBY_HW)
//	{
//		//printf("\r\nPHY Configured by Hardware Pins");
//	}
//	else
//	{
//		//printf("\r\nPHY Configured by Registers");
//	}

//	if(phyconf.mode==PHY_MODE_AUTONEGO)
//	{
//		//printf("\r\nAutonegotiation Enabled");
//	}
//	else
//	{
//		//printf("\r\nAutonegotiation NOT Enabled");
//	}

//	if(phyconf.duplex==PHY_DUPLEX_FULL)
//	{
//		//printf("\r\nDuplex Mode: Full");
//	}
//	else
//	{
//		//printf("\r\nDuplex Mode: Half");
//	}

//	if(phyconf.speed==PHY_SPEED_10)
//	{
//		//printf("\r\nSpeed: 10Mbps");
//	}
//	else
//	{
//		//printf("\r\nSpeed: 100Mbps");
//	}
//}

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
