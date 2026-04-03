/******************** (C) COPYRIGHT 2008 Motex CO., LTD ********************
* File Name	: app_usart.c
* Author		: Hong Chong Sik
* Date First Issued	: 09/09/2008
* Description	: This file provides Test functions.
********************************************************************************
* History:
* 2026/04/03 : V1.8 */

/* Includes ------------------------------------------------------------------*/
  //#include "define.h"
  //#include "hw_config.h"
  #include "main.h"
  #include "rtc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

	struct _Serial Serial[3];
	static byte const DayofWeek[7][4]  = {"Sun\0","Mon\0","Tue\0","Wed\0","Thu\0","Fri\0","Sat\0"};
	static byte const MonthComp[12][3] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

/* Private functions ---------------------------------------------------------*/

void PutString(u8 chan, u8 *s)
{
	while(*s != '\0'){
		SendByte(chan, *s++);
	}
}

void SendString(byte ch, byte *dat)
{
	
  DI_USART_SendByte(PortToDeviceID(ch), STX);
  DI_USART_SendString(PortToDeviceID(ch), (s8 *)dat, 3);
  DI_USART_SendByte(PortToDeviceID(ch), ETX);
}

u32 uatoi(byte *sor, byte siz)
{
	u32 rval = 0;
	byte dat;
	
	while(siz--) {
		dat = *sor;
		if( dat >= '0' && dat <= '9' ) {
			rval *= 10;
			rval += (*sor-'0');
			sor++;
		} else {
			return rval;
		}
	}
	return rval;
}

byte uitoa(u32 num, byte *buf, byte siz)
{
	u32 rval, wDivide;
	byte i;
	
	rval = num;
	for(i=0;i<siz;i++) {
		if(!i) wDivide = 1;
		else wDivide *= 10;
	}
	
	for(i=0;i<siz;i++) {
		*(buf+i) = rval / wDivide + '0';
		rval %= wDivide;
		wDivide /= 10;
	}
	return siz;
}

void CmdProcess(byte *p)
{
    word wLength, wCheckSum=0, i;

    wLength = (*(p+2)<<8 | *(p+3)) ;//data+checksum(28)
    for(i=0;i<(wLength+2);i++) {    //+2는 start charater 2byte
        wCheckSum += *(p+i);
    }
    wLength = (*(p+30)<<8 | *(p+31));
    
    if(wCheckSum == wLength) {//checksum 비교
        for(i=0;i<12;i++) {
            dust.pms7003[i] = (*(p+4+i*2)<<8 | *(p+5+i*2));
        }
    }
}

void InitWiFi(void)
{
	// Wifi reset은 반드시 InitUart() 함수 전에 실행한다.
	// 그렇지 않으면 리셋이 안됨.  20230812
  //WIFI_RST_L();//Wi-Fi module Active
	SendString(CH2, (byte *)"S99");//ESP8266 Reset

	WIFI_RST_H();//Wi-Fi module Active
	HAL_Delay(1000);
  
	sys.WiFiSeq = WIFISEQ_CONNECT;
	sys.WiFiMSGretry = 0;

	sys.WiFiState  = WIFI_TIMESYNC;
	ssd1306_clear_screen(0xff, 0xff);
}

