/*****************************************************
* FILE: BiosApp.c
* 
*******************************************************/

/* Include */
	/* */
	#include "main.h"
	#include "BiosFont.h"
	#include "BiosApp.h"
	//#include "hw_config.h"

/* Define */
	/* */

/* Extern */
	extern byte gbDispaly12864[128][8];
	/* */

/* Function */
	//void OLEDWeather(TWeatherValue wt);
	void OLEDIPAddress(byte x, byte y, byte *ptr);
	void OLEDWeather(byte x, byte y, byte sq, TWeatherValue wt);
	void OLEDClockDisp(byte seq, TWeatherValue wt);
	void WeatherKor(byte x, byte y, byte code);
	/* */



/* Includes ------------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/


/* Private typedef -----------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
	TWeatherValue 	weather;
	TDustValue 		dust;

	static char const DayofWeekKorean[7][7]  = {"일요일\0","월요일\0","화요일\0","수요일\0","목요일\0","금요일\0","토요일\0"};
	static char const WindDirName[8][7]  	= {"북풍  \0","북동풍\0","동풍  \0","남동풍\0","남풍  \0","남서풍\0","서풍  \0","북서풍\0"};

/* Private function prototypes -----------------------------------------------*/

	
/* Private functions ---------------------------------------------------------*/

