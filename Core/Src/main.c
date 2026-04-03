/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "BiosApp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
u8 FirmWareVer[20] = {"Dotmatrix_v1.1.1\0"};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
  typedef struct
  {
    uint32_t SYSCLK_Frequency;        /*!< SYSCLK clock frequency */
    uint32_t HCLK_Frequency;          /*!< HCLK clock frequency */
    uint32_t PCLK1_Frequency;         /*!< PCLK1 clock frequency */
    uint32_t PCLK2_Frequency;         /*!< PCLK2 clock frequency */
  } RCC_ClocksTypeDef;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void InitMessage(void)
{
	RCC_ClocksTypeDef SYS_Clocks;

  SYS_Clocks.SYSCLK_Frequency = HAL_RCC_GetSysClockFreq();
  SYS_Clocks.HCLK_Frequency = HAL_RCC_GetHCLKFreq();
  SYS_Clocks.PCLK1_Frequency = HAL_RCC_GetPCLK1Freq();
  SYS_Clocks.PCLK2_Frequency = HAL_RCC_GetPCLK2Freq();
	
  Work_PortMsgLog(PORT_UART3, "\r\n");
  Work_PortMsgLog(PORT_UART3, "\r\n===========================================");
  Work_PortMsgLog(PORT_UART3, "\r\n=         STM32F103CBT6 CPU Test          =");
  Work_PortMsgLog(PORT_UART3, "\r\n=           (ST Cortex-M3 Core)           =");
  Work_PortMsgLog(PORT_UART3, "\r\n=        (C) COPYRIGHT 2023 cshong        =");
  Work_PortMsgLog(PORT_UART3, "\r\n===========================================");
  Work_PortMsgLog(PORT_UART3, "\r\n=         Firmware Ver.  %s               =", FirmWareVer);
  Work_PortMsgLog(PORT_UART3, "\r\n=           SYSCLK   :   %dMHz            =", SYS_Clocks.SYSCLK_Frequency/1000000);
  Work_PortMsgLog(PORT_UART3, "\r\n=           HCLK     :   %dMHz            =", SYS_Clocks.HCLK_Frequency/1000000);
  Work_PortMsgLog(PORT_UART3, "\r\n=           PCLK1    :   %dMHz            =", SYS_Clocks.PCLK1_Frequency/1000000);
  Work_PortMsgLog(PORT_UART3, "\r\n=           PCLK2    :   %dMHz            =", SYS_Clocks.PCLK2_Frequency/1000000);
  Work_PortMsgLog(PORT_UART3, "\r\n===========================================");
  Work_PortMsgLog(PORT_UART3, "\r\n");

  sys.MatrixNum    = 999980;
  sys.Count        = 0;
  sys.DispDot      = ON;
  sys.AlNow        = OFF;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  //static uint8_t fTimeUpdate=0;
	//uint8_t  cnt;
  //uint8_t  timeBuf[8];//, cnt1, cnt2, cnt3;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	sys.DispDot = OFF;
	sys.DispOLED = OFF;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_TIM2_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	MX_SPI1_Init();
	MX_I2C1_Init();
	MX_SPI2_Init();
	MX_TIM3_Init();
	MX_RTC_Init();
  /* USER CODE BEGIN 2 */

	InitWiFi();
	InitFlashMemory();
	InitUart();
	InitMessage();
	SetBuzzer(BUZZ_MAIN);
	InitMax7219();
	InitOLED();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		//USBProcess();
		KeyProcess();
		AppProcess();
		SerialDust();
		SerialWifi();
		DotMatrix();
		WifiProcess(sys.WiFiSeq);    
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		// if(GetTimer(TMR_ID_UART_SEND) == 0)
		// {
		// 	SetTimer(TMR_ID_UART_SEND, 500);
		// 	Work_PortMsgLog(PORT_UART1, "[UART1] Tick Count %2d : %2d : %2d", sys.time.hour, sys.time.min, sys.time.sec);
		// 	// Work_PortMsgLog(PORT_UART3, "[UART3] Tick Count %2d : %2d : %2d", sys.time.hour, sys.time.min, sys.time.sec);

		// 	Work_SendMessageLog("[UART3] Tick Count %2d : %2d : %2d", sys.time.hour, sys.time.min, sys.time.sec);
		// }
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
