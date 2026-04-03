/**
  ******************************************************************************
  * @file    EEPROM_Emulation/src/eeprom.c 
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    07/27/2009
  * @brief   This file provides all the EEPROM emulation firmware functions.
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
/** @addtogroup EEPROM_Emulation
  * @{
  */ 

/* Includes ------------------------------------------------------------------*/
    #include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Global variable used to store variable value in read sequence */
	TEEPROM rom;

/* Virtual address defined by the user: 0xFFFF value is prohibited */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Extern functions ---------------------------------------------------------*/
	extern void    FLASH_PageErase(uint32_t PageAddress);

/**
  * @brief  Restore the pages to a known good state in case of page's status
  *   corruption after a power loss.
  * @param  None.
  * @retval - Flash error code: on write Flash error
  *         - FLASH_COMPLETE: on success
  */

void InitFlashMemory(void)
{
	//byte i=0;
	int result = 0;
  //char InitChar[16] = "FlashStartHere~\0";
  //char InitChar[16] = "WriteFlashTest~\0";
  char InitChar[16] = "csHong20230817~\0";

	/* Unlock the Flash Program Erase controller */
	Flash_EEPROM_Read_Data(PAGE1_BASE_ADDRESS, (uint32_t *)&rom.Data, (sizeof(rom.Data)/4), DATA32Bit);
	result = memcmp((char *)InitChar, (char *)rom.Data, sizeof(InitChar));
  //result = memcmp((char *)InitChar, (char *)rom.Data, 16);
	if(result != 0) {
		memset((char *)rom.Data, 0, sizeof(rom.Data));
		memcpy((char *)rom.Data, (char *)InitChar, sizeof(InitChar));

		rom.Data[EEADDR_BEGINACT]   = ACT_CLOCK3;//default
		rom.Data[EEADDR_FONT]       = F_8X8_GHOTIC;
		rom.Data[EEADDR_ALHOUR]     = 0;
		rom.Data[EEADDR_ALMIN]      = 0;
		rom.Data[EEADDR_ALONOFF]    = 0;
		rom.Data[EEADDR_SSID]       = 0;
		rom.Data[EEADDR_BRIGHT] 		= 1;
		rom.Data[EEADDR_TIMESIG] 		= 1;
        
		FlashWriteData();
	}
	
	sys.DispAction  	= rom.Data[EEADDR_BEGINACT];//ACT_CLOCK3;
	sys.FontKind    	= rom.Data[EEADDR_FONT];
	sys.AlHour      	= rom.Data[EEADDR_ALHOUR];
	sys.AlMin       	= rom.Data[EEADDR_ALMIN];
	sys.AlOnOff     	= rom.Data[EEADDR_ALONOFF];
	sys.DotMatrixBright	= rom.Data[EEADDR_BRIGHT];
	sys.TimeSignal 		= rom.Data[EEADDR_TIMESIG];
	
	//최초에 할당받은 IP를 표현하기 위함...
	sys.ViewIP				= OFF;//IP가 할당이 되면 IP를 표현하고, ON으로 변경(한번만 표현하기 위해 플래그 설정)
	sys.SaveAction		= sys.DispAction;
	sys.DispAction 		= ACT_LOCAL_IP;
}


#if 0
HAL_StatusTypeDef Flash_EEPROM_ErasePage(uint32_t PageAddress)
{
  HAL_StatusTypeDef status = HAL_OK;

  HAL_FLASH_Unlock();
  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);
  if(status != HAL_OK)
  {
    return status;
  }

  /* Erase Page */
  FLASH_PageErase(PageAddress);

  status = FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

  /* Disable the PER Bit */
  /* FLASH_PageErase(addr) 함수 내에서 CR_PER 비트를 1로 만들어 페이지를 지운 후 클리어 하지 않음 */
  /* Bit1 CR_PER : Page erase, Page Erase chosen. 페이지 삭제 비트를 클리어(0) 한다. */
  CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
  HAL_FLASH_Lock();

  return status;
}