#define X_DOT_POSI				88
void WifiProcess(byte sq)
{
	switch(sq) {
	case WIFISEQ_CONNECT://0
		sys.WiFiSeqBackup	= sys.WiFiSeq;
		sys.WiFiSeq 		= WIFISEQ_MSGWAIT;//응답대기
		Serial[CH2].RcvOK 	= NG;
		//ClearRxBuf(&Serial[CH2]);
		Serial[CH2].FlagMessage = SERIAL_END;
		SendString(CH2, (byte *)"S01");
		break;

	case WIFISEQ_SOFTAPIP://1
		sys.WiFiSeqBackup	= sys.WiFiSeq;
		sys.WiFiSeq 		= WIFISEQ_MSGWAIT;//응답대기
		Serial[CH2].RcvOK 	= NG;
		Delay(DELAY100MS);
		SendString(CH2, (byte *)"D03");//Soft AP IP불러오기 명령
		break;
		
	case WIFISEQ_LOCALIP://2
		sys.WiFiSeqBackup	= sys.WiFiSeq;
		sys.WiFiSeq 		= WIFISEQ_MSGWAIT;//응답대기
		Serial[CH2].RcvOK 	= NG;
		Delay(DELAY100MS);
		SendString(CH2, (byte *)"D04");//local IP불러오기 명령
		break;
		
	case WIFISEQ_WEATHER://3
		sys.WiFiSeqBackup	= sys.WiFiSeq;
		sys.WiFiSeq 		= WIFISEQ_MSGWAIT;//응답대기
		Serial[CH2].RcvOK 	= NG;
		// SendString(CH2, (byte *)"D00"); --> 명령에 응답이 없음	// [2025-04-06] CS.Hong
		SendString(CH2, (byte *)"D03");//	--> 임시로 명령을 바꾸어서 시간 설정은 가능하도록 변경	// [2025-04-06] CS.Hong
		break;
	
	case WIFISEQ_NTP://4
		sys.WiFiSeqBackup	= sys.WiFiSeq;
		sys.WiFiSeq 		= WIFISEQ_MSGWAIT;//응답대기
		Serial[CH2].RcvOK 	= NG;
		SendString(CH2, (byte *)"D01");
		break;
		
	case WIFISEQ_DUST://5
		sys.WiFiSeqBackup	= sys.WiFiSeq;
		sys.WiFiSeq 		= WIFISEQ_MSGWAIT;//응답대기
		Serial[CH2].RcvOK 	= NG;
		ssd1306_0807_string(0, 16, (byte *)"                  ", F_8X8_GHOTIC);//DUST에러 발생 후 리셋된 경우 여기에 먼저 들어옴.  메세지 삭제
		SendString(CH2, (byte *)"D02");
		break;
		
	case WIFISEQ_NOACTION://6
        if(sys.WiFiError) sys.WiFiError = 0;
		break;
		
	case WIFISEQ_TIMEDELAY://19
		if(GetTimer(TMR_ID_WIFISET) == 0) {
			sys.WiFiSeq = sys.WiFiSeqBackup + 1;
		}
		break;

	case WIFISEQ_MSGWAIT://20
		if(GetTimer(TMR_ID_WIFISET) == 0) {
			SetTimer(TMR_ID_WIFISET, DELAY500MS);
			if(++sys.WiFiWaitCnt > 6) {//3초
				sys.WiFiWaitCnt = 0;
				sys.WiFiSeq = sys.WiFiSeqBackup;
				if(++sys.WiFiMSGretry >= 3) {
					sys.WiFiState 	&= ~WIFI_CONNECT;
					sys.WiFiSeq 	 = WIFISEQ_ERROR;//ERROR or Not response
                    sys.WiFiError    = sys.WiFiSeqBackup;
					sys.WiFiErrorCnt = 0;
					if(sys.WiFiError == WIFISEQ_DUST) {
						dust.Update = ON;
						dust.PM2_5Val = DUST_VALUE_UNKNOWN;
						dust.PM10Val  = DUST_VALUE_UNKNOWN;
					}
				}
			}
		}

		if(Serial[CH2].RcvOK == OK) {
			sys.WiFiWaitCnt  = 0;
			sys.WiFiMSGretry = 0;
			SetTimer(TMR_ID_WIFISET, DELAY300MS);
			sys.WiFiSeq = WIFISEQ_TIMEDELAY;//sys.WiFiSeqBackup + 1;
		}
		break;

	case WIFISEQ_APMODEWAIT://99
		if(GetTimer(TMR_ID_WIFISET) == 0) {
			SetTimer(TMR_ID_WIFISET, DELAY500MS);
			sys.WiFiSeqBackup = WIFISEQ_APMODEWAIT;
			sys.WiFiSeq       = WIFISEQ_APMODEWAIT;
			if(sys.WiFiWaitCnt) {
				sys.WiFiWaitCnt = 0;
				if(sys.WiFiState & WIFI_PASSWORD_OK) {
					ssd1306_0807_string(0, 16, (byte *)".                 \0", F_8X8_GHOTIC);
				} else {
					ssd1306_0807_string(0, 16, (byte *)"Set Password..    \0", F_8X8_GHOTIC);
				}
			} else {
				sys.WiFiWaitCnt = 1;
				ssd1306_0807_string(0, 16, (byte *)"                  \0", F_8X8_GHOTIC);
			}
			
			//모듈을 리셋하는 루틴 추가함.
			// 3분, 정전 또는 AP전원을 껐다가 켠경우를 위해 리셋루틴 추가
			// SSID및 password를 처음 입력하는 경우 3분 이내에 해야하는 단점이 있음..
			if(++sys.WiFiErrorCnt == WAIT_3MIN) {
				WIFI_RST_L();//Wi-Fi module Reset
				SendString(CH2, (byte *)"S99");//하드웨어 리셋이 없을경우..
			} else if(sys.WiFiErrorCnt == (WAIT_3MIN+2)) {
				WIFI_RST_H();//Wi-Fi module Active
			} else if(sys.WiFiErrorCnt > WAIT_4MIN) {//180초, 3분(0.5s*360)
				sys.WiFiErrorCnt = 0;
                sys.WiFiSeq = WIFISEQ_CONNECT;
			}
		}
		break;
		
	case WIFISEQ_ERROR://100
		if(GetTimer(TMR_ID_WIFISET) == 0) {
			SetTimer(TMR_ID_WIFISET, DELAY500MS);
			if(sys.WiFiWaitCnt) {
				sys.WiFiWaitCnt = 0;
				if(sys.WiFiSeqBackup == WIFISEQ_DUST) {
					ssd1306_0807_string(0, 16, (byte *)">DUST Not Response", F_8X8_GHOTIC);
				} else {
					ssd1306_0807_string(0, 16, (byte *)">WiFi Not Response", F_8X8_GHOTIC);
				}
			} else {
				sys.WiFiWaitCnt = 1;
				ssd1306_0807_string(0, 16, (byte *)"                  ", F_8X8_GHOTIC);
			}
			
			//Error가 발생한건, 모듈의 응답이 없다는 결과이므로...
			//모듈을 리셋하는 루틴 추가함.
#if defined(WEATHER_BUG_FIXED)
			if(sys.WiFiSeqBackup >= WIFISEQ_WEATHER && sys.WiFiSeqBackup <= WIFISEQ_DUST) {
				if(++sys.WiFiErrorCnt == WAIT_1MIN) {
					// WIFI_RST_L();//Wi-Fi module Reset
					SendString(CH2, (byte *)"S99");//하드웨어 리셋이 없을경우..
				} else if(sys.WiFiErrorCnt == (WAIT_1MIN + (WAIT_30S/10))) { // + 3초 후
					// WIFI_RST_H();//Wi-Fi module Active
					memset(rx_ring_buffer[CH2], 0, sizeof(rx_ring_buffer[CH2]));
					DI_UART_PreInit(UART_DEVICE_2);
					DI_UART_Init(UART_DEVICE_2);
					DI_UART_DMAReset(UART_DEVICE_2);
				} else if(sys.WiFiErrorCnt > (WAIT_1MIN + WAIT_30S)) {  // +  reset 30초 후
					ssd1306_0807_string(0, 16, (byte *)"                  ", F_8X8_GHOTIC);
					sys.WiFiErrorCnt = 0;
					if(sys.WiFiSeqBackup != WIFISEQ_DUST) {
						sys.WiFiSeq = WIFISEQ_CONNECT;
						SetBuzzer(BUZZ_MIS);
					} else {
						sys.WiFiSeq = WIFISEQ_DUST;
					}
				}
			} else {
				//여기 루틴에 들어오면, 접속이 안될 경우 이다.(CONNECT에러가 발생한 경우)
				//접속에러가 발생하면, 시간 업데이트를 자동으로 못하게 되어 있음(2019.02.27)
				if(++sys.WiFiErrorCnt == WAIT_3MIN) {
					// WIFI_RST_L();//Wi-Fi module Reset
					SendString(CH2, (byte *)"S99");//하드웨어 리셋이 없을경우..
				} else if(sys.WiFiErrorCnt == (WAIT_3MIN + (WAIT_30S/10))) { // + 3초 후
					// WIFI_RST_H();//Wi-Fi module Active
					memset(rx_ring_buffer[CH2], 0, sizeof(rx_ring_buffer[CH2]));
					DI_UART_PreInit(UART_DEVICE_2);
					DI_UART_Init(UART_DEVICE_2);
					DI_UART_DMAReset(UART_DEVICE_2);
				} else if(sys.WiFiErrorCnt > (WAIT_3MIN + WAIT_30S)) {  // +  reset 30초 후
					ssd1306_0807_string(0, 16, (byte *)"                  ", F_8X8_GHOTIC);
					sys.WiFiErrorCnt = 0;
					sys.WiFiSeq = WIFISEQ_CONNECT;
					SetBuzzer(BUZZ_MIS);
				}
			}
#else
			if(sys.WiFiSeqBackup >= WIFISEQ_WEATHER && sys.WiFiSeqBackup <= WIFISEQ_DUST) {
				if(++sys.WiFiErrorCnt == WAIT_10MIN) {
					WIFI_RST_L();//Wi-Fi module Reset
					SendString(CH2, (byte *)"S99");//하드웨어 리셋이 없을경우..
				} else if(sys.WiFiErrorCnt == (WAIT_10MIN + 2)) {
					WIFI_RST_H();//Wi-Fi module Active
				} else if(sys.WiFiErrorCnt > WAIT_11MIN) {
					ssd1306_0807_string(0, 16, (byte *)"                  ", F_8X8_GHOTIC);
					sys.WiFiErrorCnt = 0;
					sys.WiFiSeq = sys.WiFiSeqBackup;
				}
			} else {
				//여기 루틴에 들어오면, 접속이 안될 경우 이다.(CONNECT에러가 발생한 경우)
				//접속에러가 발생하면, 시간 업데이트를 자동으로 못하게 되어 있음(2019.02.27)
				if(++sys.WiFiErrorCnt == WAIT_1MIN) {
					WIFI_RST_L();//Wi-Fi module Reset
					SendString(CH2, (byte *)"S99");//하드웨어 리셋이 없을경우..
				} else if(sys.WiFiErrorCnt == (WAIT_1MIN + 2)) {
					WIFI_RST_H();//Wi-Fi module Active
				} else if(sys.WiFiErrorCnt > WAIT_2MIN) {
					ssd1306_0807_string(0, 16, (byte *)"                  ", F_8X8_GHOTIC);
					sys.WiFiErrorCnt = 0;
					sys.WiFiSeq = WIFISEQ_CONNECT;
				}
			}
#endif
		}
		break;
	}
}

