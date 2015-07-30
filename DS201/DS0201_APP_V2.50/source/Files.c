/******************** (C) COPYRIGHT 2009 e-Design Co.,Ltd. *********************
* File Name: file.c  
     Author: bure   
   Hardware: DS0201V1.1~1.6   
    Version: V1.0
*******************************************************************************/
#include "Lcd.h"
#include "File.h"
#include "Function.h"
#include "stm32f10x_flash.h"
#include "string.h"
#include "HW_V1_Config.h"
#include "ASM_Funtion.h"

#define Page_Address    0x0801F000  

unsigned char BmpHead[54] = {0x42, 0x4D, 0x36, 0x58, 0x02, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
                             0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0xF0, 0x00,
                             0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x58, 0x02, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned int DiskStart, Hidden_Sec, Root_Addr, ClusterNum, File_Addr; //ClusterNum
unsigned short RsvdSecCnt, SectorSize, FAT_Size, FileLen; 
unsigned short SecPerClus, DirFliePtr, DirSecNum, FAT1_Addr, FAT_Ptr;//DirFlieSec, 
unsigned char  FAT16, F_Buff[512];

/*******************************************************************************
 MSD_FAT_Info: 读取SD盘基本信息   输入：NONE   返回值：0x00=成功
*******************************************************************************/
char FAT_Info(void)
{
  FAT16 =0;
  DiskStart =0;
  if(__MSD_ReadBlock(F_Buff, 0, 512)) return 0xFF; // Disk error!
  if((F_Buff[0] != 0xEB)&&(F_Buff[0] != 0xE9)){
    DiskStart =(F_Buff[0x1C9]<<24)+(F_Buff[0x1C8]<<16)
               +(F_Buff[0x1C7]<<8)+(F_Buff[0x1C6]);  
    if(__MSD_ReadBlock(F_Buff, DiskStart<<9, 512)) return 0x02; // Disk error!
  }
  SectorSize =(F_Buff[0x0C]<<8)+(F_Buff[0x0B]);  
  SecPerClus = F_Buff[0x0D];                       
  RsvdSecCnt =(F_Buff[0x0F]<<8)+(F_Buff[0x0E]);  
  Hidden_Sec =(F_Buff[0x1F]<<24)+(F_Buff[0x1E]<<16)+(F_Buff[0x1D]<<8)+(F_Buff[0x1C]);
  FAT_Size =(F_Buff[0x17]<<8)+(F_Buff[0x16]);
  if(FAT_Size) FAT16 = 1;
  else FAT_Size =(F_Buff[0x25]<<8)+(F_Buff[0x24]);
  FAT1_Addr  =(DiskStart + RsvdSecCnt)* SectorSize;
  Root_Addr = FAT1_Addr +( 2 * FAT_Size)* SectorSize;
  if(FAT16) DirSecNum = 0x4000 / SectorSize ;   // FAT 16 format
  else      DirSecNum = SecPerClus;             // FAT 32 format
  return 0; // Disk ok!
}
/*******************************************************************************
 Open_BMP_File: 打开指定图像文件   输入：文件名编号部分   返回值：0x00=成功 
*******************************************************************************/
char Open_BMP_File(unsigned char *n)
{
  unsigned short i, BuffCnt;

  for(BuffCnt =0; BuffCnt <(SectorSize/512)*DirSecNum; ++BuffCnt){
    if(__MSD_ReadBlock(F_Buff,Root_Addr+BuffCnt*512,512)) return 0x02; // Read Sector fail
    for(i=0; i<512; i+=32){                                            // i = Flie Dir Buff Ptr
      if((F_Buff[i]=='I')&&(F_Buff[i+1]=='M')&&(F_Buff[i+2]=='A')&&(F_Buff[i+3]=='G')
         &&(F_Buff[i+4]=='E')&&(F_Buff[i+5]==*n)&&(F_Buff[i+6]==*(n+1))&&(F_Buff[i+7]==*(n+2))
         &&(F_Buff[i+8]=='B')&&(F_Buff[i+9]=='M')&&(F_Buff[i+10]=='P')){
        ClusterNum =((F_Buff[i+0x15]<<24)+(F_Buff[i+0x14]<<16)
                     +(F_Buff[i+0x1B]<<8)+(F_Buff[i+0x1A]));          
        if(FAT16) File_Addr = Root_Addr +(DirSecNum +(ClusterNum -2)*SecPerClus)*SectorSize;
        else      File_Addr = Root_Addr +((ClusterNum -2)*SecPerClus)*SectorSize;
       return 0; // Open BMP File ok!
      }
    }
  }
  return 0x02;
}
/*******************************************************************************
 Writ_BMP_File: 改写已经打开的BMP文件   输入：NONE   返回值：0x00=成功 
*******************************************************************************/
char Writ_BMP_File(void)
{
  unsigned short x, y, i=54, R, G, B, Color;
  FAT_Ptr  = ClusterNum *4;
  memcpy(F_Buff, BmpHead, 54);
  for(y=0; y<240; ++y){
    for(x=0; x<320; ++x){
      Color = __Get_Pixel(x, y);
      R = Color & 0x001F;
      G = Color & 0x07E0;
      B = Color & 0xF800;
      Color =(R<<11)+ G +(B>>11); 
      F_Buff[i]  =(char)(Color & 0x00ff);
      F_Buff[i+1]=(char)(Color >>8);
      i+=2; 
      if(i>=512){                // Buffer full
        if(__MSD_WriteBlock(F_Buff, File_Addr, 512)) return 0x01;   // File Write Error
        i=0;
        File_Addr += 512;
      }
    }
  }
  if(i!=0) return __MSD_WriteBlock(F_Buff, File_Addr, 512);
  else     return 0;
}
/*******************************************************************************
 Open_DAT_File: 打开指定数据文件   输入：文件名编号部分   返回值：0x00=成功 
*******************************************************************************/
char Open_DAT_File(unsigned char *n)
{
  char Rvalue = 0xFF; 
  unsigned short i, SectorCnt;

  if(__MSD_ReadBlock(F_Buff, Root_Addr, 512)) return 0xFF;
   
  for(SectorCnt=0; SectorCnt < DirSecNum; ++SectorCnt){
    if(__MSD_ReadBlock(F_Buff, Root_Addr + SectorCnt*512, 512)) return Rvalue; 
    for(i=0; i<512; i+=32){
      if((F_Buff[i]=='F')&&(F_Buff[i+1]=='I')&&(F_Buff[i+2]=='L')&&(F_Buff[i+3]=='E')){
        if((F_Buff[i+8]=='D')&&(F_Buff[i+9]=='A')&&(F_Buff[i+10]=='T')){
          if((F_Buff[i+4]==*n)&&(F_Buff[i+5]==*(n+1))&&(F_Buff[i+6]==*(n+2))){
            ClusterNum =((F_Buff[i+0x15]<<24)+(F_Buff[i+0x14]<<16)
                        +(F_Buff[i+0x1B]<<8)+(F_Buff[i+0x1A]));         
            if(FAT16) File_Addr = Root_Addr +(DirSecNum +(ClusterNum-2)*SecPerClus)*SectorSize;
            else      File_Addr = Root_Addr +((ClusterNum -2)*SecPerClus)*SectorSize;
            return 0;
          }
        }
      }
    }
  }
  return 0xFF;
}
/*******************************************************************************
 Read_File: 读取指定文件   输入：NONE   返回值：0x00=成功 
*******************************************************************************/
char Read_File(void)
{
  return __MSD_ReadBlock(F_Buff, File_Addr, 512);
}
/*******************************************************************************
 Write_File: 写入指定文件并标上写入顺序   输入：NONE   返回值：0x00=成功 
*******************************************************************************/
char Write_File(void)
{
  char Rvalue = 0xFF;
  Rvalue =__MSD_WriteBlock(F_Buff, File_Addr, 512);
  if(Rvalue!=0) return Rvalue;
  return Rvalue;
}

/*******************************************************************************
Read_Parameter: 从FLASH读出参数域                           
*******************************************************************************/
void Read_Parameter(void)
{ 
  short i; 
  if((*((vu8 *)Page_Address+62))!=0x55) return ;
  if((*((vu8 *)Page_Address+63))!=0xAA) return ;
  if((*((vu8 *)Page_Address+0)) !=0x21) return ;
  if((*((vu8 *)Page_Address+1)) !=0x02) return ;
  for(i=2; i<42; i+=2)  Item_Index[i/2-1] =(*((vu16 *)(Page_Address+i)));
  for(i=42; i<62; ++i) Hide_Index[i-42] =(*((vu8 *)(Page_Address+i)));
  return ;
}
/*******************************************************************************
 Write_Parameter: 把参数域写入FLASH                         Return: 0= Success
*******************************************************************************/
char Write_Parameter(void)
{
  unsigned short i, j, k; 
  FLASH_Unlock();
  j=FLASH_ErasePage(Page_Address);
  if(j== FLASH_COMPLETE) {
    F_Buff[0]=0x21;
    F_Buff[1]=0x02;                      //Ver:2.21
    memcpy(F_Buff+2,  Item_Index, 40);
    memcpy(F_Buff+42, Hide_Index, 20);
    F_Buff[62]=0x55;
    F_Buff[63]=0xAA;                      
    for(i=0; i<64; i+=2){
      k=F_Buff[i]+(F_Buff[i+1]<<8);
      j= FLASH_ProgramHalfWord(Page_Address+i, k);
      if(j!= FLASH_COMPLETE) break;     
    }
  }    
  FLASH_Lock();
  return j;
}

/******************************** END OF FILE *********************************/

