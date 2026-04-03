/*****************************************************
Project : 
Company : Intellian Technologies
Comments:

Chip type               : STM32F407VGT
Program type            : Application
Core Clock frequency    : 25 MHz

File    : di_usart.c
Version :
Date    :
Author  :

*****************************************************/



/* Includes ------------------------------------------------------------------*/
	#include "main.h"
	#include "buffer.h"
	#include "usart.h"
	#include "my_usart.h"
	#include "my_type.h"
	#include <stdlib.h>
		

/* Defines -------------------------------------------------------------------*/
	#define UIF_MESSAGE_SIZE          1024
	#define UIF_BUFFER_SIZE_1         1024
	#define UIF_BUFFER_SIZE_2         512
	#define UIF_BUFFER_SIZE_3         512

	#define UIF_BUFFER_MAX            UIF_BUFFER_SIZE_1
	#define UIF_OVERFLOW_COUNT_MAX    25000
	#define MAX_DEBUG_MSG_NUM         16

	char UIF_INFO[NUM_UIF_COMMAND][4] =
	{
		{'O', 'R', 0, TRUE},		//	UIF_OP_RESTART
		{'O', 'S', 0, TRUE},		//	UIF_OP_SETUP
		{'M', 'L', 10, TRUE}		//	UIF_SEND_MESSAGE_LOG
	};

/* Variable -----------------------------------------------------------------*/

	TSystemValue sys;
	DI_USART_ChannelTypeDef UARTChannel[UART_DEVICE_MAX];

	//uint32_t tx_dma_size = TX_DMA_BUFFERSIZE;
	//uint32_t rx_dma_size = RX_DMA_BUFFERSIZE;
	//uint32_t rx_ring_buffer_size = UART_RING_BUFSIZE;
	//static UART_RX_COUNT rx_count[UART_DEVICE_MAX];
	//uint8_t uartEnableFlag = FALSE;

	DMA_BUF uint8_t rx_dma_buffer[UART_DEVICE_MAX][RX_DMA_BUFFERSIZE];
	DMA_BUF uint8_t tx_dma_buffer[UART_DEVICE_MAX][TX_DMA_BUFFERSIZE];

	//DMA_BUF uint8_t rx_ring_buffer[UART_DEVICE_MAX][UART_RING_BUFSIZE];
	//DMA_BUF uint8_t tx_ring_buffer[UART_DEVICE_MAX][UART_RING_BUFSIZE];
	uint8_t rx_ring_buffer[UART_DEVICE_MAX][UART_RING_BUFSIZE];
	uint8_t tx_ring_buffer[UART_DEVICE_MAX][UART_RING_BUFSIZE];

	int8_t UIF_Command_String[UIF_BUFFER_MAX];
	int8_t UIF_Send_String[UIF_BUFFER_MAX];
	int8_t UIF_Message[UIF_MESSAGE_SIZE];
  
/* Function -----------------------------------------------------------------*/

void UIF_SendString(int16_t port, int8_t* string)
{
	int16_t length, n = 0;

	UIF_Send_String[n++] = '{';
	length = strlen((char *)string);
	memcpy(UIF_Send_String+n, string, length);
	n += length;

	UIF_Send_String[n++] = '}';
	UIF_Send_String[n++] = '\r';
	UIF_Send_String[n++] = '\n';

	DI_USART_SendString(PortToDeviceID(port), UIF_Send_String, n);
}

	
/* function ---------------------------------------------------------------------------------*/
///////////////////////////////////////////////////////////////////////////////////////////////
//
// RX Error callback
// Rx Transfer Error callback
//
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	UART_DEVICE_ID uartID;

#if defined(USE_IDLE_UART1)
	if(huart->Instance == USART1)         // Main
	{
    uartID = UART_DEVICE_1;
	}
#endif
#if defined(USE_IDLE_UART2)
	if(huart->Instance == USART2)
	{
    uartID = UART_DEVICE_2;
	}	
#endif  
#if defined(USE_IDLE_UART3)
	if(huart->Instance == USART3)         // Main
	{
    uartID = UART_DEVICE_3;
	}
#endif
	else
	{
		return;
	}

	huart->RxState = HAL_UART_STATE_READY;

	HAL_UART_DMAStop(huart);
	__HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
	__HAL_UART_FLUSH_DRREGISTER(huart);
	__HAL_UART_CLEAR_PEFLAG(huart);

  DI_UART_PreInit(uartID);
  DI_UART_Init(uartID);
  
/*	//if(HAL_UART_Receive_DMA(huart, (uint8_t *)mem_rx_buffer[port_index], 1) != HAL_OK)
  if(HAL_UART_Receive_DMA(huart, (uint8_t*)(UARTChannel[port_index].dma_rx_buffer), UARTChannel[port_index].dma_rx_size) != HAL_OK)
	{
		Error_Handler();
	}*/
}


