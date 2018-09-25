/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
File Name :      Flash.c
Version :        TS080 APP 1.02
Description:
Author :         Ning
Data:            2017/11/06
History:
*******************************************************************************/
#include "APP_Version.h"
#include "Flash.h"
#include "Bios.h"

u8   ExtFlashSendByte(u8 byte);
void ExtFlashWaitForWriteEnd(void);

/*******************************************************************************
 FLASH_Prog:
*******************************************************************************/
u8 FLASH_Prog(u32 Address, u16 Data)
{
    if(FLASH_WaitForLastOperation(WAIT_TIMES) != FLASH_TIMEOUT)
        FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    return FLASH_ProgramHalfWord(Address, Data);
}
/*******************************************************************************
 FLASH_Erase:
*******************************************************************************/
void FLASH_Erase(u32 Address)
{
    if(Address % FLASH_PAGE == 0)                  // FLASH Page start (1K/Page)
    {
        if(FLASH_WaitForLastOperation(WAIT_TIMES) != FLASH_TIMEOUT)
            FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
        FLASH_ErasePage(Address);                   // FLASH Page erase
    }
}
/*********************************  END OF FILE  ******************************/
