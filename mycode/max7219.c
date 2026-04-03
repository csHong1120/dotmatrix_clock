/******************** (C) COPYRIGHT 2008 Motex CO., LTD ********************
* File Name			: max7219.c
* Author			: Hong Chong Sik
* Date First Issued	: 09/09/2008
* Description	: This file provides Test functions.
********************************************************************************
* History:
* 2026/04/03 : V1.8 */

/* Includes ------------------------------------------------------------------*/
	#include "main.h"
  #include "max7219.h"
	#include "BiosFont.h"
  #include "rtc.h"
  #include "i2c.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
	#define MAX_NUMBER_DIGIT		6
	#define MAX_LOCALIP_DIGIT		15
    #define MAX_CLOCK3_DIGIT		4
	#define FONT_WIDTH				5
	#define FONT_HEIGHT				7

/* Private variables ---------------------------------------------------------*/
	//byte DotMatrix[MAX_GARO_MATRIX][MAX_SERO_DOT];
  extern SPI_HandleTypeDef hspi1;

/* Private functions ---------------------------------------------------------*/
    //void MAX7219_Send1Byte(byte data);
	//void delay_us(byte nCount);
    //void MAX7219_BufClear(void);

void SPI1WriteByte(byte data)
{
    HAL_SPI_Transmit(&hspi1, (uint8_t *)&data, 1, 100);
}

#if 1
// /********************************/ //hcs추가
// void MAX7219_WholeMatrix(void)
// {
// 	byte sero, garo;//garo, sero

// 	for(sero=0;sero<MAX_SERO_DOT;sero++) {
// 		MAX7219_CS_L();
// 		for(garo=0;garo<MAX_GARO_MATRIX;garo++) {
// 			SPI1WriteByte(sero+1);//첫번째 줄... 두번째 줄.....
// 			SPI1WriteByte(sys.DotMatrix[garo][sero]);//첫번째 매트릭스의 첫줄 두번째 매트릭스 첫줄...  
// 		}
// 		MAX7219_CS_H();
// 	}
// }
// /********************************/ //hcs추가
void MAX7219_WholeMatrix(void)
{
	byte sero, garo;//garo, sero

	for(sero=0;sero<MAX_SERO_DOT;sero++) {
		MAX7219_CS_L();
		for(garo=0;garo<MAX_GARO_MATRIX;garo++) {
 			SPI1WriteByte(sero+1);//첫번째 줄... 두번째 줄.....
 			SPI1WriteByte(sys.DotMatrix[garo][sero]);//첫번째 매트릭스의 첫줄 두번째 매트릭스 첫줄...  
		}
		MAX7219_CS_H();
	}
}
#endif


#if 1
	/*******************************************************************************
	* Function Name  : delay_ms
	* Description    : delay
	* Attention		 : 오차 감안해서 사용 
	*******************************************************************************/
	void delay_ms(word m_sec)
	{
		while(m_sec--) {
			delay_us(1000);
		}
	}

	/*******************************************************************************
	* Function Name  : delay_us
	* Description    : delay
	* Attention		 : 오차 심함 
	*******************************************************************************/
	void delay_us(word u_sec)
	{
		while(u_sec--) {
			asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");// 5
			asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");// 10
			asm("NOP");asm("NOP");asm("NOP");asm("NOP");asm("NOP");// 15
			asm("NOP");asm("NOP");asm("NOP");
		}
	}
#else
	void delay_us(byte nCount)
	{
		for(;nCount!=0;nCount--) {
			asm("NOP");
		}
	}
#endif

byte BitReverse(byte ch)
{
	return (bitRevTable[(ch >> 4)&0x0f]) | (bitRevTable[ch & 0x0f] << 4);
}

void DrawPoint(byte chXpos, byte chYpos, byte val)
{
	byte chPos, chBx, chTemp = 0;

	if (chXpos >= 64*2+8 || chYpos >= 8) 
		return;

	chPos = chXpos / 8;
	chBx = chXpos % 8;
	chTemp = 1 << (7 - chBx);
	
	if (val) 
		sys.DotMatrix[chPos][chYpos] |= chTemp;
	else 
		sys.DotMatrix[chPos][chYpos] &= ~chTemp;
}

void SmallCharPoint(byte chXpos, byte chYpos, byte val)
{
	byte chPos, chBx, chTemp = 0;

	if (chXpos >= 5*8 || chYpos >= 8) 
		return;

	chPos = chXpos / 8;
	chBx = chXpos % 8;
	chTemp = 1 << (7 - chBx);
	
	if (val) 
		sys.SmallTemp[chPos][chYpos] |= chTemp;
	else 
		sys.SmallTemp[chPos][chYpos] &= ~chTemp;
}

byte ReadPoint(byte chXpos, byte chYpos)
{
	byte chPos, chBx, chTemp = 0;

	if (chXpos >= 64*2+8 || chYpos >= 8) 
		return 2;

	chPos = chXpos / 8;
	chBx = chXpos % 8;
	chTemp = 1 << (7 - chBx);
	
	if(sys.DotMatrix[chPos][chYpos] & chTemp) 
		return 1;
	else 
		return 0;
}

void DrawR90Font0508(byte chXpos, byte chYpos, byte chChar)
{
	byte i, j;
	byte chTemp = 0, chYpos0 = chYpos, chMode = 0;

	sys.FontX = 5; sys.FontY = 8;
	for(i=0;i<5;i++) {
		chTemp = R90_Font5x8[(chChar - ' ')*5+i];
		for(j=0;j<8;j++) {
			//좌표를 세로로 증가함. 1픽셀씩 그리는데 세로로 그림
			chMode = chTemp & 0x01 ? 1 : 0; 
			DrawPoint(chXpos, chYpos, chMode);
			chTemp >>= 1;
			chYpos++;
			
			if((chYpos - chYpos0) == 8) {
				chYpos = chYpos0;
				chXpos ++;
				break;
			}
		}
	}
}

void DrawR270Font0508(byte chXpos, byte chYpos, byte chChar)
{
	byte i, j;
	byte chTemp = 0, chYpos0 = chYpos, chMode = 0;

	sys.FontX = 5; sys.FontY = 8;
	for(i=0;i<5;i++) {
		chTemp = R270_Font5x8[(chChar - ' ')*5+i];
		for(j=0;j<8;j++) {
			//좌표를 세로로 증가함. 1픽셀씩 그리는데 세로로 그림
			chMode = chTemp & 0x01 ? 1 : 0; 
			DrawPoint(chXpos, chYpos, chMode);
			chTemp >>= 1;
			chYpos++;
			
			if((chYpos - chYpos0) == 8) {
				chYpos = chYpos0;
				chXpos ++;
				break;
			}
		}
	}
}

void DrawFont0508(byte chXpos, byte chYpos, byte chChar)
{
	byte i, j;
	byte chTemp = 0, chYpos0 = chYpos, chMode = 0;

	sys.FontX = 5; sys.FontY = 8;
	for(i=0;i<5;i++) {
		chTemp = Font5x8[(chChar - ' ')*5+i];
		for(j=0;j<8;j++) {
			//좌표를 세로로 증가함. 1픽셀씩 그리는데 세로로 그림
			chMode = chTemp & 0x80 ? 1 : 0; 
			DrawPoint(chXpos, chYpos, chMode);
			chTemp <<= 1;
			chYpos ++;
			
			if((chYpos - chYpos0) == 8) {
				chYpos = chYpos0;
				chXpos ++;
				break;
			}
		}
	}
}

void DrawFont0406(byte chXpos, byte chYpos, byte chChar, byte bufkind)
{
	byte i, j;
	byte chTemp = 0, chXpos0 = chXpos, chMode = 0;

	sys.FontX = 4; sys.FontY = 6;
    if(chChar == '.') sys.FontX = 3;
	for(i=0;i<6;i++) {
		chTemp = Font4x6[(chChar - ' ')*6+i];
		
		for(j=0;j<4;j++) {
			//좌표를 가로로 증가함. 1픽셀씩 그리는데 가로로 그림
			chMode = chTemp & 0x08 ? 1 : 0;//상위 4bit는 버림...
            if(bufkind == 0) {
                DrawPoint(chXpos, chYpos, chMode);
            } else {
                SmallCharPoint(chXpos, chYpos, chMode);
            }
			chTemp <<= 1;
			chXpos++;
			
			if((chXpos - chXpos0) == sys.FontX) {
				chXpos = chXpos0;
				chYpos++;
				break;
			}
		}
	}
    sys.FontX = 4;
}

void DrawFont0708(byte chXpos, byte chYpos, byte chChar, byte kind)
{
	byte i, j;
	byte chTemp = 0, chXpos0 = chXpos, chMode = 0;

	sys.FontX = 7; sys.FontY = 8;
	for(i=0;i<8;i++) {
		if(kind == F_8X8_SIMPLE) 		chTemp = SFont8x8[(chChar - ' ')*8+i];//
		else if(kind == F_8X8_GHOTIC) 	chTemp = BitReverse(GFont8x8[(chChar-' ')*8+i]);
		else if(kind == F_8X8_DIGITAL) 	chTemp = BitReverse(c_chFont0708[(chChar-' ')*8+i]);//c_chFont0708[(chChar - ' ')*8+i];//
        
		for(j=0;j<8;j++) {
			//좌표를 가로로 증가함. 1픽셀씩 그리는데 가로로 그림
			chMode = chTemp & 0x80 ? 1 : 0; 
			chTemp <<= 1;
			DrawPoint(chXpos, chYpos, chMode);
			chXpos++;
		
			if((chXpos - chXpos0) == 7) {
				chXpos = chXpos0;
				chYpos++;
				break;
			}
		}
	}
}

void DrawFont0708Buf(byte chXpos, byte chYpos, byte chChar, byte kind)
{
	byte i, j;
	byte chTemp = 0, chXpos0 = chXpos, chMode = 0;

	sys.FontX = 7; sys.FontY = 8;
	for(i=0;i<8;i++) {
		if(kind == F_8X8_SIMPLE) 		chTemp = SFont8x8[(chChar - ' ')*8+i];//
		else if(kind == F_8X8_GHOTIC) 	chTemp = BitReverse(GFont8x8[(chChar-' ')*8+i]);
		else if(kind == F_8X8_DIGITAL) 	chTemp = BitReverse(c_chFont0708[(chChar-' ')*8+i]);//c_chFont0708[(chChar - ' ')*8+i];//
        
		for(j=0;j<8;j++) {
			//좌표를 가로로 증가함. 1픽셀씩 그리는데 가로로 그림
			chMode = chTemp & 0x80 ? 1 : 0; 
			chTemp <<= 1;
			SmallCharPoint(chXpos, chYpos, chMode);
			chXpos++;
		
			if((chXpos - chXpos0) == 7) {
				chXpos = chXpos0;
				chYpos++;
				break;
			}
		}
	}
}