byte GetWeatherNumber(byte *src, byte *tar)
{
	byte cnt=0;

	while(*src != ',') {
		tar[cnt++] = *src++;
		if(cnt>= 4) 	break;
		if(*src == ETX) break;
	}
	return cnt;
}

void DustParse(byte *p)
{
	byte *str;
	byte isFirst=0, comma=0;
	byte MSGBuf[5] = {0};
	
	str = p;
	str += 5;
	dust.year  = uatoi(str, 4); str += 5;
	dust.month = uatoi(str, 2); str += 3;
	dust.day   = uatoi(str, 2); str += 3;
	dust.hour  = uatoi(str, 2); str += 3;
	dust.min   = uatoi(str, 2);// str += 3;
    
    if(dust.hour == 24) dust.hour = 0;//이 값은 0이 없음 0 = 24
	while(*str != ETX) {
		if(*str++ == ',') {
			comma++;
			isFirst = 0;
		}
	
		switch(comma) {
		case 1:
			if(isFirst == 0) {
				isFirst = 1;
				if(*str != '-') {
					memset(MSGBuf, 0, sizeof(MSGBuf));
					dust.PM10Val = uatoi(MSGBuf, GetWeatherNumber(str, MSGBuf));
				} else {
					dust.PM10Val = DUST_VALUE_UNKNOWN;
				}
			}
			break;
			
		case 2:
			if(isFirst == 0) {
				isFirst = 1;
				if(*str != '-') {
					memset(MSGBuf, 0, sizeof(MSGBuf));
					dust.PM2_5Val = uatoi(MSGBuf, GetWeatherNumber(str, MSGBuf));
				} else {
					dust.PM2_5Val = DUST_VALUE_UNKNOWN;
				}
			}
			break;
			
		case 3:
			if(isFirst == 0) {
				isFirst = 1;
				if(*str >= '0' && *str <= '9') {
					dust.PM10grade = *str - '0';
				} else {
					dust.PM10grade = DUST_GRADE_UNKNOWN;
				}
			}
			break;

		case 4:
			if(isFirst == 0) {
				isFirst = 1;
				if(*str >= '0' && *str <= '9') {
					dust.PM2_5grade = *str - '0';
				} else {
					dust.PM2_5grade = DUST_GRADE_UNKNOWN;
				}
			}
			break;
			
		case 5://지역코드, 0이면 작전동, 1이면 공릉동
			if(isFirst == 0) {
				isFirst = 1;
				dust.AreaCode = *str - '0';
			}
		}
	}
	dust.Update = ON;
}

