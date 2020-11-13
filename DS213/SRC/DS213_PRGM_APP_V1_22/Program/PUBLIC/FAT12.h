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
    u32 FAT1_BASE;          // FAT1����ʼ��ַ
    u32 FAT2_BASE;          // FAT2����ʼ��ַ
    u32 ROOT_BASE;          // ��Ŀ¼��ʼ��ַ
    u32 FILE_BASE;          // �ļ�����ʼ��ַ
    u32 FAT_LEN;
    u32 SEC_LEN;            // ��������
    u32 FAT_END;            // ���ӽ���
    u8  FAT1_SEC;           // FAT1������
    u8  FAT2_SEC;
  }FAT_ParamDef;

  //-------FLASH-----W25Q64BV---------------------------------------------------

  #define FILE_BASE_8M    0x7000     /*�ļ�����ʼ��ַ */
  #define ROOT_BASE_8M    0x3000     /* ��Ŀ¼��ʼ��ַ*/
  #define FAT_LEN_8M      0x1000
  #define FAT1_BASE_8M    0x1000     /* FAT1����ʼ��ַ*/
  #define FAT2_BASE_8M    0x2000     /* FAT2����ʼ��ַ*/
  #define SEC_LEN_8M      0x1000     /* �������� */
  #define FAT1_SEC_8M     0x1        /* FAT1������*/
  #define FAT2_SEC_8M     0x1        /* FAT2������*/
  #define FAT_END_8M      0x7FF      /* ���ӽ���*/

  #define OK              0          /* �������*/
  #define SEC_ERR         1          /* ������д����*/
  #define FAT_ERR         2          /* FAT���д����*/
  #define OVER            3          /* �������*/
  #define NEW             4          /* �հ�/��Ŀ¼��*/
  #define SUM_ERR         6          /* У��ʹ���*/

  #define OW              0          /* ��д(���ݴ�0��1��д)*/
  #define RW              1          /* ��д*/

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
