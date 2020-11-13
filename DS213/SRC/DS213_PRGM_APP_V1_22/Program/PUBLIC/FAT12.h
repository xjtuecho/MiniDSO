/********************* (C) COPYRIGHT 2018 e-Design Co.,Ltd. ********************
  FileName: FAT12.h  
  Versions: DS213_APP Ver 1.0a                           Author : bure & SNAKE
*******************************************************************************/
#ifndef __FAT12_H
#define __FAT12_H

  #ifdef STM32F30X
    #include "STM32F30x.h"
  #elif STM32F10X_HD
    #include "STM32F10x.h"
  #endif

  typedef struct{
    u32 FAT1_BASE;          // FAT1区开始地址
    u32 FAT2_BASE;          // FAT2区开始地址
    u32 ROOT_BASE;          // 根目录开始地址
    u32 FILE_BASE;          // 文件区开始地址
    u32 FAT_LEN;
    u32 SEC_LEN;            // 扇区长度
    u32 FAT_END;            // 链接结束
    u8  FAT1_SEC;           // FAT1扇区数
    u8  FAT2_SEC;
  }FAT_ParamDef;

  //-------FLASH-----W25Q64BV---------------------------------------------------

  #define FILE_BASE_8M    0x7000     /*文件区开始地址 */
  #define ROOT_BASE_8M    0x3000     /* 根目录开始地址*/
  #define FAT_LEN_8M      0x1000
  #define FAT1_BASE_8M    0x1000     /* FAT1区开始地址*/
  #define FAT2_BASE_8M    0x2000     /* FAT2区开始地址*/
  #define SEC_LEN_8M      0x1000     /* 扇区长度 */
  #define FAT1_SEC_8M     0x1        /* FAT1扇区数*/
  #define FAT2_SEC_8M     0x1        /* FAT2扇区数*/
  #define FAT_END_8M      0x7FF      /* 链接结束*/

  #define OK              0          /* 操作完成*/
  #define SEC_ERR         1          /* 扇区读写错误*/
  #define FAT_ERR         2          /* FAT表读写错误*/
  #define OVER            3          /* 操作溢出*/
  #define NEW             4          /* 空白/新目录项*/
  #define SUM_ERR         6          /* 校验和错误*/

  #define OW              0          /* 或写(数据从0变1改写)*/
  #define RW              1          /* 重写*/

  extern u8 Clash;
  extern FAT_ParamDef  FAT_Param;

  void InitFatParam(void);
//u8   ReadFileSec(u8 *Buffer);
//u8   ProgFileSec(u8 *Buffer);
//u8   OpenFileRd(u8 *Buffer, u8 *FileName);
//u8   OpenFileWr(u8 *Buffer, u8 *FileName);
//u8   CloseFile(u8 *Buffer, u32 Lenght);
u8   NextCluster(u16 *Cluster);
u8   ReadFileSec(u8 *pBuffer, u16 *pCluster);
u8   ProgFileSec(u8 *Buffer, u16 *Cluster);
//u8   ProgDiskPage(u8 *Buffer, u32 ProgAddr);
u8   SeekBlank(u8 *Buffer, u16 *Cluster);
u8   SetCluster(u8 *Buffer, u16 *Cluster);
u8   OpenFileRd(u8 *Buffer, u8 *FileName, u16 *Cluster, u32 *pDirAddr);
u8   OpenFileWr(u8 *Buffer, u8 *FileName, u16 *Cluster, u32 *pDirAddr);
u8   CloseFile(u8 *Buffer, u32 Lenght, u16 *Cluster, u32 *pDirAddr);
  
#endif
/********************************* END OF FILE ********************************/