void DispString(byte chXpos, byte chYpos, u8 *ptr, u8 fnt, u8 kind, u8 usediffbuf)
{
	u8 Code, xSize;
	
    if(fnt == FONT_0406) {
        xSize = 4;
    } else if(fnt == FONT_0508) {
        xSize = 5;
    } else if(fnt == FONT_0708 ) {
        xSize = 7;
    }
	while(*ptr != '\0') {
		Code = *ptr++;
        if(chXpos > (127-xSize)) {
            //MAX7219_BufClear(MEM_PAGEALL);
            break;
        }
        
        switch(fnt) {
        case FONT_0406:
            DrawFont0406(chXpos, chYpos, Code, usediffbuf);
            break;
            
        case FONT_0508:
            if(kind == 0) {
                DrawFont0508(chXpos, chYpos, Code);
            } else {
                DrawR90Font0508(chXpos, chYpos, Code);
            }
            break;

        case FONT_0708:
            if(usediffbuf == 0) {
                DrawFont0708(chXpos, chYpos, Code, kind);
            } else {
                DrawFont0708Buf(chXpos, chYpos, Code, kind);
            }
            break;
        }
        if     (fnt == FONT_0406 && Code == '.') chXpos += 3;
        else if(fnt == FONT_0708 && Code == '.') chXpos += 5;
        else                                chXpos += xSize;
	}
}

void DispTempHumi(byte x, byte y, byte kind, word num)
{
	byte DgtBuf[10] = {0};
    
    switch(kind) {
    case DISP_DATE:
        
        break;
        
    case DISP_TEMPERATURE:
        if(sys.TempMinus) {
            if(num > 99) num = 99;//-9.9도 이하로는 표현 불가.
            sprintf((char *)DgtBuf, "in-T:-%1d.%1d\0", num/10, num%10);
        } else {
            sprintf((char *)DgtBuf, "in-T:%2d.%1d\0", num/10, num%10);
        }
        break;
        
    case DISP_HUMIDITY:
		sprintf((char *)DgtBuf, "in-H:%2d.%1d\0", num/10, num%10);
        break;
        
    case DISP_WEA_TEMP:
		sprintf((char *)DgtBuf, "outT:%s\0", weather.Temperature);
        break;
        
    case DISP_WEA_HUMI:
		sprintf((char *)DgtBuf, "outH:%02d.0\0", num);
        break;
        
    case DISP_RAIN_PERCENT:
        sprintf((char *)DgtBuf, "rain:%3d%%\0", num);
        break;
        
    case DISP_WIND:
        sprintf((char *)DgtBuf, "wind:%sm/s\0", weather.WindSpd);
        //DispString( x, y+2, DgtBuf, FONT_0406, F_8X8_SIMPLE, 0);
        //return;
        break;
        
    case DISP_WEATHER_STA:
		switch(num) {
		case 0:
			sprintf((char *)DgtBuf, " Sunny\0");
			break;
		case 1:
			sprintf((char *)DgtBuf, "ptyCloudy\0");//구름조금
			break;
		case 2:
			sprintf((char *)DgtBuf, "mstCloudy\0");//구름많음
			break;
		case 3:
			sprintf((char *)DgtBuf, " Cloudy\0");//흐림
			break;
		case 4:
			sprintf((char *)DgtBuf, " Rain\0");
			break;
		case 5:
			sprintf((char *)DgtBuf, "Rain/Snow\0");
			break;
		case 6:
			sprintf((char *)DgtBuf, "Snow/Rain\0");
			break;
		case 7:
			sprintf((char *)DgtBuf, " Snow\0");
			break;
		default:
			sprintf((char *)DgtBuf, " Error\0");
			break;
		}
        break;
	}
	DispString(x, y, DgtBuf , FONT_0708, sys.FontKind, 0);
}

void DispAllTempHumi(byte x, byte y, word tmp, word humi)
{
    byte DgtBuf[16];
	
	sprintf((char *)DgtBuf, "T:%2d.%1dc/H:%2d.%1d%\0", tmp/10, tmp%10, humi/10, humi%10);
	DispString(x, y, DgtBuf, FONT_0406, F_8X8_SIMPLE, 0);
}

void MatrixDispNum(byte x, byte y, long num)
{
    byte i;
    byte isbeone;
    byte DgtBuf[MAX_NUMBER_DIGIT];
    long Denom, nextNum;
	
    nextNum = num+1;
    if(nextNum > 999999) nextNum = 0;
    
    isbeone = 0;
	Denom = 100000L;
	for(i=0;i<6;i++) {
		DgtBuf[i] = (byte)(num/Denom);
        sys.CurrNum[i] = DgtBuf[i];
        sys.NextNum[i] = (byte)(nextNum/Denom);
		num %= Denom;
        nextNum %= Denom;
		Denom /= 10;
		if(DgtBuf[i] || isbeone) {
			isbeone = 1;
			DgtBuf[i] = '0'+DgtBuf[i];
		} else {
			if( (i==4) && (!isbeone) ) {
				isbeone = 1;
			}
			DgtBuf[i] = ' ';
		}
	}

    for(i=0;i<6;i++) {
        DrawR90Font0508(x+i*sys.FontX, y, DgtBuf[i]);
        //DrawR270Font0508(x+i*sys.FontX, y, DgtBuf[i]);
		//DrawFont0508(x+i*sys.FontX, y, DgtBuf[i]);
		//DrawFont0708(x+i*sys.FontX, y, DgtBuf[i], F_8X8_GHOTIC);//F_8X8_GHOTIC F_8X8_SIMPLE
	}
}

void ClockNumber1(byte x, byte y, RTC_Time_t *t, byte flag)
{
    static byte NumBlink=0;
    
    byte i=0, NextH, NextM, NextS;
    byte DgtBuf[MAX_NUMBER_DIGIT];

    NextS = t->sec+1;
    NextM = t->min;
    NextH = t->hour;
    
    if(flag != 0) {
        NumBlink ^= 1;
    }
    
    if(NextS >= 60) {
        NextS = 0;
        if(++NextM >= 60) {
            NextM = 0;
            if(++NextH >= 24) {
                NextH = 0;
            }
        }
    }

    if(flag == 1) {
        if(NumBlink) {
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
        } else {
            DgtBuf[i++] = (t->hour/10)+'0';
            DgtBuf[i++] = (t->hour%10)+'0';
        }
        DgtBuf[i++] = (t->min/10)+'0';
        DgtBuf[i++] = (t->min%10)+'0';
        DgtBuf[i++] = (t->sec/10)+'0';
        DgtBuf[i++] = (t->sec%10)+'0';
    } else if(flag == 2) {
        DgtBuf[i++] = (t->hour/10)+'0';
        DgtBuf[i++] = (t->hour%10)+'0';
        if(NumBlink) {
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
        } else {
            DgtBuf[i++] = (t->min/10)+'0';
            DgtBuf[i++] = (t->min%10)+'0';
        }
        DgtBuf[i++] = (t->sec/10)+'0';
        DgtBuf[i++] = (t->sec%10)+'0';
    } else if(flag == 3) {
        DgtBuf[i++] = (t->hour/10)+'0';
        DgtBuf[i++] = (t->hour%10)+'0';
        DgtBuf[i++] = (t->min/10)+'0';
        DgtBuf[i++] = (t->min%10)+'0';
        if(NumBlink) {
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
        } else {
            DgtBuf[i++] = (t->sec/10)+'0';
            DgtBuf[i++] = (t->sec%10)+'0';
        }
    } else {
        DgtBuf[i++] = (byte)(t->hour/10);
        DgtBuf[i++] = (byte)(t->hour%10);
        DgtBuf[i++] = (byte)(t->min/10);
        DgtBuf[i++] = (byte)(t->min%10);
        DgtBuf[i++] = (byte)(t->sec/10);
        DgtBuf[i++] = (byte)(t->sec%10);
            
        for(i=0;i<6;i++) {
            sys.CurrNum[i] = DgtBuf[i];
            DgtBuf[i] += '0';
        }
        
        i=0;
        sys.NextNum[i++] = (byte)(NextH/10);
        sys.NextNum[i++] = (byte)(NextH%10);
        sys.NextNum[i++] = (byte)(NextM/10);
        sys.NextNum[i++] = (byte)(NextM%10);
        sys.NextNum[i++] = (byte)(NextS/10);
        sys.NextNum[i++] = (byte)(NextS%10);

        if(flag == 4) {
            if(NumBlink) {
                DispString(x+48, 1, (byte *)"   ", FONT_0508, F_8X8_SIMPLE, 0);
            } else {
                DispString(x+48, 1, (byte *)&DayOfWeek[sys.time.wday][0], FONT_0508, F_8X8_SIMPLE, 0);
            }
        } else {
/*            if(sys.Temperature < 1000) {
                TempBuf = sys.Temperature;
                Denom = 100;
                for(i=0;i<4;i++) {
                    if(i==2) {DgtTmp[i] = '.'; i++;}
                    DgtTmp[i] = (TempBuf/Denom)+'0';
                    if(i == 0 && DgtTmp[i] == '0') DgtTmp[i] = ' ';
                    TempBuf %= Denom;
                    Denom /= 10;
                }
                DgtTmp[i] = 0;
                DispString(x+48, 2, DgtTmp, FONT_0406, F_8X8_SIMPLE, 0);//
            }
 */
            
            /*
            TempBuf = sys.Temperature;
            if(sys.TempMinuus) {
                if(TempBuf > 99) TempBuf = 99;//-9.9도 이하로는 표현 불가.
                sprintf((char *)DgtTmp, "-%1d.%1d\0", TempBuf/10, TempBuf%10);
            } else {
                sprintf((char *)DgtTmp, "%2d.%1d\0", TempBuf/10, TempBuf%10);
            }
            DispString(x+48, 2, DgtTmp, FONT_0406, F_8X8_SIMPLE, 0);*/
        }
    }
    
    for(i=0;i<2;i++) {
		DrawFont0708(x+i*sys.FontX, y, DgtBuf[i], sys.FontKind);
	}
    for(;i<4;i++) {
        DrawFont0708(x+i*sys.FontX+3, y, DgtBuf[i], sys.FontKind);
	}
    for(;i<6;i++) {
		DrawFont0708(x+i*sys.FontX+6, y, DgtBuf[i], sys.FontKind);
	}
}

