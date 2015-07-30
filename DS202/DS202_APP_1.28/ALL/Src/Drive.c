/********************* (C) COPYRIGHT 2014 e-Design Co.,Ltd. ********************
  Brief   : 底层硬件配置  
 Version  : DS202 Ver 1.0                                       Author : bure 
*******************************************************************************/
#include "TouchPad.h"
#include "Version.h"
#include "usb_lib.h"
#include "Process.h"
#include "Drive.h"
#include "Draw.h"
#include "Menu.h"
#include "Func.h"
#include "Bios.h"
#include "Disk.h"
#include "LCD.h"

u16  KeyIn,  Vbat, Ichg;
s16  PopCnt, MenuCnt;
vu32 WaitCnt, Cnt_mS, SysCnt;
u8   StateA, StateB, CouplA, CouplB, GainA, GainB, KindA, KindB, Twink;
u16  Tmp_PWR = 0, Tmp_RUN = 0;
u8   Key_UPD = 0;

extern u8  Menu_Temp[5];

/*******************************************************************************
 Wait_mS: 延时程序。 输入: 延时的毫秒值（在72MHz主频情况下）
*******************************************************************************/
void Wait_mS(u16 mS) // 输入值为 0~65536 mS
{
  WaitCnt = mS;
  while(WaitCnt){};
}
/*******************************************************************************
 Ctp_ISP:  Capacitor Touch Pad 中断处理程序 
*******************************************************************************/
void Ctp_ISP(void)
{
  Read_TP();
}
/*******************************************************************************
 SysTick_ISP:  SysTick 定时中断处理程序 
*******************************************************************************/
void SysTick_ISP(void)
{
  if(PwrCnt ) PwrCnt--;
  if(WaitCnt) WaitCnt--;
  if((++SysCnt%20 )== 0){
    if(Key_Wait_Cnt)    Key_Wait_Cnt--;
    if(Key_Repeat_Cnt)  Key_Repeat_Cnt--;
    Key_UPD = 1;        //按键扫描标志
    if(BeepCnt > 40) BeepCnt-= 40;
    else {      
      __Ctrl(BUZZ_ST, DISABLE);
    }
    if(PwrCnt  > 20) PwrCnt -= 20;
    else             PwrCnt  = 0; 
    if(SysCnt == 1000)SysCnt = 0;  
  }
}