///////////////////////////////////////////////////////////////////////////////////////////////
//
// RX interrut callback
// Rx Transfer completed callback
// [2023-02-28] cs.Hong
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  if(huart->Instance == USART1)       // Main
  {
#if defined(USE_IDLE_UART1)
    DI_UART_RxProcess(UART_DEVICE_1);
#endif    
  }
  else if(huart->Instance == USART2)
  {
#if defined(USE_IDLE_UART2)
    DI_UART_RxProcess(UART_DEVICE_2);
#endif
  }
  else if(huart->Instance == USART3)       // Main
  {
#if defined(USE_IDLE_UART3)
    DI_UART_RxProcess(UART_DEVICE_3);
#endif
  }
	else
  {
    return;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// TX interrut callback
// Tx Transfer completed callback
// [2023-02-28] cs.Hong
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)       // Main
  {
#if defined(USE_IDLE_UART1)
    UARTChannel[UART_DEVICE_1].UartReady = SET;
    DI_UART_TxProcess(UART_DEVICE_1);
#endif
  }
  else if(huart->Instance == USART2)
  {
#if defined(USE_IDLE_UART2)
    UARTChannel[UART_DEVICE_2].UartReady = SET;
    DI_UART_TxProcess(UART_DEVICE_2);
#endif    
  }
  else if(huart->Instance == USART3)
  {
#if defined(USE_IDLE_UART3)
    UARTChannel[UART_DEVICE_3].UartReady = SET;
    DI_UART_TxProcess(UART_DEVICE_3);
#endif    
  }
	else
  {
    return;
  }
}


