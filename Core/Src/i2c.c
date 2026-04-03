/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
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

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
uint8_t I2CGetData(uint8_t i2cflag)
{

// Sends I2C data over I2C1:
//  1) Sends Start Condition. Checks for I2C EV5
//  2) Sends 7 bit address & checks for EV6
//  3) Sends 8 bit command byte (0x00) & checks for EV8
//  4) Sends 8 bits (1 byte) of data & checks for EV8
//  5) Sends Stop Condition
	#define WRITE_CMD		0x50
	uint8_t  bData[4];//ě¸Ąě ę?�°ęłźę�?? 4ë°ě´?¸

	//?´?? ?¤ ?¸? ? ??? 50ms ? ?? ?? ?´ę°? ??´?ź ?°?´?°ëĽ? ????´ ?Ź ? ??.
	//?´?? ?¤ ?¸? ëś?ëśęłź ?°?´?° ???? ëś?ëśě ???´? ????´?´.HAL_Delay?¨? ?­? ..
	if(i2cflag == I2C_SET_ADDR)
  {
		if(HAL_I2C_Master_Transmit(&hi2c1, WRITE_CMD, bData, 0, 100) )
    {
			/* Check error */
			if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
      {
				return 1;
			}
		}
		return 0;

	}
  else if(i2cflag == I2C_GET_ADDR)
  {
		if(HAL_I2C_Master_Receive(&hi2c1, WRITE_CMD, bData, 4, 100) )
    {
			/* Check error */
			if (HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
      {
				return 2;
			}
		}
	}
  else
  {
		return 3;
	}

	float Cal1, Cal2;
	Cal1 = bData[0];
	Cal2 = bData[1];

	Cal1 *= 256;
	Cal1 /= 163.83;
	Cal2 /= 163.83;

	Cal1 += Cal2;
  Cal1 += (float)10.0;//20180512 HCS ??   +10%

	Cal1 *= 100;
	Cal1 += 5;
	Cal1 /= 10;

	sys.Humidity = (word)Cal1;

	Cal1 = bData[2];
	Cal2 = bData[3];

	Cal1 *= 64; Cal1 /= 16384; Cal1 *= 165;
	Cal2 /= 4;  Cal2 /= 16384; Cal2 *= 165;
	//Cal1 = Cal1 + Cal2 - 40;
  Cal1 = Cal1 + Cal2 - 45;//20180512 HCS ?? , -5? ëł´ě ...(?? ę°ě? -5?)

  if(Cal1 < 0)
  {
    sys.TempMinus = 1;
    Cal1 *= (-1);
  }
  else
  {
    sys.TempMinus = 0;
  }
	Cal1 *= 100;
	Cal1 += 5;
	Cal1 /= 10;

	sys.Temperature = (word)Cal1;
  if(sys.TempMinus)
  {
    if(sys.Temperature == 0) sys.TempMinus = 0;
  }
	return 0;//HCS
}
/* USER CODE END 1 */