/*******************************************************************************
TIM6_ISP:  定时中断处理程序  1ms
*******************************************************************************/
void TIM6_ISP(void)
{
  u16 i,Vusb;
  
  if((++Cnt_mS%20 )== 0){
    if(PopCnt > 40) PopCnt -= 20;
    else if(PopCnt > 20) {
      PopCnt = 0;
      if(!(PopType & (DAILOG_POP|PWR_POP))){
        if(PopType & FILE_POP){ 
                              //如果是文件管理子窗口，恢复打开时的当前页和选项
          menu.current = Menu_Temp[0];
          menu.menu_index[menu.current] = Menu_Temp[1];
        }
        ClosePop();           //DAILOG_POP对话框不自动关闭
      }
      Windows_Flag = 0;      //关闭windows
      if(Windows_Flag == 0){ //清除小窗口 后期放到定时器处理
        Clr_WavePosi(DAR);
      }
    }
    //------------电量检测------------------
    if(Cnt_mS == 1000){
      Cnt_mS = 0;
      Vbat = __Info(P_VBAT);
      Vusb = __Info(P_VUSB);
      Ichg = __Info(P_ICHG);
      for(i=0; i<4; i++) if(Vbat <= V_BT[i]) break;
      if(Vusb > 4200){
        if(Ichg < 10) Battery = 6;
        else          Battery = 5;
      } else         Battery = i;
      //----------------待机---------------- 
      if((PopMenu3_Value[SYS_Standy]!=0)&& (PD_Cnt > 0))
        PD_Cnt--;      //待机记时
      if((PopMenu3_Value[SYS_PowerOff]!=0)&& (AutoPwr_Cnt > 0))
        AutoPwr_Cnt--; //自动关机记时
    }
  }
}
/*******************************************************************************
 Key status detect                                             Return: KeyIn
*******************************************************************************/
void Keys_Detect(void)
{ 
  static u16 Key_Last; 
  u16 Key_Now = 0,KeyCode = 0;
  
  if(Key_UPD == 1){
    Key_UPD = 0;
    Key_Now = __Info(KEY_IN);
    if(Key_Now &(~Key_Last)){
      Key_Wait_Cnt   = 50;                            // 重置持续按键 1.0S 计数
      Key_Repeat_Cnt = 3;                             // 设定 60mS 自动重复周期
      if(Key_Now & 0x01)  KeyCode = K_PWR;
      if(Key_Now & 0x02)  Tmp_RUN = K_RUN;
    }
    else{
      KeyCode = Touch_Motion(); // 触摸板
      if(Key_Now & Key_Last){
        if((Key_Wait_Cnt < 25)&&(Key_Repeat_Cnt == 0)){//持续按键 0.5S 以上
          if(Key_Now & 0x3C)  Key_Repeat_Cnt = 3;      //设定 60mS 自动重复周期
        }
        if((Key_Wait_Cnt == 0)&&(Key_Repeat_Cnt == 0)){
          if(Key_Now & 0x02){KeyCode = RUN_HOLD,Tmp_RUN = 0;}
          if(Key_Now & 0x03) Key_Repeat_Cnt = 25;      //设定 1.0S 自动重复周期
        }
      }else {
        if(Tmp_RUN) {KeyCode = Tmp_RUN; Tmp_RUN = 0;}
        Key_Wait_Cnt=50;
      }  
    }
    Key_Last = Key_Now;
    KeyIn = KeyCode;
  }
}
/*******************************************************************************
 DevCtrl:  硬件设备控制
*******************************************************************************/
void AiPosi(u8 Val)
{
  __Ctrl(AOFFSET,(((u8)Val-100)*1024*GK[GainA+5*KindA])/KgA[KindA]+KpA[KindA]);
}
void BiPosi(u8 Val)
{
  __Ctrl(BOFFSET,(((u8)Val-100)*1024*GK[GainB+5*KindB])/KgB[KindB]+KpB[KindB]);
}
void Beep(u16 mS)
{
  BeepCnt = mS; 
  __Ctrl(BUZZVOL,PopMenu3_Value[SYS_Volume]*10);     // 设定蜂鸣器音量(0~100%)
  __Ctrl(BUZZ_ST, ENABLE);
}
/*******************************************************************************
  TIM6_Config
*******************************************************************************/
void TIM6_Config(void) {    
  
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;
     
  TIM_TimeBaseStructure.TIM_Prescaler=100-1; //1ms
  TIM_TimeBaseStructure.TIM_Period=720-1;
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM6,&TIM_TimeBaseStructure); 
  TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
  TIM6->DIER &= 0x0FEFF;   //关闭DAC中断
  
  NVIC_InitStructure.NVIC_IRQChannel=TIM6_DAC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_Cmd(TIM6, ENABLE);
}  
/*******************************************************************************
  Mass Storge Device Disk Config
*******************************************************************************/
void USB_MSD_Config(void)
{
  NVIC_InitTypeDef  NVIC_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  SCI_CLK_HIGH();
  GPIOA->MODER  &= 0xFF3FFFFF;
  GPIOA->MODER  |= 0x00400000;
  SCI_DIO_LOW();
  GPIOA->MODER  &= 0xFCFFFFFF;
  GPIOA->MODER  |= 0x02000000;
  GPIOA->OTYPER |= 0x1000;      // Open-Drain Output
  __Ctrl(DELAYmS, 100);         // 延时1秒后检测按键状态
  SCI_CLK_HIGH();
  SCI_DIO_HIGH();
    
  GPIOA->BSRR    = USB_DN;
  GPIOA->MODER  &= 0xFF3FFFFF;
  GPIOA->MODER  |= 0x00800000;

  GPIOA->BSRR    = USB_DP;
  GPIOA->MODER  &= 0xFCFFFFFF;
  GPIOA->MODER  |= 0x02000000;
  GPIOA->OTYPER &= 0xEFFF;      // Push-Pull Output

  GPIO_InitStructure.GPIO_Pin   = SPI_SCK | SPI_MISO | SPI_MOSI;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = SPI_CS;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_PinAFConfig(MAP_SCK );
  GPIO_PinAFConfig(MAP_MISO); 
  GPIO_PinAFConfig(MAP_MOSI);  
  
  SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial     = 7;
  SPI_Init(SPIx, &SPI_InitStructure);

  SPI_RxFIFOThresholdConfig(SPIx, SPI_RxFIFOThreshold_QF);
  SPI_Cmd(SPIx, ENABLE);

  ExtFlash_CS_HIGH();
  ExtFlash_RST_LOW ();
  __Ctrl(DELAYmS, 100);
  ExtFlash_RST_HIGH();
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel                   = USB_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  USB_Init();
  Disk_Init();
} 
 /******************************************************************************
 ADC_Start: 重新开始ADC扫描采样
*******************************************************************************/
void ADC_Start(void)
{
  u16 Dpth;
  
  __Ctrl(SMPL_ST, DISABLE);
  if(((PopMenu1_Value[TRI_Sync] == NONE)||(PopMenu1_Value[TRI_Sync] == SCAN))
     &&(PopMenu1_Value[TIM_Base]>11)){
    Dpth = 302;                                        //5ms
    PopMenu1_Value[WIN_Posi]= 0;
  }else Dpth = DEPTH[PopMenu1_Value[WIN_Depth]];
   if(PopMenu1_Value[TIM_Base]<2){  
    __Ctrl(SMPL_ST, INTERLEAVE);                       // 设定交错工作模式
    __Ctrl(SMPLBUF, (u32)Smpl);                        // 重新设定DMA通道1
    __Ctrl(SMPLNUM, Dpth/2);
  } else {
    __Ctrl(SMPL_ST, SIMULTANEO);                       // 设定并行工作模式
    __Ctrl(SMPLBUF, (u32)Smpl);                        // 重新设定DMA通道1
    __Ctrl(SMPLNUM, Dpth);
  }  
  __Ctrl(SMPL_ST, ENABLE);                           // 重新开始扫描采样
} 

/*******************************************************************************
 设置 USB 设备 IO 端口   NewState = ENABLE / DISABLE
*******************************************************************************/
void __USB_Port(u8 NewState)
{
  SCI_CLK_HIGH();
  if(NewState == DISABLE){         // USB DN & DP Pins Disconnect
    SCI_DIO_LOW();
    SCI_CLK_LOW();
    GPIOA->MODER  &= 0xFC3FFFFF;
    GPIOA->MODER  |= 0x01400000;
    GPIOA->OTYPER |= 0x1800;      // Dio Output Open-Drain
  } else {                        // USB DN & DP Pins Connect
    GPIOA->MODER  &= 0xFC3FFFFF;
    GPIOA->MODER  |= 0x02800000;
    GPIOA->OTYPER &= 0xE7FF;      // Push-Pull Output
  }
}

/******************************** END OF FILE *********************************/
