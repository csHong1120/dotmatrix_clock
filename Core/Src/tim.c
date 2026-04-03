/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
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
  #include "tim.h"
  #if defined(BUZZ_BUG_FIXED)
    #include "gpio.h"
  #endif

/* USER CODE BEGIN 0 */
  #include "main.h"
  #include "max7219.h"
  #include "rtc.h"

	void WatchBuzzer(void);
	void InitBuzzer(void);
	void IntroKey(void);

  volatile uint16_t wTmr[TMR_ID_MAX];

	TBuzz buzz;
	TBuzzVal buzVal;

	#define _BUZZER_TEST 1
	#if _BUZZER_TEST
		static float ScaleVal[19] = {
				 523.3,  587.3,  659.3,  698.5,  784.0,  880.0,  987.8,  1046.5, \
				1174.7, 1318.5, 1396.9, 1568.0, 1760.0, 1975.5, 2093.0,  1244.5, \
				3951.1, 5274.0, 6271.9/*4000.0, 5000.0, 6000.0*/
		};
	#endif

	extern byte gbDispaly12864[128][8];
  byte KeyCnt;
	byte OldScanCode, ScanCode, KeyCode, KeyPressLong;
/* USER CODE END 0 */

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* TIM2 init function */
void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = (72-1);
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = (1000-1);
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
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
  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END TIM2_Init 2 */

}
/* TIM3 init function */
void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = (72-1);
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(timHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspPostInit 0 */

  /* USER CODE END TIM3_MspPostInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM3 GPIO Configuration
    PB0     ------> TIM3_CH3
    */
    GPIO_InitStruct.Pin = BBUZZ_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BBUZZ_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM3_MspPostInit 1 */

  /* USER CODE END TIM3_MspPostInit 1 */
  }

}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /* TIM2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if(tim_pwmHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void Nop(uint16_t Ntimes)
{
    uint16_t i;
    for(i=0;i<Ntimes;i++) asm("NOP");
}

uint16_t GetTimer(uint8_t tmid)
{
	if(tmid > TMR_ID_MAX) return 0;
	return wTmr[tmid];
}

void SetTimer(uint8_t tmid, uint16_t tmData)
{
	if(tmid < TMR_ID_MAX){
		wTmr[tmid]=tmData;
	}
}

void Delay(u16 tmData)
{
  //HAL_Delay(tmData);
	SetTimer(TMR_ID_TMR, tmData);
	while(GetTimer(TMR_ID_TMR));
}

void InitBuzzer(void)
{
    buzVal.BzKindToDemand = BUZZ_MAX;
    buzVal.BzKind = 0;
    buzVal.BzCnt = 0;
    buzVal.BzRepeat = 0;
    buzVal.BzStatus = OFF;
}

void BuzzFreqSet(u8 en, float freq)
{
#if defined(BUZZ_BUG_FIXED)
  float CalFreq;
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

  if(en) {
    // GPIO_InitStruct.Pin = BBUZZ_Pin;
    // GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    // HAL_GPIO_Init(BBUZZ_GPIO_Port, &GPIO_InitStruct);
    HAL_TIM_MspPostInit(&htim3);
    HAL_TIM_PWM_MspInit(&htim3);

    CalFreq = (1.0 / freq);
    CalFreq *= 1000000;

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = (72-1);
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = (uint32_t)(CalFreq-1);
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)	{
      Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) {
      Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM2;
    sConfigOC.Pulse = (uint32_t)(CalFreq/2);
    sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
      Error_Handler();
    }
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  } else {
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
    HAL_TIM_PWM_MspDeInit(&htim3);

    GPIO_InitStruct.Pin = BBUZZ_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BBUZZ_GPIO_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(BBUZZ_GPIO_Port, BBUZZ_Pin, GPIO_PIN_SET);
  }
#else
  float CalFreq;
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

	// ?©©???¡Æ ?©©???? : SYS_CLK(72MHz)?????? 72?????? ???¢©??¢©???¢©??¢© ???¢©??¢©???¢©??¢©???¢©??¢© 1MHz?????? ???¢©??¢©???¢©??¢©..
	// 1Mhz = 1/1000000(0.000001) ?????? ???¢©??¢©???¢©??¢© 1???¢©??¢©???¢©??¢© ?¡Æ????..
	// ???¢©??¢©???¢©??¢©???¢©??¢©?????? ???¢©??¢©?? ?¨©???????¢©??¢©?????? 500Hz???¢©??¢©?????? ?¨©??¢¬¡Æ???¢©??¢© 1/500???¢©??¢©????????????.. 0.002?¢¥???¢©? ???¢©??¢©???¢©??¢©..
	// ???¢©??¢©???¢©??¢©?????? = 0.002 / 0.000001 = 0.002 * 10^6 = 2000
	CalFreq = (1.0 / freq);
	CalFreq *= 1000000;

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = (72-1);
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = (uint32_t)(CalFreq-1);
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)	{
		Error_Handler();
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM2;
	sConfigOC.Pulse = (uint32_t)(CalFreq/2);
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
		Error_Handler();
	}

  if(en) {
		HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	} else {
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_3);
	}
#endif
}

void SetBuzzer(u8 buzz)
{
  if(buzz < BUZZ_MAX){
    buzVal.BzKindToDemand = buzz;
  }
}

void WatchBuzzer(void)
{
  if(buzVal.BzKindToDemand != BUZZ_MAX) {
    if(buzVal.BzKindToDemand == BUZZ_KEY1) {
      buzVal.BzFreq = ScaleVal[OCT2_DO];
    } else if(buzVal.BzKindToDemand == BUZZ_KEY2) {
      buzVal.BzFreq = ScaleVal[OCT2_SOL];
    } else if(buzVal.BzKindToDemand == BUZZ_OK) {
      buzVal.BzFreq = ScaleVal[OCT3_DO];
    } else {
      buzVal.BzFreq = ScaleVal[OCT1_SOL];
    }

    buzVal.BzKind = buzVal.BzKindToDemand;
    buzVal.BzKindToDemand = BUZZ_MAX;
    buzVal.BzCnt = 0;
    buzVal.BzRepeat = BzTbl[buzVal.BzKind].bRepeat;
    if(buzVal.BzRepeat){
      buzVal.BzStatus = OFF;
    } else {
      buzVal.BzStatus = ON;
    }
  } else {
    if(buzVal.BzCnt==0){
      if(buzVal.BzRepeat){
        buzVal.BzStatus = (buzVal.BzStatus == ON) ? OFF : ON;
        if(buzVal.BzStatus==ON) {
          /* Buzzer On */
          //BUZZ_ON();
          if(buzVal.BzKind == BUZZ_MAIN) {
            //						if(buzVal.BzRepeat==2) 		BuzzFreqSet(1, ScaleVal[OCT2_SOL]);//ScaleVal[OCT1_SOL]);
            //							else			   		BuzzFreqSet(1, ScaleVal[OCT2_RESHOP]);//ScaleVal[OCT1_DO]);
            if(buzVal.BzRepeat==3)		BuzzFreqSet(1, ScaleVal[OCT4_DO]);
            else if(buzVal.BzRepeat==2)	BuzzFreqSet(1, ScaleVal[OCT4_SOL]);
            else if(buzVal.BzRepeat==1)	BuzzFreqSet(1, ScaleVal[OCT4_SI]);

          } else if(buzVal.BzKind == BUZZ_TIMESIG) {
            if(buzVal.BzRepeat==8) 		BuzzFreqSet(1, ScaleVal[OCT2_DO]);
            else if(buzVal.BzRepeat==7)	BuzzFreqSet(1, ScaleVal[OCT2_RE]);
            else if(buzVal.BzRepeat==6)	BuzzFreqSet(1, ScaleVal[OCT2_MI]);
            else if(buzVal.BzRepeat==5)	BuzzFreqSet(1, ScaleVal[OCT2_FA]);
            else if(buzVal.BzRepeat==4)	BuzzFreqSet(1, ScaleVal[OCT2_SOL]);
            else if(buzVal.BzRepeat==3)	BuzzFreqSet(1, ScaleVal[OCT2_RA]);
            else if(buzVal.BzRepeat==2)	BuzzFreqSet(1, ScaleVal[OCT2_SI]);
            else if(buzVal.BzRepeat==1)	BuzzFreqSet(1, ScaleVal[OCT3_DO]);
						
          } else {
            BuzzFreqSet(1, buzVal.BzFreq);
          }
          buzVal.BzCnt = BzTbl[buzVal.BzKind].wOnTime;
        } else {
          /* Buzzer Off */
          //BUZZ_OFF();
          BuzzFreqSet(0, 0);
          buzVal.BzCnt = BzTbl[buzVal.BzKind].bOffTime;
          buzVal.BzRepeat--;
        }
          buzVal.BzCnt--;
      } else {
        if(buzVal.BzStatus==ON){
          /* Buzzer Off */
          //BUZZ_OFF();
          BuzzFreqSet(0, 0);
          buzVal.BzStatus = OFF;
          }
      }
    } else {
      buzVal.BzCnt--;
    }
  }
}

