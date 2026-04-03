/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern TIM_HandleTypeDef htim2;

extern TIM_HandleTypeDef htim3;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_TIM2_Init(void);
void MX_TIM3_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* USER CODE BEGIN Prototypes */
  uint16_t GetTimer(uint8_t tmid);
  void SetTimer(uint8_t tmid, uint16_t tmData);
	void BuzzFreqSet(u8 en, float freq);
	void SetBuzzer(u8 buzz);
	void Delay(u16 tmData);
  void KeyProcess(void);

	#define KEY_CHAT			5
	#define KEY_DELAY			8
	#define KEY_LONG			200
	
	// Mapping Key variable to GPIO Pin.
	#define KEY_0				  1	// PC0, KF_CALL
	#define KEY_1				  2	// PC1, KF_MAMORY
	#define KEY_2				  3	// PC2, KF_ENTER
	#define KEY_3				  4	// PC3, KF_UP
	#define KEY_4				  5	// PC4, KF_DOWN
	#define KEY_5				  6	// PC4, KF_DOWN
	#define KEY_6				  7	// PC4, KF_DOWN
	#define KEY_7				  8	// PC4, KF_DOWN

	#define KF_1    			1//KEY_1
	#define KF_2				2//KEY_2
	#define KF_3				3//KEY_3
	#define KF_4				4//KEY_4

	#define MENU_NORMAL			0
	#define MENU_TIME			1
	#define MENU_CALENDAR		2
	#define MENU_ALARM			3
	#define MENU_FONT			4


	// Software Timer
	enum {
		TMR_ID_BLK,			// Blink Timer
		TMR_ID_TMR,			// Delay Timer
		TMR_ID_DISPUPDATE,
		TMR_ID_I2CREAD,
		TMR_ID_DISPSMALL,
		TMR_ID_DISPSPI,
		TMR_ID_WIFISET,
		TMR_ID_COLON,
    	TMR_ID_UART_SEND,
		TMR_ID_MAX
	};
  
	#define TIM2_1STEP		1
	#define DELAY1MS		TIM2_1STEP
	#define DELAY2MS		(2*DELAY1MS)
	#define DELAY3MS		(3*DELAY1MS)
	#define DELAY5MS		(5*DELAY1MS)
	#define DELAY10MS		(10*DELAY1MS)
	#define DELAY20MS		(2*DELAY10MS)
	#define DELAY30MS		(3*DELAY10MS)
	#define DELAY40MS		(4*DELAY10MS)
	#define DELAY50MS		(5*DELAY10MS)
	#define DELAY60MS		(6*DELAY10MS)
	#define DELAY70MS		(7*DELAY10MS)
	#define DELAY80MS		(8*DELAY10MS)
	#define DELAY90MS		(9*DELAY10MS)
	#define DELAY100MS		(10*DELAY10MS)
	#define DELAY200MS		(2*DELAY100MS)
	#define DELAY300MS		(3*DELAY100MS)
	#define DELAY400MS		(4*DELAY100MS)
	#define DELAY500MS		(5*DELAY100MS)
	#define DELAY600MS		(6*DELAY100MS)
	#define DELAY700MS		(7*DELAY100MS)
	#define DELAY800MS		(8*DELAY100MS)
	#define DELAY900MS		(9*DELAY100MS)
	#define DELAY1000MS		(10*DELAY100MS)
	#define DELAY1300MS		(13*DELAY100MS)
	#define DELAY1500MS		(15*DELAY100MS)
	#define DELAY2000MS		(20*DELAY100MS)
	#define DELAY3000MS		(30*DELAY100MS)
	#define DELAY4000MS		(40*DELAY100MS)
	#define DELAY5000MS		(50*DELAY100MS)
	#define DELAY6000MS		(60*DELAY100MS)
	#define DELAY7000MS		(70*DELAY100MS)
	#define DELAY8000MS		(80*DELAY100MS)
	#define DELAY9000MS		(90*DELAY100MS)
	#define DELAY10S		(2*DELAY5000MS)
	#define DELAY15S		(15*DELAY1000MS)
	#define DELAY20S		(2*DELAY10S)
	#define DELAY30S		(3*DELAY10S)
	#define DELAY40S		(4*DELAY10S)
	#define DELAY50S		(5*DELAY10S)
	#define DELAY60S		(6*DELAY10S)
	#define DELAY70S		(7*DELAY10S)
	#define DELAY80S		(8*DELAY10S)
	#define DELAY90S		(9*DELAY10S)
	#define DELAY100S		(10*DELAY10S)

	#define DELAY1MIN		(DELAY60S)
	#define DELAY2MIN		(2*DELAY60S)
	#define DELAY5MIN		(5*DELAY60S)
	#define DELAY6MIN		(6*DELAY60S)
	#define DELAY7MIN		(7*DELAY60S)
	#define DELAY8MIN		(8*DELAY60S)
	#define DELAY9MIN		(9*DELAY60S)
	#define DELAY10MIN	(10*DELAY60S)

	/* Buzzer */
	enum {
    BUZZ_NONE,
    BUZZ_OK,
    BUZZ_ERR,
    BUZZ_KEY1,
    BUZZ_KEY2,
    BUZZ_MIS,
    BUZZ_TIMESIG,
    BUZZ_MAIN,
    BUZZ_MAX
	};


	enum {
    OCT1_DO,
    OCT1_RE,
    OCT1_MI,
    OCT1_FA,
    OCT1_SOL,
    OCT1_RA,
    OCT1_SI,
    OCT2_DO,
    OCT2_RE,
    OCT2_MI,
    OCT2_FA,
    OCT2_SOL,
    OCT2_RA,
    OCT2_SI,
    OCT3_DO,
    OCT2_RESHOP,

    OCT4_DO,
    OCT4_SOL,
    OCT4_SI
    //4186.0, 6271.9, 7902.1
	};

  typedef struct {
    u8 bKind;
    u8 bRepeat;
    u16 wOnTime;
    u8 bOffTime;
  } TBuzz;
	extern TBuzz buzz;

	static const TBuzz BzTbl[] = {
		{BUZZ_NONE,			0,	 0,	 0},
		{BUZZ_OK,				1, 450, 10},
		{BUZZ_ERR,			4,  50,	20},
		{BUZZ_KEY1,			1,	80,	10},
		{BUZZ_KEY2,			1,	80,	10},
		{BUZZ_MIS,			3,	30,	30},
		{BUZZ_TIMESIG,	8, 100,  2},
		{BUZZ_MAIN,		  3, 100,	 5}
  };

  typedef struct {
    u8 	BzKindToDemand;
    u8 	BzKind;
    u16 BzCnt;
    u8 	BzRepeat;
    s16 BzStatus;
    float BzFreq;
  } TBuzzVal;
	extern TBuzzVal buzVal;
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

