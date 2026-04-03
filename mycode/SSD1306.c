/*
********************************************************************************
*          			STM32F103 OLED TEST
*                   All Rights Reserved
*
* Filename   		: 	SSD1306.c
* Version			: 	VER V0.0
* Date			: 	2017.11.1\0
* Programmer 	: 	HCS
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
	#include "SSD1306.h"
	#include "max7219.h"
	#include "BiosFont.h"
	#include <math.h>

/* Private typedef -----------------------------------------------------------*/
    extern SPI_HandleTypeDef hspi2;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
	byte gbDispaly12864[128][8];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void SPI2WriteByte(byte *data, byte siz)
{
    HAL_SPI_Transmit(&hspi2, data, (uint16_t)siz, 500);
}

/*******************************************************************************
* Function Name  : ssd1306_write_byte
* Description    : ssd1306 1바이트 전송
* Input : 
*		chData: Data to be writen to the display data ram or the command register
*		chCmd:  
*                           0: Writes to the command register
*                           1: Writes to the display data ram
* Attention		 : 150805 바이트 전송 이후 딜레이 필요
*******************************************************************************/
void ssd1306_write_byte(byte chData, byte chCmd) 
{
	SSD1306_CS_L();

	if(chCmd) SSD1306_DC_H();
	else 	  SSD1306_DC_L();

	HAL_SPI_Transmit(&hspi2, (uint8_t *)&chData,	1, 500);
	
	// ** 딜레이 필요 **
	//delay_us(1);
	SSD1306_DC_H();
	SSD1306_CS_H();
}


/*******************************************************************************
* Function Name  : ssd1306_refresh_gram
* Description    : ssd1306 display buffer 전송
* Attention		 : None
*******************************************************************************/
void ssd1306_refresh_gram(byte pagenum)
{
	byte i, buf[128] = {0, };
	byte addr[3];//, dat1, dat2;
	
	for(i=0;i<128;i++) buf[i] = gbDispaly12864[i][pagenum];

	addr[0] = (0xB0+pagenum);
	addr[1] = 0x00;
	addr[2] = 0x10;

	SSD1306_CS_L();

	// SSD1306_DC_L();
	// HAL_SPI_Transmit(&hspi2, (uint8_t *)&addr[0], 1, 1000);

	// SSD1306_DC_L();
	// HAL_SPI_Transmit(&hspi2, (uint8_t *)&addr[1], 1, 1000);

	// SSD1306_DC_L();
	// HAL_SPI_Transmit(&hspi2, (uint8_t *)&addr[2], 1, 1000);

	SSD1306_DC_L();
	SPI2WriteByte(addr, 3);

	SSD1306_DC_H();
	//HAL_SPI_Transmit(&hspi2, buf, sizeof(buf), 1000);
	SPI2WriteByte(buf, sizeof(buf));

	// dmaSendCmdCont(0xB0+pagenum);
	// dmaSendCmdCont(0x00);
	// dmaSendCmdCont(0x10);
	// dmaSendDataCont8(buf, sizeof(buf));
	SSD1306_CS_H();
}

/*******************************************************************************
* Function Name  : ssd1306_clear_screen
* Description    : ssd1306 화면채움
* Input :  채울 데이터 
* Attention		 : None
*******************************************************************************/
//void ssd1306_clear_screen(byte chFill)  
void ssd1306_clear_screen(byte start, byte end)
{
	byte i, j;

	if(start == 0xff) {
		memset(gbDispaly12864, 0, (128*8));		
	} else {
		end   = 7 - end;
		start = 7 - start;
		for(i=end;i<(start+1);i++) {
			for(j=0;j<128;j++) {
				gbDispaly12864[j][i] = 0;
			}
		}
	}
	//ssd1306_refresh_gram();
}

/*******************************************************************************
* Function Name  : ssd1306_draw_point
* Description    : ssd1306 bit 입력 ( 디스플레이 버퍼 갱신 )
* Input : x 좌표 , y 좌표 , 출력 character , chPoint: 0: the point turns off    1: the point turns on 
* Attention		 : None
*******************************************************************************/
void ssd1306_draw_point(byte chXpos, byte chYpos, byte chPoint)
{
	byte chPos, chBx, chTemp = 0;

	if (chXpos > 127 || chYpos > 63) 
		return;

	chPos = 7 - chYpos / 8;
	chBx = chYpos % 8;
	chTemp = 1 << (7 - chBx);

	//좌표가   0, 0 이면, gbDispaly12864  [0][7]
	//좌표가 127,63 이면, gbDispaly12864[127][0]
	if (chPoint) 
		gbDispaly12864[chXpos][chPos] |= chTemp;
	else 
		gbDispaly12864[chXpos][chPos] &= ~chTemp;
}


