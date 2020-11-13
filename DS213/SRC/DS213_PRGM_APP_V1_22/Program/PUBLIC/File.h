/********************* (C) COPYRIGHT 2018 e-Design Co.,Ltd. ********************
  File.h Ver: 2.0                                                Author : bure
*******************************************************************************/
#ifndef __FILE_H
#define __FILE_H

  #ifdef STM32F30X
    #include "STM32F30x.h"
  #elif STM32F10X_HD
    #include "STM32F10x.h"
  #endif

  #define FLASH_WAIT_TIMEOUT      100000 
  #define PAGE_0                  0 
  #define PAGE_1                  1 
  #define PAGE_2                  2 
  #define PAGE_3                  3 

  #define OK                      0 // 操作完成
  #define SEC_ERR                 1 // 扇区读写错误
  #define FAT_ERR                 2 // FAT表读写错误
  #define OVER                    3 // 操作溢出
  #define NEW                     4 // 空白/新目录项
  #define EMPT                    4 // 文件不存在
  #define VER_ERR                 5 // 版本错误
  #define SUM_ERR                 6 // 校验和错误
  #define RD_ERR                  8 // 扇区读错误
  #define WR_ERR                  9 // 扇区写错误
  #define DISK_ERR                10 // 磁盘错误

  extern uc8 F_NAME[][12];
  extern u8  TrckBuf[];
  u8  Snapshot(void);
  u8  SaveParam(void);
  u8  LoadParam(void);
  u8  SaveBuf(u8 FileNum);
  u8  SaveCsv(u8 FileNum);
  u8  SaveTrack(u8 FileNum);
  u8  LoadTrack(u8 FileNum);
  u16 SeekMaxFileNum(u8 *pName); 

#endif
/********************************* END OF FILE ********************************/
