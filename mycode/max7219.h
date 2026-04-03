/******************** (C) COPYRIGHT 2008 Motex Co., LTD ********************
* File Name			: app_usart.h
* Author			: HCS
* Date First Issued	: 09/09/2008
* Description	: Functions for only App. test
********************************************************************************
* History:
* 09/09/2008: V0.0 */
/* Define to prevent recursive inclusion -------------------------------------*/
	#ifndef __APP_MAX7219_H
	#define __APP_MAX7219_H
	
	#include "main.h"

/* Includes ------------------------------------------------------------------*/
/* Exported defines --------------------------------------------------------*/
    #define _UPSCROLL

    #define F_8X8_SIMPLE		0
	#define F_8X8_GHOTIC		1
    #define F_8X8_DIGITAL		2

    #define FONT_0406           0
    #define FONT_0508           1
    #define FONT_0708           2

	#define FONT_SAVE_POSI      128

    #define MEM_PAGE0           0
    #define MEM_PAGE1           1
    #define MEM_PAGEALL         2


    enum {
		/*  0 */SM_DISP_DATE,
        /*  1 */SM_DISP_DUST,
        /*  2 */SM_DISP_DUST_2_5,
        /*  3 */SM_DISP_DUST_10,
        /*  4 */SM_DISP_WEATHER,
		/*  5 */SM_DISP_INFOTIME,
        /*  6 */SM_DISP_IN_TEMP,
        /*  7 */SM_DISP_IN_HUMI,
        /*  8 */SM_DISP_OUT_TEMP,
        /*  9 */SM_DISP_OUT_HUMI,
        /* 10 */SM_DISP_RAIN_PER,//����Ȯ��
        /* 11 */SM_DISP_WIND,
        /* 12 */SM_DISP_WEA_STA,
        /* 13 */SM_DISP_MAX
    };

	static byte const WeatherState[7][9]  	= {"  Sunny \0",\
											   "paCloudy\0",\
											   "msCloudy\0",\
											   " Cloudy \0",\
											   "  Rain  \0",\
											   "RainSnow\0",\
											   "SnowRain\0"  };

#if !USE_SPI1
	#define SET_DOUT()			GPIOA->BSRR = AMTRIX_DOUT;//1
	#define CLR_DOUT()			GPIOA->BRR 	= AMTRIX_DOUT;//0
	
	#define SET_CS()			GPIOA->BSRR = AMTRIX_CS;//1
	#define CLR_CS()			GPIOA->BRR 	= AMTRIX_CS;//0
	
	#define SET_CLK()			GPIOA->BSRR = AMTRIX_CLK;//1
	#define CLR_CLK()			GPIOA->BRR 	= AMTRIX_CLK;//0
#endif

	#define REG_NOOP      		0x00      // MAX7219 command registers
	#define REG_DIGIT0      	0x01
	#define REG_DIGIT1      	0x02
	#define REG_DIGIT2      	0x03
	#define REG_DIGIT3      	0x04
	#define REG_DIGIT4      	0x05
	#define REG_DIGIT5      	0x06
	#define REG_DIGIT6      	0x07
	#define REG_DIGIT7      	0x08
	#define REG_DECODE      	0x09      // "decode mode" register
	#define REG_INTENSITY      	0x0a      // "intensity" register
	#define REG_SCAN_LIMIT      0x0b      // "scan limit" register
	#define REG_SHUTDOWN      	0x0c      // "shutdown" register
	#define REG_DISPLAY_TEST   	0x0f      // "display test" register 
	
	void InitMax7219(void);
	void delay_us(word u_sec);
	void delay_ms(word m_sec);
	byte BitReverse(byte ch);
    void DrawPoint(byte chXpos, byte chYpos, byte val);
	byte ReadPoint(byte chXpos, byte chYpos);
	void MatrixDispNum(byte x, byte y, long num);
    void ClockNumber1(byte x, byte y, RTC_Time_t *t, byte flag);
    void ClockNumber2(byte x, byte y, RTC_Time_t *t, byte flag);
    void ClockNumber3(byte x, byte y, RTC_Time_t *t, byte flag);
    void DispLocalIP(byte x, byte y, byte *ptr);
		
    void DispMoveSmallTemp(byte movechar);
    void DispMoveSmallWeather(byte movechar);
	void CalendarDisp(byte x, byte y, RTC_Time_t *t, byte flag);
    void ClockDot(byte page, byte state);
    void ClockDot3(byte page, byte state);
	
	void DispAllTempHumi(byte x, byte y, word tmp, word humi);
        
    void MAX7219_BufClear(byte page);
    void MAX7219_Clear(void);
	void MAX7219_SetReg(byte reg, byte val);

    void MAX7219_WholeMatrix(void);
    void DrawR90Font0508(byte chXpos, byte chYpos, byte chChar);
    void DrawR270Font0508(byte chXpos, byte chYpos, byte chChar);
	void DrawFont0406(byte chXpos, byte chYpos, byte chChar, byte bufkind);
	void DrawFont0508(byte chXpos, byte chYpos, byte chChar);
	void DrawFont0708(byte chXpos, byte chYpos, byte chChar, byte kind);
	void DrawFont0708Buf(byte chXpos, byte chYpos, byte chChar, byte kind);
	
    void DispString(byte chXpos, byte chYpos, u8 *ptr, u8 fnt, u8 kind, u8 usediffbuf);
	void DotMatrix(void);
	
#endif		/* __APP_MAX7219_H */



/******************* (C) COPYRIGHT 2008 Motex CO., LTD *****END OF FILE****/