/********************* (C) COPYRIGHT 2014 e-Design Co.,Ltd. ********************
 File Name : EXT_Flash.h
 Version   : DS202 Ver 1.0                                         Author : bure
*******************************************************************************/
#ifndef __EXT_FLASH_H
#define __EXT_FLASH_H

#include "stm32f30x_flash.h"
 
u8   FLASH_Prog(u32 Addr, u16 Data);
void FLASH_Erase(u32 Addr);
void ExtFlashSecWr(u8* pBuffer, u32 WriteAddr);
void ExtFlashDataRd(u8* pBuffer, u32 ReadAddr, u16 Lenght);

#endif
/*********************************  END OF FILE  ******************************/
