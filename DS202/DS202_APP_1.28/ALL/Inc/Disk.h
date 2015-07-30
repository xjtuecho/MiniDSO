/********************* (C) COPYRIGHT 2014 e-Design Co.,Ltd. ********************
 Brief : USB Disk                                                 Author : bure
*******************************************************************************/
#ifndef __DISK_H
#define __DISK_H

#include "STM32F30x.h"

#define SECTOR_SIZE    SEC_SIZE
#define SECTOR_CNT     SEC_AMNT

void Disk_Init(void);
void Write_Memory(u32 W_Offset, u32 W_Length);
void Read_Memory (u32 R_Offset, u32 R_Length);

#endif
/*********************************  END OF FILE  ******************************/