void TimeParse(byte *p)
{
	byte i, TimeMSG[9]={0};
	byte *str;

	if(sys.WiFiState & WIFI_TIMESYNC) {
		str = p;
		memcpy(&TimeMSG[0], str+16, 8);

		str += 5;
		for(i=0;i<7;i++) {
			if(strncmp((const char *)str, (char *)&DayofWeek[i], 3) == 0) {
				sys.time.wday = i;  // 0..6, sunday = 0
				break;
			}
		}

		str += 4;
		for(i=0;i<12;i++) {
			if(strncmp((const char *)str, (char *)&MonthComp[i], 3) == 0) {
				sys.time.month = i+1;//1월이 0, +1
				break;
			}
		}
        
		str += 4;
        if(*str == ' ') {
            sys.time.day  = *(str+1) - '0';//한자리수 날짜는 앞에 공백이 날아옴..
        } else {
            sys.time.day  = uatoi(str, 2);
        }
		
		str += 12;
		sys.time.year = uatoi(str, 4);
		
		if(sys.time.year >= 2010) {
			sys.time.hour	= uatoi(&TimeMSG[0], 2);
			sys.time.min	= uatoi(&TimeMSG[3], 2);
			sys.time.sec	= uatoi(&TimeMSG[6], 2);
		
			sys.SettingFlag = ON;

		#if defined(WEATHER_BUG_FIXED)
			weather.year  = sys.time.year;
			weather.month = sys.time.month;
			weather.day   = sys.time.day;
			weather.hour  = sys.time.hour;

			weather.ForecastTime = sys.time.hour;
		#endif

	#if USE_RTC
			rtc_settime(&sys.time, 3);	//위에서 시간값이 정해지면...  그 값에 3초를 더함..
	#endif
			if(sys.time.hour >= 9 && sys.time.hour <= 23)// [2025-11-02] CS.Hong 아침9시 부터 23시까지..
				SetBuzzer(BUZZ_KEY2);		//단순하게 초에 3초를 더하면, 60초가 넘어가기도 하고, 분 시 일 월 년 까지 변경될 수 있음.
										//예) 2010년 12월 31일 23시 59분 58초에 +3을 하면, 2011년 1월 1일 0시 0분 1초로 세팅해야함.
	#if USE_RTC
			rtc_gettime(&sys.time);	  	//3초를 더한 후, sys.time값을 다시 읽어옴...
	#endif            
			sys.ClockAfterSet = 0;		// 시계를 세팅하고 얼마나 지났는지 확인함..
			sys.SecCompare	= 100;		//시간업데이트 후 값을 변경시킴, BiosRTC.c에서 1초씩 변경하기위한 변수
			//sys.DispSeq		= 0;		//디스플레이 업데이트 시퀀스를 0으로 만들어 OLED표현을 새로함..
			sys.SettingFlag = OFF;
			sys.WiFiState &= ~WIFI_TIMESYNC;    // 시간동기화 하지 않음, 이 함수의 조건문에 들어오지 못 함.
			sys.WiFiState |= WIFI_TIME_UPDATE;
			HAL_GPIO_WritePin(CMAIN_LED_GPIO_Port, CMAIN_LED_Pin, GPIO_PIN_RESET);
		}
	}
}