/*******************************************************************************
* Function Name  : ssd1306_display_char
* Description    : ssd1306 character 출력 
* Input : x 좌표 , y 좌표 , 출력 character , 글자크기 , mode 
* Attention		 : None
*******************************************************************************/
void ssd1306_display_char(byte chXpos, byte chYpos, byte chChr, byte chSize, byte chMode)
{      	
	byte i, j;
	byte chTemp, chYpos0 = chYpos;

	chChr = chChr - ' ';				   
	for(i=0;i<chSize;i++) {   
		if(chSize == 12) {
			if(chMode) 
				chTemp = c_chFont1206[chChr][i];
			else 
				chTemp = ~c_chFont1206[chChr][i];
		} else if(chSize == 16) {
			if(chMode)
				chTemp = c_chFont1608[chChr][i];
			else 
				chTemp = ~c_chFont1608[chChr][i];
		}

		for(j=0;j<8;j++) {
			//좌표를 세로로 증가함..  1픽셀씩 그리는데 세로로 그림
			if(chTemp & 0x80)
				ssd1306_draw_point(chXpos, chYpos, 1);
			else 
				ssd1306_draw_point(chXpos, chYpos, 0);
			
			chTemp <<= 1;
			chYpos ++;

			if((chYpos - chYpos0) == chSize) {
				//y축을 다 그리면, x축 증가함.
				chYpos = chYpos0;
				chXpos ++;
				break;
			}
		}  	 
	} 
}

/*******************************************************************************
* Function Name  : ssd1306_display_string
* Description    : ssd1306 string 출력 
* Input : x 좌표 , y 좌표 , 출력 스트링 , 글자크기 , mode 
* Attention		 : None
*******************************************************************************/
void ssd1306_display_string(byte chXpos, byte  chYpos, const byte  *pchString, byte chSize, byte chMode)
{
	while(*pchString != '\0') {       
		if(chXpos > (SSD1306_WIDTH - chSize / 2)) {
			//x좌표 위치가 1줄이 넘어가면(좌표가 128보다 커지면 Y좌표 증가)
			chXpos = 0;
			chYpos += chSize;
			if(chYpos > (SSD1306_HEIGHT - chSize)) {
				//y좌표 위차가 64보다 커지면 화면 지우고 초기화
				//chYpos = chXpos = 0;
				//ssd1306_clear_screen(0xff, 0xff);
				return;
			}
		}
		ssd1306_display_char(chXpos, chYpos, *pchString, chSize, chMode);
		chXpos += chSize / 2;
		pchString ++;
	}
}

/*******************************************************************************
* Function Name  : ssd1306_draw_bitmap
* Description    : ssd1306 bitmap data 버퍼 저장 
* Input : x 좌표 , y 좌표 , bitmap data , 넓이 , 높이
* Attention		 : None
*******************************************************************************/
void ssd1306_draw_bitmap(byte chXpos, byte chYpos, const byte *pchBmp, byte chWidth, byte chHeight, byte overwrite, byte val)
{
	word i, j, byteWidth = (chWidth + 7) / 8;

	for(j=0;j<chHeight;j++) {
		for(i=0;i<chWidth;i++) {
			if(val) {
				if(*(pchBmp + j * byteWidth + i / 8) & (128 >> (i & 7))) {
					ssd1306_draw_point(chXpos + i, chYpos + j, 1);
				} else {
					//overwrite는 이전에 그려져 있던 바탕위에 다시 그림...  Overwrite모드가 아니면 현재 비트를 다시 그림.
					if(!overwrite) ssd1306_draw_point(chXpos + i, chYpos + j, 0);
				}
			} else {
				//값이 0이면 무조건 지움..
				ssd1306_draw_point(chXpos + i, chYpos + j, 0);
			}
		}
	}
}

