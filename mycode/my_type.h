
/*****************************************************
Project : 
Company : Intellian Technologies
Comments:

Chip type               : STM32F429ZGT
Program type            : Application
Core Clock frequency    : 25 MHz

File    : di_type.h
Version :
Date    :
Author  :

*****************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DI_TYPE_H__
#define __DI_TYPE_H__


/* Includes ------------------------------------------------------------------*/
	#include "main.h"



/* Defines -------------------------------------------------------------------*/

	/** Macro used to place DMA buffers in DMA accessible memory.
  	* Only CCM (0x10000000) is not accessible to DMA. */
	#define DMA_BUF _Pragma("location=\"DMA_BUF\"")

	typedef enum _DI_RETURN_TYPE_
	{
		DI_ERROR_OK      = 0,
		DI_ERROR_NOK,
		DI_ERROR_INVALID_ARG,
		DI_ERROR_NOT_SUPPORTED,
		DI_ERROR_NO_DEVICE,
		DI_ERROR_NO_RESOURCE,
		DI_ERROR_RESOURSE_BUSY,
		DI_ERROR_OUT_OF_MEMORY,
		DI_ERROR_OUT_OF_DATA,
		DI_ERROR_POWER_OFF,
		DI_ERROR_MAX
	} DI_RETURN_TYPE;


/* Prototypes ----------------------------------------------------------------*/

#endif /*__DI_TYPE_H__ */