void ClockNumber2(byte x, byte y, RTC_Time_t *t, byte flag)
{
    static byte NumBlink=0;
    
    word YearCal;
    byte i=0, NextH, NextM, NextS;
    byte DgtBuf[MAX_NUMBER_DIGIT];

    NextS = t->sec+1;
    NextM = t->min;
    NextH = t->hour;
    
    if(flag != 0) {
        NumBlink ^= 1;
    }
    
    if(NextS >= 60) {
        NextS = 0;
        if(++NextM >= 60) {
            NextM = 0;
            if(++NextH >= 24) {
                NextH = 0;
            }
        }
    }

    if(flag == 1) {
        if(NumBlink) {
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
        } else {
            DgtBuf[i++] = (t->hour/10)+'0';
            DgtBuf[i++] = (t->hour%10)+'0';
        }
        DgtBuf[i++] = (t->min/10)+'0';
        DgtBuf[i++] = (t->min%10)+'0';
        DgtBuf[i++] = (t->sec/10)+'0';
        DgtBuf[i++] = (t->sec%10)+'0';
    } else if(flag == 2) {
        DgtBuf[i++] = (t->hour/10)+'0';
        DgtBuf[i++] = (t->hour%10)+'0';
        if(NumBlink) {
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
        } else {
            DgtBuf[i++] = (t->min/10)+'0';
            DgtBuf[i++] = (t->min%10)+'0';
        }
        DgtBuf[i++] = (t->sec/10)+'0';
        DgtBuf[i++] = (t->sec%10)+'0';
    } else if(flag == 3) {
        DgtBuf[i++] = (t->hour/10)+'0';
        DgtBuf[i++] = (t->hour%10)+'0';
        DgtBuf[i++] = (t->min/10)+'0';
        DgtBuf[i++] = (t->min%10)+'0';
        if(NumBlink) {
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
        } else {
            DgtBuf[i++] = (t->sec/10)+'0';
            DgtBuf[i++] = (t->sec%10)+'0';
        }
    } else {
        DgtBuf[i++] = (byte)(t->hour/10);
        DgtBuf[i++] = (byte)(t->hour%10);
        DgtBuf[i++] = (byte)(t->min/10);
        DgtBuf[i++] = (byte)(t->min%10);
        DgtBuf[i++] = (byte)(t->sec/10);
        DgtBuf[i++] = (byte)(t->sec%10);
            
        for(i=0;i<6;i++) {
            sys.CurrNum[i] = DgtBuf[i];
            DgtBuf[i] += '0';
        }
        
        i=0;
        sys.NextNum[i++] = (byte)(NextH/10);
        sys.NextNum[i++] = (byte)(NextH%10);
        sys.NextNum[i++] = (byte)(NextM/10);
        sys.NextNum[i++] = (byte)(NextM%10);
        sys.NextNum[i++] = (byte)(NextS/10);
        sys.NextNum[i++] = (byte)(NextS%10);

        if(flag == 4) {
            if(NumBlink) {
                DispString(x+48, 1, (byte *)"   ", FONT_0508, F_8X8_SIMPLE, 0);
            } else {
                DispString(x+48, 1, (byte *)&DayOfWeek[t->wday][0], FONT_0508, F_8X8_SIMPLE, 0);
            }
        } else {
            //DispString(x+48, 1, (byte *)&DayOfWeek[t->wday][0], FONT_0508, F_8X8_SIMPLE, 0);
        }
    }
    
    y+=1;
    for(i=0;i<2;i++) {
        DrawFont0508(x+i*sys.FontX,  y, DgtBuf[i]);
	}
    for(;i<4;i++) {
		DrawFont0508(x+i*sys.FontX+1, y, DgtBuf[i]);
	}
    for(;i<6;i++) {
        DrawFont0508(x+i*sys.FontX+2, y, DgtBuf[i]);
	}
 
    i = 0;
    if(flag == 5) {
        if(NumBlink) {
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
        } else {
            YearCal = t->year;
			YearCal %= 100;
            DgtBuf[i++] = (YearCal/10)+'0';
            DgtBuf[i++] = (YearCal%10)+'0';            
        }
        DgtBuf[i++] = (t->month/10)+'0';
        DgtBuf[i++] = (t->month%10)+'0';
        DgtBuf[i++] = (t->day/10)+'0';
        DgtBuf[i++] = (t->day%10)+'0';
    } else if(flag == 6) {
        YearCal = t->year;
        YearCal %= 100;
        DgtBuf[i++] = (YearCal/10)+'0';
        DgtBuf[i++] = (YearCal%10)+'0';
        if(NumBlink) {
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
        } else {
            DgtBuf[i++] = (t->month/10)+'0';
            DgtBuf[i++] = (t->month%10)+'0';            
        }
        DgtBuf[i++] = (t->day/10)+'0';
        DgtBuf[i++] = (t->day%10)+'0';
    } else if(flag == 7) {
        YearCal = t->year;
        YearCal %= 100;
        DgtBuf[i++] = (YearCal/10)+'0';
        DgtBuf[i++] = (YearCal%10)+'0';
        
        DgtBuf[i++] = (t->month/10)+'0';
        DgtBuf[i++] = (t->month%10)+'0'; 
        if(NumBlink) {
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
        } else {
            DgtBuf[i++] = (t->day/10)+'0';
            DgtBuf[i++] = (t->day%10)+'0';
        }
    } else {
        YearCal = t->year;
        YearCal %= 100;
        DgtBuf[i++] = (YearCal/10)+'0';
        DgtBuf[i++] = (YearCal%10)+'0';

        DgtBuf[i++] = (t->month/10)+'0';
        DgtBuf[i++] = (t->month%10)+'0'; 
        DgtBuf[i++] = (t->day/10)+'0';
        DgtBuf[i++] = (t->day%10)+'0';
    }

    for(i=0;i<2;i++) {
		DrawFont0508(x+34+i*sys.FontX, y, DgtBuf[i]);
	}
	
    for(;i<4;i++) {
        DrawFont0508(x+34+i*sys.FontX, y, DgtBuf[i]);
	}
	
    for(;i<6;i++) {
		DrawFont0508(x+34+i*sys.FontX, y, DgtBuf[i]);
	}
}

void ClockNumber3(byte x, byte y, RTC_Time_t *t, byte flag)
{
    byte i=0, NextH, NextM, NextS;
    byte DgtBuf[MAX_NUMBER_DIGIT];

    NextS = t->sec+1;
    NextM = t->min;
    NextH = t->hour;
   
    if(NextS >= 60) {
        NextS = 0;
        if(++NextM >= 60) {
            NextM = 0;
            if(++NextH >= 24) {
                NextH = 0;
            }
        }
    }

    DgtBuf[i++] = (byte)(t->hour/10);
    DgtBuf[i++] = (byte)(t->hour%10);
    DgtBuf[i++] = (byte)(t->min/10);
    DgtBuf[i++] = (byte)(t->min%10);
    for(i=0;i<6;i++) {
        sys.CurrNum[i] = DgtBuf[i];
        DgtBuf[i] += '0';
    }
    
    i=0;
    sys.NextNum[i++] = (byte)(NextH/10);
    sys.NextNum[i++] = (byte)(NextH%10);
    sys.NextNum[i++] = (byte)(NextM/10);
    sys.NextNum[i++] = (byte)(NextM%10);

    for(i=0;i<2;i++) {
		DrawFont0708(x+i*sys.FontX, y, DgtBuf[i], sys.FontKind);
	}
    for(;i<4;i++) {
        DrawFont0708(x+i*sys.FontX+3, y, DgtBuf[i], sys.FontKind);
	}
}

void DispLocalIP(byte x, byte y, byte *ptr)
{
	DispString(x, y, ptr, FONT_0406, F_8X8_SIMPLE, 0);
}

void DispMoveSmallTemp(byte movechar)
{
    byte i, j, DgtTmp[12];
    word TempBuf;

    if(movechar==1) {//움직이는 글자 표현
        if(GetTimer(TMR_ID_DISPSMALL) == 0) {
            SetTimer(TMR_ID_DISPSMALL, DELAY100MS);
            if(sys.SmallCnt == 0) {
                switch(sys.SmallDisplay) {
                case 0:
/*                  TempBuf = sys.Temperature;
                    if(sys.TempMinuus) {
                        if(TempBuf > 99) TempBuf = 99;//-9.9도 이하로는 표현 불가.
                        sprintf((char *)DgtTmp, "  TMP:-%1d.%1d\0", TempBuf/10, TempBuf%10);
                    } else {
                        sprintf((char *)DgtTmp, "  TMP:%2d.%1d\0", TempBuf/10, TempBuf%10);
                    }*/
					sprintf((char *)DgtTmp, " oTMP:%s\0", weather.Temperature);
                    break;

                case 1:
/*                  TempBuf = sys.Humidity;
                    sprintf((char *)DgtTmp, "  Hum:%2d.%1d\0", TempBuf/10, TempBuf%10);
					
					*/
					sprintf((char *)DgtTmp, " oHum:%2d.0\0", weather.Humidity);
                    break;
                    
                case 2:
                    //TempBuf = dust.pms7003[0];//P1.0
                    //sprintf((char *)DgtTmp, " P1.0:%4d\0", TempBuf);
					if(sys.WiFiError == WIFISEQ_DUST) {
						sprintf((char *)DgtTmp, " *DustErr*\0");
						sys.SmallDisplay = 4;//미세먼지 정보가 없으면, 다음단계로 진행(+ 1 단계로 이동)
					} else {
						sprintf((char *)DgtTmp, "-DustJ%02d \0", dust.hour);
						switch(dust.AreaCode) {
						case AREA_CODE_JACKJEON_DONG://0
							DgtTmp[5] = 'J';
							break;
						case AREA_CODE_GONGNEUNG_DONG://1
							DgtTmp[5] = 'G';
							break;
						case AREA_CODE_JUNGNEUNG2_DONG://2
							DgtTmp[5] = '8';
							break;
						case AREA_CODE_PAJU_UNJUNG1_DONG://3
							DgtTmp[5] = 'P';
							break;
						case AREA_CODE_ASAN_TANGJUNG_MYUN://4
							DgtTmp[5] = 'A';
							break;
                        case AREA_CODE_CHUNGLA://5
                            DgtTmp[5] = 'C';
                            break;
						case AREA_CODE_OSAN://6
                            DgtTmp[5] = 'O';
                            break;
                        case AREA_CODE_GODEOK://7
							DgtTmp[5] = 'D';
							break;
						}
					}
					break;

                case 3:
                    TempBuf = dust.PM2_5Val;//P2.5
					if(TempBuf == DUST_VALUE_UNKNOWN) {
						sprintf((char *)DgtTmp, " P2.5:----\0");
					} else {
						if(TempBuf > 999) TempBuf = 999;
						sprintf((char *)DgtTmp, " P2.5:%4d\0", TempBuf);
					}
                    break;

                case 4:
                    TempBuf = dust.PM10Val;//P10
					if(TempBuf == DUST_VALUE_UNKNOWN) {
						sprintf((char *)DgtTmp, " P10 :----\0");
					} else {
						if(TempBuf > 999) TempBuf = 999;
						sprintf((char *)DgtTmp, " P10 :%4d\0", TempBuf);
					}
                    break;
                }

                if(++sys.SmallDisplay > 4) sys.SmallDisplay = 0;
                DispString( 0, 2, DgtTmp, FONT_0406, F_8X8_SIMPLE, movechar);
            } else {
                if(sys.SmallCnt <= 40) {
                    for(j=0;j<8;j++) {
                        sys.DotMatrix[6][j] =  sys.DotMatrix[6][j] << 1 | (sys.DotMatrix[7][j] & 0x80 ? 1 : 0);
                    }
                    
                    for(j=0;j<8;j++) {
                        sys.DotMatrix[7][j] =  sys.DotMatrix[7][j] << 1 | (sys.SmallTemp[0][j] & 0x80 ? 1 : 0);
                    }
                    
                    for(i=0;i<5;i++) {
                        for(j=0;j<8;j++) {
                            sys.SmallTemp[i][j] =  sys.SmallTemp[i][j] << 1 | (sys.SmallTemp[i+1][j] & 0x80 ? 1 : 0);
                        }
                    }
                } else if(sys.SmallCnt > 48) {
                    sys.SmallCnt = 0xff;//최대값 세팅하고 아래에서 1을 더하면 0이됨.
                }
            }
            sys.SmallCnt++;
        }
    } else {//화면 전환 후 처음 표현하고 있는 글자 표현(온도표시)
#if 0//온도
        TempBuf = sys.Temperature;
        if(sys.TempMinuus) {
            if(TempBuf > 99) TempBuf = 99;//-9.9도 이하로는 표현 불가.
            sprintf((char *)DgtTmp, "-%1d.%1d\0", TempBuf/10, TempBuf%10);
        } else {
            sprintf((char *)DgtTmp, "%2d.%1d\0", TempBuf/10, TempBuf%10);
        }
        if(movechar == 0)       DispString(64+48, 2, DgtTmp, FONT_0406, F_8X8_SIMPLE, movechar);
        else if(movechar == 2)  DispString(   48, 2, DgtTmp, FONT_0406, F_8X8_SIMPLE, 0);
#endif
        DispString(64+48, 1, (byte *)&DayOfWeek[sys.time.wday][0], FONT_0508, F_8X8_SIMPLE, 0);
    }
}

