/******************** (C) COPYRIGHT 2008 Motex Co., LTD ********************
* File Name			: app_usart.h
* Author			: HCS
* Date First Issued	: 09/09/2008
* Description		: Functions for only App. test
********************************************************************************
* History:
* 09/09/2008: V0.0 */
/* Define to prevent recursive inclusion -------------------------------------*/
	#ifndef __APP_USART_H
	#define __APP_USART_H
	
	//#include "stm32f10x.h"
	//#include "stdio.h"
	#include "main.h"


/* Includes ------------------------------------------------------------------*/
/* Exported defines --------------------------------------------------------*/
	#define ApplicationAddress 0x08004000
	
	#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
	#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
	#define IS_09(c)  ((c >= '0') && (c <= '9'))
	#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
	#define ISVALIDDEC(c)  IS_09(c)
	#define CONVERTDEC(c)  (c - '0')
	#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
	#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

/* Exported defines --------------------------------------------------------*/
	#define PACKET_SEQNO_INDEX		(1)
	#define PACKET_SEQNO_COMP_INDEX	(2)
	
	#define PACKET_HEADER			(3)
	#define PACKET_TRAILER			(2)
	#define PACKET_OVERHEAD			(PACKET_HEADER + PACKET_TRAILER)
	#define PACKET_SIZE		        (128)
	#define PACKET_1K_SIZE			(1024)
	
	#define FILE_NAME_LENGTH		(256)
	#define FILE_SIZE_LENGTH	    (16)

	#define		SOH					0x01
	#define		STX					0x02
	#define		ETX					0x03
	#define		EOT					0x04
	#define		ENQ					0x05
	#define		ACK					0x06
	#define		BEL					0x07
	#define		BS					0x08
	#define		TAB					0x09
	#define		LF					0x0A
	#define		VT					0x0B
	#define		FF					0x0C
	#define		CR					0x0D
	#define		SO					0x0E
	#define		SI					0x0F
	#define		DLE					0x10
	#define		DC1					0x11
	#define		DC2					0x12
	#define		DC3					0x13
	#define		DC4					0x14
	#define		NAK					0x15
	#define		SYN					0x16
	#define		ETB					0x17
	//#define	CAN					0x18
	#define		EM					0x19
	#define		SUB					0x1A
	#define		ESC					0x1B
	#define		FS					0x1C
	#define		GS					0x1D
	#define		RS					0x1E
	#define		US					0x1F
	#define		MD_CHG				5//U05

	#define	MAX_RX_BUF				512
	#define	MAX_TX_BUF				128

	#define SERIAL_START			1
	#define SERIAL_END				0

	#define WAIT_30S				60
	#define WAIT_60S				(WAIT_30S*2)
	#define WAIT_90S				(WAIT_30S*3)
	#define WAIT_120S				(WAIT_30S*4)
	#define WAIT_150S				(WAIT_30S*5)
	#define WAIT_180S				(WAIT_30S*6)
	#define WAIT_210S				(WAIT_30S*7)

	#define WAIT_1MIN				WAIT_60S
	#define WAIT_2MIN				(WAIT_1MIN*2)
	#define WAIT_3MIN				(WAIT_1MIN*3)
	#define WAIT_4MIN				(WAIT_1MIN*4)
	#define WAIT_5MIN				(WAIT_1MIN*5)//60*2*5
	#define WAIT_6MIN				(WAIT_1MIN*6)
	#define WAIT_10MIN				(WAIT_1MIN*10)
	#define WAIT_11MIN				(WAIT_1MIN*11)
#if 1
	#define WIFISEQ_CONNECT			0
	#define WIFISEQ_SOFTAPIP		1
	#define WIFISEQ_LOCALIP			2
	#define WIFISEQ_WEATHER			3
	#define WIFISEQ_NTP				4
	#define WIFISEQ_DUST			5
	#define WIFISEQ_NOACTION		6

	#define WIFISEQ_TIMEDELAY		19
	#define WIFISEQ_MSGWAIT			20
	#define WIFISEQ_APMODEWAIT		99
	#define WIFISEQ_ERROR			100
#else
	#define WIFISEQ_CWMODE_ST		0//Station Mode
	#define WIFISEQ_CWJAP			1

	#define WIFISEQ_CIPMUX			2
	#define WIFISEQ_CWSAP			3
	#define WIFISEQ_CIPSERVER		4
	
	#define WIFISEQ_CIPSTART		5
	#define WIFISEQ_CIPSEND			6
	#define WIFISEQ_GETQUERY		7
	#define WIFISEQ_CIPSNTPCFG		8

	#define WIFISEQ_MSGWAIT			11
	#define WIFISEQ_CIPSNTPTIME		21
	#define WIFISEQ_TIMEDELAY		22
	#define WIFISEQ_ERROR			100
#endif

	#define CH1						0//SERIAL_CH1
	#define CH2						1//SERIAL_CH2
	#define CH3						2//SERIAL_CH3

	struct _Serial {
		u8	FlagMessage;
		u8  RcvOK;
	};
	extern struct _Serial Serial[3];
	
/* Exported types ------------------------------------------------------------*/
	//typedef  void (*pFunction)(void);

/* Exported functions ------------------------------------------------------- */
	void SendByte(u8 ch, u8 dat);
	void SendString(byte ch, byte *dat);
	
	void InitSerial(void);
	void PutString(u8 chan, u8 *s);
	byte ReceiveData(byte *bin, word size, byte chan);
	
	void InitWiFi(void);
	void WifiProcess(byte sq);
	
	void SerialDust(void);
	void SerialWifi(void);
	

	

#endif		/* __APP_USART_H */



/******************* (C) COPYRIGHT 2008 Motex CO., LTD *****END OF FILE****/


