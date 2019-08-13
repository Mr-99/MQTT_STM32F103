#ifndef   __FLASH_H__
#define   __FLASH_H__

#include "stm32f10x.h"
#include "stm32f10x_flash.h"

#define FLASH_PAGE_SIZE    ((uint16_t)0x400)	//STM32F103RCT6的内部Flash的一个扇区为1K字节

int Flash_Read(uint32_t iAddress, uint8_t *buf, int32_t iNbrToRead);
int Flash_Write(uint32_t iAddress, uint8_t *buf, uint32_t iNbrToWrite);

//void RCC_Configuration_Flash(void);

#endif