void IntroKey(void)
{
	byte ReadKeyBuf = 0;

#if defined(WIFI_RESET_TEST)
	if(ScanCode==0) {
		if(HAL_GPIO_ReadPin(GPIOB, BKEY1_Pin) == 0) {
      ReadKeyBuf = 3;
		} else if(HAL_GPIO_ReadPin(GPIOB, BKEY2_Pin) == 0) {
			ReadKeyBuf = 2;
		} else if(HAL_GPIO_ReadPin(GPIOB, BKEY3_Pin) == 0) {
      ReadKeyBuf = 1;
		}
		ScanCode = ReadKeyBuf;
	}
#else
	if(ScanCode==0) {
		if(HAL_GPIO_ReadPin(GPIOB, BKEY2_Pin) == 0) {
      if(HAL_GPIO_ReadPin(GPIOB, BKEY3_Pin) == 0) {
        ReadKeyBuf = 4;
      } else {
        ReadKeyBuf = 3;
      }
		} else if(HAL_GPIO_ReadPin(GPIOB, BKEY2_Pin) == 0) {
			ReadKeyBuf = 2;
		} else if(HAL_GPIO_ReadPin(GPIOB, BKEY3_Pin) == 0) {
      if(HAL_GPIO_ReadPin(GPIOB, BKEY1_Pin) == 0) {
          ReadKeyBuf = 4;
      } else {
          ReadKeyBuf = 1;
      }
		}
		ScanCode = ReadKeyBuf;
	}
#endif
	if(ScanCode != 0x00) {                                 // Key Event Occurrence
		KeyCnt++;
		if(ScanCode==OldScanCode) {
			ScanCode = 0;
			if(KeyCnt > KEY_CHAT) {
				if(KeyCnt > KEY_CHAT + 1) {
					if(KeyCnt > KEY_DELAY) {
						if(KeyCnt > KEY_LONG) {
							KeyCnt = KEY_DELAY + 1;
							KeyCode = OldScanCode;
							KeyPressLong = 1;
							return;
						} else return;
					} else return;
				} else {
					KeyCode = OldScanCode;	// Return KeyCode(Short Key)
					return;
				}
			} else	{
				KeyPressLong = 0;
				return;
			}
		} else {
			OldScanCode = ScanCode;	// Update Current Event Key
			KeyCnt = 0;
			return;
		}
	} else {
		OldScanCode = 0;		// No Key Event
    KeyPressLong = 0;
	}
	ScanCode = 0;
	KeyCnt = 0;
}

