/*
********************************************************************************
*          			STM32F103 OLED TEST
*
*
* Filename   		: 	SSD1306.h
* Version			: 	VER V0.0 
* Date			: 	2017.11.10
* Programmer 	: 	HCS
********************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ___SSD1306_h___
#define ___SSD1306_h___

/* Includes ------------------------------------------------------------------*/
	#include "main.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
	#define SSD1306_CMD    		0
	#define SSD1306_DAT    		1

	#define SSD1306_WIDTH    	128
	#define SSD1306_HEIGHT   	64

	#define F_8X8_MYUNGJO		0
	#define F_8X8_GHOTIC		1

	#define OLED_OVERWRITE		1
	#define OLED_REWRITE		0

	//#define SH1106
	#define SSD1306

	/* Exported macro ------------------------------------------------------------*/
	/* Exported variables ------------------------------------------------------- */
	/* Exported functions ------------------------------------------------------- */


	void ssd1306_write_byte(byte chData, byte chCmd);
	//void ssd1306_refresh_gram(void);
	void ssd1306_refresh_gram(byte pagenum);
	void ssd1306_clear_screen(byte start, byte end);
	void ssd1306_draw_point(byte chXpos, byte chYpos, byte chPoint);
	void ssd1306_display_char(byte chXpos, byte chYpos, byte chChr, byte chSize, byte chMode);
	void ssd1306_display_string(byte chXpos, byte  chYpos, const byte  *pchString, byte  chSize, byte  chMode);
	void ssd1306_0604_string(byte chXpos, byte  chYpos, const byte  *pchString);
	void ssd1306_0807_string(byte chXpos, byte  chYpos, const byte  *pchString, byte kind);
	void ssd1306_draw_bitmap(byte chXpos, byte chYpos, const byte *pchBmp, byte chWidth, byte chHeight, byte overwrite, byte val);
	
	void ssd1306_draw_0604char(byte chXpos, byte chYpos, byte chChar);
	void ssd1306_draw_0608char(byte chXpos, byte chYpos, byte chChar);
	void ssd1306_draw_1612char(byte chXpos, byte chYpos, byte chChar);
	void ssd1306_draw_3216char(byte chXpos, byte chYpos, byte chChar);
	
	void ssd1306_init(void);
	
	void DisplaySPI(void);
	void DispOLEDString(byte chXpos, byte chYpos, u8 *ptr);
	void LCD_drawRect(byte x, byte y, byte w, byte h, byte color);
	void LCD_drawLine(byte x0, byte y0, byte x1, byte y1, byte color);
		
	void InitOLED(void);

#endif /*___SSD1306_h___*/

/******************* (C) COPYRIGHT 2015 DongIl Co. *****END OF FILE****/