void DispMoveSmallWeather(byte movechar)
{
    byte i, j, DgtTmp[12], clearBuf[12];
#if !defined(WEATHER_BUG_FIXED)
	byte tmpMon, tmpDay;
#endif
    word TempBuf;

    if(movechar==1) {//움직이는 글자 표현
        if(GetTimer(TMR_ID_DISPSMALL) == 0) {
			if(sys.WiFiSeq == WIFISEQ_APMODEWAIT) {
				SetTimer(TMR_ID_DISPSMALL, DELAY500MS);
				if(sys.SmallCnt) {
					sys.SmallCnt = 0;
					DispString(32, 2, (byte *)"SET WiFi \0", FONT_0406, F_8X8_SIMPLE, 0);
				} else {
					sys.SmallCnt = 1;
					DispString(32, 0, (byte *)"         \0", FONT_0406, F_8X8_SIMPLE, 0);
                    DispString(32, 2, (byte *)"         \0", FONT_0406, F_8X8_SIMPLE, 0);
				}
#if 0				
			} else if(sys.WiFiSeq == WIFISEQ_ERROR) {
				SetTimer(TMR_ID_DISPSMALL, DELAY500MS);
				if(sys.SmallCnt) {
					sys.SmallCnt = 0;
                    DispString(32, 0, (byte *)"     \0",     FONT_0708, F_8X8_SIMPLE, 0);
					DispString(32, 2, (byte *)"WiFi Err \0", FONT_0406, F_8X8_SIMPLE, 0);
				} else {
					sys.SmallCnt = 1;
					DispString(32, 2, (byte *)"         \0", FONT_0406, F_8X8_SIMPLE, 0);
				}
#endif

#if 0//defined(WEATHER_BUG_FIXED)
            } else if(sys.WiFiSeq == WIFISEQ_ERROR) {
                //DispString(32, 2, (byte *)"Err%01d-%03d\0", FONT_0406, F_8X8_SIMPLE, sys.WiFiSeqBackup, sys.WiFiErrorCnt);
                sprintf((char *)DgtTmp, "Err%01d-%03d\0", sys.WiFiSeqBackup, sys.WiFiErrorCnt);
                DispString( 0, 2, DgtTmp, FONT_0406, F_8X8_SIMPLE, movechar);
#endif
			} else {
				SetTimer(TMR_ID_DISPSMALL, DELAY30MS);
				sys.SmallCnt++;
				if(sys.SmallCnt == 0) {
					switch(sys.SmallDisplay) {
					case SM_DISP_DATE:
						sprintf((char *)DgtTmp, "%04d%02d%02d\0", sys.time.year, sys.time.month, sys.time.day);
						break;
						
					case SM_DISP_DUST:
						//if(sys.WiFiSeq == WIFISEQ_ERROR && sys.WiFiSeqBackup == WIFISEQ_DUST) {
                        if(sys.WiFiError == WIFISEQ_DUST) {
							sprintf((char *)DgtTmp, "-DustErr \0");
							sys.SmallDisplay = SM_DISP_DUST_10;//미세먼지 정보가 없으면, 다음단계로 진행(SM_DISP_DUST_10 + 1 단계로 이동)
						} else {
							sprintf((char *)DgtTmp, "-DustJ%02d \0", dust.hour);
							switch(dust.AreaCode) {
							case AREA_CODE_JACKJEON_DONG://0
								DgtTmp[5] = 'J';
								break;
							case AREA_CODE_GONGNEUNG_DONG://1
								DgtTmp[5] = 'G';
								break;
							case AREA_CODE_JUNGNEUNG2_DONG://2
								DgtTmp[5] = '8';
								break;
							case AREA_CODE_PAJU_UNJUNG1_DONG://3
								DgtTmp[5] = 'P';
								break;
							case AREA_CODE_ASAN_TANGJUNG_MYUN://4
								DgtTmp[5] = 'A';
								break;
                            case AREA_CODE_CHUNGLA://5
                                DgtTmp[5] = 'C';
                                break;
							case AREA_CODE_OSAN://6
								DgtTmp[5] = 'O';
								break;
                            case AREA_CODE_GODEOK://7
                                DgtTmp[5] = 'D';
                                break;
							}
						}
                    #if defined(WEATHER_BUG_FIXED)
                        if(sys.time.min >= 15 && sys.time.min <= 25) {
                    #else
                        if(sys.time.min >= 8) {
                    #endif
							if(sys.time.hour != dust.hour) {
								if(sys.WiFiSeq == WIFISEQ_NOACTION) {
									sys.WiFiSeq = WIFISEQ_DUST;
								}
							}
						}
						break;

					case SM_DISP_DUST_2_5:
						TempBuf = dust.PM2_5Val;//P2.5
						if(TempBuf >= DUST_VALUE_UNKNOWN) {
							sprintf((char *)DgtTmp, "P2.5:--- \0");
						} else {
							sprintf((char *)DgtTmp, "P2.5:%3d \0", TempBuf);
						}
						break;

					case SM_DISP_DUST_10:
						TempBuf = dust.PM10Val;//P10
						if(TempBuf >= DUST_VALUE_UNKNOWN) {
							sprintf((char *)DgtTmp, "P10 :--- \0");
						} else {
							sprintf((char *)DgtTmp, "P10 :%3d \0", TempBuf);
						}
						break;
						
					case SM_DISP_WEATHER:
						sprintf((char *)DgtTmp, "-WEATHER\0");
						break;
						
					case SM_DISP_INFOTIME:
                #if defined(WEATHER_BUG_FIXED)
                        sprintf((char *)DgtTmp, "%02d%02d %02dH\0", weather.month, weather.day, weather.ForecastTime);
                #else
						if(weather.hour < 20){
							sprintf((char *)DgtTmp, "%02d%02d %02dH\0", weather.month, weather.day, weather.ForecastTime);
						} else {
							tmpMon = weather.month;
							tmpDay = weather.day;
							
							tmpDay += 1;
							if(tmpDay > weather.lastday) {//그 달의 마지막 날이 아니면... 다음날로 표시
								tmpMon += 1; tmpDay = 1;//다음달 1일
								if(tmpMon == 13) tmpMon = 1;
							}
							sprintf((char *)DgtTmp, "%02d%02d %02dH\0", tmpMon, tmpDay, weather.ForecastTime);
						}
                #endif
						break;
						
					case SM_DISP_IN_TEMP:
						TempBuf = sys.Temperature;
						if(sys.TempMinus) {
							if(TempBuf > 99) TempBuf = 99;//-9.9도 이하로는 표현 불가.
							sprintf((char *)DgtTmp, "i-T:-%1d.%1d \0", TempBuf/10, TempBuf%10);
						} else {
							// sprintf((char *)DgtTmp, "i-T:%2d.%1d \0", TempBuf/10, TempBuf%10);
                            sprintf((char *)DgtTmp, ".%1d ", TempBuf%10);
                            DispString(19, 0, DgtTmp, FONT_0708, F_8X8_GHOTIC, movechar);

                            sprintf((char *)DgtTmp, "T%2d", TempBuf/10);
                            DispString(0, 0, DgtTmp, FONT_0708, F_8X8_GHOTIC, movechar);
						}
						break;
						
					case SM_DISP_IN_HUMI:
						TempBuf = sys.Humidity;
                        // sprintf((char *)DgtTmp, "i-H:%2d.%1d \0", TempBuf/10, TempBuf%10);
                        sprintf((char *)DgtTmp, ".%1d ", TempBuf%10);
                        DispString(19, 0, DgtTmp, FONT_0708, F_8X8_GHOTIC, movechar);

                        sprintf((char *)DgtTmp, "H%2d", TempBuf/10);
                        DispString(0, 0, DgtTmp, FONT_0708, F_8X8_GHOTIC, movechar);
						break;
						
					case SM_DISP_OUT_TEMP:
						sprintf((char *)DgtTmp, "o-T:%s \0", weather.Temperature);

                        // sprintf((char *)DgtTmp, "Err%01d-%03d\0", sys.WiFiSeqBackup, sys.WiFiErrorCnt);//test disp
                        // DispString( 0, 2, DgtTmp, FONT_0406, F_8X8_SIMPLE, movechar);
						break;
						
					case SM_DISP_OUT_HUMI:
						sprintf((char *)DgtTmp, "o-H:%2d.0 \0", weather.Humidity);
						break;

					case SM_DISP_RAIN_PER:
						sprintf((char *)DgtTmp, "RAI:%3d%% \0", weather.RainyPercent);
						break;

					case SM_DISP_WIND:
						sprintf((char *)DgtTmp, "WIN:%sm \0", weather.WindSpd);
						break;
					
					case SM_DISP_WEA_STA:
						memcpy(&DgtTmp[0], WeatherState[weather.WeatherCode], 9);
						break;
					}
                    // DispString( 0, 2, DgtTmp, FONT_0406, F_8X8_SIMPLE, movechar);
                    if(sys.SmallDisplay < SM_DISP_IN_TEMP || sys.SmallDisplay > SM_DISP_IN_HUMI) {
                        sprintf((char *)clearBuf, "        \0");
                        DispString( 0, 0, clearBuf, FONT_0708, F_8X8_GHOTIC, movechar);
                        DispString( 0, 2, DgtTmp, FONT_0406, F_8X8_SIMPLE, movechar);
                    }

            #if defined(WEATHER_BUG_FIXED)
                    if(++sys.SmallDisplay >= SM_DISP_OUT_TEMP) sys.SmallDisplay = 0;
                    // if(++sys.SmallDisplay >= SM_DISP_OUT_HUMI) sys.SmallDisplay = 0;//test disp
            #else
					if(++sys.SmallDisplay >= SM_DISP_MAX) sys.SmallDisplay = 0;
            #endif
				} else {
					if(sys.SmallCnt > (80+8)) {	//8은 디스플레이 변경시간, 70은 딜레이 시간(80*30ms=2400ms딜레이)
						sys.SmallCnt = 0xff;	//최대값 세팅하고 아래에서 1을 더하면 0이됨.
					} else {
						if(sys.SmallCnt <= 8) {
							for(j=0;j<7;j++) {//0~6번째까지는 아래에 있는 값을 위로 올리고...
								for(i=4;i<8;i++) {
									sys.DotMatrix[i][j] = sys.DotMatrix[i][j+1];
								}
							}
							for(i=4;i<8;i++) {//7번째 값은 새로운 버퍼에서 채움...
								sys.DotMatrix[i][7] = sys.SmallTemp[i-4][sys.SmallCnt-1];
							}
						}
					}
				}
			}
        }
    } else {//화면 전환 후 처음 표현하고 있는 글자 표현(온도표시)
        //DispString(64+48, 1, (byte *)"2018/05/14", FONT_0508, F_8X8_SIMPLE, 0);
		sprintf((char *)DgtTmp, "%04d%02d%02d\0", sys.time.year, sys.time.month, sys.time.day);
		DispString( 0, 2, DgtTmp, FONT_0406, F_8X8_SIMPLE, movechar);
    }
}

void CalendarDisp(byte x, byte y, RTC_Time_t *t, byte flag)
{
    static byte CalenBlink=0;

    byte i=0;
    byte DgtBuf[8];
    byte CalYear;
	
	CalYear = (byte)(t->year % 100);
    if(flag != 0) {
        CalenBlink ^= 1;
    }
    
    if(flag == 5) {
        if(CalenBlink) {
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
        } else {
            DgtBuf[i++] = 2+'0';
            DgtBuf[i++] = 0+'0';//2000년 표시
            DgtBuf[i++] = (CalYear/10)+'0';
            DgtBuf[i++] = (CalYear%10)+'0';            
        }
        DgtBuf[i++] = (t->month/10)+'0';
        DgtBuf[i++] = (t->month%10)+'0';
        DgtBuf[i++] = (t->day/10)+'0';
        DgtBuf[i++] = (t->day%10)+'0';
    } else if(flag == 6) {
        DgtBuf[i++] = 2+'0';
        DgtBuf[i++] = 0+'0';//2000년 표시
        DgtBuf[i++] = (CalYear/10)+'0';
        DgtBuf[i++] = (CalYear%10)+'0';
        if(CalenBlink) {
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
        } else {
            DgtBuf[i++] = (t->month/10)+'0';
            DgtBuf[i++] = (t->month%10)+'0';            
        }
        DgtBuf[i++] = (t->day/10)+'0';
        DgtBuf[i++] = (t->day%10)+'0';
    } else if(flag == 7) {
        DgtBuf[i++] = 2+'0';
        DgtBuf[i++] = 0+'0';//2000년 표시
        DgtBuf[i++] = (CalYear/10)+'0';
        DgtBuf[i++] = (CalYear%10)+'0';
        DgtBuf[i++] = (t->month/10)+'0';
        DgtBuf[i++] = (t->month%10)+'0'; 
        if(CalenBlink) {
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
        } else {
            DgtBuf[i++] = (t->day/10)+'0';
            DgtBuf[i++] = (t->day%10)+'0';
        }
    } else {
        DgtBuf[i++] = 2+'0';
        DgtBuf[i++] = 0+'0';//2000년 표시
        DgtBuf[i++] = (CalYear/10)+'0';
        DgtBuf[i++] = (CalYear%10)+'0';
        DgtBuf[i++] = (t->month/10)+'0';
        DgtBuf[i++] = (t->month%10)+'0'; 
        DgtBuf[i++] = (t->day/10)+'0';
        DgtBuf[i++] = (t->day%10)+'0';
    }
	
    for(i=0;i<4;i++) {
        //DrawR90Font0508(x+i*sys.FontX, y, DgtBuf[i]);
		DrawFont0708(x+i*sys.FontX, y, DgtBuf[i], sys.FontKind);
	}
	
    for(;i<6;i++) {
		//DrawR90Font0508(x+i*sys.FontX+1, y, DgtBuf[i]);
        DrawFont0708(x+i*sys.FontX+3, y, DgtBuf[i], sys.FontKind);
	}
	
    for(;i<8;i++) {
        //DrawR90Font0508(x+i*sys.FontX+2, y, DgtBuf[i]);
		DrawFont0708(x+i*sys.FontX+6, y, DgtBuf[i], sys.FontKind);
	}
    
    DrawPoint(x+28, 3, 1); DrawPoint(x+29, 3, 1);
    DrawPoint(x+45, 3, 1); DrawPoint(x+46, 3, 1);
}

void ClockDot(byte page, byte state)
{
    byte i;
    
    for(i=0;i<2;i++) {
        DrawPoint(14+i+page*64, 1, state);
        DrawPoint(14+i+page*64, 2, state);
        DrawPoint(14+i+page*64, 4, state);
        DrawPoint(14+i+page*64, 5, state);

        DrawPoint(31+i+page*64, 1, state);
        DrawPoint(31+i+page*64, 2, state);
        DrawPoint(31+i+page*64, 4, state);
        DrawPoint(31+i+page*64, 5, state);
    }
}

void ClockDot2(byte page, byte state)
{
    DrawPoint(10+page*64, 2, state);
    DrawPoint(10+page*64, 5, state);

    DrawPoint(21+page*64, 2, state);
    DrawPoint(21+page*64, 5, state);
}

void ClockDot3(byte page, byte state)
{
    byte i;
    
    for(i=0;i<2;i++) {
        DrawPoint(14+i+page*64, 1, state);
        DrawPoint(14+i+page*64, 2, state);
        DrawPoint(14+i+page*64, 4, state);
        DrawPoint(14+i+page*64, 5, state);
    }
}

#define ALM_DGT     8
void AlarmNumber(byte x, byte y, byte hh, byte mm, byte onoff, byte flag)
{
    static byte NumBlink=0;
    
    byte i=0, DgtBuf[ALM_DGT];

    if(flag != 0) {
        NumBlink ^= 1;
    }
    
    if(flag ==9) {
        if(NumBlink) {
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
        } else {
            DgtBuf[i++] = (hh/10)+'0';
            DgtBuf[i++] = (hh%10)+'0';
        }
        DgtBuf[i++] = (mm/10)+'0';
        DgtBuf[i++] = (mm%10)+'0';
    } else if(flag == 10) {
        DgtBuf[i++] = (hh/10)+'0';
        DgtBuf[i++] = (hh%10)+'0';
        if(NumBlink) {
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
        } else {
            DgtBuf[i++] = (mm/10)+'0';
            DgtBuf[i++] = (mm%10)+'0';
        }
    } else if(flag == 11) {
        DgtBuf[i++] = (hh/10)+'0';
        DgtBuf[i++] = (hh%10)+'0';
        DgtBuf[i++] = (mm/10)+'0';
        DgtBuf[i++] = (mm%10)+'0';

        if(NumBlink) {
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
            DgtBuf[i++] = ' ';
        } else {
            if(onoff) {
                DgtBuf[i++] = 'A';
                DgtBuf[i++] = 'L';
                DgtBuf[i++] = 'O';
                DgtBuf[i++] = 'N';
            } else {
                DgtBuf[i++] = 'A';
                DgtBuf[i++] = 'L';
                DgtBuf[i++] = 'O';
                DgtBuf[i++] = 'F';
            }
        }
    }
    
    if(flag < 11) {
        if(onoff) {
            DgtBuf[i++] = 'A';
            DgtBuf[i++] = 'L';
            DgtBuf[i++] = 'O';
            DgtBuf[i++] = 'N';
        } else {
            DgtBuf[i++] = 'A';
            DgtBuf[i++] = 'L';
            DgtBuf[i++] = 'O';
            DgtBuf[i++] = 'F';
        }
    }
    
    for(i=0;i<2;i++) {
		DrawFont0708(x+i*sys.FontX, y, DgtBuf[i], sys.FontKind);
	}
    for(;i<4;i++) {
        DrawFont0708(x+i*sys.FontX+3, y, DgtBuf[i], sys.FontKind);
	}
    for(;i<8;i++) {
		DrawFont0708(x+i*sys.FontX+6, y, DgtBuf[i], sys.FontKind);
	}
    
    for(i=0;i<2;i++) {
        DrawPoint(14+i, 1, flag);
        DrawPoint(14+i, 2, flag);
        DrawPoint(14+i, 4, flag);
        DrawPoint(14+i, 5, flag);
    }
}

void DisplayMenu(byte menunum)
{
    static byte BlinkFlag=0;
    
    BlinkFlag ^= 1;
    if(BlinkFlag) DispString( 0, 0, (byte *)"TiCaAlFo", FONT_0708, sys.FontKind, 0);
    else {
        switch(menunum) {
        case 1:
            DispString( 0, 0, (byte *)"  CaAlFo", FONT_0708, sys.FontKind, 0);
            break;
            
        case 2:
            DispString( 0, 0, (byte *)"Ti  AlFo", FONT_0708, sys.FontKind, 0);
            break;
            
        case 3:
            DispString( 0, 0, (byte *)"TiCa  Fo", FONT_0708, sys.FontKind, 0);
            break;
            
        case 4:
            DispString( 0, 0, (byte *)"TiCaAl  ", FONT_0708, sys.FontKind, 0);
            break;

        }
    }
}

#if 0
void MAX7219_SetReg(byte reg, byte val)
{
	MAX7219_CS_L();
	SPI1WriteByte(reg);
	SPI1WriteByte(val);
	MAX7219_CS_H();
}
#else
#define _WHITE_LED
void MAX7219_SetReg(byte reg, byte val)
{
	byte i;
	
	MAX7219_CS_L();
	for(i=0;i<MAX_GARO_MATRIX;i++) {//8개의 모듈에 모두 세팅..
		SPI1WriteByte(reg);
#ifdef _WHITE_LED
		if(reg == REG_INTENSITY) {
			SPI1WriteByte(val-1);
		} else {
			SPI1WriteByte(val);
		}
#else
        SPI1WriteByte(val);
#endif
	}
	MAX7219_CS_H();
}
#endif


void MAX7219_NOP(void)
{
	MAX7219_CS_L();
	SPI1WriteByte(0);
	SPI1WriteByte(0);
	MAX7219_CS_H();
}

void MAX7219_BufClear(byte page)
{
	byte i, j;
    byte start, end;
    
    switch(page) {
    case MEM_PAGE0:
        start = 0;
        end = MAX_GARO_MATRIX;
        break;
        
    case MEM_PAGE1:
        start = MAX_GARO_MATRIX;
        end = MAX_GARO_MATRIX*2;
        break;
        
    case MEM_PAGEALL:
        start = 0;
        end = MAX_GARO_MATRIX*2+1;
        break;
    }
    
	for(i=start;i<end;i++) {
        for(j=0;j<MAX_SERO_DOT;j++) {
            sys.DotMatrix[i][j] = 0;
        }
    }
}

void MAX7219_Clear(void)
{
	byte i, j;
    
    sys.DispDot = OFF;
	for(i=0;i<MAX_GARO_MATRIX;i++) {
        MAX7219_CS_L();
        for(j=0;j<MAX_SERO_DOT;j++) {
			SPI1WriteByte(i+1);
			SPI1WriteByte(0);
        }
        delay_us(1);
        MAX7219_CS_H();
    }
    sys.DispDot = ON;
}

void InitMax7219(void)
{
	
#if 1
	byte i, j, k=0x80;

    MAX7219_BufClear(MEM_PAGEALL);
    MAX7219_Clear();
	Delay(DELAY10MS);
    
	MAX7219_SetReg(REG_DISPLAY_TEST,0x00);   Delay(DELAY500MS);
    MAX7219_SetReg(REG_SHUTDOWN,	0x00);
    MAX7219_SetReg(REG_DECODE,		0x00);
    MAX7219_SetReg(REG_SCAN_LIMIT,	0x07);
    MAX7219_SetReg(REG_INTENSITY,	sys.DotMatrixBright);//bright : MAX 0x0f
    MAX7219_SetReg(REG_SHUTDOWN,	0x01);

    sys.DispDot = ON;
	//가로 1번째 줄(좌에서 우로) 64개 Dot
	for(i=0;i<64;i++) {
		sys.DotMatrix[i/8][0] |= k;
		k >>= 1;
		if(k == 0) k = 0x80;
		Delay(DELAY5MS);
	}

	//세로 마지막 줄(위에서 아래로) 7개 Dot
	for(j=1;j<8;j++) {
		sys.DotMatrix[7][j] = 0x01;
		Delay(DELAY5MS);
	}

	//가로 마지막 줄(우에서 좌로) 64개 Dot
	k = 0x01;
	for(i=64;i>0;i--) {
		sys.DotMatrix[(i-1)/8][7] |= k;
		k <<= 1;
		if(k == 0) k = 0x01;
		Delay(DELAY5MS);
	}
	
	//세로 첫번째 줄(아래에서 위로) 7개 Dot
	for(j=6;j>0;j--) {
		sys.DotMatrix[0][j] = 0x80;
		Delay(DELAY5MS);
	}
	Delay(DELAY800MS);
	MAX7219_BufClear(MEM_PAGE0);

	k = 0x80;
	for(i=0;i<64;i++) {
        for(j=0;j<8;j++) {
            sys.DotMatrix[i/8][j] = k;
        }
		k >>= 1;
		if(k == 0) k = 0x80;
		Delay(DELAY30MS);
		MAX7219_BufClear(MEM_PAGE0);
    }
	
/*	for(i=0;i<8;i++) {
        for(j=0;j<8;j++) {
            sys.DotMatrix[i][j] = 0xff;
            Delay(DELAY30MS);
			sys.DotMatrix[i][j] = 0;
        }
    }*/
	
 	MAX7219_Clear();
	//테스트 코드, Max7219 모듈중에 하나의 도트매트릭스가 불량이어서 테스트 진행함..
/*	for(i=0;i<8;i++) {
        for(j=0x80;j!=0;j>>=1) {
            sys.DotMatrix[2][i] = j;
            Delay(DELAY500MS);
        }
    }*/
    Delay(DELAY100MS); 
#endif
    

#if 0
	byte i;
	
    MAX7219_BufClear(MEM_PAGEALL);
	Delay(DELAY10MS);
    for(i=0;i<MAX_GARO_MATRIX;i++) {
        MAX7219_SetReg(REG_DISPLAY_TEST,0x01); 
        MAX7219_SetReg(REG_INTENSITY,	sys.DotMatrixBright);//bright : MAX 0x0f
        MAX7219_SetReg(REG_SCAN_LIMIT,	0x07);
        MAX7219_SetReg(REG_SHUTDOWN,	0x01);
        MAX7219_SetReg(REG_DECODE,		0x00);
    }
 	MAX7219_Clear();
    Delay(DELAY500MS);
   
    for(i=0;i<MAX_GARO_MATRIX;i++) MAX7219_SetReg(REG_DISPLAY_TEST,0x00);   Delay(DELAY500MS);
    for(i=0;i<MAX_GARO_MATRIX;i++) MAX7219_SetReg(REG_DISPLAY_TEST,0x01);   Delay(DELAY500MS);
    for(i=0;i<MAX_GARO_MATRIX;i++) MAX7219_SetReg(REG_DISPLAY_TEST,0x00);   Delay(DELAY500MS);

    Delay(DELAY500MS);
#endif

}

void DotMatrix(void)
{
    static byte fTimeUpdate=0, fTimer=0;
	byte i, j, val, xpos, ypos;//, Buff, val;
    byte DgtTmp[12] = {0, };// [2025-06-02] CS.Hong
    
    if(CalendarPorcess() == OK) fTimeUpdate = 1;
	switch(sys.DispAction) {
	case ACT_LEFTSCROLL:
        if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
            SetTimer(TMR_ID_DISPUPDATE, DELAY50MS);//50
            if(sys.Count++ > 13) {
                sys.Count = 0;
                MatrixDispNum(0,0,sys.MatrixNum++);
                if(sys.MatrixNum > 999999) sys.MatrixNum = 0;
            } else {
#if 1
				if(sys.Count <= sys.FontX) {
					for(i=0;i<MAX_NUMBER_DIGIT;i++) {
						if(sys.CurrNum[i] != sys.NextNum[i]) {
							//DrawR90Font0508(FONT_SAVE_POSI, 0, (sys.NextNum[i]+'0'));
                            DrawFont0708(FONT_SAVE_POSI, 0,(sys.NextNum[i]+'0'), F_8X8_GHOTIC);//F_8X8_GHOTIC F_8X8_SIMPLE
							for(xpos=0;xpos<(sys.FontX-1);xpos++) {
								for(ypos=0;ypos<sys.FontY;ypos++) {
									val = ReadPoint(i*sys.FontX+xpos+1, ypos);
									DrawPoint(i*sys.FontX+xpos, ypos, val);
								}
							}
							for(ypos=0;ypos<sys.FontY;ypos++) {
								val = ReadPoint(FONT_SAVE_POSI+sys.Count-1, ypos);
								DrawPoint(i*sys.FontX+xpos, ypos, val);//xpos=(sys.FontX-1)
							}							
						}
					}
				}
#endif
            }
        }
		break;
		
	case ACT_UPSCROLL:
        if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
            SetTimer(TMR_ID_DISPUPDATE, DELAY50MS);
            if(sys.Count++ > 13) {//
                sys.Count = 0;
                MatrixDispNum(0,0,sys.MatrixNum++);
                if(sys.MatrixNum > 999999) sys.MatrixNum = 0;
            } else {
				if(sys.Count <= sys.FontY) {
					for(i=0;i<MAX_NUMBER_DIGIT;i++) {
						if(sys.CurrNum[i] != sys.NextNum[i]) {
							//DrawR270Font0508(FONT_SAVE_POSI, 0, (sys.NextNum[i]+'0'));
                            DrawR90Font0508(FONT_SAVE_POSI, 0, (sys.NextNum[i]+'0'));
                            //DrawFont0708(FONT_SAVE_POSI, 0,(sys.NextNum[i]+'0'), F_8X8_GHOTIC);//F_8X8_GHOTIC F_8X8_SIMPLE
                            
							for(ypos=0;ypos<(sys.FontY-1);ypos++) {
								for(xpos=0;xpos<sys.FontX;xpos++) {
									val = ReadPoint(i*sys.FontX+xpos, ypos+1);
									DrawPoint(i*sys.FontX+xpos, ypos, val);
								}
							}
							for(xpos=0;xpos<sys.FontX;xpos++) {
								val = ReadPoint(FONT_SAVE_POSI+xpos, sys.Count-1);
								DrawPoint(i*sys.FontX+xpos, ypos, val);//ypos=sys.FontY-1
							}							
						}
					}
				}
            }
        }
		break;
		
	case ACT_ALL_LEFTSCR:
        if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
        	SetTimer(TMR_ID_DISPUPDATE, DELAY50MS);
            if(sys.Count++ >= 64) {
                Delay(DELAY3000MS);
                MAX7219_BufClear(MEM_PAGEALL);

                sys.DispAction = ACT_LEFTRAIN;
                sys.Count = 0;
                sys.bPosition = 0;
            } else {
                for(i=0;i<16;i++) {
                    for(j=0;j<8;j++) {
                        if(i==15) {
                            sys.DotMatrix[i][j] =  sys.DotMatrix[i][j] << 1;
                        } else {
                            sys.DotMatrix[i][j] =  sys.DotMatrix[i][j] << 1 | (sys.DotMatrix[i+1][j] & 0x80 ? 1 : 0);
                        }
                    }
                }
            }
		}
		break;
		
	case ACT_ALL_RIGHSCR:
        if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
        	SetTimer(TMR_ID_DISPUPDATE, DELAY50MS);
            
            if(sys.Count++ > 128) {
                DispString(64, 0, (byte *)"C.S. Hong", FONT_0708, F_8X8_SIMPLE, 0);//*F_8X8_SIMPLE  F_8X8_GHOTIC
                //DispString(64, 0, (byte *)"ABCDEFGHI", FONT_0708, F_8X8_SIMPLE, 0);//*F_8X8_SIMPLE  F_8X8_GHOTIC
                sys.Count = 0;
                sys.DispAction = ACT_ALL_LEFTSCR;
            } else {
                for(i=16;i!=0;i--) {
                    for(j=0;j<8;j++) {
                        if(i > 1) {
                            if(i == 16) {
                                sys.DotMatrix[i][j] =  sys.DotMatrix[i][j] >> 1 | (sys.DotMatrix[i-1][j] & 0x01 ? 0x80 : 0x00);
                            }
                            sys.DotMatrix[i-1][j] =  sys.DotMatrix[i-1][j] >> 1 | (sys.DotMatrix[i-1-1][j] & 0x01 ? 0x80 : 0x00);
                        } else {
                            sys.DotMatrix[i-1][j] =  sys.DotMatrix[i-1][j] >> 1 | (sys.DotMatrix[16][j] & 0x80 ? 0x80 : 0x00);
                        }
                    }
                }
            }
		}
		break;
        
	case ACT_DISPCHG_ALL_UP:
        if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
            SetTimer(TMR_ID_DISPUPDATE, DELAY50MS);
            if(sys.Count++ > 15) {//
                sys.Count = 0;
                sys.DispAction = sys.SaveAction;
                //MAX7219_BufClear(MEM_PAGEALL);
                //ClockNumber1( 64, 0,sys.time.hour, sys.time.min, sys.time.sec, sys.SettingFlag);
                //CalendarDisp( 0, 0, sys.time.year, sys.time.month, sys.time.day, sys.SettingFlag);
            } else {
				if(sys.Count <= sys.FontY) {
                    for(ypos=0;ypos<7;ypos++) {
                        for(xpos=0;xpos<8;xpos++) {
                            sys.DotMatrix[xpos][ypos] = sys.DotMatrix[xpos][ypos+1];
                        }
                    }
                    for(xpos=0;xpos<8;xpos++) {
                        sys.DotMatrix[xpos][7] = sys.DotMatrix[xpos+8][sys.Count-1];
                    }
                }
            }
        }	
		break;
        
	case ACT_ALL_UP:
        if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
            SetTimer(TMR_ID_DISPUPDATE, DELAY50MS);
            if(sys.Count++ > 15) {//
                sys.Count = 0;
                sys.DispAction = ACT_ALL_DOWN;
                MAX7219_BufClear(MEM_PAGEALL);
                ClockNumber1( 64, 0, &sys.time, sys.SettingFlag);
				CalendarDisp( 0, 0, &sys.time, sys.SettingFlag);
            } else {
				if(sys.Count <= sys.FontY) {
                    for(ypos=0;ypos<7;ypos++) {
                        for(xpos=0;xpos<8;xpos++) {
                            sys.DotMatrix[xpos][ypos] = sys.DotMatrix[xpos][ypos+1];
                        }
                    }
                    for(xpos=0;xpos<8;xpos++) {
                        sys.DotMatrix[xpos][7] = sys.DotMatrix[xpos+8][sys.Count-1];
                    }

                }
            }
        }	
		break;

	case ACT_ALL_DOWN:
        if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
            SetTimer(TMR_ID_DISPUPDATE, DELAY50MS);
            if(sys.Count++ > 15) {//
                sys.Count = 0;
                sys.DispAction = ACT_CLOCK1;
                MAX7219_BufClear(MEM_PAGEALL);
                ClockNumber1(  0, 0, &sys.time, sys.SettingFlag);
				CalendarDisp(64, 0, &sys.time, sys.SettingFlag);
            } else {
				if(sys.Count <= sys.FontY) {
                    for(ypos=7;ypos!=0;ypos--) {
                        for(xpos=0;xpos<8;xpos++) {
                            sys.DotMatrix[xpos][ypos] = sys.DotMatrix[xpos][ypos-1];
                        }
                    }
                    for(xpos=0;xpos<8;xpos++) {
                        sys.DotMatrix[xpos][0] = sys.DotMatrix[xpos+8][8-sys.Count];
                    }

                }
            }
        }
        break;
        
    case ACT_LEFTRAIN:
        if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
            SetTimer(TMR_ID_DISPUPDATE, DELAY2MS);
            if(sys.bPosition == 0) {
                if(sys.SaveAction == ACT_CLOCK1) {
                    ClockNumber1(64,0, &sys.time, sys.SettingFlag);
                    ClockDot(MEM_PAGE1, 1);
                } else if(sys.SaveAction == ACT_CLOCK2) {
                    ClockNumber2(64, 0,&sys.time, 0);
                } else if(sys.SaveAction == ACT_CLOCK3) {
                    ClockNumber3(64, 0,&sys.time, 0);
                }
                sys.bPosition++;
            } else if(++sys.Count <= (64-sys.bPosition)) {//sys.bPosition
                for(ypos=0;ypos<8;ypos++) {
                    DrawPoint(64-sys.Count, ypos, 0);
                    val = ReadPoint(64+(sys.bPosition-1), ypos);
                    DrawPoint(64-sys.Count-1, ypos, val);
                }
            } else {
                sys.Count = 0;
                sys.bPosition++;
                if(sys.bPosition >= 64) {
                    sys.DispAction = sys.SaveAction;
                    sys.Count = 15;
                }
            }
        }
        break;
        
    case ACT_SETTING:
        if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
            SetTimer(TMR_ID_DISPUPDATE, DELAY300MS);
            fTimer ^= 1;
            if(!sys.SettingFlag) {
                DisplayMenu(sys.SettingMenu);
            } else if(sys.SettingFlag >= 1 && sys.SettingFlag <= 4) {
                ClockNumber1( 0, 0, &sys.time, sys.SettingFlag);
                ClockDot(MEM_PAGE0, fTimer);      
            } else if(sys.SettingFlag >= 5 && sys.SettingFlag < 8) {
				CalendarDisp(0, 0, &sys.time, sys.SettingFlag);
            } else if(sys.SettingFlag == 8) {
                if(sys.SaveAction >= ACT_CLOCK1 && sys.SaveAction <= ACT_CLOCK3) {
                    if(fTimer) {
                        DispString( 0, 0, (byte *)"FONT:1.23", FONT_0708, sys.FontKind, 0);//*F_8X8_SIMPLE  F_8X8_GHOTIC
                    } else {
                        DispString( 0, 0, (byte *)"         ", FONT_0708, sys.FontKind, 0);//*F_8X8_SIMPLE  F_8X8_GHOTIC
                    }
                }
            } else if(sys.SettingFlag >= 9 && sys.SettingFlag <= 11) {
                AlarmNumber(0, 0, sys.AlHour, sys.AlMin, sys.AlOnOff, sys.SettingFlag);
            }
        }
        break;
        
	case ACT_TEMPERATURE:
		if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
            SetTimer(TMR_ID_DISPUPDATE, DELAY50MS);

            if(sys.Count++ > 30) {//
                sys.Count = 0;
                if(sys.TempHumidity == DISP_TEMPERATURE) {
                    sys.TempHumidity = DISP_HUMIDITY;
                    MAX7219_BufClear(MEM_PAGE1);
                    DispTempHumi(64, 0, sys.TempHumidity, sys.Humidity);
                } else if(sys.TempHumidity == DISP_HUMIDITY) {
                    sys.TempHumidity = DISP_HUMIDITY_DONE;
                    MAX7219_BufClear(MEM_PAGE1);
                    sys.time.sec += 2;
                    ClockNumber1( 64, 0, &sys.time, sys.SettingFlag);
					sys.time.sec -= 2;
                    DispMoveSmallTemp(0);//정지글자표현
                } else if(sys.TempHumidity == DISP_HUMIDITY_DONE) {
                    sys.DispAction = ACT_CLOCK1;
                    sys.SmallCnt   = 0;//SmallDisplay 초기화(다시 그리기)
                }
            } else {
				if(sys.Count <= 8) {//Matrix 세로사이즈
                    for(ypos=0;ypos<7;ypos++) {
                        for(xpos=0;xpos<8;xpos++) {
                            sys.DotMatrix[xpos][ypos] = sys.DotMatrix[xpos][ypos+1];
                        }
                    }
                    for(xpos=0;xpos<8;xpos++) {
                        sys.DotMatrix[xpos][7] = sys.DotMatrix[xpos+8][sys.Count-1];
                    }
				}
            }
        }	
		break;
        
    case ACT_CLOCK1://10
        if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
            if(fTimeUpdate == 1) {
                fTimeUpdate = 0;
                sys.Count = 0;
				
				//OLEDClockNumber( 0, 0, sys.time.hour, sys.time.min, sys.time.sec, 1, FONT_3216_2);
				if(sys.time.sec == 0) {
                    if(sys.time.min == 0) {
                        if(sys.time.hour > 8) {//0시~8시까지 울리지 않음.
                            if(sys.TimeSignal) {
                                if(sys.time.wday > 0 && sys.time.wday < 6) {//1,2,3,4,5 :월 화 수 목 금
                                    SetBuzzer(BUZZ_TIMESIG);
                                    //SetBuzzer(BUZZ_KEY2);
                                    //SetBuzzer(BUZZ_KEY2);
                                }
                            }
                        }
                        sys.WiFiState &= ~WIFI_TIME_UPDATE;//매시 정각에 없데이트 플래그를 리셋함..
                    }
                    
					if(sys.time.hour == sys.AlHour && sys.time.min == sys.AlMin && sys.AlOnOff == ON) {
						if( (sys.time.wday > 0) && (sys.time.wday < 6) ) sys.AlNow = ON;//월~금요일까지.
					} else {
						if(sys.AlNow == ON) sys.AlNow = OFF;
					}

					if(sys.time.min >= 5 && !(sys.WiFiState & WIFI_TIME_UPDATE)) {//매시 5분에 - 기상정보가 정시에 업데이트 되지 않음...  5분 뒤에 업데이트함..
						if(sys.ClockAfterSet > 20) {//20초 보다 크면
							if(sys.WiFiSeq == WIFISEQ_NOACTION || (sys.WiFiSeq == WIFISEQ_ERROR && sys.WiFiError != WIFISEQ_CONNECT) ) {
								sys.WiFiSeq = WIFISEQ_CONNECT;
								sys.WiFiState |= WIFI_TIMESYNC;//시간동기화 가능하도록 변경
							}
						}
					}
					fTimer = 0;
                    
#if 0                    
					sys.DispAction = ACT_TEMPERATURE;//ACT_ALL_LEFTSCR;
					MAX7219_BufClear(MEM_PAGEALL);
					
					sys.TempHumidity = DISP_TEMPERATURE;
					DispTempHumi(64, 0, sys.TempHumidity, sys.Temperature);
#else
					sys.DispAction = ACT_WEATHER_INFO;//ACT_ALL_LEFTSCR;
					MAX7219_BufClear(MEM_PAGEALL);
					
					sys.TempHumidity = DISP_DATE;
                    CalendarDisp(64, 0, &sys.time, 0);
#endif
				}
				//if(sys.AlNow == ON) SetBuzzer(BUZZ_MAIN);

				ClockNumber1( 0, 0, &sys.time, sys.SettingFlag);
                ClockDot(MEM_PAGE0, 1);
            } else {
                byte space=0;
                SetTimer(TMR_ID_DISPUPDATE, DELAY50MS);

				sys.Count++;
				if(sys.Count <= 8) {//Matrix 세로사이즈
					for(i=0;i<MAX_NUMBER_DIGIT;i++) {
						if(sys.CurrNum[i] != sys.NextNum[i]) {
							DrawFont0708(FONT_SAVE_POSI, 0,(sys.NextNum[i]+'0'), sys.FontKind);
							if(i>=4)        space = 6;
							else if(i>=2)   space = 3;
							else            space = 0;
							for(ypos=0;ypos<(sys.FontY-1);ypos++) {
								for(xpos=0;xpos<sys.FontX;xpos++) {
									val = ReadPoint(i*sys.FontX+xpos+space, ypos+1);
									DrawPoint(i*sys.FontX+xpos+space, ypos, val);
								}
							}
							for(xpos=0;xpos<sys.FontX;xpos++) {
								val = ReadPoint(FONT_SAVE_POSI+xpos, sys.Count-1);
								DrawPoint(i*sys.FontX+xpos+space, ypos, val);
							}							
						}
					}
                } else if(sys.Count == 9) {
                    if(I2CGetData(I2C_SET_ADDR) != 0) {//온도, 습도
                        sys.Humidity = (word)0;
                        sys.Temperature = (word)0;
                    }
                } else if(sys.Count == 10) {
					//OLEDClockNumber( 0, 0, sys.time.hour, sys.time.min, sys.time.sec, 0, FONT_3216_2);
					ClockDot(MEM_PAGE0, 0);
                    if(I2CGetData(I2C_GET_ADDR) != 0) {//온도, 습도
                        sys.Humidity = (word)0;
                        sys.Temperature = (word)0;
                    }
				}
            }
        }
        break;
      
    case ACT_CLOCK2://11
        if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
            if(fTimeUpdate == 1) {
                fTimeUpdate = 0;
                sys.Count = 0;
				//OLEDClockNumber( 0, 0, sys.time.hour, sys.time.min, sys.time.sec, 1, FONT_3216_2);
                if(sys.SettingFlag < 5) {
                    ClockNumber2( 0, 0, &sys.time, sys.SettingFlag);
                    ClockDot2(MEM_PAGE0, 1);
                }

				if(sys.time.sec == 0) {
                    if(sys.time.min == 0) {
                        if(sys.time.hour > 8) {//0시~8시까지 울리지 않음.
                            if(sys.TimeSignal) {
                                if(sys.time.wday > 0 && sys.time.wday < 6) {//1,2,3,4,5 :월 화 수 목 금
                                    SetBuzzer(BUZZ_TIMESIG);
                                    //SetBuzzer(BUZZ_KEY2);
                                    //SetBuzzer(BUZZ_KEY2);
                                }
                            }
                        }
                        sys.WiFiState &= ~WIFI_TIME_UPDATE;//매시 정각에 없데이트 플래그를 리셋함..
                    }
                    
					if(sys.time.hour == sys.AlHour && sys.time.min == sys.AlMin && sys.AlOnOff == ON) {
						if( (sys.time.wday > 0) && (sys.time.wday < 6) ) sys.AlNow = ON;//월~금요일까지.
					} else {
						if(sys.AlNow == ON) sys.AlNow = OFF;
					}

					if(sys.time.min >= 5 && !(sys.WiFiState & WIFI_TIME_UPDATE)) {//매시 5분에 - 기상정보가 정시에 업데이트 되지 않음...  5분 뒤에 업데이트함..
						if(sys.ClockAfterSet > 20) {//20초 보다 크면
							if(sys.WiFiSeq == WIFISEQ_NOACTION || (sys.WiFiSeq == WIFISEQ_ERROR && sys.WiFiError != WIFISEQ_CONNECT) ) {
								sys.WiFiSeq = WIFISEQ_CONNECT;
								sys.WiFiState |= WIFI_TIMESYNC;//시간동기화 가능하도록 변경
							}
						}
					}
					fTimer = 0;
				}
            } else {
                SetTimer(TMR_ID_DISPUPDATE, DELAY50MS);
                sys.Count++;
                if(sys.SettingFlag == 0) {//노멀모드이면....
                    if(sys.Count <= sys.FontY) {

                    } else if(sys.Count == 9) {
                        if(I2CGetData(I2C_SET_ADDR) != 0) {//온도, 습도
                            sys.Humidity = (word)0;
                            sys.Temperature = (word)0;
                        }
                    } else if(sys.Count == 10) {
                        //OLEDClockNumber( 0, 0, sys.time.hour, sys.time.min, sys.time.sec, 0, FONT_3216_2);
                        ClockDot2(MEM_PAGE0, 0);
                        if(I2CGetData(I2C_GET_ADDR) != 0) {//온도, 습도
                            sys.Humidity = (word)0;
                            sys.Temperature = (word)0;
                        }
                    }

                    
                }
            }
        }
        break;
        

    case ACT_CLOCK3://12
        if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
            if(fTimeUpdate == 1) {
                fTimeUpdate = 0;
                sys.Count = 0;
				
				if(sys.time.sec == 0) {
                    // 매시 정각에...  매시 0분 0초에 
                    if(sys.time.min == 0) {// 0분
                        if(sys.time.hour > 8) {//0시~8시까지 울리지 않음.
                            if(sys.TimeSignal) {
                                if(sys.time.wday > 0 && sys.time.wday < 6) {//1,2,3,4,5 :월 화 수 목 금
                                    SetBuzzer(BUZZ_TIMESIG);
                                    //SetBuzzer(BUZZ_KEY2);
                                    //SetBuzzer(BUZZ_KEY2);
                                    //SetBuzzer(BUZZ_MAIN);
                                }
                            }
                        }
                        sys.WiFiState &= ~WIFI_TIME_UPDATE;//매시 정각에 없데이트 플래그를 리셋함..
                    }
                    
                    // 알람 시간 비교 / 알람을 켜 놓았는지 확인...
					if(sys.time.hour == sys.AlHour && sys.time.min == sys.AlMin && sys.AlOnOff == ON) {
						if( (sys.time.wday > 0) && (sys.time.wday < 6) ) sys.AlNow = ON;//월~금요일까지.
					} else {
						if(sys.AlNow == ON) sys.AlNow = OFF;
					}

                    // 매시 5분 0초에...  WIFI_TIME_UPDATE 시간 업데이트가 되어 있지 않으면...
					if(sys.time.min >= 5 && !(sys.WiFiState & WIFI_TIME_UPDATE)) {//매시 5분에 - 기상정보가 정시에 업데이트 되지 않음...  5분 뒤에 업데이트함..
						if(sys.ClockAfterSet > 20) {// 일반적인경우 아래로 진입, 시계가 켜진지 20초가 지나지 않으면 다음 0초가 될 때 까지 기다림... (Wi-fi잡는시간동안 기다림)
							if(sys.WiFiSeq == WIFISEQ_NOACTION || (sys.WiFiSeq == WIFISEQ_ERROR && sys.WiFiError != WIFISEQ_CONNECT) ) {
								sys.WiFiSeq = WIFISEQ_CONNECT;  // Wifi 모듈의 시퀀스를 처음부터 다시 읽음. (IP, 시간, 날씨, 미세먼지 등을 모두 읽어 오도록 변경함)
								sys.WiFiState |= WIFI_TIMESYNC; // 시간동기화 가능하도록 변경
							}
						}
					}

					fTimer = 0;
				}
				ClockNumber3( 0, 0, &sys.time, sys.SettingFlag);
                ClockDot3(MEM_PAGE0, 1);
            } else {
                byte space=0;
                SetTimer(TMR_ID_DISPUPDATE, DELAY50MS);

				sys.Count++;
				if(sys.Count <= 8) {//Matrix 세로사이즈
					for(i=0;i<MAX_CLOCK3_DIGIT;i++) {
						if(sys.CurrNum[i] != sys.NextNum[i]) {
							DrawFont0708(FONT_SAVE_POSI, 0,(sys.NextNum[i]+'0'), sys.FontKind);
							if(i>=2)   space = 3;
							else       space = 0;
							for(ypos=0;ypos<(sys.FontY-1);ypos++) {
								for(xpos=0;xpos<sys.FontX;xpos++) {
									val = ReadPoint(i*sys.FontX+xpos+space, ypos+1);
									DrawPoint(i*sys.FontX+xpos+space, ypos, val);
								}
							}
							for(xpos=0;xpos<sys.FontX;xpos++) {
								val = ReadPoint(FONT_SAVE_POSI+xpos, sys.Count-1);
								DrawPoint(i*sys.FontX+xpos+space, ypos, val);
							}
						}
					}
				} else if(sys.Count == 9) {
					if(I2CGetData(I2C_SET_ADDR) != 0) {//온도, 습도
						sys.Humidity = (word)0;
						sys.Temperature = (word)0;
					}
				} else if(sys.Count == 10) {
					ClockDot3(MEM_PAGE0, 0);
					if(I2CGetData(I2C_GET_ADDR) != 0) {//온도, 습도
						sys.Humidity = (word)0;
						sys.Temperature = (word)0;
					}
				}
            }
        }
        break;
        
	case ACT_WEATHER_INFO:
        if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
            SetTimer(TMR_ID_DISPUPDATE, DELAY30MS);        	