void WeatherParse(byte *p)
{
	//byte cnt=0;
	byte MSGBuf[10] = {0};
	byte *str;
	byte isFirst=0, comma=0;

	str = p;
	while(*str != ETX) {
		if(*str++ == ',') {
			comma++;
			isFirst = 0;
		}
	
		switch(comma) {
		case 1:
			if(isFirst == 0) {
				isFirst = 1;
				weather.year  = uatoi(str, 4); str += 4;
				weather.month = uatoi(str, 2); str += 2;
				weather.day   = uatoi(str, 2); str += 2;
				weather.hour  = uatoi(str, 2);
				
				if(weather.month == 2) {//윤달계산..
					if( (weather.year%4==0 && weather.year%100!=0) || weather.year%400==0) {// 윤년 계산.
						weather.lastday = 29;
					} else {
						weather.lastday = 28;
					}
				} else {
					weather.lastday = MonthLastDay[weather.month];//한달이 며칠인지 계산함..
				}
			}
			break;

		case 2:
			if(isFirst == 0) {
				isFirst = 1;
				memset(MSGBuf, 0, sizeof(MSGBuf));
				weather.ForecastTime = uatoi(MSGBuf, GetWeatherNumber(str, MSGBuf));
                if(weather.ForecastTime == 24) weather.ForecastTime = 0;
			}
			break;
			
		case 3:
			if(isFirst == 0) {
				isFirst = 1;
				GetWeatherNumber(str, weather.Temperature);
			}
			break;
			
		case 4://하늘상태코드
			if(isFirst == 0) {
				isFirst = 1;
				weather.SkyCode = *str - '0';
			}
			break;

		case 5://강수상태코드
			if(isFirst == 0) {
				isFirst = 1;
				weather.RainyCode = *str - '0';
			}
			break;

		case 6://강수확률
			if(isFirst == 0) {
				isFirst = 1;
				memset(MSGBuf, 0, sizeof(MSGBuf));
				weather.RainyPercent = uatoi(MSGBuf, GetWeatherNumber(str, MSGBuf));
			}
			break;

		case 7://풍속
			if(isFirst == 0) {
				isFirst = 1;
				GetWeatherNumber(str, weather.WindSpd);
				weather.WindSpd[3] = 0x00;//소수점 2번째 자리 버림...
			}
			break;
		
		case 8://풍향
			if(isFirst == 0) {
				isFirst = 1;
				weather.WindDir = *str - '0';
			}
			break;
		
		case 9://습도
			if(isFirst == 0) {
				isFirst = 1;
				weather.Humidity = uatoi(MSGBuf, GetWeatherNumber(str, MSGBuf));
			}
			break;
			
		case 10://지역코드, 0이면 작전동, 1이면 공릉동
			if(isFirst == 0) {
				isFirst = 1;
				weather.AreaCode = *str - '0';
			}
			break;
		}
	}
	//weather.WeatherCode  //0:맑음,1:구름조금,2:구름많음,3:흐림,4:비,5:비/눈,6:눈/비,7:눈,8:err
	if(weather.RainyCode == 0) {
		if(weather.SkyCode == 1) 		weather.WeatherCode = 0;
		else if(weather.SkyCode == 2) 	weather.WeatherCode = 1;
		else if(weather.SkyCode == 3) 	weather.WeatherCode = 2;
		else if(weather.SkyCode == 4) 	weather.WeatherCode = 3;
		else 							weather.WeatherCode = 8;
	} else {
		if(weather.RainyCode == 1) 		weather.WeatherCode = 4;
		else if(weather.RainyCode == 2) weather.WeatherCode = 5;
		else if(weather.RainyCode == 3) weather.WeatherCode = 6;
		else if(weather.RainyCode == 4) weather.WeatherCode = 7;
		else							weather.WeatherCode = 8;
	}
	weather.Update = ON;
}