void InitKey(void)		// Reset KeyCode
{
	KeyCode=0;
}

u8 IsKeyPress(void)		// Return KeyCode, No Key = return 0x3f
{
	return KeyCode;
}

u8 GetKey(void)		// Return KeyCode & Reset
{
	u8 Key;
	Key=IsKeyPress();
	KeyCode=0;
	return Key;
}

void KeyProcess(void)
{
	byte bKeyVal=0;
	
	if(!IsKeyPress()) return;
    
    if(sys.AlNow == ON) {
        sys.AlNow = OFF;//????..
        InitKey();
        Delay(DELAY500MS);
    }
    
	if(sys.DispAction < ACT_SETTING) {
		InitKey(); 
		SetBuzzer(BUZZ_ERR);
        sys.DispAction = ACT_CLOCK1;
		return;//?? ??????? ??? ??.. 
	}

	bKeyVal = GetKey();
	SetBuzzer(BUZZ_KEY1);

#if defined(WIFI_RESET_TEST)
  switch(bKeyVal) {
	case KF_1://LEFT
    SendString(CH2, (byte *)"S99");
	Delay(DELAY3000MS);

	// DI_UART_DMAReset(UART_DEVICE_2);
	// DI_UART_DeInit(UART_DEVICE_2);
	// Delay(DELAY500MS);
	
	memset(rx_ring_buffer[CH2], 0, sizeof(rx_ring_buffer[CH2]));
	DI_UART_PreInit(UART_DEVICE_2);
	DI_UART_Init(UART_DEVICE_2);
	DI_UART_DMAReset(UART_DEVICE_2);
	break;

  case KF_2://LEFT
    // DI_UART_PreInit(UART_DEVICE_2);
    // DI_UART_Init(UART_DEVICE_2);
	// Delay(DELAY500MS);
    // DI_UART_DMAReset(UART_DEVICE_2);

	// DI_UART_DeInit(UART_DEVICE_2);
    //USART_Reset(&huart2);
	Delay(DELAY500MS);
	// DI_UART_DMAReset(UART_DEVICE_2);
    break;

  case KF_3://LEFT
    sys.WiFiErrorCnt = 0;
    sys.WiFiSeq = WIFISEQ_CONNECT;
    SetBuzzer(BUZZ_MAIN);
    break;
  }
#else
  switch(bKeyVal) {
	case KF_1://LEFT
        if(sys.DispAction == ACT_SETTING) {
            if(sys.SettingFlag == 0) {
                if(sys.SettingMenu > MENU_TIME) sys.SettingMenu--;
                else sys.SettingMenu = MENU_FONT;
            }

            switch(sys.SettingFlag) {
            case 1:
                if(sys.SettingMenu == MENU_TIME) {
                    if(sys.time.hour > 0) sys.time.hour--;
                    else sys.time.hour = 23;
                }
                break;
                
            case 2:
                if(sys.SettingMenu == MENU_TIME) {
                    if(sys.time.min > 0) sys.time.min--;
                    else sys.time.min = 59;
                }
                break;
                
            case 3:
                if(sys.SettingMenu == MENU_TIME) {
                    sys.time.sec = 0;
                }
                break;
                
            case 4:
                if(sys.SettingMenu == MENU_TIME) {
                    if(sys.time.wday > 0) sys.time.wday--;
                    else sys.time.wday = 6;
                }
                break;
                
            case 5:
                if(sys.SettingMenu == MENU_CALENDAR) {
                    if(sys.time.year > 2000) sys.time.year--;
                    else sys.time.year = 2099;
                }
                break;
                
            case 6:
                if(sys.SettingMenu == MENU_CALENDAR) {
                    if(sys.time.month > 1) sys.time.month--;
                    else sys.time.month = 12;
                }
                break;
                
            case 7:
                if(sys.SettingMenu == MENU_CALENDAR) {
                    if(sys.time.day > 1) sys.time.day--;
                    else sys.time.day = 31;
                }
                break;
                
            case 8:
                if(sys.SettingMenu == MENU_FONT) {
                    //if(sys.FontKind == F_8X8_SIMPLE)        sys.FontKind = F_8X8_GHOTIC;
                    //else if(sys.FontKind == F_8X8_GHOTIC)   sys.FontKind = F_8X8_SIMPLE;
                    if(sys.FontKind == 0) sys.FontKind = F_8X8_DIGITAL;
                    else sys.FontKind--;
                    
                    //BKP_WriteBackupRegister(ADDR_BEGIN_FONT, sys.FontKind);
                    rom.Data[EEADDR_FONT] = sys.FontKind;
                    FlashWriteData();
                    
                    MAX7219_BufClear(MEM_PAGEALL);
                    DispString( 0, 0, (byte *)"FONT:1.23", FONT_0708, sys.FontKind, 0);
                }
                break;
                
            case 9:
                if(sys.SettingMenu == MENU_ALARM) {
                    if(sys.AlHour > 1) sys.AlHour--;
                    else sys.AlHour = 23;
                }
                break;
                    
            case 10:
                if(sys.SettingMenu == MENU_ALARM) {
                    if(sys.AlMin > 1) sys.AlMin--;
                    else sys.AlMin = 59;
                }
                break;

            case 11:
                if(sys.SettingMenu == MENU_ALARM) {
                    if(sys.AlOnOff == ON) sys.AlOnOff = OFF;
                    else sys.AlOnOff = ON;
                }
                break;

            }
        } else {
            if(sys.SettingFlag == 0) {
                MAX7219_BufClear(MEM_PAGEALL);
                if(sys.DispAction == ACT_CLOCK1) {
                    sys.SaveAction = ACT_CLOCK2;
                    ClockNumber1(  0, 0, &sys.time, sys.SettingFlag);
                    ClockNumber2(64,  0, &sys.time, 0);
                } else if(sys.DispAction == ACT_CLOCK2) {
                    sys.SaveAction = ACT_CLOCK3;
                    ClockNumber2( 0,  0, &sys.time, 0);
                    ClockNumber3(64,  0, &sys.time, sys.SettingFlag);
                } else if(sys.DispAction == ACT_CLOCK3) {
                    sys.SaveAction = ACT_CLOCK1;
                    ClockNumber3( 0,  0, &sys.time, 0);
                    ClockNumber1(64,  0, &sys.time, sys.SettingFlag);
                }

                //BKP_WriteBackupRegister(ADDR_BEGIN_ACT, sys.SaveAction);
                rom.Data[EEADDR_BEGINACT]   = sys.SaveAction;
                FlashWriteData();

                sys.DispAction = ACT_DISPCHG_ALL_UP;
                sys.Count = 0;
            }
        }
		break;
        
	case KF_4://??
        if(sys.SettingFlag) {
            sys.SettingFlag = 0;
            MAX7219_BufClear(MEM_PAGE0);
        } else {
            if(sys.SettingMenu) {
                sys.SettingMenu = MENU_NORMAL;
                sys.DispAction = sys.SaveAction;

                MAX7219_BufClear(MEM_PAGEALL);
                if(sys.DispAction == ACT_CLOCK1)
                    ClockNumber1( 0, 0, &sys.time, 0);//sys.SettingFlag ? ??? ??? ????? ??? ??..
                else
                    ClockNumber2( 0, 0, &sys.time, 0);
                
                InitKey();
            } else {
                SetBuzzer(BUZZ_ERR);
            }
        }
        Delay(DELAY500MS);
        break;
		
	case KF_2://??
        if(KeyPressLong == 0) {
			if(sys.DispAction < ACT_CLOCK1 && sys.DispAction > ACT_CLOCK3) {
                SetBuzzer(BUZZ_ERR);
				return;
			} else {
				//??? ? ? ?? ??(ACT_CLOCK1, ACT_CLOCK2)? ????, Wi-Fi??? ?? ??? ???...
				if(sys.WiFiState & WIFI_CONNECT) {
					//SetBuzzer(BUZZ_ERR);
					//return;
				}
			}
			
            switch(sys.SettingMenu) {
            case MENU_NORMAL:
                sys.SaveAction = sys.DispAction;
                sys.SettingMenu = MENU_TIME;
                sys.SettingFlag = 0;
                sys.DispAction = ACT_SETTING;
                MAX7219_BufClear(MEM_PAGE0);
                DispString( 0, 0, (byte *)"TiCaAlFo", FONT_0708, sys.FontKind, 0);
                break;
                
            case MENU_TIME:
                if(sys.SettingFlag == 0) MAX7219_BufClear(MEM_PAGE0);
                if(++sys.SettingFlag >= 4) sys.SettingFlag = 1;
                break;
                
            case MENU_CALENDAR:
                if(sys.SettingFlag < 5) {
                    MAX7219_BufClear(MEM_PAGE0);
                    sys.SettingFlag = 5;
                } else {
                    sys.SettingFlag++;
                    if(sys.SettingFlag > 7) sys.SettingFlag = 5;
                }
                break;
                
            case MENU_ALARM:
                if(sys.SettingFlag < 9) {
                    MAX7219_BufClear(MEM_PAGE0);
                    sys.SettingFlag = 9;
                } else {
                    sys.SettingFlag++;
                    if(sys.SettingFlag > 11) sys.SettingFlag = 9;
                }
                break;
                
            case MENU_FONT:
                if(sys.SaveAction == ACT_CLOCK1 || sys.SaveAction == ACT_CLOCK3) {
                    if(sys.SettingFlag < 8) sys.SettingFlag = 8;
                    MAX7219_BufClear(MEM_PAGE0);
                } else {
                    SetBuzzer(BUZZ_ERR);
                }
                break;
            }
        } else {
            if(KeyPressLong) {            
                SetBuzzer(BUZZ_OK);
                sys.DispAction = sys.SaveAction;

                while(KeyPressLong);
                InitKey();
                Delay(DELAY500MS);            

                MAX7219_BufClear(MEM_PAGEALL);
                if(sys.DispAction == ACT_CLOCK1)
                    ClockNumber1( 0, 0, &sys.time, 0);//sys.SettingFlag ? ??? ??? ????? ??? ??..
                else if(sys.DispAction == ACT_CLOCK2)
                    ClockNumber2( 0, 0, &sys.time, 0);
                else if(sys.DispAction == ACT_CLOCK3)
                    ClockNumber3( 0, 0, &sys.time, 0);                    


                if(sys.SettingFlag < 8) {
    #if USE_RTC
                    rtc_settime(&sys.time, 0);
    #endif
                } else if(sys.SettingFlag >= 9 && sys.SettingFlag <= 11) {
                    //BKP_WriteBackupRegister(ADDR_ALR_HOUR, sys.AlHour);
                    //BKP_WriteBackupRegister(ADDR_ALR_MIN,  sys.AlMin);
                    //BKP_WriteBackupRegister(ADDR_ALR_ONOFF,sys.AlOnOff);
                    rom.Data[EEADDR_ALHOUR]     = sys.AlHour;
                    rom.Data[EEADDR_ALMIN]      = sys.AlMin;
                    rom.Data[EEADDR_ALONOFF]    = sys.AlOnOff;
                    FlashWriteData();
                }
                sys.SecCompare = 100;//?????? ? ?? ????, BiosRTC.c?? 1?? ?????? ??
                sys.SettingMenu = MENU_NORMAL;
                sys.SettingFlag = 0;
            }
        }
        break;
        
	case KF_3://right
        if(sys.DispAction == ACT_SETTING) {
            if(sys.SettingFlag == 0) {
                if(++sys.SettingMenu > MENU_FONT) sys.SettingMenu = MENU_TIME;
            }

            switch(sys.SettingFlag) {
            case 1:
                if(sys.SettingMenu == MENU_TIME) {
                    if(sys.time.hour < 23) sys.time.hour++;
                    else sys.time.hour = 0;
                }
                break;
                
            case 2:
                if(sys.SettingMenu == MENU_TIME) {
                    if(sys.time.min < 59) sys.time.min++;
                    else sys.time.min = 0;
                }
                break;
                
            case 3:
                if(sys.SettingMenu == MENU_TIME) {
                    sys.time.sec = 0;
                }
                break;
                
            case 4:
                if(sys.SettingMenu == MENU_TIME) {
                    if(sys.time.wday < 6) sys.time.wday++;
                    else sys.time.wday = 0;
                }
                break;
                
            case 5:
                if(sys.SettingMenu == MENU_CALENDAR) {
                    if(sys.time.year < 2099) sys.time.year++;
                    else sys.time.year = 2000;
                }
                break;
                
            case 6:
                if(sys.SettingMenu == MENU_CALENDAR) {
                    if(sys.time.month < 12) sys.time.month++;
                    else sys.time.month = 1;
                }
                break;
                
            case 7:
                if(sys.SettingMenu == MENU_CALENDAR) {
                    if(sys.time.day < 31) sys.time.day++;
                    else sys.time.day = 1;
                }
                break;

            case 8:
                if(sys.SettingMenu == MENU_FONT) {
                    //if(sys.FontKind == F_8X8_SIMPLE)        sys.FontKind = F_8X8_GHOTIC;
                    //else if(sys.FontKind == F_8X8_GHOTIC)   sys.FontKind = F_8X8_SIMPLE;
                    
                    if(++sys.FontKind > F_8X8_DIGITAL)  sys.FontKind = F_8X8_SIMPLE;
                    //BKP_WriteBackupRegister(ADDR_BEGIN_FONT, sys.FontKind);
                    rom.Data[EEADDR_FONT] = sys.FontKind;
                    FlashWriteData();
                    
                    MAX7219_BufClear(MEM_PAGEALL);
                    DispString( 0, 0, (byte *)"FONT:1.23", FONT_0708, sys.FontKind, 0);
                }
                break;
                
            case 9:
                if(sys.SettingMenu == MENU_ALARM) {
                    if(sys.AlHour < 23) sys.AlHour++;
                    else sys.AlHour = 0;
                }
                break;
                    
            case 10:
                if(sys.SettingMenu == MENU_ALARM) {
                    if(sys.AlMin < 59) sys.AlMin++;
                    else sys.AlMin = 0;
                }
                break;

            case 11:
                if(sys.SettingMenu == MENU_ALARM) {
                    if(sys.AlOnOff == ON) sys.AlOnOff = OFF;
                    else sys.AlOnOff = ON;
                }
                break;
            }
            break;
        } else {
            if(sys.SettingFlag == 0) {
                MAX7219_BufClear(MEM_PAGEALL);
                sys.SaveAction = sys.DispAction;
                if(sys.DispAction == ACT_CLOCK1) {
                    ClockNumber1(  0, 0, &sys.time, sys.SettingFlag);
                  CalendarDisp( 64, 0, &sys.time, sys.SettingFlag);
                } else if(sys.DispAction == ACT_CLOCK2) {
                  ClockNumber2( 0, 0,&sys.time, 0);
					        DispAllTempHumi(64, 0, sys.Temperature, sys.Humidity);
                    //DispString(64, 0, (byte *)"T:19.8c/H:23.5%", FONT_0406, F_8X8_SIMPLE, 0);//*F_8X8_SIMPLE  F_8X8_GHOTIC
                    //DispString(64, 0, (byte *)"T:    C/H:    %", FONT_0406, F_8X8_SIMPLE, 0);//*F_8X8_SIMPLE  F_8X8_GHOTIC
                } else if(sys.DispAction == ACT_CLOCK3) {
                    ClockNumber3(  0, 0, &sys.time, sys.SettingFlag);
                    CalendarDisp( 64, 0, &sys.time, sys.SettingFlag);
                }
                sys.DispAction = ACT_ALL_LEFTSCR;
                sys.Count = 0;
            }
        }
    }
#endif
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  uint8_t i;
  static uint8_t wT2_5ms, wTimerCnt, wOLEDCnt, DispI2CPageNum=0;

  if(htim->Instance == htim2.Instance)
  {
    if(wT2_5ms++ > 1) {
      wT2_5ms = 0;
      IntroKey();	// Key Event Check
    }

    for(i=0;i<TMR_ID_MAX;i++)
    {
			// SW Timer
      if(wTmr[i]!=0) wTmr[i]--;
    }

    WatchBuzzer();
    if(sys.DispDot == TRUE)
    {
			if(wTimerCnt++ >= 20)
      {
        wTimerCnt = 0;
        //sys.PreMeasureTime = uwTick;
        MAX7219_WholeMatrix();
        //sys.PastTimeMS = uwTick - sys.PreMeasureTime;
			}
    }

    if(wOLEDCnt++ >= 5)
    {
      wOLEDCnt = 0;
      if(sys.DispOLED == ON)
      {
        ssd1306_refresh_gram(DispI2CPageNum);
        if(++DispI2CPageNum >= 8) DispI2CPageNum = 0;
      }
    }
  }
}
/* USER CODE END 1 */