#else

HAL_StatusTypeDef Flash_EEPROM_ErasePage(uint32_t EraseAddress)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t PAGEError = 0;
  static FLASH_EraseInitTypeDef EraseInitStruct;

  HAL_FLASH_Unlock(); // lock 풀어주기

  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = EraseAddress;
  EraseInitStruct.NbPages = 1;//(END_ADDR - START_ADDR) / 1024;
  status = HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);

  HAL_FLASH_Lock(); // lock 잠그기
 
  return status;
}
#endif

/**** Function : Write the internal Flash data****/
HAL_StatusTypeDef Flash_EEPROM_Write_Data(uint32_t Start_Address, uint32_t *data, uint16_t No_of_Data, uint8_t Data_format)
{
	uint32_t Flash_EEPROM_Addr;
	uint32_t End_Of_Address=0;
	
	HAL_FLASH_Unlock();
	
	/*Start EEPROM*/
	Flash_EEPROM_Addr = Start_Address;
	End_Of_Address = Start_Address +(No_of_Data * Data_format) ;
	/* Write page data*/
	while((Flash_EEPROM_Addr<End_Of_Address) ) {
		switch(Data_format)	{      
		case DATA8Bit:
			break;
		
		case DATA16Bit:
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Flash_EEPROM_Addr, *data) == HAL_OK)
			{
				Flash_EEPROM_Addr = Flash_EEPROM_Addr + 2;
				data++;
			}
			else
			{
				uint32_t errorcode = HAL_FLASH_GetError();
				return HAL_ERROR;
			}
			break;
		
		case DATA32Bit:
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Flash_EEPROM_Addr, *data) == HAL_OK)
			{
				Flash_EEPROM_Addr = Flash_EEPROM_Addr + 4;
				data++;
			}
			else
			{
				uint32_t errorcode = HAL_FLASH_GetError();
				return HAL_ERROR;
			}
			break;
		}
	}
	HAL_FLASH_Lock();
	return HAL_OK;
}

uint16_t Flash_EEPROM_Read_Data(uint32_t Start_Address, uint32_t *dest_data, uint16_t No_of_Data, uint8_t Data_format)
{
	uint16_t FlashReadCnt=0;

	uint32_t End_Of_Address=0;
	uint32_t Flash_EEPROM_Addr;

	/*Start EEPROM*/
	Flash_EEPROM_Addr = Start_Address;
	End_Of_Address= Start_Address +(No_of_Data * Data_format) ;
	while((Flash_EEPROM_Addr<End_Of_Address)) {
		switch(Data_format)	{
		case DATA8Bit:
			*dest_data = *(__IO uint8_t *)Flash_EEPROM_Addr;
			dest_data++;
			Flash_EEPROM_Addr+=1;
			break;
	
		case DATA16Bit:
			*dest_data = *(__IO uint16_t *)Flash_EEPROM_Addr;
			dest_data++;
			Flash_EEPROM_Addr+=2;
			break;
		 
		case DATA32Bit:
			*dest_data = *(__IO uint32_t *)Flash_EEPROM_Addr;
			dest_data++;
			Flash_EEPROM_Addr+=4;
			break;
	
		default:
			break;
		}
	}

	if(Flash_EEPROM_Addr == End_Of_Address)
		FlashReadCnt = OK;
	else FlashReadCnt = NG;
	//HAL_FLASH_Lock();

	return FlashReadCnt;
}

void FlashWriteData(void)
{
	Delay(DELAY30MS);

	__disable_irq();
	if(Flash_EEPROM_ErasePage(PAGE1_BASE_ADDRESS) == HAL_OK) 
	{
		Flash_EEPROM_Write_Data(PAGE1_BASE_ADDRESS, (uint32_t *)&rom.Data, (sizeof(rom.Data)/4), DATA32Bit);
	}
	__enable_irq();

	Delay(DELAY50MS);
}


/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