/*
void ssd1306_draw_bitmap_overwrite(byte chXpos, byte chYpos, const byte *pchBmp, byte chWidth, byte chHeight, byte color)
{
	unsigned int i, j, byteWidth = (chWidth + 7) / 8;

	for(j=0;j<chHeight;j++) {
		for(i=0;i<chWidth;i++) {
			if(color) {
				if(*(pchBmp + j * byteWidth + i / 8) & (128 >> (i & 7)))
					ssd1306_draw_point(chXpos + i, chYpos + j, 1);
			} else {
				// 영역 지우기..
				ssd1306_draw_point(chXpos + i, chYpos + j, 0);
			}
		}
	}
}
*/

void ssd1306_draw_0604char(byte chXpos, byte chYpos, byte chChar)
{
	byte i, j;
	byte chTemp = 0, chXpos0 = chXpos, chMode = 0;

	for(i=0;i<6;i++) {
		chTemp = Font4x6[(chChar - ' ')*6+i];
		
		for(j=0;j<4;j++) {
			//좌표를 가로로 증가함. 1픽셀씩 그리는데 가로로 그림
			chMode = chTemp & 0x08 ? 1 : 0;//상위 4bit는 버림...
			ssd1306_draw_point(chXpos, chYpos, chMode);
			chTemp <<= 1;
			chXpos++;
			
			if((chXpos - chXpos0) == 4) {
				chXpos = chXpos0;
				chYpos++;
				break;
			}
		}
	}
}

void ssd1306_draw_0807char(byte chXpos, byte chYpos, byte chChar, byte kind)
{
	byte i, j;
	byte chTemp = 0, chXpos0 = chXpos, chMode = 0;

	for(i=0;i<8;i++) {
		if(kind == F_8X8_MYUNGJO)
			chTemp = SFont8x8[(chChar - ' ')*8+i];//
		else
			chTemp = BitReverse(GFont8x8[(chChar-' ')*8+i]);//GFont8x8[(chChar - ' ')*8+i];
		
		for(j=0;j<8;j++) {
			//좌표를 가로로 증가함. 1픽셀씩 그리는데 가로로 그림
			chMode = chTemp & 0x80 ? 1 : 0; 
			chTemp <<= 1;
			ssd1306_draw_point(chXpos, chYpos, chMode);
			chXpos++;
			if((chXpos - chXpos0) == 7) {
				chXpos = chXpos0;
				chYpos++;
				break;
			}
		}
	}
}

void ssd1306_0604_string(byte chXpos, byte  chYpos, const byte  *pchString)
{
	while(*pchString != '\0') {       
		if(chXpos > (SSD1306_WIDTH - 4)) {
			//x좌표 위치가 1줄이 넘어가면(좌표가 128보다 커지면 Y좌표 증가)
			chXpos = 0;
			chYpos += 6;
			if(chYpos > (SSD1306_HEIGHT - 6)) {
				//y좌표 위차가 64보다 커지면 화면 지우고 초기화
				//chYpos = chXpos = 0;
				//ssd1306_clear_screen(0xff, 0xff);
				return;
			}
		}
		ssd1306_draw_0604char(chXpos, chYpos, *pchString);
		chXpos += 4;
		pchString++;
	}
}

void ssd1306_0807_string(byte chXpos, byte  chYpos, const byte  *pchString, byte kind)
{
	while(*pchString != '\0') {       
		if(chXpos > (SSD1306_WIDTH - 7)) {
			//x좌표 위치가 1줄이 넘어가면(좌표가 128보다 커지면 Y좌표 증가)
			chXpos = 0;
			chYpos += 8;
			if(chYpos > (SSD1306_HEIGHT - 8)) {
				//y좌표 위차가 64보다 커지면 화면 지우고 초기화
				//chYpos = chXpos = 0;
				//ssd1306_clear_screen(0xff, 0xff);
				return;
			}
		}
		ssd1306_draw_0807char(chXpos, chYpos, *pchString, kind);
		chXpos += 7;
		pchString++;
	}
}
/*******************************************************************************
* Function Name  : ssd1306_draw_0608char
* Description    : ssd1306 0608 글자 버퍼 입력 
* Input : x 좌표 , y 좌표 , character
* Attention		 : None
*******************************************************************************/
void ssd1306_draw_0608char(byte chXpos, byte chYpos, byte chChar)
{
	byte i, j;
	byte chTemp = 0, chXpos0 = chXpos, chMode = 0;

	for(i=0;i<8;i++) {
		chTemp = c_chFont0608[chChar - '0'][i];
		
		for(j=0;j<8;j++) {
			//좌표를 가로로 증가함. 1픽셀씩 그리는데 가로로 그림
			chMode = chTemp & 0x80 ? 1 : 0; 
			ssd1306_draw_point(chXpos, chYpos, chMode);
			chTemp <<= 1;
			chXpos ++;
			
			if((chXpos - chXpos0) == 6) {
				chXpos = chXpos0;
				chYpos ++;
				break;
			}
		}
	}
}