void AppProcess(void)
{
	//byte i, j;
	static byte gbCount=0;
	
    if(GetTimer(TMR_ID_BLK) == 0) {
        SetTimer(TMR_ID_BLK,DELAY100MS);
		
		//if(sys.DispSeq < 60) OLEDDispIcon((127-8), (63-8), gbCount);//FONT_1608_0 FONT_0808_G
		OLEDDispIcon((127-8), (63-8), gbCount);//FONT_1608_0 FONT_0808_G
		if(++gbCount >= 4) {
			gbCount = 0;
			HAL_GPIO_TogglePin(GPIOC, CMAIN_LED_Pin);
		}
    }

	if(sys.ViewIP == OFF) {
		// 처음 부팅시 1회만 실행함.  PC나 스마트폰에서 접속 할 수 있도록 할당된 IP를 표현함.
		SetTimer(TMR_ID_I2CREAD,DELAY3000MS);// 아래의 SPI OLED에 기상상태를 표현 못하게 함..
		if(sys.LocalIP[0] >= '0' && sys.LocalIP[0] <= '9') {
			sys.ViewIP = ON;//IP는 처음에 한번만 표현함.
			MAX7219_BufClear(MEM_PAGEALL);//Buff Clear
			DispLocalIP(0, 2, sys.LocalIP);
			if(sys.SaveAction == ACT_CLOCK3) {
				ClockNumber3(64, 0, &sys.time, 0);
				ClockDot3(MEM_PAGE1, 1);
			}
			OLEDIPAddress(0, 24, sys.LocalIP);
			SetTimer(TMR_ID_DISPUPDATE, DELAY2000MS);
		} else {
			if(sys.WiFiSeq == WIFISEQ_ERROR || sys.WiFiSeq == WIFISEQ_APMODEWAIT) {
				sys.ViewIP = ON;//Wi-Fi 에러 또는 AP모드일 경우 IP표현하지 않고 강제로 넘어감..
				sys.DispAction = sys.SaveAction;
				MAX7219_BufClear(MEM_PAGEALL);//Buff Clear
			}
		}
	} else {
		if(sys.DispAction == ACT_CLOCK1) {
			DispMoveSmallTemp(1);//움직임글자표현
		} else if(sys.DispAction == ACT_CLOCK3) {
			DispMoveSmallWeather(1);//움직임글자표현
		}
	}

	//if(sys.WiFiSeq != WIFISEQ_SERVERWAIT /*WIFISEQ_MSGWAIT*/) return;
#if 1
	if(sys.OLEDUpdate == ON) {
		SetTimer(TMR_ID_COLON, DELAY500MS);
		sys.OLEDUpdate = OFF;
		OLEDClockNumber( 0, 0, sys.time.hour, sys.time.min, sys.time.sec, 1, FONT_1612_1);//FONT_1612_1  FONT_3216_2
		//OLEDDispTemp( 0, 16, FONT_0808_M, sys.Temperature, STATE_TEMPERATURE);//FONT_0808_G  FONT_1608_0
		//OLEDDispTemp(64, 16, FONT_0808_M, sys.Humidity   , STATE_HUMMIDITY);//FONT_1608_0
	} else {
		if(GetTimer(TMR_ID_COLON) == 0) {
			SetTimer(TMR_ID_COLON, DELAY1000MS);//다음 sys.OLEDUpdate가 ON이 될때까지... 기다림..
			OLEDClockNumber( 0, 0, sys.time.hour, sys.time.min, sys.time.sec, 0, FONT_1612_1);//FONT_1612_1  FONT_3216_2
		}
	}

	if(GetTimer(TMR_ID_I2CREAD) == 0) {
		SetTimer(TMR_ID_I2CREAD,DELAY1000MS);
		if(weather.Update == ON) {
			weather.Update = OFF;//값이 읽어 졌는지 확인
			//WeatherCode 		   0:맑음,1:구름조금,2:구름많음,3:흐림,4:비,5:비/눈,6:눈/비,7:눈,8:err
			//IMG_WeatherIcon      1:맑음,2:구름조금,3:구름많음,4:흐림,5:비,6:비/눈,7:눈,8:err
			if(weather.WeatherCode < 6) 	ssd1306_draw_bitmap(0, 24, &IMG_WeatherIcon[320*(weather.WeatherCode+1)], 	60, 40, OLED_REWRITE, 1);
			else							ssd1306_draw_bitmap(0, 24, &IMG_WeatherIcon[320*weather.WeatherCode], 		60, 40, OLED_REWRITE, 1);
			OLEDAreaCodeDisp(weather.AreaCode);
			OLEDWeather(60, 24, 6, weather);
		}
		
		if(dust.Update == ON) {
			dust.Update = OFF;//값이 읽어 졌는지 확인
			//OLEDDust(102, 32);//60
		}
	}
#else

    #define NUMFLAKES 	10
    #define XPOS 		0
    #define YPOS 		1
    #define DELTAY 		2
    
    static byte icons[NUMFLAKES][3], isFirst=1;
	byte f;

    // initialize
    if(isFirst == 1) {
        isFirst = 0;
		srand(sys.time.sec);
        for (f=0;f<NUMFLAKES;f++) {
            icons[f][XPOS] = (byte)rand()%SSD1306_WIDTH;
            icons[f][YPOS] = 0;
            icons[f][DELTAY] = (byte)rand()%5 + 1;
        }
    }
	if(GetTimer(TMR_ID_COLON) == 0) {
		SetTimer(TMR_ID_COLON, DELAY30MS);//다음 sys.OLEDUpdate가 ON이 될때까지... 기다림..

		sys.DispOLED = OFF;
        // erase it + move it
        for(f=0;f<NUMFLAKES;f++) {
			ssd1306_draw_bitmap(icons[f][XPOS], icons[f][YPOS], &star_1616_bmp[0], 16, 16, OLED_OVERWRITE, 0);//

            // move it
            icons[f][YPOS] += icons[f][DELTAY];
            // if its gone, reinit
            if (icons[f][YPOS] > SSD1306_HEIGHT) {
                icons[f][XPOS] = (byte)rand()%SSD1306_WIDTH;
                icons[f][YPOS] = 0;
                icons[f][DELTAY] = (byte)rand()%5 + 1;
            }
        }
		
		// draw each icon
		for(f=0;f<NUMFLAKES;f++) {
			ssd1306_draw_bitmap(icons[f][XPOS], icons[f][YPOS], &star_1616_bmp[0], 16, 16, OLED_OVERWRITE, 1);
        }
		sys.DispOLED = ON;
    }
#endif
}

void OLEDIPAddress(byte x, byte y, byte *ptr)
{
	byte DateMSG[16] = {0};
	
	sprintf((char *)DateMSG, "-Local IP Address-\0");
	ssd1306_0807_string(x, y, DateMSG, 	F_8X8_GHOTIC); y+=10;
	ssd1306_0807_string(x, y, ptr, 		F_8X8_GHOTIC);
}

