/**********************(C) COPYRIGHT 2018 e-Design Co.,Ltd.*********************
  DS213_APP AppDisk.c                                    Author : bure & SNAKE
*******************************************************************************/
#include "Sys.h"

#define TXFR_IDLE      0
#define TXFR_ONGOING   1

#define W_ADDR         0
#define ADDR           1
#define H_ADDR         2
#define OFFSET         3
#define SEC_CNT        4
#define COUNT          5
#define RD_CNT         6
#define WR_CNT         7

#define USB_ST         0
#define SEG_KIND       1
#define SEG_LEN        2
#define SEG_SUM        3
#define SEG_TMP        4
#define SEG_ST         5
#define DATA_CNT       6
#define F_TYPE         7
#define F_FLAG         8
#define SEG_DATA       9              

uc8 Inquiry_Str[] = {0x00, 0x80, 0x02, 0x02, 36-4, 0x00, 0x00, 0x00,
                      'M',  'i',  'n',  'i',  'W',  'a',  'r',  'e',
                      '8',  'M',  'B',  '_',  'D',  'i',  's',  'k',   
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00};

u8   *Var, *Data; // 磁盘工作区  8 位变量指针
u32  *V32;        // 磁盘工作区 32 位变量指针
u32  SectorSize, SectorAmnt;

/*******************************************************************************
  Description  : USB Disk initial   
*******************************************************************************/
void AppDiskInit(void)
{
  SectorSize  = Hw.DiskSecSize;
  SectorAmnt  = Hw.DiskSecAmnt;
  V32         = (u32*)&DiskBuf[SectorSize+0x04];
  Var         = (u8*) &DiskBuf[SectorSize+0x24];
  Data        = (u8*) &DiskBuf[SectorSize+0x24]+SEG_DATA;
  V32[WR_CNT] = 0;
  V32[RD_CNT] = 0;
}

/*******************************************************************************
  Description  : USB Disk sector write  
*******************************************************************************/
void ProgDiskSect(u8 *pBuf, u32 Addr)
{
  Hw.pSpiFlashSecW((u32)&pBuf[0], Addr, SectorSize);
}

/*******************************************************************************
  Description  : Virtual Disk sector read   
*******************************************************************************/
void ReadDiskData(u8 *pBuf, u32 Addr, u16 n)
{
  Hw.pSpiFlashRead((u32)&pBuf[0], Addr, n);
}

/*******************************************************************************
  Description  : Handle the Read operation from the USB disk.      
*******************************************************************************/
void Read_Memory(u32 RdOffset, u32 RdLength)
{
  static u32 Offset, Length, Block;

  if(Var[USB_ST] == TXFR_IDLE){
    Offset = RdOffset*SectorSize;
    Length = RdLength*SectorSize;
    Var[USB_ST] = TXFR_ONGOING;
  }
  if(Var[USB_ST] == TXFR_ONGOING){
    if(!V32[RD_CNT]){
      ReadDiskData(DiskBuf, Offset, SectorSize);
      UserToPMABufferCopy(DiskBuf, ENDP1_TXADDR, BULK_MAX_PACKET_SIZE);
      V32[RD_CNT] = SectorSize-BULK_MAX_PACKET_SIZE;
      Block       = BULK_MAX_PACKET_SIZE;
    } else {
      UserToPMABufferCopy(DiskBuf+Block, ENDP1_TXADDR, BULK_MAX_PACKET_SIZE);
      V32[RD_CNT] -= BULK_MAX_PACKET_SIZE;
      Block       += BULK_MAX_PACKET_SIZE;
    }
    SetEPTxCount(ENDP1, BULK_MAX_PACKET_SIZE);
    SetEPTxStatus(ENDP1, EP_TX_VALID);
    Offset += BULK_MAX_PACKET_SIZE;
    Length -= BULK_MAX_PACKET_SIZE;

    CSW.dDataResidue -= BULK_MAX_PACKET_SIZE;
  }
  if(Length == 0){
    V32[RD_CNT] = 0;
    Block       = 0;
    Offset      = 0;
    Bot_State   = BOT_DATA_IN_LAST;
    Var[USB_ST] = TXFR_IDLE;
  }
}
/*******************************************************************************
  Description  : Handle the Write operation to the USB disk.  
*******************************************************************************/
void Write_Memory(u32 WrOffset, u32 WrLength)
{
  static u32 Offset, Length;
  u32 Idx, temp = V32[WR_CNT]+64;
  if(Var[USB_ST] == TXFR_IDLE){
    Offset = WrOffset*SectorSize;
    Length = WrLength*SectorSize;
    Var[USB_ST] = TXFR_ONGOING;
  }
  if(Var[USB_ST] == TXFR_ONGOING){
    for(Idx = 0; V32[WR_CNT] < temp; V32[WR_CNT]++) 
      *(u8*)(DiskBuf+V32[WR_CNT]) = Bulk_Buff[Idx++];
    Offset += Data_Len;
    Length -= Data_Len;
    if(!(Length % SectorSize)){
      V32[WR_CNT] = 0;
      ProgDiskSect(DiskBuf, Offset-SectorSize);
    }
    CSW.dDataResidue -= Data_Len;
    SetEPRxStatus(ENDP2, EP_RX_VALID);
  }
  if(Length == 0 || Bot_State == BOT_CSW_Send){
    V32[WR_CNT] = 0;
    Set_CSW(CSW_CMD_PASSED, SEND_CSW_ENABLE);
    Var[USB_ST] = TXFR_IDLE;
  }
}
/*********************************  END OF FILE  ******************************/