/*******************************************************************************
* Function Name  : ssd1306_draw_1608char
* Description    : ssd1306 1608 글자 버퍼 입력 
* Input : x 좌표 , y 좌표 , character
* Attention		 : None
*******************************************************************************/
void ssd1306_draw_1608char(byte chXpos, byte chYpos, byte *ptr)
{
	//가로 1byte 세로 2byte
	byte i, j;
	byte chTemp = 0, chXpos0 = chXpos, chMode = 0;

	for(i=0;i<16;i++) {
		//chTemp = ASCII8x16_MY[chChar - '0'][i];
		chTemp = *(ptr + i);
		
		for(j=0;j<8;j++) {
			//좌표를 가로로 증가함. 1픽셀씩 그리는데 가로로 그림
			chMode = chTemp & 0x80 ? 1 : 0; 
			ssd1306_draw_point(chXpos, chYpos, chMode);
			chTemp <<= 1;
			chXpos ++;
			
			if((chXpos - chXpos0) == 8) {
				chXpos = chXpos0;
				chYpos ++;
				break;
			}
		}
	}
}

/*******************************************************************************
* Function Name  : ssd1306_draw_1612char
* Description    : ssd1306 1616 글자 버퍼 입력 
* Input : x 좌표 , y 좌표 , character
* Attention		 : None
*******************************************************************************/
void ssd1306_draw_1612char(byte chXpos, byte chYpos, byte chChar)
{
	//가로 2byte 세로 2byte
	byte i, j;
	byte chTemp = 0, chYpos0 = chYpos, chMode = 0;

	for(i=0;i<32;i++) {
		chTemp = c_chFont1612[chChar - '0'][i];
		
		for(j=0;j<8;j++) {
			//좌표를 세로로 증가함..  1픽셀씩 그리는데 세로로 그림
			chMode = chTemp & 0x80 ? 1 : 0; 
			ssd1306_draw_point(chXpos, chYpos, chMode);
			chTemp <<= 1;
			chYpos++;
			
			if((chYpos - chYpos0) == 16) {
				chYpos = chYpos0;
				chXpos++;
				break;
			}
		}
	}
}

/*******************************************************************************
* Function Name  : ssd1306_draw_1616char
* Description    : ssd1306 1616 글자 버퍼 입력 
* Input : x 좌표 , y 좌표 , character
* Attention		 : None
*******************************************************************************/
void ssd1306_draw_1616char(byte chXpos, byte chYpos, byte *ptr)
{
	//가로 2byte 세로 2byte
	byte i, j;
	byte chTemp = 0, chXpos0 = chXpos, chMode = 0;

	for(i=0;i<32;i++) {
		//chTemp = ASCII8x16_MY[chChar - '0'][i];
		chTemp = *(ptr + i);
		
		for(j=0;j<8;j++) {
			//좌표를 가로로 증가함. 1픽셀씩 그리는데 가로로 그림
			chMode = chTemp & 0x80 ? 1 : 0; 
			ssd1306_draw_point(chXpos, chYpos, chMode);
			chTemp <<= 1;
			chXpos++;
			
			if((chXpos - chXpos0) == 16) {
				chXpos = chXpos0;
				chYpos ++;
				break;
			}
		}
	}
}

void DispOLEDString(byte chXpos, byte chYpos, byte *ptr)
{
	byte HiCode, LoCode;
	byte temp[32];
	
	while(*ptr != '\0') {
		HiCode = *ptr++;
		if(HiCode >= 128) {
			//한글
			LoCode = *ptr++;
			GetKlFont(KStoKSSM(HiCode, LoCode), temp, 0);
			if(chXpos > (SSD1306_WIDTH - 16)) {
				chXpos = 0;
				chYpos += 16;
				if(chYpos > (SSD1306_HEIGHT - 16)) {
					chYpos = chXpos = 0;
					ssd1306_clear_screen(0xff, 0xff);
				}
			}
			ssd1306_draw_1616char(chXpos, chYpos, temp);
			chXpos += 16;
		} else {
			//영문
			GetEngFont(HiCode, temp);
			if(chXpos > (SSD1306_WIDTH - 8)) {
				chXpos = 0;
				chYpos += 16;
				if(chYpos > (SSD1306_HEIGHT - 16)) {
					chYpos = chXpos = 0;
					ssd1306_clear_screen(0xff, 0xff);
				}
			}
			ssd1306_draw_1608char(chXpos, chYpos, temp);
			chXpos += 8;
		}
	}
}