void OLEDAreaCodeDisp(byte code)
{
	switch(code) {
	case AREA_CODE_JACKJEON_DONG://0
		ssd1306_0807_string(0, 56, (byte *)"J\0", F_8X8_GHOTIC);
		break;
		
	case AREA_CODE_GONGNEUNG_DONG://1
		ssd1306_0807_string(0, 56, (byte *)"G\0", F_8X8_GHOTIC);
		break;

	case AREA_CODE_JUNGNEUNG2_DONG://2
		ssd1306_0807_string(0, 56, (byte *)"8\0", F_8X8_GHOTIC);
		break;
		
	case AREA_CODE_PAJU_UNJUNG1_DONG://3
		ssd1306_0807_string(0, 56, (byte *)"P\0", F_8X8_GHOTIC);
		break;
		
	case AREA_CODE_ASAN_TANGJUNG_MYUN://4
		ssd1306_0807_string(0, 56, (byte *)"A\0", F_8X8_GHOTIC);
		break;
        
    case AREA_CODE_CHUNGLA:
        ssd1306_0807_string(0, 56, (byte *)"C\0", F_8X8_GHOTIC);
        break;
		
	case AREA_CODE_OSAN:
		ssd1306_0807_string(0, 56, (byte *)"O\0", F_8X8_GHOTIC);
		break;
	}
}

void OLEDClockDisp(byte seq, TWeatherValue wt)
{
	if(seq == 0) {
		OLEDCalendar(&sys.time);
	}
	OLEDDispTemp(84, 32, FONT_0808_G, sys.Temperature, STATE_TEMPERATURE);//FONT_0808_G  FONT_1608_0
	OLEDDispTemp(84, 40, FONT_0808_G, sys.Humidity   , STATE_HUMMIDITY);//FONT_1608_0
	OLEDDispFineDust( 0, 48, FONT_0808_M , dust.pms7003[0], DUST_PM1_0);
	OLEDDispFineDust(42, 48, FONT_0808_M , dust.pms7003[1], DUST_PM2_5);
	OLEDDispFineDust(84, 48, FONT_0808_M , dust.pms7003[2], DUST_PM10);

	if(sys.WiFiSeq != WIFISEQ_ERROR) OLEDWeather(0, 16, seq/5, wt);
}

void OLEDWeather(byte x, byte y, byte sq, TWeatherValue wt)
{
	byte DateMSG[16] = {0};
	byte tmpMon, tmpDay;
		
	switch(sq) {
	case 0:
		sprintf((char *)DateMSG, "%04d%02d%02d %02dH발표", wt.year, wt.month, wt.day, wt.hour);
		DispOLEDString( x, y, (byte *)DateMSG);
		break;
		
	case 1:
		sprintf((char *)DateMSG, "온%sC \0", wt.Temperature);
		DispOLEDString( x,   y,(byte *)DateMSG);

		sprintf((char *)DateMSG, "습%02d.0%% \0", wt.Humidity);
		DispOLEDString(x+64, y,(byte *)DateMSG);
		break;
		
	case 2:
		sprintf((char *)DateMSG, "강수확률: %2d%%   \0", wt.RainyPercent);
		DispOLEDString( x, y,(byte *)DateMSG);
		break;
		
	case 3:
		DispOLEDString( x, y,(byte *)DateMSG);
		sprintf((char *)DateMSG, "풍속:%sm/s  ",wt.WindSpd);
		DispOLEDString( x,   y,(byte *)DateMSG);
		break;

	case 4:
		DispOLEDString( x, y,(byte *)DateMSG);
		sprintf((char *)DateMSG, "풍향:%s     ", WindDirName[wt.WindDir]);
		DispOLEDString( x,   y,(byte *)DateMSG);
		break;

	case 5:
		WeatherKor(x, y, wt.WeatherCode);
		break;
		
	case 6:
		if(wt.hour < 20){
			sprintf((char *)DateMSG, "%02d-%02d %02dH\0", wt.month, wt.day, wt.ForecastTime);
		} else {
			tmpMon = wt.month;
			tmpDay = wt.day;
			
			tmpDay += 1;
			if(tmpDay > wt.lastday) {//그 달의 마지막 날이 아니면... 다음날로 표시
				tmpMon += 1; tmpDay = 1;//다음달 1일
				if(tmpMon == 13) tmpMon = 1;
			}
			sprintf((char *)DateMSG, "%02d%02d %02dH\0", tmpMon, tmpDay, wt.ForecastTime);
		}
		//sprintf((char *)DateMSG, "%02d-%02d %02dH\0", wt.month, wt.day, wt.ForecastTime);
		ssd1306_0807_string(x, y, DateMSG, F_8X8_GHOTIC); y+=8;
		
		sprintf((char *)DateMSG, "rain:%3d%%\0", wt.RainyPercent);
		ssd1306_0807_string(x, y, DateMSG, F_8X8_MYUNGJO); y+=8;
		
		sprintf((char *)DateMSG, "wind:%sm\0",wt.WindSpd);
		ssd1306_0807_string(x, y, DateMSG, F_8X8_MYUNGJO); y+=8;
		
		sprintf((char *)DateMSG, "T:%sC\0", wt.Temperature);
		ssd1306_0807_string(x, y, DateMSG, F_8X8_MYUNGJO); y+=8;
		
		sprintf((char *)DateMSG, "H:%02d.0%%\0", wt.Humidity);
		ssd1306_0807_string(x, y, DateMSG, F_8X8_MYUNGJO); y+=8;
		break;
	}
}

