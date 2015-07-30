/******************** (C) COPYRIGHT 2014 e-Design Co.,Ltd. *********************
Project Name: DS202 V1.0		
* File Name:  Files.c       Hardware: DS202 V2.1                     Author: xie
*******************************************************************************/
#include <stdlib.h>
#include "Version.h"
#include "string.h"
#include "STM32F30x.h"
#include "stm32f30x_flash.h"
#include "Draw.h"
#include "Func.h"
#include "File.h"
#include "FAT12.h"
#include "Menu.h"
#include "Process.h"
#include "Flash.h"
#include "Func.h"
#include "Drive.h"
#include "Bios.h"
#include "Lcd.h"

#define Page_Address    0x08007800   
#define Kpg_Address     0x08007800  /*保存校准后的Kpg[]零点偏移和增益系数 */

u8   Cal_Flag   = 1;
u8   Rest_Flag  = 0;
u8   Save_Flag  = 0;
u8   About_Flag = 0;
u8*  NamePtr;
u16  BmpNum;
u16  BMP_Ag_Color[16];

uc8  BmpHead[54]   = {0X42, 0X4D, 0X76, 0X96, 0X00, 0X00, 0X00, 0X00,
                      0X00, 0X00, 0X76, 0X00, 0X00, 0X00, 0X28, 0X00,
                      0X00, 0X00, 0X40, 0X01, 0X00, 0X00, 0XF0, 0X00,
                      0X00, 0X00, 0X01, 0X00, 0X04, 0X00, 0X00, 0X00,
                      0X00, 0X00, 0X82, 0X0B, 0X00, 0X00, 0X12, 0X0b,
                      0X00, 0X00, 0X12, 0X0b, 0X00, 0X00, 0X00, 0X00,
                      0X00, 0X00, 0X00, 0X00, 0X00, 0X00};

uc16 CLK_TAB[44] ={
   0x000,0x000,0x000,0x000,0x070,0x3FF,0x070,0x000,0x000,0x000,0x000,
   0x000,0x002,0x004,0x008,0x070,0x070,0x070,0x080,0x100,0x200,0x000,
   0x020,0x020,0x020,0x020,0x070,0x070,0x070,0x020,0x020,0x020,0x020,
   0x000,0x200,0x100,0x080,0x070,0x070,0x070,0x008,0x004,0x002,0x000};

uc16 BMP_Color[16] = {WHT,  CYN, CYN_,  YEL,  YEL_, PUR, PUR_, GRN,
                      GRN_, GRY, ORN, BLU, RED, BLK, LGN,  DAR};

u8   FileBuff[910];
u8   F_Sector[128];
u16  RsvdSecCnt, SectorSize, FAT_Size, FileLen;
u16  SecPerClus, DirFliePtr, DirSecNum, FAT1_Addr, FAT_Ptr;
u32  DiskStart, Hidden_Sec, Root_Addr, ClusterNum, File_Addr;
u32* FilePtr;

extern u8 DiskBuf[4096];

/*******************************************************************************
Color_Num: 求出当前颜色的对应调色板编号
*******************************************************************************/
u8 Color_Num(u16 Color)
{
  if(Color == WHT)                    return 0;
  else if((Color & CYN  )== CYN  )    return 1;
  else if((Color & CYN_ )== CYN_ )    return 2;
  else if((Color & YEL  )== YEL  )    return 3;
  else if((Color & YEL_ )== YEL_ )    return 4;
  else if((Color & PUR  )== PUR  )    return 5;
  else if((Color & PUR_ )== PUR_ )    return 6;
  else if((Color & GRN  )== GRN  )    return 7;
  else if((Color & GRN_ )== GRN_ )    return 8;
  else if((Color & GRY  )== GRY  )    return 9;
  else if((Color & ORN  )== ORN  )    return 10;
  else if((Color & BLU  )== BLU  )    return 11;
  else if((Color & RED  )== RED  )    return 12;
  else if((Color & BLK  )== BLK  )    return 13;
  else if((Color & LGN  )== LGN  )    return 14;
  else                                return 15;
}
/*******************************************************************************
 打开指定扩展名的文件            输入：文件扩展名             返回值：0x00=成功
*******************************************************************************/
void Make_Filename(s16 FileNo,unsigned char* FileName)
{
  u8 Num[4];
  u8ToDec3Str(Num, FileNo);
  FileName[4] = Num[0];
  FileName[5] = Num[1];
  FileName[6] = Num[2];
}

