/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
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
#include "rtc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 12;
  sTime.Minutes = 0;
  sTime.Seconds = 0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_TUESDAY;
  DateToUpdate.Month = RTC_MONTH_AUGUST;
  DateToUpdate.Date = 8;
  DateToUpdate.Year = 23;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
  HAL_RTCEx_SetSecond_IT(&hrtc);
  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();

    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
s8 TimeCompare(byte sec)
{
	if(sec >= 1 && sec <= 30) {
		return 1;
	} else if(sec > 30 && sec <= 59) {
		return -1;
	}
	return 0;
}

byte CalendarPorcess(void)
{
  RTC_TimeTypeDef stime;

  if(sys.SettingFlag == 0) {
    if(sys.TimeUpdate == ON) {
      sys.TimeUpdate = OFF;
      sys.OLEDUpdate = ON;

      HAL_RTC_GetTime(&hrtc, &stime, RTC_FORMAT_BIN);
      sys.time.hour = stime.Hours;
      sys.time.min  = stime.Minutes;
      sys.time.sec  = stime.Seconds;
      if(sys.AlNow == ON) SetBuzzer(BUZZ_MAIN);

      if(sys.SecCompare > 60) {//ěľě´?ďż?�ďż�? ę°ě ???ďż?�ďż�??ďż?�ďż˝ď�???? ?ďż?�ďż�??ďż?�ďż�?..
        sys.SecCompare = sys.time.sec;
        sys.MinCompare = sys.time.min;
      } else if(sys.SecCompare == 59){//?ďż?�ďż�??ďż?�ďż�? ě´ďż??? 59ďż??? ?ďż?�ďż˝ď�????..
        if(sys.time.sec == 0) {		//?ďż?�ďż�??ďż?�ďż�? ě´ďż??? 0ě´ëŠ´... ë§ęł ...
          sys.SecCompare = sys.time.sec;
          sys.MinCompare = sys.time.min;
        } else {					//?ďż?�ďż�??ďż?�ďż�? ě´ďż??? 0ě´ďż??? ?ďż?�ďż�??ďż?�ďż˝ď�????...
          // ?ďż?�ďż�??ďż?�ďż�? ě´ďż??? 59ďż???. ?ďż?�ďż�??ďż?�ďż˝ě´ďż�?? 0?ďż?�ďż�? ?ďż?�ďż�??ďż?�ďż�??ďż?�ďż�? ?ďż?�ďż�??ďż?�ďż�??ďż?�ďż�?, 0?ďż?�ďż�? ?ďż?�ďż�??ďż?�ďż˝ď�????, ?ďż?�ďż�??ďż?�ďż˝ě´ďż�??? ?ďż?�ďż�??ďż?�ďż�? ę°ęłź ë§ěś¤..
          if(TimeCompare(sys.time.sec) > 0) {//?ďż?�ďż�??ďż?�ďż�? ?ďż?�ďż�??ďż?�ďż�??ďż?�ďż�??ďż?�ďż�??ďż?�ďż�? ?ďż?�ďż�??ďż?�ďż�? ?ďż?�ďż˝ę°ëł�??ďż?�ďż�? ?ďż?�ďż�??ďż?�ďż�?.
            sys.time.sec   = 0;
            sys.SecCompare = sys.time.sec;
            sys.MinCompare = sys.time.min;
          } else {						   //?ďż?�ďż�??ďż?�ďż�? ?ďż?�ďż�??ďż?�ďż�??ďż?�ďż�??ďż?�ďż�??ďż?�ďż�? ?ďż?�ďż�??ďż?�ďż�? ?ďż?�ďż˝ę°ëł�??ďż?�ďż�? ?ďż?�ďż�??ďż?�ďż�??ďż?�ďż�?.
            sys.time.sec   = 0;
            if(sys.time.min >= 59) 	{
              sys.time.min = 0;
              if(sys.time.hour >= 23) {
                sys.time.hour = 0;
              } else {
                sys.time.hour += 1;
              }
            } else {
              sys.time.min  += 1;
            }
            sys.SecCompare = sys.time.sec;
            sys.MinCompare = sys.time.min;
          }
        }
      } else if(sys.SecCompare < 59) {//0~58ě´ęšďż???... ëšęľ?ďż?�ďż�?.
        if(sys.time.min == sys.MinCompare) {
          if(sys.time.sec == (sys.SecCompare+1)) {
            sys.SecCompare = sys.time.sec;
          } else {
            sys.time.sec   = sys.SecCompare + 1;
            sys.SecCompare = sys.time.sec;
          }
        } else {//ëśě´ ę°ďż??? ?ďż?�ďż�??ďż?�ďż˝ď�????... (?ďż?�ďż˝ę°ě�? ę°ě ďż??? ěĄ°ě ?ďż?�ďż�??ďż?�ďż�? ëł´ëŠ´, 1ě´ěŠ ě°¨ě´?ďż?�ďż�??ďż?�ďż�? ę?�˝ě�? 59ě´ě?ďż?�ďż�? ëśě´ ďż????ďż?�ďż�??ďż?�ďż�? ę?�˝ě�? ë°ě)
          //sys.time.sec   = 0;//?ďż?�ďż�??ďż?�ďż�? ďż??? ?ďż?�ďż�??ďż?�ďż�??ďż?�ďż�?.. ëśě´ ?ďż?�ďż�??ďż?�ďż˝ď�????ďż???, ě´ë 0?ďż?�ďż˝ď�???? ë°ęż.
          sys.SecCompare = sys.time.sec;
          sys.MinCompare = sys.time.min;
        }
      }
      return OK;
    }
  }
  return NG;
}

void rtc_settime(const RTC_Time_t *rtc, uint8_t compen)
{
	RTC_TimeTypeDef stime;
	RTC_DateTypeDef sdate;

	stime.Hours   = rtc->hour;
	stime.Minutes = rtc->min;
	stime.Seconds = rtc->sec;

	sdate.WeekDay = rtc->wday;
	sdate.Month   = rtc->month;
	sdate.Date    = rtc->day;
	sdate.Year    = (uint8_t)(rtc->year - 2000);

	if(stime.Seconds < 57 && compen <= 3) {
		stime.Seconds += compen;
	}

	if (HAL_RTC_SetTime(&hrtc, &stime, RTC_FORMAT_BIN) != HAL_OK) 	Error_Handler();
	if (HAL_RTC_SetDate(&hrtc, &sdate, RTC_FORMAT_BIN) != HAL_OK) 	Error_Handler();

}

void rtc_gettime (RTC_Time_t *rtc)
{
  RTC_TimeTypeDef stime;
  RTC_DateTypeDef sdate;

	HAL_RTC_GetTime(&hrtc, &stime, RTC_FORMAT_BIN);
	rtc->hour = stime.Hours;
	rtc->min  = stime.Minutes;
	rtc->sec  = stime.Seconds;

	HAL_RTC_GetDate(&hrtc, &sdate, RTC_FORMAT_BIN);
	rtc->wday   = sdate.WeekDay;
	rtc->month 	= sdate.Month;
	rtc->day 	= sdate.Date;
	rtc->year    = (uint16_t)(sdate.Year + 2000);
}
/* USER CODE END 1 */
