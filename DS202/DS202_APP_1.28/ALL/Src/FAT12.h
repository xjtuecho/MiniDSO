/********************* (C) COPYRIGHT 2014 e-Design Co.,Ltd. ********************
 File Name : FAT12.h  
 Version   : DS202 Ver 1.0                                         Author : bure
*******************************************************************************/
#ifndef __FAT12_H
#define __FAT12_H
#include "STM32F30x.h"

//-------FLASH-----W25Q64BV--------------------------------------------------//
#define FILE_BASE_8M    0x7000     /*文件区开始地址 */ 
#define ROOT_BASE_8M    0x3000     /* 根目录开始地址*/ 
#define FAT_LEN_8M      0x1000 
#define FAT1_BASE_8M    0x1000     /* FAT1区开始地址*/ 
#define FAT2_BASE_8M    0x2000     /* FAT2区开始地址*/ 
#define SEC_LEN_8M      0x1000     /* 扇区长度 */ 
#define FAT1_SEC_8M     0x1        /* FAT1扇区数*/ 
#define FAT2_SEC_8M     0x1        /* FAT2扇区数*/ 
#define FAT_END_8M      0x7FF      /* 链接结束*/ 

#define OK           0             /* 操作完成*/ 
#define SEC_ERR      1             /* 扇区读写错误*/ 
#define FAT_ERR      2             /* FAT表读写错误*/ 
#define OVER         3             /* 操作溢出*/ 
#define NEW          4             /* 空白/新目录项*/ 
#define SUM_ERR      6             /* 校验和错误*/ 

#define VER_ERR      1             /* 版本错误*/ 
#define NO_FILE      2             /* 文件不存在*/ 
#define FILE_RW_ERR  3             /* 扇区读错误*/ 
#define DISK_RW_ERR  4             /* 磁盘错误*/ 

#define OW           0             /* 或写(数据从0变1改写)*/ 
#define RW           1             /* 重写*/ 

extern u8 Clash;

u8   ReadFileSec(u8* Buffer, u16* Cluster);
u8   ReadDiskData(u8* pBuffer, u32 ReadAddr, u16 Lenght);
u8   NextCluster(u16* Cluster);
u8   ProgFileSec(u8* Buffer, u16* Cluster);
u8   ProgDiskPage(u8* Buffer, u32 ProgAddr);
u8   SeekBlank(u8* Buffer, u16* Cluster);
u8   SetCluster(u8* Buffer, u16* Cluster);
u8   OpenFileRd(u8* Buffer, u8* FileName, u16* Cluster, u32* pDirAddr);
u8   OpenFileWr(u8* Buffer, u8* FileName, u16* Cluster, u32* pDirAddr);
u8   CloseFile(u8* Buffer, u32 Lenght, u16* Cluster, u32* pDirAddr);
void ExtFlash_PageWrite(u8* pBuffer, u32 WriteAddr, u8 Mode);
int  Init_Fat_Value(void);
#endif
/********************************* END OF FILE ********************************/