#if 0
void OLEDWeather(TWeatherValue wt)
{
	byte DateMSG[16] = {0};

	sprintf((char *)DateMSG, "%04d%02d%02d %02dH발표", wt.year, wt.month, wt.day, wt.hour);
	DispOLEDString( 0, 0,(byte *)DateMSG);
	
	sprintf((char *)DateMSG, "온    C\0");
	memcpy(&DateMSG[2], wt.Temperature, 4);
	DispOLEDString( 0, 16,(byte *)DateMSG);

	sprintf((char *)DateMSG, "습%02d.0%%\0", wt.Humidity);
	DispOLEDString(64, 16,(byte *)DateMSG);

	sprintf((char *)DateMSG, "강수확률:%02d%%\0", wt.RainyPercent);
	DispOLEDString( 0, 32,(byte *)DateMSG);
	
	WeatherKor(0, 48, wt.WeatherCode);
}
#endif

void WeatherKor(byte x, byte y, byte code)
{
	//weather.WeatherCode  //0:맑음,1:구름조금,2:구름많음,3:흐림,4:비,5:비/눈,6:눈/비,7:눈,8:err	
	if(code == 0) {
		switch(code) {
		case 0:
			DispOLEDString( x, y,(byte *)"날씨:맑음       ");
			break;
		case 1:
			DispOLEDString( x, y,(byte *)"날씨:구름조금   ");
			break;
		case 2:
			DispOLEDString( x, y,(byte *)"날씨:구름많음   ");
			break;
		case 3:
			DispOLEDString( x, y,(byte *)"날씨:흐림       ");
			break;
		case 4:
			DispOLEDString( x, y,(byte *)"날씨:비         ");
			break;
		case 5:
			DispOLEDString( x, y,(byte *)"날씨:비/눈      ");
			break;
		case 6:
			DispOLEDString( x, y,(byte *)"날씨:눈/비      ");
			break;
		case 7:
			DispOLEDString( x, y,(byte *)"날씨:눈         ");
			break;
		default:
			DispOLEDString( x, y,(byte *)"날씨:에러       ");
			break;
			
		}
	}
}

#define MAX_TIME_DGT	8
#define MAX_DATE_DGT	10
#define MAX_NUM_DGT		6
void OLEDClockNumber(byte x, byte y, byte hh, byte mm, byte ss, byte sta, byte fntSize)
{
    byte i, DgtBuf[MAX_TIME_DGT+1];
	
	//if(sys.WiFiSeq <= 10) return;
	sprintf((char *)DgtBuf, "%02d:%02d:%02d\0",hh,mm,ss);
	if(sta) {
		DgtBuf[2] = DgtBuf[5] = ':';//:
		//USB_Send_Data((byte *)DgtBuf);
	} else {
		if(fntSize == FONT_1608_0) {
			DgtBuf[2] = DgtBuf[5]  =' ';//공백
		} else {
			DgtBuf[2] = DgtBuf[5] = 11+'0';//공백
		}
	}

	//sys.DispOLED = OFF;
	if(fntSize == FONT_1608_0) {
		DispOLEDString( x, y, DgtBuf);
	} else if(fntSize == FONT_1612_1) {
		for(i=0;i<MAX_TIME_DGT;i++) ssd1306_draw_1612char(x+i*16, y, DgtBuf[i]);
	} else if(fntSize == FONT_3216_2) {
		for(i=0;i<MAX_TIME_DGT;i++) ssd1306_draw_3216char(x+i*16, y, DgtBuf[i]);
	}
	//LCD_drawRect(0, 0, 127, 31, 1);
	//sys.DispOLED = ON;
}