/*******************************************************************************
Save_Dat: 保存当前屏幕显示图像原始数据    输入：文件编号     返回值：0x00=成功
*******************************************************************************/
u8 Save_Dat(s16 FileNo)
{
  u8  pFileName[12]="FILE    DAT";
  u16 i;
  u16 Rvalue=DISK_RW_ERR ;
  u16 pCluster[3];
  u32 pDirAddr[1];
  Make_Filename(FileNo, pFileName);
  memset(DiskBuf,0,4096);
  if(OpenFileWr(DiskBuf, pFileName, pCluster, pDirAddr)!=OK) return Rvalue;
  DiskBuf[0]=PopMenu1_Value[CH1_Posi];
  DiskBuf[1]=PopMenu1_Value[CH2_Posi];
  DiskBuf[2]=PopMenu1_Value[CH3_Posi];
  DiskBuf[3]=PopMenu1_Value[TIM_Base];
  DiskBuf[4]=PopMenu1_Value[CH1_Vol];
  DiskBuf[6]=PopMenu1_Value[CH2_Vol];
  
  for(i=0; i<3; i++){
    memcpy(DiskBuf+8+300*i, V_Buf+TR1_pBUF+300*i, 300);
  } 
  if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
  if(CloseFile(DiskBuf, 512*2, pCluster, pDirAddr)!= OK) return FILE_RW_ERR;
  if(PopMenu3_Value[SAVE_Dat]<99)PopMenu3_Value[SAVE_Dat]++;
  return OK;
}
/*******************************************************************************
Load_Dat: 加载保存过的屏幕图像原始数据    输入：文件编号     返回值：0x00=成功
*******************************************************************************/
u8 Load_Dat(s16 FileNo)
{
  u8 pFileName[12]="FILE    DAT";
  u16 pCluster[3];
  u32 pDirAddr[1];
  u16  Rvalue=0;
  u16 i;
  Make_Filename(FileNo, pFileName);
  if((Rvalue=OpenFileRd(DiskBuf, pFileName, pCluster, pDirAddr))!= OK) 
    return Rvalue;
  if(ReadFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;
  for(i=0; i<3; i++){
    memcpy(FileBuff+300*i, DiskBuf+300*i, 300);
  }
  PopMenu1_Value[TIM_Base] = DiskBuf[3];
  PopMenu1_Value[CH1_Vol]  = DiskBuf[4];
  PopMenu1_Value[CH2_Vol]  = DiskBuf[6];
  if(PopMenu3_Value[LOAD_Dat]<99)PopMenu3_Value[LOAD_Dat]++;
  return OK;
}

/*******************************************************************************
Save_Bmp: 保存当前屏幕显示图像为BMP格式    输入：文件编号     返回值：0x00=成功
*******************************************************************************/
u8 Save_Bmp(s16 FileNo)
{
  u8  pFileName[12]="IMG_    BMP";
  u16 pCluster[3];
  u32 pDirAddr[1];
  u32 Rvalue=DISK_RW_ERR;
  s16 x, y, i=54, j,ColorH,ColorL ;
  u16 k=0, l=0;
  u32 length=4096;

  __USB_Port(DISABLE);
  Make_Filename(FileNo, pFileName);
  if(OpenFileWr(DiskBuf, pFileName, pCluster, pDirAddr)!=OK) return Rvalue;
  memcpy(DiskBuf, BmpHead, 54);
  i = 0x0036;                                     // 调色板存放开始地址
  for(j=0; j<16; ++j){
    DiskBuf[j*4 +i+0]=(BMP_Color[j] & 0xF800)>>8;  // Bule
    DiskBuf[j*4 +i+1]=(BMP_Color[j] & 0x07E0)>>3;  // Green&
    DiskBuf[j*4 +i+2]=(BMP_Color[j] & 0x001F)<<3;  // Red
    DiskBuf[j*4 +i+3]= 0;                          // Alpha
  }
  i = 0x0076;                                     // 图像数据开始存放地址
  for(y=0; y<240; y++){
    for(x=0; x<320 ; x+=2){
      __SetPosi(x, y); 
      ColorH = __ReadPixel();
      __SetPosi(x+1, y); 
      ColorL = __ReadPixel();
      DiskBuf[i] =(Color_Num(ColorH)<<4)+ Color_Num(ColorL);
      i++;
      if(i>=length){
        i=0;
        if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
        if(l==0)PrintClk(168,2,(k++ >>1)& 3);         // 进度指示
        l++;
        if(l>=2)l=0;
      }
    }
  }
  if(i!=0){
      if(ProgFileSec(DiskBuf, pCluster)!= OK){List_Pop();return FILE_RW_ERR;} 
    }
  if(CloseFile(DiskBuf, 76*512, pCluster, pDirAddr)!= OK) return FILE_RW_ERR;
  if(PopMenu3_Value[SAVE_Bmp]<99)PopMenu3_Value[SAVE_Bmp]++;
  __USB_Port(ENABLE);
  return OK;
}

/*******************************************************************************
Open_Bmp: 在屏幕显示图像为BMP格式    输入：文件编号     返回值：0x00=成功
*******************************************************************************/
u8 Open_Bmp(u8 FileNum)
{
    u8  Filename[12] = "ABOUT_F BMP";
    u16 width,high; 
    u16 i,j, x=10, y=20;
    u16 pCluster[3];
    u32 pDirAddr[1]; 
     
    if(OpenFileRd(DiskBuf, Filename, pCluster, pDirAddr)!=OK) {
      About_Pop(" About.file is Error ", 105, 20,16, 23*6 );
      About_Flag = 0;
      return FILE_RW_ERR;
    }
     if(ReadFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;
     i = 0x0036;                              //调色板存放开始地址
     for(j=0; j<16; j++){
       BMP_Ag_Color[j]=((DiskBuf[j*4 +i+0]<<8)&0xF800)
                      |((DiskBuf[j*4 +i+1]<<3)&0x07E0)
                      |((DiskBuf[j*4 +i+2]>>3)&0x001F);
     }
     width = DiskBuf[0x12];
     high =  DiskBuf[0x16];
     if((width == 250) &&(high == 200) ){
       Clear_Label_L(DAR);                   //清楚左栏通道小图标
       Draw_RECT(CYN, 7, 20, 203, 254, 2 );  //画空矩形
       i = 0x0076;                           //图像数据开始存放地址
       for(y=22; y<199+22; y++){
         for(x=9; x<256+9 ; x+=2){
           if(x>249+9);
           else{
             __SetPosi(x, y);
             __SetPixel(BMP_Ag_Color[DiskBuf[i]>>4]);
             __SetPosi(x+1, y);
             __SetPixel(BMP_Ag_Color[DiskBuf[i]-(DiskBuf[i]>>4)*16]);
           }
           i++;
           if(i>=4096){
             if(ReadFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;
             i=0;
           }
         }
       }       
       
     }else {
       About_Pop(" About.Bmp is Error ", 105, 20,16, 22*6 );
       About_Flag = 0;
     }
  return 0;
}

/*******************************************************************************
Save_Buf: 保存采集数据缓存区为BUF格式    输入：文件编号     返回值：0x00=成功
*******************************************************************************/
u8 Save_Buf(s16 FileNo)
{
  u8 pFileName[12]="DATA_   BUF";
  u16 pCluster[3];
  u32 pDirAddr[1];
  u32 i=0, Rvalue=DISK_RW_ERR;
  u16 *p;
  u8  l=0;
  u16 n,k;

  Make_Filename(FileNo, pFileName);
  memset(DiskBuf,0,4096);
  if(OpenFileWr(DiskBuf, pFileName, pCluster, pDirAddr)!=OK) return Rvalue;
  n=4; k=4096;
  
  for(i=0; i<n; i++){ 
    if(PopMenu1_Value[WIN_Depth]==3)
    {
      if(i<4)memcpy(DiskBuf,&(Smpl[i*k/2]),k);
      else   memcpy(DiskBuf,&(Smpl[i*k/2+4096]),k);
    }else{
      if(i<2)memcpy(DiskBuf,&(Smpl[i*k/2]),k);
      else   memcpy(DiskBuf,&(Smpl[i*k/2+4096]),k);
    }
    if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
    PrintClk(264,2,(l++ >>1) & 3);                              // 进度指示
  }
  memset(DiskBuf,0,k);
  p =(u16*)&DiskBuf;
  *p++=Smpl[0];
  *p++=Smpl[1];
  *p++=PopMenu1_Value[CH1_Vol];
  *p++=PopMenu1_Value[CH2_Vol];
  *p++=PopMenu1_Value[WIN_Depth];
  *p++=PopMenu1_Value[TIM_Base];
  if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
  if(CloseFile(DiskBuf, 34*512, pCluster, pDirAddr)!= OK) return FILE_RW_ERR;
  if(PopMenu3_Value[SAVE_Buf]<99)PopMenu3_Value[SAVE_Buf]++;
  return OK;
}

/*******************************************************************************
Load_Buf: 加载保存过的采集数据缓冲区    输入：文件编号     返回值：0x00=成功
*******************************************************************************/
u8 Load_Buf(s16 FileNo)
{
  u8 pFileName[12]="DATA    BUF";
  u16 pCluster[3];
  u32 pDirAddr[1];
  u16 *ptr;
  u32 i,Rvalue=1;
  u32 n,k;

  Make_Filename(FileNo, pFileName);
  if( (Rvalue=OpenFileRd(DiskBuf, pFileName, pCluster, pDirAddr))!= OK) 
    return Rvalue;
  n=4;k=4096;
  for(i=0;i<n;i++){
    if(ReadFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;
    if(PopMenu1_Value[WIN_Depth]==3)
    {
      if(i<4)memcpy(&(Smpl[i*k/2]),DiskBuf,k);
      else   memcpy(&(Smpl[i*k/2+4096]),DiskBuf,k);
    }else{
      if(i<2)memcpy(&(Smpl[i*k/2]),DiskBuf,k);
      else   memcpy(&(Smpl[i*k/2+4096]),DiskBuf,k);
    }
  }
  
  if(ReadFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;
  ptr=(u16*)F_Sector;                               //保存现场
  *ptr++=0xaa55;
  *ptr++=Smpl[0];
  *ptr++=Smpl[1];
  *ptr++=PopMenu1_Value[CH1_Vol];
  *ptr++=PopMenu1_Value[CH2_Vol];
  *ptr++=PopMenu1_Value[WIN_Depth];
  *ptr++=PopMenu1_Value[TIM_Base];  			
  ptr =(u16*)DiskBuf;                              //读原来的参数
  Smpl[0]=*ptr++;
  Smpl[1]=*ptr++;
  PopMenu1_Value[CH1_Vol]   = *ptr++;
  PopMenu1_Value[CH2_Vol]   = *ptr++;
  PopMenu1_Value[WIN_Depth] = *ptr++;
  PopMenu1_Value[TIM_Base] = *ptr++;
  Status |=  STOP;
  Update_Status();
  if(PopMenu3_Value[LOAD_Buf]<99)PopMenu3_Value[LOAD_Buf]++;
  return 0;
}


void make_Vertical(u8 TRACK,u8* buf,u8* len)
{
  u8 i=0;
  u8* ptr;

  ptr = (u8*)&Vol_Str[PopMenu1_Value[TRACK]];
  while(*ptr != 0){
    if(*ptr == 0x21)  buf[i] = 0x20;
    else              buf[i] = *ptr;
    ptr++;
    i++;
  };
  buf[i]   = 0x2c;
  buf[i+1] = 0;
  *len = i+1;
}
/*******************************************************************************
Save_Csv: 保存采集数据缓存区为CSV格式    输入：文件编号     返回值：0x00=成功
*******************************************************************************/
u8 Save_Csv(s16 FileNo)
{
  s32 Ak = (KgA[KindA]*4)/GK[GainA+5*KindA];
  s32 Bk = (KgB[KindB]*4)/GK[GainB+5*KindB]; // 8192~409
  
  u8 pFileName[12]="DATA    CSV";
  u16 pCluster[3];
  u32 pDirAddr[1];
  u32 i,j,k=0,l=0,Rvalue=DISK_RW_ERR,length;
  u8 Num[4],track[4];
  s16 temp;
  u8 count;
  
  length=4096;
  
  Make_Filename(FileNo, pFileName);
  if(OpenFileWr(DiskBuf, pFileName, pCluster, pDirAddr)!=OK) return Rvalue;
  memcpy(DiskBuf,"CH1",3);
  make_Vertical(CH1_Vol, &DiskBuf[3],&count);
  k=3+count;
  memcpy(&DiskBuf[k],"CH2",3);
  make_Vertical(CH2_Vol, &DiskBuf[k+3],&count);
  k=k+3+count;
  memcpy(&DiskBuf[k], "CH3\r\n", 4);
  k += 4;

  for(i=0; i<DEPTH[PopMenu1_Value[WIN_Depth]]; i++){
    temp = ((((Smpl[i])-2048)*Ak)>>12)+100;          // 计算当前波形点的基值
    if(temp > 0){
      if(temp > 200)  track[0] = 199;
      else            track[0] = temp;
    } else            track[0] = 0;
    temp = ((((Smpl[i+8192])-2048)*Bk)>>12)+100;     // 计算当前波形点的基值
    if(temp > 0){
      if(temp > 200)  track[1] = 199;
      else            track[1] = temp;
    } else            track[1] = 0;
    
   for(j=0; j<2; j++){
    u8ToDec3Str(Num,track[j]);
    for(count=0; count<3; count++){
      if(Num[count] == 0) break;
      DiskBuf[k++] = Num[count];
      if(k >= length){
        if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
        PrintClk(264,2,(l++ >>1) & 3);                              // 进度指示
        k = 0;
      }
    }
    DiskBuf[k++] = 0x2c;
    if(k >= length){
      if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
      PrintClk(264,2,(l++ >>1)& 3);                               // 进度指示
      k = 0;
    }
   }
    DiskBuf[k++] = 0x0d;
    if(k >= length){
      if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
      PrintClk(264,2,(l++ >>1)& 3);                               // 进度指示
      k = 0;
    }
    DiskBuf[k++] = 0x0a;
    if(k >= length){
      if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
      PrintClk(264,2,(l++ >>1)& 3);                               // 进度指示
      k = 0;
    }
  }
  if(k != 0){
    DiskBuf[k++]=0x0d;
    DiskBuf[k++]=0x0a;
    memset(&DiskBuf[k],0,(length-k));
    k=0;
    if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
    PrintClk(264,2,(l++ >>1)& 3);                               // 进度指示
  }
  if(CloseFile(DiskBuf, l*length, pCluster, pDirAddr)!= OK) 
    return DISK_RW_ERR;
  if(PopMenu3_Value[SAVE_Csv]<99)PopMenu3_Value[SAVE_Csv]++;
  return OK;
}
/*******************************************************************************
 Save_Parameter: 保存当前的工作参数 到Flash                  Return: 0= Success
*******************************************************************************/
u8 Save_Parameter(void)           
{
  unsigned short i, j;
  u16* ptr;

  FLASH_Unlock();
  j=FLASH_ErasePage(Page_Address);
  if(j== FLASH_COMPLETE){
    ptr=(u16*)&DiskBuf;
    *ptr++=0x0300;                              //Ver:3.00
    *ptr++=menu.current;
    *ptr++=Status;
    for(i=0;i<10;i++) *ptr++=menu.menu_index[i];
    for(i=0;i<33;i++) *ptr++=PopMenu1_Value[i];
    for(i=0;i<18;i++) *ptr++=PopMenu2_Value[i];
    for(i=0;i<17;i++) *ptr++=PopMenu3_Value[i];
    for(i=0;i<8;i++)  *ptr++=Kpg[i];             //88+3
    *ptr++=0xaa55;
    ptr=(u16*)&DiskBuf;
    for(i=0; i<256; i+=2){                       //128 SHORT
      j= FLASH_ProgramHalfWord(Page_Address+i, *ptr++);
      if(j!= FLASH_COMPLETE) break;
    }
  }
  FLASH_Lock();
  return j;
}

/*******************************************************************************
Read_Parameter: 从FLASH读出参数域
*******************************************************************************/
void Read_Parameter(void)  //从FLASH读出参数域
{
  u8 i;
  u16* ptr;
  ptr=(u16*)(Page_Address+89*2);
  if(*ptr!=0xaa55) return ;
  ptr=(u16*)Page_Address;
  if(*ptr++!=0x0300) return ;
  menu.current=*ptr++;
  ptr++;
  for(i=0;i<10;i++) menu.menu_index[i]=*ptr++;
  for(i=0;i<33;i++) PopMenu1_Value[i] =*ptr++;
  for(i=0;i<18;i++) PopMenu2_Value[i] =*ptr++;
  for(i=0;i<17;i++) PopMenu3_Value[i] =*ptr++;

  return ;
}
/*******************************************************************************
 Save_Kpg: 保存校准参数                          Return: 0= Success
*******************************************************************************/
u8 Save_Kpg(void)
{
  unsigned short i, j;
  u16* ptr;

  FLASH_Unlock();
  j=FLASH_ErasePage(Kpg_Address);
  if(j== FLASH_COMPLETE){
    ptr=(u16*)&DiskBuf;
    *ptr++=0x0300;                              //Ver:3.00
    *ptr++=Cal_Flag;
    for(i=0;i<8;i++)  *ptr++=Kpg[i];
    *ptr++=0xaa55;
    ptr=(u16*)&DiskBuf;
    for(i=0; i<256; i+=2){                      //128 SHORT
      j= FLASH_ProgramHalfWord(Kpg_Address+i, *ptr++);
      if(j!= FLASH_COMPLETE) break;
    }
  }
  FLASH_Lock();
  return j;
}
/*******************************************************************************
Read_Kpg: 从FLASH读出校准参数
*******************************************************************************/
void Read_Kpg(void)
{
  u8 i;
  u16* ptr;
  ptr=(u16*)(Kpg_Address+10*2);
  if(*ptr!=0xaa55) return ;
  ptr=(u16*)Kpg_Address;
  if(*ptr++!=0x0300) return ;
  Cal_Flag = *ptr++;
  for(i=0;i< 4;i++) Kpg[i] = *ptr++;  //CH_A,CH_B 零点偏移
  return ;
}
/*******************************************************************************
Read_Kpg: 从FLASH读出校准参数
*******************************************************************************/
void Read_CalFlag(void)
{
  u16* ptr;
  ptr=(u16*)(Kpg_Address+10*2);
  if(*ptr!=0xaa55) return ;
  ptr=(u16*)Kpg_Address;
  if(*ptr++!=0x0300) return ;
  Cal_Flag = *ptr++;
  return ;
}

/*******************************************************************************
 Print_Clk: 进度指示
*******************************************************************************/
void PrintClk(u16 x0, u16 y0, u8 Phase)
{
  u16 i, j, b;
  x0=272;
  SetColor(DAR, WHT);
  DispStr(x0-20, 2, PRN, "          ");
  for(i=0; i<11; ++i){
    b = CLK_TAB[Phase *11 + i];
    for(j=0; j<11; ++j){
      __SetPosi((x0+j), (y0+1+i));
      if((b >> j)& 0x001)
        __SetPixel(RED);
      else
        __SetPixel(Background);
    }
  }
}
/*******************************************************************************
 Save_Parameter: 保存当前的工作参数                         Return: 0= Success
*******************************************************************************/
u8 Save_Param(void)             // 保存工作参数表文件 到U盘
{
  u8  Sum = 0, Filename[12], Versions = 0x06; 
  u16 i, Tmp[2];
  u16* ptr =(u16*)DiskBuf;
  
  u16 pCluster[3];
  u32 pDirAddr[1]; 

  Word2Hex(Filename, __Info(DEV_SN));
  Filename[8] = 'P'; Filename[9] = 'A'; Filename[10] = 'R';
  switch (OpenFileRd(DiskBuf, Filename, pCluster, pDirAddr)){
  case OK:                                                     //原WPT文件存在
    Tmp[0] = *pCluster;
    Filename[8] = 'B'; Filename[9] = 'A'; Filename[10] = 'K';  //转成BAK文件
    if(OpenFileWr(DiskBuf, Filename, pCluster, pDirAddr)!= OK)
      return DISK_RW_ERR;
    if(ReadFileSec(DiskBuf, Tmp     )!= OK) return FILE_RW_ERR;
    if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;//保存BAK文件
    if(CloseFile(DiskBuf, 512, pCluster, pDirAddr)!= OK) 
      return FILE_RW_ERR;
  case NEW:                                                    //原WPT文件不存在
    Filename[8] = 'P'; Filename[9] = 'A'; Filename[10] = 'R';  //创建WPT文件
    if(OpenFileWr(DiskBuf, Filename, pCluster, pDirAddr)!= OK) 
      return DISK_RW_ERR;
    memset(DiskBuf, 0, 512);
    *ptr++ =(menu.current <<8)+ Versions;           //保存参数表版本号及当前页
    *ptr++=Status;
    for(i=0;i<10;i++) *ptr++=menu.menu_index[i];
    for(i=0;i<33;i++) *ptr++=PopMenu1_Value[i];
    for(i=0;i<18;i++) *ptr++=PopMenu2_Value[i];
    for(i=0;i<17;i++) *ptr++=PopMenu3_Value[i];

    for(i=0; i<511; i++)  Sum += DiskBuf[i];  // 计算参数表校验和
    DiskBuf[511] = (~Sum)+ 1;
    if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR; // 写入数据
    if(CloseFile(DiskBuf, 512, pCluster, pDirAddr)!= OK) 
      return FILE_RW_ERR;
    
    Save_Flag = 1; //保存参数标志位
    
    return OK;
    default:  return FILE_RW_ERR;
  }
}
/*******************************************************************************
 Save_One_Param: 保存当前的工作参数                         Return: 0= Success
*******************************************************************************/
u8 Save_One_Param(u8 Page, u8 Num)             // 保存工作参数表文件 到U盘
{
  u8   Sum = 0, Filename[12], Versions = 0x06; 
  u16  i,Tmp[2];
  u16* ptr =(u16*)DiskBuf;
  s16  Temp[2];
  
  u16 pCluster[3];
  u32 pDirAddr[1]; 
  
  
  switch(Page){                           //读取之前保存的参数
    case Oscillo:
      Temp[0] = PopMenu1_Value[Num]; 
      if(Load_Param())Restore_OrigVal();  //如果保存参数则读取，没有恢复出厂参数
      else PopMenu1_Value[Num] = Temp[0]; //保存要单独保存的参数
      break;
    case Measure:
      Temp[0] = PopMenu2_Value[Num];
      if(Load_Param())Restore_OrigVal();
      else PopMenu2_Value[Num] = Temp[0];
      break;
    case Option:
      Temp[0] = PopMenu3_Value[Num];
      if(Load_Param())Restore_OrigVal();
      else {
        PopMenu3_Value[Num] = Temp[0];
         //恢复出厂没保存时，Bmp序号为未恢复前序号；
        if((Rest_Flag==1) && (Save_Flag==0))
        PopMenu3_Value[SAVE_Bmp] = BmpNum;
      }
      break;
    }

  Word2Hex(Filename, __Info(DEV_SN));
  Filename[8] = 'P'; Filename[9] = 'A'; Filename[10] = 'R';
  switch (OpenFileRd(DiskBuf, Filename, pCluster, pDirAddr)){
  case OK:                                                     // 原WPT文件存在
    Tmp[0] = *pCluster;
    Filename[8] = 'B'; Filename[9] = 'A'; Filename[10] = 'K';  // 转成BAK文件
    if(OpenFileWr(DiskBuf, Filename, pCluster, pDirAddr)!= OK)
      return DISK_RW_ERR;
    if(ReadFileSec(DiskBuf, Tmp     )!= OK) return FILE_RW_ERR;
    if(ProgFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;  // 保存BAK文件
    if(CloseFile(DiskBuf, 512, pCluster, pDirAddr)!= OK) 
      return FILE_RW_ERR;/**/
  case NEW:                                                   // 原WPT文件不存在
    Filename[8] = 'P'; Filename[9] = 'A'; Filename[10] = 'R'; // 创建WPT文件
    if(OpenFileWr(DiskBuf, Filename, pCluster, pDirAddr)!= OK)
      return DISK_RW_ERR;
    memset(DiskBuf, 0, 512);
    
    *ptr++ =(menu.current <<8)+ Versions;           // 保存参数表版本号及当前页
    *ptr++=Status;
    for(i=0;i<10;i++) *ptr++=menu.menu_index[i];
    for(i=0;i<33;i++) *ptr++=PopMenu1_Value[i];
    for(i=0;i<18;i++) *ptr++=PopMenu2_Value[i];
    for(i=0;i<17;i++) *ptr++=PopMenu3_Value[i];
    
    for(i=0; i<511; i++)  Sum += DiskBuf[i];  // 计算参数表校验和
    DiskBuf[511] = (~Sum)+ 1;
    if(ProgFileSec(DiskBuf, pCluster)!= OK) 
      return FILE_RW_ERR; // 写入数据
    if(CloseFile(DiskBuf, 512, pCluster, pDirAddr)!= OK) 
      return FILE_RW_ERR;
    return OK;
    default:  return FILE_RW_ERR;
  }
}
/*******************************************************************************
Load_Parameter: 加载之前的工作参数                           Return: 0= Success
*******************************************************************************/
u8 Load_Param(void)    //加载之前的工作参数 从U盘
{ 
  u8  Sum = 0, Filename[12], Versions = 0x06; 
  u16 i,rc;
  u16* ptr =(u16*)DiskBuf;
  
  u16 pCluster[3];
  u32 pDirAddr[1]; 
  
  Word2Hex(Filename, __Info(DEV_SN));
  Filename[8] = 'P'; Filename[9] = 'A'; Filename[10] = 'R'; Filename[11] = 0; 
  rc = OpenFileRd(DiskBuf, Filename, pCluster, pDirAddr);
  if(rc != OK) return FILE_RW_ERR;

  else
  {
    if(ReadFileSec(DiskBuf, pCluster)!= OK) return FILE_RW_ERR;
    if(Versions !=(*ptr & 0xFF)) return VER_ERR;  // 版本出错返回
    for(i=0; i<512; ++i) Sum += DiskBuf[i];
    if(Sum != 0) return SUM_ERR;                  // 校验和出错返回
    
    menu.current =(*ptr++ >>8);                   // 加载之前的 Current Title
    ptr++;
    for(i=0;i<10;i++) menu.menu_index[i]=*ptr++;
    for(i=0;i<33;i++) PopMenu1_Value[i] =*ptr++;
    for(i=0;i<18;i++) PopMenu2_Value[i] =*ptr++;
    for(i=0;i<17;i++) PopMenu3_Value[i] =*ptr++;
  
  return rc;
 }
}

/*******************************************************************************
Restore_OrigVal     恢复出厂参数
*******************************************************************************/
void Restore_OrigVal(void)
{
    u8 i;
    Rest_Flag = 1;
    BmpNum = PopMenu3_Value[SAVE_Bmp];
    menu.current = Oscillo;
    for(i=0;i<3;i++)  menu.menu_index[i]=0;
    for(i=0;i<33;i++) PopMenu1_Value[i] =PopMenu1_CValue[i];
    for(i=0;i<18;i++) PopMenu2_Value[i] =PopMenu2_CValue[i];
    for(i=0;i<17;i++) PopMenu3_Value[i] =PopMenu3_CValue[i];
}
/******************************** END OF FILE *********************************/