///////////////////////////////////////////////////////////////////////////
//
//
//
void USART_Reset(UART_HandleTypeDef *huart)
{
  UART_DEVICE_ID uartID;

  if(huart->Instance == USART1)
  {
#if defined(USE_IDLE_UART1)
    uartID = UART_DEVICE_1;
#endif
  }
  else if(huart->Instance == USART2)
  {
#if defined(USE_IDLE_UART2)    
    uartID = UART_DEVICE_2;
#endif
  }
  else if(huart->Instance == USART3)
  {
#if defined(USE_IDLE_UART3)
    uartID = UART_DEVICE_3;
#endif
  }
	else
  {
    return;
  }

  huart->RxState = HAL_UART_STATE_READY;

  HAL_UART_DMAStop(huart);
  __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
  __HAL_UART_FLUSH_DRREGISTER(huart);
  __HAL_UART_CLEAR_PEFLAG(huart);

  DI_UART_PreInit(uartID);
  DI_UART_Init(uartID); 

/*  //if(HAL_UART_Receive_DMA(huart, (uint8_t *)mem_rx_buffer[port_index], 1) != HAL_OK)
  if(HAL_UART_Receive_DMA(huart, (uint8_t*)(UARTChannel[port_index].dma_rx_buffer), UARTChannel[port_index].dma_rx_size) != HAL_OK)
  {
    Error_Handler();
  }*/
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void HAL_UART_RxIdleCallback(UART_HandleTypeDef *huart)
{
  uint8_t uartID = 0;

  for( uartID = 0; uartID < UART_DEVICE_MAX; uartID++ )
  {
    if(UARTChannel[uartID].use_uart_flag != TRUE)
    {
      continue;
    }

    if(huart->Instance == UARTChannel[uartID].uartHandle->Instance)
    {
      if( UARTChannel[uartID].use_dma_flag == TRUE)
      {
        DI_UART_RxProcess((UART_DEVICE_ID)uartID);
      }
      else
      {
        //TODO
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void Work_SendMessageLog(uint8_t *format, ...)
{
  va_list arg;

  va_start(arg, format);
  vsprintf((char *)UIF_Message, (char *)format, arg);
  va_end(arg);

  UIF_SendString(PORT_UART3, UIF_Message);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void Work_PortMsgLog(int16_t port, uint8_t *format, ...)
{
  va_list arg;

  va_start(arg, format);
  vsprintf((char *)UIF_Message, (char *)format, arg);
  va_end(arg);

  UIF_SendString(port, UIF_Message);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void DI_UART_DeInit(UART_DEVICE_ID uartID)
{
  UART_HandleTypeDef *huart;
  HAL_StatusTypeDef halReturn = HAL_OK;

  if(UARTChannel[uartID].use_uart_flag != TRUE)
  {
    TRACE_ERROR("Error [%s %d] %d\n\r",__func__,__LINE__,uartID);
    return;
  }

  UARTChannel[uartID].use_uart_flag = FALSE;

  huart = UARTChannel[uartID].uartHandle;

  if (UARTChannel[uartID].use_dma_flag == TRUE)
  {
    halReturn = HAL_UART_DMAStop(huart);
    if (halReturn != HAL_OK)
    {
      TRACE_ERROR("Error [%s %d] %d %x\n\r",__func__,__LINE__,uartID, halReturn);
    }
  }

  halReturn = HAL_UART_DeInit(huart);
  if (halReturn != HAL_OK)
  {
    TRACE_ERROR("Error [%s %d] %d %x\n\r",__func__,__LINE__,uartID, halReturn);
  }
  UARTChannel[uartID].rx_count._prevRxXferCount = 0;
  UARTChannel[uartID].rx_count._lastRxXferCount = 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void DI_UART_DMAReset(UART_DEVICE_ID uartID)
{
  HAL_StatusTypeDef halReturn = HAL_OK;
  UART_HandleTypeDef *huart;
  UART_RING_BUFFER_T *ringBuffer;

  huart = UARTChannel[uartID].uartHandle;
  ringBuffer = (UART_RING_BUFFER_T *)&UARTChannel[uartID].UART_rb;

  if (UARTChannel[uartID].use_dma_flag == TRUE)
  {
    halReturn = HAL_UART_DMAStop(huart);
    if (halReturn != HAL_OK)
    {
      TRACE_ERROR("Error [%s %d] %d %x\n\r",__func__,__LINE__,uartID, halReturn);
    }
  }

  DI_UART_Init(uartID);

  DI_UART_RB_BufferCountInit(ringBuffer);
  UARTChannel[uartID].rx_count._prevRxXferCount = 0;
  UARTChannel[uartID].rx_count._lastRxXferCount = 0;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
UART_RETURN_TYPE DI_UART_GetTxStatus(UART_DEVICE_ID uartID)
{
  if(UARTChannel[uartID].use_uart_flag != TRUE)
  {
    return UART_ERROR_NO_RESOURCE;
  }

  if(UARTChannel[uartID].UartReady != SET)
  {
    UART_HandleTypeDef *huart;
    huart = UARTChannel[uartID].uartHandle;

    if(huart->gState == HAL_UART_STATE_READY)
    {
      UARTChannel[uartID].UartReady = RESET;
      return UART_ERROR_OK;
    }
    return UART_ERROR_RESOURSE_BUSY;
  }
  UARTChannel[uartID].UartReady = RESET;

  return UART_ERROR_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void DI_UART_RB_BufferCountInit(UART_RING_BUFFER_T *ringBuffer)
{
  __BUF_RESET(ringBuffer->rx_head);
  __BUF_RESET(ringBuffer->rx_tail);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void DI_UART_RxProcess(UART_DEVICE_ID uartID)
{
  uint16_t _prevRxXferCount=0;
  uint16_t _lastRxXferCount=0;

  uint16_t readSize=0;
  uint16_t i=0;

  uint8_t *receiveData=NULL;
  uint32_t receiveBufferSize= 0;

  UART_HandleTypeDef *huart;
  UART_RING_BUFFER_T *ringBuffer;


  huart = UARTChannel[uartID].uartHandle;

  receiveData = (uint8_t *)UARTChannel[uartID].dma_rx_buffer;
  receiveBufferSize = UARTChannel[uartID].dma_rx_size;
  ringBuffer = (UART_RING_BUFFER_T *)&UARTChannel[uartID].UART_rb;

  _prevRxXferCount = UARTChannel[uartID].rx_count._prevRxXferCount;
  _lastRxXferCount = UARTChannel[uartID].rx_count._lastRxXferCount;

  _prevRxXferCount = _lastRxXferCount;

  //huart1->RxXferSize = RX buffer size
  //huart1->hdmarx->Instance->NDTR = remained buffer size
  _lastRxXferCount = huart->RxXferSize - huart->hdmarx->Instance->CNDTR;

  if(_prevRxXferCount != _lastRxXferCount)
  {
    if(_lastRxXferCount > 0 )
    {
      if(_lastRxXferCount < _prevRxXferCount)
      {
        /* Check if buffer is more space
        * If no more space, remaining character will be trimmed out
        */
        for(i=0;i<(huart->RxXferSize-_prevRxXferCount);i++)
        {
          if (!__BUF_IS_FULL(ringBuffer->rb_size,ringBuffer->rx_head,ringBuffer->rx_tail))
          {
            ringBuffer->rx[ringBuffer->rx_head] = receiveData[_prevRxXferCount+i];
            __BUF_INCR(ringBuffer->rb_size, ringBuffer->rx_head);
          }
          else
          {
            //TODO
            //LOG_MSG(LOG_MSG_LEVEL_ERROR,"***** RX %d buffer is full",uartID);
            TRACE_ERROR("*****%d RX %d buffer is full\n\r",__LINE__,uartID);
            __BUF_RESET(ringBuffer->rx_head);
            __BUF_RESET(ringBuffer->rx_tail);
          }
        }

        _prevRxXferCount = 0;
      }
      readSize = _lastRxXferCount - _prevRxXferCount;
    }
    else if(_prevRxXferCount > 0 )
    {
      readSize = huart->RxXferSize - _prevRxXferCount;
    }

    if(readSize > 0)
    {
      /* Check if buffer is more space
      * If no more space, remaining character will be trimmed out
      */
      for(i=0;i<readSize;i++)
      {
        if (!__BUF_IS_FULL(ringBuffer->rb_size,ringBuffer->rx_head,ringBuffer->rx_tail))
        {
          ringBuffer->rx[ringBuffer->rx_head] = receiveData[_prevRxXferCount+i];
          __BUF_INCR(ringBuffer->rb_size, ringBuffer->rx_head);
        }
        else
        {
          //TODO
          //LOG_MSG(LOG_MSG_LEVEL_ERROR,"***** RX %d buffer is full",uartID);
          TRACE_ERROR("*****%d RX %d buffer is full",__LINE__,uartID);
          __BUF_RESET(ringBuffer->rx_head);
          __BUF_RESET(ringBuffer->rx_tail);
        }
      }
    }

    if(huart->hdmarx->Instance->CNDTR == huart->RxXferSize)
    {
      _prevRxXferCount=0;
      _lastRxXferCount=0;
      if(UARTChannel[uartID].use_dma_flag)
      {
        HAL_UART_Receive_DMA(huart, (uint8_t*)receiveData, receiveBufferSize);
      }
      else
      {
        HAL_UART_Receive_IT(huart, (uint8_t*)receiveData, receiveBufferSize);
      }
    }
  }
  UARTChannel[uartID].rx_count._prevRxXferCount = _prevRxXferCount;
  UARTChannel[uartID].rx_count._lastRxXferCount = _lastRxXferCount;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void DI_UART_TxProcess(UART_DEVICE_ID uartID)
{
  uint32_t readSize=0;
  UART_RETURN_TYPE returnValue=UART_ERROR_OK;

  UART_HandleTypeDef *huart;
  uint8_t *sendData=NULL;
  UART_RING_BUFFER_T *ringBuffer;

  huart = UARTChannel[uartID].uartHandle;
  sendData = (uint8_t *)UARTChannel[uartID].dma_tx_buffer;
  ringBuffer = (UART_RING_BUFFER_T *)&UARTChannel[uartID].UART_rb;

  if(__BUF_IS_EMPTY(ringBuffer->rb_size, ringBuffer->tx_head,ringBuffer->tx_tail))
  {
    return;
  }

  returnValue = DI_UART_GetTxStatus(uartID);
  if (returnValue != UART_ERROR_OK)
  {
    return;
  }

  while(!__BUF_IS_EMPTY(ringBuffer->rb_size, ringBuffer->tx_head, ringBuffer->tx_tail))
  {
    sendData[readSize]=ringBuffer->tx[ringBuffer->tx_tail];
    __BUF_INCR(ringBuffer->rb_size, ringBuffer->tx_tail);
    readSize++;
  }
  if(readSize > UARTChannel[uartID].dma_tx_size)
  {
    // Notice : Printing causes problems in operation.
    // TRACE_ERROR("Error [%s %d] count = %d (%d)\n\r",__func__,__LINE__, readSize , UARTChannel[uartID].dma_tx_size);  //Don't change TRACE_ERROR.
    readSize = UARTChannel[uartID].dma_tx_size;
  }

  if(readSize > 0)
  {
    HAL_UART_IRQHandler(huart);
    //HAL_UART_Transmit_DMA(huart, (uint8_t*)sendData, readSize);
    //HAL_UART_Transmit_IT(huart, (uint8_t*)sendData, readSize);
    if(huart->hdmatx == NULL)
      HAL_UART_Transmit(huart, (uint8_t*)sendData, readSize, 1000);
    else
      HAL_UART_Transmit_DMA(huart, (uint8_t*)sendData, readSize);
  }
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void DI_UART_PreInit(UART_DEVICE_ID uartID)
{
#if USE_IDLE_UART1
  if(uartID == UART_DEVICE_1)
  {
    UARTChannel[UART_DEVICE_1].use_uart_flag = FALSE;
    UARTChannel[UART_DEVICE_1].uartID = UART_DEVICE_1;
    UARTChannel[UART_DEVICE_1].uartHandle = &huart1;
    #if defined(USE_DMA_UART1)
      UARTChannel[UART_DEVICE_1].use_dma_flag = TRUE;
    #else
      UARTChannel[UART_DEVICE_1].use_dma_flag = FALSE;
    #endif
  }
#endif

#if USE_IDLE_UART2
  if(uartID == UART_DEVICE_2)
  {
    UARTChannel[UART_DEVICE_2].use_uart_flag = FALSE;
    UARTChannel[UART_DEVICE_2].uartID = UART_DEVICE_2;
    UARTChannel[UART_DEVICE_2].uartHandle = &huart2;
    UARTChannel[UART_DEVICE_2].use_dma_flag = TRUE;
    #if defined(USE_DMA_UART2)
      UARTChannel[UART_DEVICE_2].use_dma_flag = TRUE;
    #else
      UARTChannel[UART_DEVICE_2].use_dma_flag = FALSE;
    #endif
  }
#endif

#if USE_IDLE_UART3
  if(uartID == UART_DEVICE_3)
  {
    UARTChannel[UART_DEVICE_3].use_uart_flag = FALSE;
    UARTChannel[UART_DEVICE_3].uartID = UART_DEVICE_3;
    UARTChannel[UART_DEVICE_3].uartHandle = &huart3;
    UARTChannel[UART_DEVICE_3].use_dma_flag = TRUE;
    #if defined(USE_DMA_UART3)
      UARTChannel[UART_DEVICE_3].use_dma_flag = TRUE;
    #else
      UARTChannel[UART_DEVICE_3].use_dma_flag = FALSE;
    #endif
  }
#endif

#if USE_IDLE_UART4
  if(uartID == UART_DEVICE_4)
  {
    UARTChannel[UART_DEVICE_4].use_uart_flag = FALSE;
    UARTChannel[UART_DEVICE_4].uartID = UART_DEVICE_4;
    UARTChannel[UART_DEVICE_4].uartHandle = &huart4;
    UARTChannel[UART_DEVICE_4].use_dma_flag = TRUE;
    #if defined(USE_DMA_UART4)
      UARTChannel[UART_DEVICE_4].use_dma_flag = TRUE;
    #else
      UARTChannel[UART_DEVICE_4].use_dma_flag = FALSE;
    #endif
  }
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void DI_UART_Init(UART_DEVICE_ID uartID)
{
  HAL_StatusTypeDef halReturn = HAL_OK;
  UART_HandleTypeDef *huart;

  UARTChannel[uartID].use_uart_flag = TRUE;

  huart = UARTChannel[uartID].uartHandle;

  if(UARTChannel[uartID].use_dma_flag == TRUE)
  {
    // Reset ring buf head and tail idx
    __BUF_RESET(UARTChannel[uartID].UART_rb.rx_head);
    __BUF_RESET(UARTChannel[uartID].UART_rb.rx_tail);
    __BUF_RESET(UARTChannel[uartID].UART_rb.tx_head);
    __BUF_RESET(UARTChannel[uartID].UART_rb.tx_tail);

    UARTChannel[uartID].UART_rb.rx = *(rx_ring_buffer+uartID);    //rx_ring_buffer[uartID]
    UARTChannel[uartID].UART_rb.tx = *(tx_ring_buffer+uartID);
    UARTChannel[uartID].UART_rb.rb_size = UART_RING_BUFSIZE;

    UARTChannel[uartID].dma_tx_buffer = *(tx_dma_buffer+uartID);
    UARTChannel[uartID].dma_rx_buffer = *(rx_dma_buffer+uartID);
    UARTChannel[uartID].dma_tx_size = TX_DMA_BUFFERSIZE;
    UARTChannel[uartID].dma_rx_size = RX_DMA_BUFFERSIZE;

    halReturn = HAL_UART_Receive_DMA(huart, (uint8_t*)(UARTChannel[uartID].dma_rx_buffer), UARTChannel[uartID].dma_rx_size);
    if (halReturn != HAL_OK)
    {
      //#if !defined (USE_GPS)  // if without GPS module, too much talker....
      TRACE_ERROR("Error [%s %d] %d %x\n\r",__func__,__LINE__,uartID, halReturn);
      //#endif
    }
  }
  else
  {
    HAL_UART_Receive_IT(huart, (uint8_t *)(UARTChannel[uartID].dma_rx_buffer), 1);
  }
}



///////////////////////////////////////////////////////////////////////////
//
//
// [2023-02-28] cs.Hong
void InitUart(void)
{
#if USE_IDLE_UART1
  DI_UART_PreInit(UART_DEVICE_1);
  DI_UART_Init(UART_DEVICE_1);
#endif
  
#if USE_IDLE_UART2
  DI_UART_PreInit(UART_DEVICE_2);
  DI_UART_Init(UART_DEVICE_2);
#endif

#if USE_IDLE_UART3
  DI_UART_PreInit(UART_DEVICE_3);
  DI_UART_Init(UART_DEVICE_3);
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
uint32_t DI_UART_RB_GetRxReceviedBufferCount(UART_RING_BUFFER_T *ringBuffer)
{
  uint32_t count;

  if (__BUF_IS_EMPTY(ringBuffer->rb_size,ringBuffer->rx_head,ringBuffer->rx_tail))
  {
    count = 0;
  }
  else if (__BUF_IS_FULL(ringBuffer->rb_size,ringBuffer->rx_head,ringBuffer->rx_tail))
  {
    count = ringBuffer->rb_size;
  }
  else if (ringBuffer->rx_head > ringBuffer->rx_tail)
  {
    count = ringBuffer->rx_head - ringBuffer->rx_tail;
  }
  else
  {
    count = ringBuffer->rb_size - ringBuffer->rx_tail;
    if (ringBuffer->rx_head > 0)
    {
      count += ringBuffer->rx_head;
    }
  }

  return count;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
uint32_t DI_UART_RB_GetTxRemainBufferCount(UART_RING_BUFFER_T *ringBuffer)
{
  uint32_t count;

  if (__BUF_IS_EMPTY(ringBuffer->rb_size, ringBuffer->tx_head,ringBuffer->tx_tail))
  {
    count = ringBuffer->rb_size;
  }
  else if (__BUF_IS_FULL(ringBuffer->rb_size,ringBuffer->tx_head,ringBuffer->tx_tail))
  {
    count = 0;
  }
  else if (ringBuffer->tx_head > ringBuffer->tx_tail)
  {
    count = ringBuffer->rb_size - ringBuffer->tx_head;
    if (ringBuffer->tx_tail > 0)
    {
      count += ringBuffer->tx_tail;
    }
  }
  else
  {
    count = ringBuffer->tx_tail - ringBuffer->tx_head;
  }

  return count;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
UART_RETURN_TYPE DI_UART_PutChar(UART_DEVICE_ID uartID, uint8_t data)
{
  UART_RETURN_TYPE returnValue=UART_ERROR_OK;
  uint8_t Tx_Char = data;
  UART_HandleTypeDef *huart;

  huart = UARTChannel[uartID].uartHandle;

  if(UARTChannel[uartID].use_uart_flag != TRUE)
  {
    return UART_ERROR_NO_RESOURCE;
  }

  HAL_UART_Transmit(huart,(uint8_t *)&Tx_Char,1,0xFFFF);

  return returnValue;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
UART_RETURN_TYPE DI_UART_TxMessage(UART_DEVICE_ID uartID, uint8_t *message, uint16_t size)
{
  HAL_StatusTypeDef returnValue=HAL_OK;
  UART_RETURN_TYPE returnUart=UART_ERROR_OK;
  UART_HandleTypeDef *huart;
  uint8_t *sendData=NULL;

  huart = UARTChannel[uartID].uartHandle;
  sendData = (uint8_t *)UARTChannel[uartID].dma_tx_buffer;

  returnUart = DI_UART_GetTxStatus(uartID);
  if (returnUart != UART_ERROR_OK)
  {
    TRACE_ERROR("Error [%s %d]\n\r",__func__,__LINE__);
    return UART_ERROR_RESOURSE_BUSY;
  }

  memcpy((void *)sendData,message,size);

  HAL_UART_IRQHandler(huart);

  returnValue = HAL_UART_Transmit_DMA(huart, (uint8_t*)sendData, size);
  if(returnValue != HAL_OK)
  {
    TRACE_ERROR("Error [%s %d]\n\r",__func__,__LINE__);
    if(returnValue == HAL_BUSY)
    {
      return UART_ERROR_RESOURSE_BUSY;
    }
    return UART_ERROR_TRANSMIT;
  }

  return UART_ERROR_OK;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
UART_RETURN_TYPE DI_UART_TxBufferedMessage(UART_DEVICE_ID uartID, uint8_t *message, uint16_t size)
{
  uint8_t i=0;
  UART_RING_BUFFER_T *ringBuffer;
  uint32_t count = 0;
  ringBuffer = (UART_RING_BUFFER_T *)&UARTChannel[uartID].UART_rb;

  count = DI_UART_RB_GetTxRemainBufferCount(ringBuffer);
  if( count < size)
  {
    //notice : Don't change to TRACE_ERROR.
    //Notice : Printing causes problems in operation. just for test...
    TRACE_ERROR("Error [%s %d] count = %d (%d)\n\r",__func__,__LINE__, count , size);
    return UART_ERROR_NO_RESOURCE;
  }

  for(i=0;i<size;i++)
  {
    ringBuffer->tx[ringBuffer->tx_head] = message[i];
    __BUF_INCR(ringBuffer->rb_size, ringBuffer->tx_head);
  }
  return UART_ERROR_OK;
}


#if 0
//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void DI_UART_ChangeBaudRate(UART_DEVICE_ID uartID, uint32_t baudRate)
{
  UART_HandleTypeDef *huart;
  UART_RING_BUFFER_T *ringBuffer;
  huart = UARTChannel[uartID].uartHandle;
  ringBuffer = (UART_RING_BUFFER_T *)&UARTChannel[uartID].UART_rb;

  while(HAL_IS_BIT_CLR(huart->Instance->SR, USART_SR_TXE));
  //TODO : USART_FLAG_TC ???

  /* Disable the peripheral */
  __HAL_UART_DISABLE(huart);

  /* Set the UART Communication parameters */
  huart->Init.BaudRate = baudRate;

  if(huart->Instance == USART1)
  {
    huart->Instance->BRR = UART_BRR_SAMPLING16(HAL_RCC_GetPCLK2Freq(), huart->Init.BaudRate);
  }
  else
  {
    huart->Instance->BRR = UART_BRR_SAMPLING16(HAL_RCC_GetPCLK1Freq(), huart->Init.BaudRate);
  }

  /* In asynchronous mode, the following bits must be kept cleared:
  * LINEN and CLKEN bits in the USART_CR2 register,
  * SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
  CLEAR_BIT(huart->Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(huart->Instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

  /* Enable the peripheral */
  __HAL_UART_ENABLE(huart);

  /* Initialize the UART state */
  huart->ErrorCode = HAL_UART_ERROR_NONE;
  huart->gState= HAL_UART_STATE_READY;
  huart->RxState= HAL_UART_STATE_READY;

  SET_BIT(huart->Instance->CR1,USART_CR1_RE|USART_CR1_TE|USART_CR1_UE|USART_CR1_RXNEIE);

  __HAL_UART_ENABLE_IT(huart,UART_IT_RXNE);
  __HAL_UART_ENABLE_IT(huart,UART_IT_IDLE);

  DI_UART_Init(uartID);

  DI_UART_RxProcess(uartID);
  DI_UART_RB_BufferCountInit(ringBuffer);
}
#endif

#if 0
//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
uint8_t DI_USART_GetByte(UART_DEVICE_ID uartID)
{
  uint8_t data;
  uint32_t count=0;
  UART_RING_BUFFER_T *ringBuffer;

  ringBuffer = (UART_RING_BUFFER_T *)&UARTChannel[uartID].UART_rb;
  if(!__BUF_IS_EMPTY(ringBuffer->rb_size, ringBuffer->rx_head, ringBuffer->rx_tail))
  {
    count = DI_UART_RB_GetRxReceviedBufferCount(ringBuffer);

    if(count > 0)
    {
      data = ringBuffer->rx[ringBuffer->rx_tail];
      __BUF_INCR(ringBuffer->rb_size, ringBuffer->rx_tail);
    }
  }
  
  return data;
}
#else

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
uint8_t DI_USART_GetByte(UART_DEVICE_ID uartID, uint8_t *p)
{
  uint32_t count=0;
  UART_RING_BUFFER_T *ringBuffer;

  ringBuffer = (UART_RING_BUFFER_T *)&UARTChannel[uartID].UART_rb;
  count = DI_UART_RB_GetRxReceviedBufferCount(ringBuffer);
  if(count > 0)
  {
    *p = ringBuffer->rx[ringBuffer->rx_tail];
    __BUF_INCR(ringBuffer->rb_size, ringBuffer->rx_tail);
  }
  else 
  {
    return FALSE;
  }
  return TRUE;
}
#endif

int16_t DI_USART_GetString(UART_DEVICE_ID uartID, int8_t *out_string)
{
  uint32_t count=0, i;
  UART_RING_BUFFER_T *ringBuffer;

  ringBuffer = (UART_RING_BUFFER_T *)&UARTChannel[uartID].UART_rb;
  count = DI_UART_RB_GetRxReceviedBufferCount(ringBuffer);

  if(count > 0) 
  {
    for(i=0; i<count; i++)
    {
      *(out_string+i) = ringBuffer->rx[ringBuffer->rx_tail];
      __BUF_INCR(ringBuffer->rb_size, ringBuffer->rx_tail);
    }
  }
  return (int16_t)count;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void DI_USART_SendByte(UART_DEVICE_ID uartID, uint8_t c)
{
  UART_RING_BUFFER_T *ringBuffer;

  ringBuffer = (UART_RING_BUFFER_T*)&UARTChannel[uartID].UART_rb;
  if(DI_UART_RB_GetTxRemainBufferCount(ringBuffer) < sizeof(c))
  {
    return;
  }

  ringBuffer->tx[ringBuffer->tx_head] = c;
  __BUF_INCR(ringBuffer->rb_size, ringBuffer->tx_head);

  SET_BIT(UARTChannel[uartID].uartHandle->Instance->CR1, USART_CR1_TCIE);
}



void DI_USART_SendString(UART_DEVICE_ID uartID, int8_t* in_string, uint16_t length)
{
  uint16_t  i=0;
  UART_RING_BUFFER_T *ringBuffer;

  ringBuffer = (UART_RING_BUFFER_T*)&UARTChannel[uartID].UART_rb;

  if(DI_UART_RB_GetTxRemainBufferCount(ringBuffer) < length)
  {
    return;
  }

  for(i=0; i<length; i++)
  {
    ringBuffer->tx[ringBuffer->tx_head] = in_string[i];
    __BUF_INCR(ringBuffer->rb_size, ringBuffer->tx_head);
  }

  SET_BIT(UARTChannel[uartID].uartHandle->Instance->CR1, USART_CR1_TCIE);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
void DI_USART_PutByte(UART_DEVICE_ID uartID, int8_t c)
{
  uint8_t  receiveData=NULL;
  UART_RING_BUFFER_T *ringBuffer;


  receiveData = (uint8_t)c;
  ringBuffer = (UART_RING_BUFFER_T *)&UARTChannel[uartID].UART_rb;

  /* Check if buffer is more space
  * If no more space, remaining character will be trimmed out
  */
  if (!__BUF_IS_FULL(ringBuffer->rb_size,ringBuffer->rx_head,ringBuffer->rx_tail))
  {
    ringBuffer->tx[ringBuffer->tx_head] = receiveData;
    __BUF_INCR(ringBuffer->rb_size, ringBuffer->tx_head);    
  }
  else
  {
    //TODO
    //LOG_MSG(LOG_MSG_LEVEL_ERROR,"***** RX %d buffer is full",uartID);
    TRACE_ERROR("*****%d RX %d buffer is full",__LINE__,uartID);
    __BUF_RESET(ringBuffer->rx_head);
    __BUF_RESET(ringBuffer->rx_tail);
  }
}

// [2023-02-28] cs.Hong
UART_DEVICE_ID PortToDeviceID(uint16_t port)
{
	UART_DEVICE_ID uartID;

  if(port >= NUM_UIF_PORT) return (UART_DEVICE_ID)0;//'0' port

#if defined(USE_IDLE_UART1)
	if(port == PORT_UART1) uartID = UART_DEVICE_1;//1
#endif

#if defined(USE_IDLE_UART2)
	if(port == PORT_UART2) 		uartID = UART_DEVICE_2;
#endif

#if defined(USE_IDLE_UART3)
	if(port == PORT_UART3) 	  uartID = UART_DEVICE_3;
#endif

	return uartID;
}


uint32_t ErrCount;
#define SERAIL_START  1
#define SERAIL_END    0
void SendErrorMsg(uint32_t errcnt)
{
  int8_t ErrMsg[40];

  memset(ErrMsg, 0x00, sizeof(ErrMsg));
  sprintf((char *)ErrMsg, "[Error Count] : %10d\r\n", errcnt);
  DI_USART_SendString(UART_DEVICE_2, ErrMsg, sizeof(ErrMsg));
}

uint32_t Parsing(uint8_t *ptr)
{
  uint8_t start_char, end_char, i;
  uint32_t val=0;

  start_char = *(ptr+0);
  end_char   = *(ptr+6);

  if(start_char == '{' && end_char == '}')
  {
    for(i=1;i<6;i++)
    {
      if(*(ptr+i) >= '0' && *(ptr+i) <= '9')
      {
        if(val) val *= 10;
        val += (*(ptr+i) - '0');
      }
    }
  }
  return val;
}


void Work_Serial(void)
{
  //static uint16_t RcvCount=0;
	static uint8_t SerialCnt=0, SerialFlag;
  static uint8_t UartData[100];
  uint8_t ch=0;

	if(DI_USART_GetByte(UART_DEVICE_2, &ch) != FALSE)
  {
    switch(ch) {
    case '{':
      SerialCnt = 0;
      SerialFlag = SERAIL_START;
      UartData[SerialCnt++] = ch;
      break;

    case '}':
      if(SerialFlag == SERAIL_START) 
      {
        UartData[SerialCnt++] = ch;
        Parsing(UartData);
      }
      else
      {
        ErrCount++;
        SendErrorMsg(ErrCount);
      }
      break;

    default:
      if(SerialFlag == SERAIL_START)
      {
        UartData[SerialCnt++] = ch;
      }
      else
      {
        SerialCnt = 0;
      }
      break;
    }

    if(SerialCnt > 30)
    {
      //error
      ErrCount++;
      SerialCnt = 0;
      SerialFlag = SERAIL_END;
      SendErrorMsg(ErrCount);
    }
  }
}