#define MAX_TEMP_DGT	6
void OLEDDispTemp(byte x, byte y, byte fntsiz, word num, byte tmphum)
{
	byte DgtBuf[MAX_TEMP_DGT+1] = {0};

	if(tmphum == STATE_TEMPERATURE) {
		sprintf((char *)DgtBuf, "T%2d.%1dC\0", num/10, num%10);
	} else {
		sprintf((char *)DgtBuf, "H%2d.%1d%%\0", num/10, num%10);
	}
	
	//sys.DispOLED = OFF;
	switch(fntsiz) {
		case FONT_1608_0:
			DispOLEDString( x, y, DgtBuf);
			break;
			
		case FONT_0808_G:
			ssd1306_0807_string(x, y, DgtBuf, F_8X8_GHOTIC);
			break;
			
		case FONT_0808_M:
			ssd1306_0807_string(x, y, DgtBuf, F_8X8_MYUNGJO);
			break;
	}
	//LCD_drawLine(0, 47, 128, 47, 1);
	//sys.DispOLED = ON;
}

#define MAX_DUST_DGT	5
void OLEDDispFineDust(byte x, byte y, byte fntsiz, word num, byte kind)
{
    //byte i;
	byte DgtBuf[MAX_TEMP_DGT+1];

	if(kind == DUST_PM1_0) {
		ssd1306_0807_string(x, y, (byte *)"PM1.0", F_8X8_GHOTIC);
		sprintf((char *)DgtBuf, "%3d\0", num);
	} else if(kind == DUST_PM2_5) {
		ssd1306_0807_string(x, y, (byte *)"PM2.5", F_8X8_GHOTIC);
		sprintf((char *)DgtBuf, "%3d\0", num);
	} else if(kind == DUST_PM10) {
		ssd1306_0807_string(x, y, (byte *)"PM10 ", F_8X8_GHOTIC);
		sprintf((char *)DgtBuf, "%3d\0", num);
	}
	
	y+=8;
	if(kind != DUST_PM10) x+=10;
		else			  x+=6;
		
	switch(fntsiz) {
	case FONT_1608_0:
		DispOLEDString( x, y, DgtBuf);
		break;
		
	case FONT_0808_G:
		ssd1306_0807_string(x, y, DgtBuf, F_8X8_GHOTIC);
		break;
		
	case FONT_0808_M:
		ssd1306_0807_string(x, y, DgtBuf, F_8X8_MYUNGJO);
		break;
	}
}

void OLEDDispIcon(byte x, byte y, byte num)
{
	byte DgtBuf[2] = {0, 0};

	switch(num) {
	case 0:
		DgtBuf[0] = '-';
		break;
	case 1:
		DgtBuf[0] = '\\';
		break;
	case 2:
		DgtBuf[0] = '|';
		break;
	case 3:
		DgtBuf[0] = '/';
		break;
	}
	DgtBuf[1] = 0;
	ssd1306_0807_string(x, y, DgtBuf, F_8X8_GHOTIC);
		
#if 0
	switch(fntSize) {
	case FONT_1608_0:
		DispOLEDString( x, y, DgtBuf);
		break;
		
	case FONT_1612_1:
		ssd1306_draw_1612char(x, y, DgtBuf[0]);
		break;
		
	case FONT_3216_2:
		ssd1306_draw_3216char(x, y, DgtBuf[0]);
		break;
		
	case FONT_0808_G:
		ssd1306_0807_string(x, y, DgtBuf, F_8X8_GHOTIC);
		break;
		
	case FONT_0808_M:
		ssd1306_0807_string(x, y, DgtBuf, F_8X8_MYUNGJO);
		break;
	}
#endif
}

void OLEDCalendar(RTC_Time_t *t)
{
	byte DateMSG[16] = {0};
	
	//if(sys.fWiFiConnect == OK) {
	if(sys.WiFiState & WIFI_CONNECT) {
		sprintf((char *)DateMSG, "%04d\0", t->year);
		ssd1306_0807_string( 0, 32, DateMSG, F_8X8_GHOTIC);

		sprintf((char *)DateMSG, "%02d%02d", t->month, t->day);
		ssd1306_0807_string( 0, 40, DateMSG, F_8X8_GHOTIC);

		sprintf((char *)DateMSG, "%s", (u8 *)DayofWeekKorean[t->wday]);
		DispOLEDString(32,32,(byte *)DateMSG);
	}
}