void SerialProcess(byte *p)
{
	byte i, CmdStr=0, CmdNum=0, CmdData=0;
	
	CmdStr = *(p+1);
	CmdNum = uatoi((p+2), 2);
	switch(CmdStr) {
	case 'S':
		Serial[CH2].RcvOK = OK;//일단, 코드라도 맞으면 OK
		switch(CmdNum) {
		case 0:
			CmdData = *(p+4) - '0';
			if(CmdData) {
				//SetBuzzer(BUZZ_OK);
/*				//20180827
				//Wi-Fi Error상태(예, DustErr)에서 리셋 시그널을 보낸후 모듈에서 날아오는 첫번째 메세지
				//이 루틴을 활성화하면... 리셋후 도트매트릭스 상에서 화면처리에 딜레이가 보임
				if(sys.WiFiSeq == WIFISEQ_ERROR) {
					sys.WiFiSeq = WIFISEQ_CONNECT;
					Delay(DELAY500MS);
				}*/
				if(sys.WiFiSeq == WIFISEQ_ERROR) {
					if(sys.WiFiSeqBackup == WIFISEQ_DUST) {
						sys.WiFiSeq = WIFISEQ_DUST;
					} else {
						sys.WiFiSeq = WIFISEQ_CONNECT;
					}
				}
			} else {
				//SetBuzzer(BUZZ_ERR);
			}
			break;
			
		case 1:
			CmdData = *(p+4) - '0';
			if(CmdData) {
				sys.WiFiState |=  (WIFI_CONNECT | WIFI_PASSWORD_OK);
			} else {
				sys.WiFiState &= ~(WIFI_CONNECT | WIFI_PASSWORD_OK);
				sys.WiFiSeq    =  WIFISEQ_APMODEWAIT;
			}
			break;
			
		case 51://알람 On/Off message
			CmdData = *(p+4) - '0';
			if(rom.Data[EEADDR_ALONOFF] != CmdData) {
				sys.AlOnOff = CmdData;
				rom.Data[EEADDR_ALONOFF] = CmdData;
				FlashWriteData();
				SetBuzzer(BUZZ_OK);
			} else {
				SetBuzzer(BUZZ_KEY2);
			}
			break;
			
		case 52://알람시간 설정 message
			CmdData = *(p+4) - '0';
			if(CmdData) {
				sys.AlHour	= uatoi((p+5), 2);
				sys.AlMin	= uatoi((p+7), 2);
				rom.Data[EEADDR_ALHOUR]     = sys.AlHour;
				rom.Data[EEADDR_ALMIN]      = sys.AlMin;
				FlashWriteData();
				SetBuzzer(BUZZ_OK);
			} else {
				SetBuzzer(BUZZ_ERR);
			}
			break;
			
		case 53://날씨정보, 미세먼지정보의 지역코드
			//이 값은 단순히 입력된 데이터가 시리얼로 넘어오는지만 판단함.
			//지역코드는 날씨정보나, 미세먼지 정보에서 확인
			CmdData = *(p+4);
			if(CmdData == 'N') {
				SetBuzzer(BUZZ_ERR);
			} else {
				SetBuzzer(BUZZ_OK);
				if(sys.WiFiSeq == WIFISEQ_NOACTION) {
					//지역 코드가 변경되어, 정보를 다시 불러옴.
					//정보를 다시 읽어 오면, 정보의 내용중에 지역코드가 포함됨
					//날씨, 먼지 정보 파싱에서 변수 업데이트함.
					sys.WiFiSeq = WIFISEQ_CONNECT;
				}
			}
			break;
			
		case 54://도트매트릭스 밝기선택
			CmdData = *(p+4);
			if(CmdData == 'N') {
				SetBuzzer(BUZZ_ERR);
			} else {
				sys.DotMatrixBright	= uatoi((p+4), 2);
				rom.Data[EEADDR_BRIGHT] = sys.DotMatrixBright;
				FlashWriteData();
				SetBuzzer(BUZZ_OK);
				
				sys.DispDot = OFF;
				Delay(DELAY30MS);
				MAX7219_SetReg(REG_INTENSITY,	sys.DotMatrixBright);//bright : MAX 0x0f
				Delay(DELAY30MS);
				sys.DispDot = ON;
			}
			break;

		case 55://TimeSignal On/Off message
			CmdData = *(p+4) - '0';
			if(rom.Data[EEADDR_TIMESIG] != CmdData) {
				sys.TimeSignal = CmdData;
				rom.Data[EEADDR_TIMESIG] = CmdData;
				FlashWriteData();
				SetBuzzer(BUZZ_OK);
			} else {
				SetBuzzer(BUZZ_KEY2);
			}
			break;
			
		case 98://Reset 명령
			SetBuzzer(BUZZ_OK);
			Delay(DELAY1000MS);
			NVIC_SystemReset();
		}
		break;
		
	case 'D':
		Serial[CH2].RcvOK = OK;//일단, 코드라도 맞으면 OK
		switch(CmdNum) {
		case 0:
			WeatherParse(p);
			break;
			
		case 1:
			TimeParse(p);
			break;

		case 2:
			DustParse(p);
			break;
			
		case 3:
			memset(sys.SoftApIP, 0, sizeof(sys.SoftApIP));
			for(i=0;i<sizeof(sys.SoftApIP);i++) {
				CmdData = *(p+5+i);
				if(CmdData == 0x03) break;
				sys.SoftApIP[i] = CmdData;
			}
			break;
			
		case 4:
			memset(sys.LocalIP, 0, sizeof(sys.LocalIP));
			for(i=0;i<sizeof(sys.LocalIP);i++) {
				CmdData = *(p+5+i);
				if(CmdData == 0x03) break;
				sys.LocalIP[i] = CmdData;
			}
			break;
		}
		break;
	}
	//ClearRxBuf(&Serial[CH2]);
	Serial[CH2].FlagMessage = SERIAL_END;
}

