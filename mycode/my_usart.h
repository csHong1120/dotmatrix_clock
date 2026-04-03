/*****************************************************
Project : 
Company : Intellian Technologies
Comments:

Chip type               : STM32F429ZIT6
Program type            : Application
Core Clock frequency    : 8 MHz

File    : di_usart.h
Version :
Date    :
Author  :

*****************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DI_USART_H__
#define __DI_USART_H__


/* Includes ------------------------------------------------------------------*/
	#include "main.h"


/* Defines -------------------------------------------------------------------*/
	#define LOG_MESSAGE_ENABLE//add cshong
	
	#define USE_IDLE_UART1 	1
	#define USE_IDLE_UART2 	1
	#define USE_IDLE_UART3 	1

	#define USE_DMA_UART1
	#define USE_DMA_UART2
	#define USE_DMA_UART3

	#if defined (USE_DMA_UART1)
      #pragma message("          =====================================")
      #pragma message("          =====      UART1 DMA Mode       =====")
      #pragma message("          =====================================")
	#endif

	#if defined (USE_DMA_UART2)
      #pragma message("          =====================================")
      #pragma message("          =====      UART2 DMA Mode       =====")
      #pragma message("          =====================================")
	#endif

	#if defined (USE_DMA_UART3)
      #pragma message("          =====================================")
      #pragma message("          =====      UART3 DMA Mode       =====")
      #pragma message("          =====================================")
	#endif

	#define TRACE_ERROR(...)      //printf(__VA_ARGS__)

	#if defined(DEBUG_MODE)
		#define TRACE_DEBUG(...)      //printf(__VA_ARGS__)
	#else
		#define TRACE_DEBUG(...)
	#endif

	#define RX_DMA_BUFFERSIZE   1024   //256*4
	#define TX_DMA_BUFFERSIZE   1024   //256*4

	#define UART_RING_BUFSIZE   1024   //256*8

	// [2023-02-28] cs.Hong
	typedef enum _UART_DEVICE_ID_
	{
		//UART_DEVICE_0 = 0x00,
	#if USE_IDLE_UART1
		UART_DEVICE_1 = 0x00,
	#endif
	#if USE_IDLE_UART2
		UART_DEVICE_2,
	#endif
	#if USE_IDLE_UART3
		UART_DEVICE_3,
	#endif
		UART_DEVICE_MAX
	} UART_DEVICE_ID;

	enum
	{
		PORT_UART1 = 0,
		PORT_UART2,
		PORT_UART3,
		NUM_UIF_PORT
	};


	typedef enum _UART_RETURN_TYPE_
	{
		UART_ERROR_OK      = 0,
		UART_ERROR_NOK,
		UART_ERROR_NO_RESOURCE,
		UART_ERROR_RESOURSE_BUSY,
		UART_ERROR_TRANSMIT,
		UART_ERROR_MAX
	} UART_RETURN_TYPE;

	typedef struct _UART_RING_BUFFER_T_
	{
		uint32_t tx_head;
		uint32_t tx_tail;
		uint32_t rx_head;
		uint32_t rx_tail;
		__IO uint8_t  *tx;
		__IO uint8_t  *rx;
		uint32_t rb_size;
	} UART_RING_BUFFER_T;


	typedef struct _UART_RX_COUNT_
	{
		uint16_t _prevRxXferCount;
		uint16_t _lastRxXferCount;
	} UART_RX_COUNT;

	typedef struct _DI_USART_ChannelTypeDef_
	{
		UART_DEVICE_ID uartID;
		UART_HandleTypeDef *uartHandle;

		UART_RING_BUFFER_T UART_rb;

		__IO FlagStatus UartReady;

		__IO uint8_t  *dma_tx_buffer;
		__IO uint8_t  *dma_rx_buffer;
		uint16_t dma_rx_size;
		uint16_t dma_tx_size;

		bool use_uart_flag;
		bool use_dma_flag;

		bool use_bypass_flag;

		//UART_DEVICE_ID bypass_uartID;
		UART_RX_COUNT rx_count;
	} DI_USART_ChannelTypeDef;



///////////////////////////////////////////////////////////////////////////////
// type definition enum
//

	enum
	{
		UIF_OP_RESTART = 0,
		UIF_OP_SETUP,
		UIF_SEND_MESSAGE_LOG,
		UIF_GYRO_SEND,
		NUM_UIF_COMMAND
	};
	
	
/* Variables -----------------------------------------------------------------*/
	extern DI_USART_ChannelTypeDef UARTChannel[];
	extern uint8_t rx_ring_buffer[UART_DEVICE_MAX][UART_RING_BUFSIZE];
	extern uint8_t tx_ring_buffer[UART_DEVICE_MAX][UART_RING_BUFSIZE];

	//extern UART_HandleTypeDef huart1;
	//extern UART_HandleTypeDef huart6;
	//extern UART_HandleTypeDef huart4;
	extern UART_HandleTypeDef huart3;
	extern UART_HandleTypeDef huart2;

/* Prototypes ----------------------------------------------------------------*/
	void HAL_UART_RxIdleCallback(UART_HandleTypeDef *huart);
	void Work_SendMessageLog(uint8_t *format, ...);
	void Work_PortMsgLog(int16_t port, uint8_t *format, ...);

	void USART_Reset(UART_HandleTypeDef *huart);
	void DI_UART_DeInit(UART_DEVICE_ID uartID);
	void DI_UART_DMAReset(UART_DEVICE_ID uartID);
	void DI_UART_PreInit(UART_DEVICE_ID uartID);
	void DI_UART_Init(UART_DEVICE_ID uartID);
	void DI_UART_RxProcess(UART_DEVICE_ID uartID);
	void DI_UART_TxProcess(UART_DEVICE_ID uartID);
	void DI_UART_RB_BufferCountInit(UART_RING_BUFFER_T *ringBuffer);
	uint32_t DI_UART_RB_GetRxReceviedBufferCount(UART_RING_BUFFER_T *ringBuffer);
	uint32_t DI_UART_RB_GetTxRemainBufferCount(UART_RING_BUFFER_T *ringBuffer);
	UART_RETURN_TYPE DI_UART_TxBufferedMessage(UART_DEVICE_ID uartID, uint8_t *message, uint16_t size);
	void DI_UART_ChangeBaudRate(UART_DEVICE_ID uartID, uint32_t baudRate);
	UART_RETURN_TYPE DI_UART_GetTxStatus(UART_DEVICE_ID uartID);
	UART_RETURN_TYPE DI_UART_TxMessage(UART_DEVICE_ID uartID, uint8_t *message, uint16_t size);
	#if 0
		uint8_t DI_USART_GetByte(UART_DEVICE_ID uartID);
	#else
		uint8_t DI_USART_GetByte(UART_DEVICE_ID uartID, uint8_t *p);
	#endif
	int16_t DI_USART_GetString(UART_DEVICE_ID uartID, int8_t *out_string);
	void DI_USART_SendByte(UART_DEVICE_ID uartID, uint8_t c);
	void DI_USART_SendString(UART_DEVICE_ID uartID, int8_t* in_string, uint16_t length);
	void DI_USART_PutByte(UART_DEVICE_ID uartID, int8_t c);

	UART_DEVICE_ID PortToDeviceID(uint16_t port);
	void InitUart(void);
	void Work_Serial(void);
#endif  /* __DI_USART_H__ */