#ifdef _UPSCROLL
            if(sys.Count++ >= 50) {//50 우측스크롤
#else
            if(sys.Count++ >= 80) {//80 우측스크롤
#endif
                sys.Count = 0;
                sys.TempHumidity++;
                if(sys.TempHumidity != DISP_CLOCK_RETURN) MAX7219_BufClear(MEM_PAGE1);

                switch(sys.TempHumidity) {
                case DISP_TEMPERATURE:
                    DispTempHumi(64, 0, sys.TempHumidity, sys.Temperature);
                    break;
                    
                case DISP_HUMIDITY:
                    DispTempHumi(64, 0, sys.TempHumidity, sys.Humidity);
                    break;

                case DISP_WEA_TEMP:
                    DispTempHumi(64, 0, sys.TempHumidity, 0);
                    break;
                    
                case DISP_WEA_HUMI:
                    DispTempHumi(64, 0, sys.TempHumidity, weather.Humidity);
                    break;
                    
                case DISP_RAIN_PERCENT:
                    DispTempHumi(64, 0, sys.TempHumidity, (word)weather.RainyPercent);
                    break;
                    
                case DISP_WIND:
                    DispTempHumi(64, 0, sys.TempHumidity, 0);
                    break;
                    
                case DISP_WEATHER_STA:
                    DispTempHumi(64, 0, sys.TempHumidity, (word)weather.WeatherCode);
                    break;
                    
                case DISP_HUMIDITY_DONE:
                    sys.time.sec += 2;
                    ClockNumber1( 64, 0, &sys.time, sys.SettingFlag);
					sys.time.sec -= 2;
                    DispMoveSmallTemp(0);//정지글자표현
                    break;
                    
                case DISP_CLOCK_RETURN:
                    sys.DispAction = ACT_CLOCK1;
                    sys.SmallCnt   = 0;//SmallDisplay 초기화(다시 그리기)
                    break;
                }
            } else {
#ifdef _UPSCROLL
                //세로스크롤
				if(sys.Count <= 8) {//Matrix 세로사이즈
                    for(ypos=0;ypos<7;ypos++) {
                        for(xpos=0;xpos<8;xpos++) {
                            sys.DotMatrix[xpos][ypos] = sys.DotMatrix[xpos][ypos+1];
                        }
                    }
                    for(xpos=0;xpos<8;xpos++) {
                        sys.DotMatrix[xpos][7] = sys.DotMatrix[xpos+8][sys.Count-1];
                    }
				}                
#else
                //우측스크롤
                if(sys.Count <=  64) {//Matrix 가로사이즈
                    for(i=0;i<16;i++) {
                        for(j=0;j<8;j++) {
                            if(i==15) {
                                sys.DotMatrix[i][j] =  sys.DotMatrix[i][j] << 1;
                            } else {
                                sys.DotMatrix[i][j] =  sys.DotMatrix[i][j] << 1 | (sys.DotMatrix[i+1][j] & 0x80 ? 1 : 0);
                            }
                        }
                    }
                }
#endif          
            }
		}
		break;

	case ACT_LOCAL_IP:
		if(sys.ViewIP == ON) {
			if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
				SetTimer(TMR_ID_DISPUPDATE, DELAY50MS);
				if(sys.Count++ > 15) {//
					sys.Count = 0;
					sys.DispAction = sys.SaveAction;
					//MAX7219_BufClear(MEM_PAGEALL);
					//ClockNumber1( 64, 0,sys.time.hour, sys.time.min, sys.time.sec, sys.SettingFlag);
				} else {
					if(sys.Count <= sys.FontY) {
						for(ypos=0;ypos<7;ypos++) {
							for(xpos=0;xpos<8;xpos++) {
								sys.DotMatrix[xpos][ypos] = sys.DotMatrix[xpos][ypos+1];
							}
						}
						for(xpos=0;xpos<8;xpos++) {
							sys.DotMatrix[xpos][7] = sys.DotMatrix[xpos+8][sys.Count-1];
						}
					} else {
                        if(sys.Count < 8 && sys.FontY < 8) MAX7219_BufClear(MEM_PAGEALL);//작은폰트에서 화면에 남는 쓰레기 값 지움
                    }
				}
			}
		} else {
			if(GetTimer(TMR_ID_DISPUPDATE) == 0) {
				SetTimer(TMR_ID_DISPUPDATE, DELAY500MS);
				fTimer ^= 1;
				if(fTimer) {
					DispString( 0, 0, (byte *)"IP Assign", FONT_0708, sys.FontKind, 0);//*F_8X8_SIMPLE  F_8X8_GHOTIC
				} else {
					DispString( 0, 0, (byte *)"         ", FONT_0708, sys.FontKind, 0);//*F_8X8_SIMPLE  F_8X8_GHOTIC
				}
                sprintf((char *)DgtTmp, "Ver%02d.%02d\0", VERSION_NUM/100, VERSION_NUM%100);
                DispString( 0, 2, DgtTmp, FONT_0406, F_8X8_SIMPLE, 1);
	 		}
		}
		break;
        
	default:
		
		break;
	}
}


