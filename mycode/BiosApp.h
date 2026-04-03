/******************** (C) COPYRIGHT 2018 Tekbase Co., LTD ********************
* File Name          : BiosApp.h
* Author             : Hong Chong Sik
* Date First Issued  : 06/20/2012
********************************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/

/* Includes ------------------------------------------------------------------*/
	#include "main.h"



/* Exported functions ------------------------------------------------------- */
    void AppProcess(void);
	void OLEDAreaCodeDisp(byte code);
	void OLEDClockNumber(byte x, byte y, byte hh, byte mm, byte ss, byte sta, byte fntSize);
	void OLEDDispTemp(byte x, byte y, byte fntsiz, word num, byte tmphum);
	void OLEDDispFineDust(byte x, byte y, byte fntsiz, word num, byte kind);
	void OLEDDispIcon(byte x, byte y, byte num);
	void OLEDCalendar(RTC_Time_t *t);
	