/*******************************************************************************
* Function Name  : ssd1306_draw_3216char
* Description    : ssd1306 3216 글자 버퍼 입력 
* Input : x 좌표 , y 좌표 , character
* Attention		 : None
*******************************************************************************/
void ssd1306_draw_3216char(byte chXpos, byte chYpos, byte chChar)
{
	//가로 2byte 세로 4byte
	byte i, j;
	byte chTemp = 0, chYpos0 = chYpos, chMode = 0;

	for(i=0;i<64;i++) {
		chTemp = c_chFont3216[chChar - 0x30][i];
		
		for(j=0;j<8;j++) {
			//좌표를 세로로 증가함..  1픽셀씩 그리는데 세로로 그림
			chMode = chTemp & 0x80? 1 : 0; 
			ssd1306_draw_point(chXpos, chYpos, chMode);
			chTemp <<= 1;
			chYpos ++;
			
			if((chYpos - chYpos0) == 32) {
				chYpos = chYpos0;
				chXpos ++;
				break;
			}
		}
	}
}

/*******************************************************************************
* Function Name  : ssd1306_init
* Description    : ssd1306 초기화
* Attention		 : None
*******************************************************************************/
void ssd1306_init(void)
{
	SSD1306_RST_L();  //RES clr
	Delay(DELAY200MS);	
	
	SSD1306_CS_H();   //CS set
	SSD1306_DC_L();   //D/C reset
	SSD1306_RST_H();  //RES set//SSD1306_RST_H();  //RES set
	Delay(DELAY200MS);	
	
	ssd1306_write_byte(0xAE, SSD1306_CMD);//--turn off oled panel
	ssd1306_write_byte(0x00, SSD1306_CMD);//---set low column address
	ssd1306_write_byte(0x10, SSD1306_CMD);//---set high column address
	ssd1306_write_byte(0x40, SSD1306_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	ssd1306_write_byte(0x81, SSD1306_CMD);//--set contrast control register
	ssd1306_write_byte(0xCF, SSD1306_CMD);// Set SEG Output Current Brightness
	ssd1306_write_byte(0xA1, SSD1306_CMD);//--Set SEG/Column Mapping     
	ssd1306_write_byte(0xC0, SSD1306_CMD);//Set COM/Row Scan Direction   
	ssd1306_write_byte(0xA6, SSD1306_CMD);//--set normal display
	ssd1306_write_byte(0xA8, SSD1306_CMD);//--set multiplex ratio(1 to 64)
	ssd1306_write_byte(0x3f, SSD1306_CMD);//--1/64 duty
	ssd1306_write_byte(0xD3, SSD1306_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	ssd1306_write_byte(0x00, SSD1306_CMD);//-not offset
	ssd1306_write_byte(0xd5, SSD1306_CMD);//--set display clock divide ratio/oscillator frequency
	ssd1306_write_byte(0x80, SSD1306_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	ssd1306_write_byte(0xD9, SSD1306_CMD);//--set pre-charge period
	ssd1306_write_byte(0xF1, SSD1306_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	ssd1306_write_byte(0xDA, SSD1306_CMD);//--set com pins hardware configuration
	ssd1306_write_byte(0x12, SSD1306_CMD);
	ssd1306_write_byte(0xDB, SSD1306_CMD);//--set vcomh
	ssd1306_write_byte(0x40, SSD1306_CMD);//Set VCOM Deselect Level
	ssd1306_write_byte(0x20, SSD1306_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	ssd1306_write_byte(0x02, SSD1306_CMD);//
	ssd1306_write_byte(0x8D, SSD1306_CMD);//--set Charge Pump enable/disable
	ssd1306_write_byte(0x14, SSD1306_CMD);//--set(0x10) disable
	ssd1306_write_byte(0xA4, SSD1306_CMD);// Disable Entire Display On (0xa4/0xa5)
	ssd1306_write_byte(0xA6, SSD1306_CMD);// Disable Inverse Display On (0xa6/a7) 
	ssd1306_write_byte(0xAF, SSD1306_CMD);//--turn on oled panel

	ssd1306_clear_screen(0xff, 0xff);
}

/*******************************************************************************
* Function Name  : OLED_Init
* Description    : OLED 초기화
* Attention		 : None
*******************************************************************************/
void InitOLED(void)
{
	// port init
	SSD1306_CS_H();
	sys.DispOLED = OFF;
	Delay(DELAY100MS);

	ssd1306_init();
	ssd1306_clear_screen(0xff, 0xff);
	sys.DispSPISeq = 7;//다르게 설정해 놔야 처음에 구동됨...
	sys.DispOLED = ON;
	
	DispOLEDString(0, 0,(byte *)"  OLED DISPLAY  ");
	DispOLEDString(0,16,(byte *)"*RTC 구동 완료..");
	DispOLEDString(0,32,(byte *)"*DotMatrix Init");
	DispOLEDString(0,48,(byte *)"*Wi-Fi접속중....");
}

void DisplaySPI(void)
{
	//static byte DispSPISeq=0, Flag500MS=0;
	
	if(GetTimer(TMR_ID_DISPSPI) == 0) {
		sys.SystickCnt = 0;
		SetTimer(TMR_ID_DISPSPI, DELAY50MS);
		//GPIO_WriteBit(GPIOC, CLED2, (BitAction)((1-GPIO_ReadOutputDataBit(GPIOC, CLED2))));
	
		switch(sys.DispSPISeq) {
		case 0:
			// Draw the big nums
			ssd1306_display_string(0, 0, "~!@#$%^&*()_+|`123", 12, 1);
			break;
			
		case 1:
			ssd1306_0807_string(0, 8, "1234567890ABCDEFGH", F_8X8_GHOTIC);
			break;

		case 2:
			ssd1306_0807_string(0, 16, "1234567890ABCDEFGH", F_8X8_GHOTIC);
			break;

		case 3:
			ssd1306_0807_string(0, 24, "abcdefghijklmnopqr", F_8X8_MYUNGJO);
			break;

		case 4:
			ssd1306_0807_string(0, 32, "!@#$%^&*()QWERTYUI", F_8X8_MYUNGJO);
			break;

		case 5:
			ssd1306_0807_string(0, 40, "abcdefghijklmnopqr", F_8X8_MYUNGJO);
			break;

		case 6:
			DispOLEDString(0,48,(byte *)"홍종식짱홍종식짱");
			break;

		case 9:
			ssd1306_clear_screen(0xff, 0xff);
			break;

		}
	}
}

void LCD_putPixel(byte x, byte y, byte color) 
{
	ssd1306_draw_point(x, y, color);
}

void LCD_drawRect(byte x, byte y, byte w, byte h, byte color)
{
	LCD_drawLine(x  , y  , x+w, y  , color);
	LCD_drawLine(x  , y+h, x+w, y+h, color);
	LCD_drawLine(x  , y  , x  , y+h, color);
	LCD_drawLine(x+w, y  , x+w, y+h, color);
}

#define abs(a) ((a)<0?-(a):a)
#define _int16_swap(a, b) { int16_t t = a; a = b; b = t; }
void LCD_drawLine(byte x0, byte y0, byte x1, byte y1, byte color)
{
    s16 Dx = (s16) abs(x1 - x0),
        Dy = (s16) abs(y1 - y0);

    if(Dx == 0 && Dy == 0) {
        LCD_putPixel(x0, y0, color);
        return;
    }

    s16 steep = Dy > Dx;
    s16 dx, dy, err, yStep;

    if(steep) {
        _int16_swap(x0, y0);
        _int16_swap(x1, y1);
    }

    if (x0 > x1) {
        _int16_swap(x0, x1);
        _int16_swap(y0, y1);
    }

    dx = x1 - x0;
    dy = (s16) abs(y1 - y0);

    err = (s16) (dx / 2);

    if (y0 < y1) {
        yStep = 1;
    } else {
        yStep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            LCD_putPixel(y0, x0, color);
        } else {
            LCD_putPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += yStep;
            err += dx;
        }
    }
}


void LCD_drawCircle(byte x0, byte y0, byte r, byte color)
{
    if (r == 0) {
        LCD_putPixel(x0, y0, color);
        return;
    }

    s16 f  = (s16) (1 - r),
        dx = 1,
        dy = (s16) (-2 * r),
        x  = 0;

    u16 y = r;

    LCD_putPixel(x0, y0 + r, color);//상
    LCD_putPixel(x0, y0 - r, color);//하
    LCD_putPixel(x0 + r, y0, color);//우
    LCD_putPixel(x0 - r, y0, color);//좌

    while(x < y) {
        if(f >= 0) {
            y--;
            dy += 2;
            f += dy;
        }
        x++;
        dx += 2;
        f += dx;

        LCD_putPixel(x0 + x, y0 + y, color);
        LCD_putPixel(x0 - x, y0 + y, color);
        LCD_putPixel(x0 + x, y0 - y, color);
        LCD_putPixel(x0 - x, y0 - y, color);
        LCD_putPixel(x0 + y, y0 + x, color);
        LCD_putPixel(x0 - y, y0 + x, color);
        LCD_putPixel(x0 + y, y0 - x, color);
        LCD_putPixel(x0 - y, y0 - x, color);
    }
}

//#define _FillLINE
#define COMPASS_LENGTH	30	//클수록 작아짐.
#define COMPEN_NORTH_ANGLE		180.0
#define COMPEN_SOUTH_ANGLE		(COMPEN_NORTH_ANGLE + 180.0)
#define COMPEN_CENTER1_ANGLE	(COMPEN_NORTH_ANGLE + 90.0)
#define COMPEN_CENTER2_ANGLE	(COMPEN_NORTH_ANGLE + 270.0)
void LCD_drawCompassByAngle(byte x0, byte y0, byte r, u16 ang, byte color)
{

	static word remX1=0, remY1=0, remX2=0, remY2=0;	
	static word NorthX=0xffff, NorthY=0, SouthX=0, SouthY=0, remAng=0;
	double x=0, y=0, angle=0;

	//Clear, 이미 그려져 있던 나침반을 지움...
	if(NorthX != 0xffff) {
		if(remAng == ang) return;
		remAng = ang;//각도 저장

		LCD_drawLine(NorthX, NorthY, remX1, remY1, 0);//나침반의 북쪽방향
		LCD_drawLine(NorthX, NorthY, remX2, remY2, 0);//나침반의 북쪽방향
		LCD_drawLine(SouthX, SouthY, remX1, remY1, 0);//나침반의 반대방향, 즉 남쪽 방향지침
		LCD_drawLine(SouthX, SouthY, remX2, remY2, 0);//나침반의 반대방향, 즉 남쪽 방향지침
	} else {
		remAng = ang;//각도 저장
	}

	//North point
	angle = (double)(ang+COMPEN_NORTH_ANGLE);
	if(angle >= 360) angle -= (double)360.0;
	angle = angle * 3.141592 / 180.0;//dgree to radian
	//x = (double)(cos(angle)*(double)(r-COMPASS_LENGTH)) + x0 + 0.5;
	//y = (double)(sin(angle)*(double)(r-COMPASS_LENGTH)) + y0 + 0.5;
	x = (double)(cos(angle)*(double)r) + x0 + 0.5;
	y = (double)(sin(angle)*(double)r) + y0 + 0.5;

	NorthX = (word)x; NorthY = (word)y;
	
	
	angle = (double)(ang+COMPEN_CENTER1_ANGLE);//
	if(angle >= 360) angle -= (double)360.0;
	angle = angle * 3.141592 / 180.0;//dgree to radian
 
	x = (double)(cos(angle)*(double)2) + x0 + 0.5;
	y = (double)(sin(angle)*(double)2) + y0 + 0.5;
	remX1 = (word)x; remY1 = (word)y; 


	angle = (double)(ang+COMPEN_CENTER2_ANGLE);
	angle = angle * 3.141592 / 180.0;//dgree to radian

	x = (double)(cos(angle)*(double)2) + x0 + 0.5;
	y = (double)(sin(angle)*(double)2) + y0 + 0.5;
	remX2 = (word)x; remY2 = (word)y; 

	LCD_drawLine(NorthX, NorthY, remX1, remY1, color);
	LCD_drawLine(NorthX, NorthY, remX2, remY2, color);
	LCD_drawCircle(x0, y0, 2, 1);
}
/******************* (C) COPYRIGHT 2017 *****END OF FILE****/
