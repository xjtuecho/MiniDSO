/**********************(C) COPYRIGHT 2018 e-Design Co.,Ltd.*********************
  AppDisk.h for DS213 APP                                        Author : bure                                   Author : bure & SNAKE
*******************************************************************************/
#ifndef __APP_DISK_H
#define __APP_DISK_H

  #ifdef STM32F30X
    #include "STM32F30x.h"
  #elif STM32F10X_HD
    #include "STM32F10x.h"
  #endif

  void AppDiskInit(void);
  void ProgDiskSect(u8 *pBuf, u32 DiskAddr);
  void ReadDiskData(u8 *pBuf, u32 DiskAddr, u16 n);
  void Write_Memory(u32 W_Offset, u32 W_Length);
  void Read_Memory (u32 R_Offset, u32 R_Length);

#endif
/*********************************  END OF FILE  ******************************/