#define MAX_DATA	1024
void SerialWifi(void)
{
	u8 ch;
    static u8 bSeData[MAX_DATA];
	static u16 cnt=0;
	
	if(DI_USART_GetByte(UART_DEVICE_2, &ch) != FALSE)
	{
		if(cnt>=MAX_DATA) cnt = 0;//Overflow 주의
		switch(ch) {
		case STX:
			Serial[CH2].FlagMessage = SERIAL_START;
			cnt = 0;
			bSeData[cnt++] = ch;
			break;
			
		case ETX://
			if(Serial[CH2].FlagMessage == SERIAL_START) {
				SerialProcess(bSeData);
				memset(bSeData, 0, MAX_DATA);
				Serial[CH2].FlagMessage = SERIAL_END;
			}
			break;

		default:
			if(Serial[CH2].FlagMessage == SERIAL_START) {
				bSeData[cnt++] = ch;
			}
			break;
		}
	}
}

void SerialDust(void)
{
	u8 ch;
    static u8 bSeData[50], cnt;
    
	if(DI_USART_GetByte(UART_DEVICE_1, &ch) != FALSE)
	{
        if(cnt>=50) cnt = 0;//Overflow 주의

        switch(ch) {
        case 0x42://Start Character
            Serial[CH1].FlagMessage = SERIAL_START;
            cnt = 0;
            bSeData[cnt++] = ch;
            break;
            
        default:
            if(Serial[CH1].FlagMessage == SERIAL_START) {
                bSeData[cnt++] = ch;
            } else {
                cnt = 0;
            }
            break;
        }
        
        if(cnt == 32) {
            if(Serial[CH1].FlagMessage == SERIAL_START) {
                CmdProcess(bSeData);
            }
            Serial[CH1].FlagMessage = SERIAL_END;
            cnt = 0;
        }
	}
}
/******************* (C) COPYRIGHT 2008 *****END OF FILE****/



