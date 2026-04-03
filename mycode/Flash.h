/**
  ******************************************************************************
  * @file    EEPROM_Emulation/inc/eeprom.h 
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    07/27/2009
  * @brief   This file contains all the functions prototypes for the EEPROM 
  *          emulation firmware library.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPROM_H
#define __EEPROM_H

/* Includes ------------------------------------------------------------------*/
	#include "stm32f1xx.h"


/* Exported constants --------------------------------------------------------*/
/* Define the STM32F10Xxx Flash page size depending on the used STM32 device */
	#define PAGE_SIZE  (uint16_t)0x400  /* Page size = 1KByte */

/* EEPROM start address in Flash */
	#define EEPROM_START_ADDRESS    ((uint32_t)0x0801F800)/* EEPROM emulation start address : after 64KByte of used Flash memory */
  //#define EEPROM_START_ADDRESS    ((uint32_t)0x0800F800)/* EEPROM emulation start address : after 64KByte of used Flash memory */

/* Pages 0 and 1 base and end addresses */
	#define PAGE0_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + 0x000))
	#define PAGE0_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))
	
	#define PAGE1_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + PAGE_SIZE))
	#define PAGE1_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (2 * PAGE_SIZE - 1)))
	
/* Used Flash pages for EEPROM emulation */
	#define PAGE0                   ((uint16_t)0x0000)
	#define PAGE1                   ((uint16_t)0x0001)

/* No valid page define */
	#define NO_VALID_PAGE           ((uint16_t)0x00AB)

/* Page status definitions */
	#define ERASED                  ((uint16_t)0xFFFF)     /* PAGE is empty */
	#define RECEIVE_DATA            ((uint16_t)0xEEEE)     /* PAGE is marked to receive data */
	#define VALID_PAGE              ((uint16_t)0x0000)     /* PAGE containing valid data */

/* Valid pages in read and write defines */
	#define READ_FROM_VALID_PAGE    ((uint8_t)0x00)
	#define WRITE_IN_VALID_PAGE     ((uint8_t)0x01)

/* Page full define */
	#define PAGE_FULL               ((uint8_t)0x80)

/* Variables' number */
	#define NumbOfVar               ((uint8_t)0x03)

	#define EEADDR_SSID1			32//32~63(32byte)
	#define EEADDR_PASS1			(EEADDR_SSID1+(32*1))//64~95(32byte)

	#define EEADDR_SSID2			(EEADDR_SSID1+(32*2))//
	#define EEADDR_PASS2			(EEADDR_SSID1+(32*3))//

	#define EEADDR_SSID3			(EEADDR_SSID1+(32*4))//
	#define EEADDR_PASS3			(EEADDR_SSID1+(32*5))//

	#define EEADDR_BEGINACT			1000
	#define EEADDR_FONT				(EEADDR_BEGINACT+1)
	#define EEADDR_ALHOUR			(EEADDR_BEGINACT+2)
	#define EEADDR_ALMIN			(EEADDR_BEGINACT+3)
	#define EEADDR_ALONOFF  		(EEADDR_BEGINACT+4)
	#define EEADDR_SSID       		(EEADDR_BEGINACT+5)
	#define EEADDR_BRIGHT      		(EEADDR_BEGINACT+6)
	#define EEADDR_TIMESIG     		(EEADDR_BEGINACT+7)

	enum {
		DATA8Bit =1,
		DATA16Bit=2,
		DATA32Bit=4
	};

	typedef struct {
		byte Data[1024];
	} TEEPROM;
	
	extern TEEPROM rom;
	
/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
	//uint16_t EE_Init(void);
	void InitFlashMemory(void);
	void ReadSSIDandPass(byte num);

	HAL_StatusTypeDef Flash_EEPROM_Write_Data(uint32_t Start_Address, uint32_t *data, uint16_t No_of_Data, uint8_t Data_format);
	HAL_StatusTypeDef Flash_EEPROM_ErasePage(uint32_t PageAddress);  
	uint16_t Flash_EEPROM_Read_Data(uint32_t Start_Address, uint32_t *dest_data, uint16_t No_of_Data, uint8_t Data_format);
	uint16_t Flash_EEPROM_Format( uint32_t Start_Address, uint32_t End_Address);
	void FlashWriteData(void);
	
#endif /* __EEPROM_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
