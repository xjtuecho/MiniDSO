/********************* (C) COPYRIGHT 2014 e-Design Co.,Ltd. ********************
 FILENAME: TouchPad.c   DS202 Ver1.00  MCU STM32F303VC             Author : xie
*******************************************************************************/
#include "Version.h"
#include "TouchPad.h"
#include "TP_Upgrade.h"
#include "Drive.h"
#include "Draw.h"
#include "Bios.h"
#include "Menu.h"
#include "Func.h"
#include "Math.h"
#include "File.h"

                  
/*------------------TP_CTL 信息包控制寄存器( 0x5C R/W )-------------------------
bit7:   RD_FLAG   信息包读取标志位 [ =1 信息包已读取 ]
bit6~4: Reserved              
bit3:   TP_DIS    触点报告禁止位   [ =0 触点报告正常，=1 禁止所有触点报告 ]
bit2:   HOV_DIS   虚触点报告禁止位 [ =0 虚触点报告正常，=1 禁止虚触点报告 ]
bit1:   Reserved 
bit0:   TP_ SORT  触点排序控制位   [ =0 按ID值升序排列，=1 按压力值降序排列 ]
------------------------------------------------------------------------------*/

#define TP_CTL      0x5C
#define TP_STATUS   0x7C
#define TP_DATA     0x5D
#define GESTURE_EN  0x6C

/*******************************************************************************
  触摸屏状态数据  CTP_Buf[] 数据定义  
+---------------------------------------+---------------------------------------+
|           触摸板即时触摸数据          |           触摸板手势数据              |
+--------+--------+----------+----------+--------+--------+----------+----------+
| TP状态 | 触摸ID | 触点 y值 | 触点 x值 | 手势码 | 手势ID |   x终值  |   y终值  |
+--------+--------+----------+----------+--------+--------+----------+----------+
| Byte:0 | Byte:1 | Byte:2~3 | Byte:4~5 | Byte:6 | Byte:7 | Byte:8~9 |Byte:10~11| 
+--------+--------+----------+----------+--------+--------+----------+----------+
|   TPs  |   TPn  |    TPy   |    TPy   |   TGk  |   TGn  |    TEx   |    TEy   |
+--------+--------+----------+----------+--------+--------+----------+----------+
*******************************************************************************/
u8  CTP_Buf[12];
u8  *TPs = (u8* )(CTP_Buf+0), *TPn = (u8* )(CTP_Buf+1);
u16 *TPx = (u16*)(CTP_Buf+4), *TPy = (u16*)(CTP_Buf+2);
u8  *TGk = (u8* )(CTP_Buf+6), *TGn = (u8* )(CTP_Buf+7);
u16 *TEx = (u16*)(CTP_Buf+8), *TEy = (u16*)(CTP_Buf+10);

/*******************************************************************************
GESTURE_EN 手势能使寄存器( 0x1D R/W )
bit0: 翻页，bit1: 滑动，bit2: 单击，bit3: 双击，bit4: 长按
*******************************************************************************/
void Gesture_Enable(void)
{
  __I2C_Write(TP_ID, GESTURE_EN,    0x1D  ); 
}

/*******************************************************************************
Read Touch information                                 
*******************************************************************************/
void Read_TP(void)
{ 
  u8 Val[2];
  
  if(__I2C_Read (TP_ID, TP_STATUS, Val, 1)) return;      // Read 0x7C_Reg
  if(__I2C_Write(TP_ID, TP_CTL,    0x80  )) return;      // Set  0x5C_Reg(7) = 1  
  if(__I2C_Read (TP_ID, TP_DATA,   Val, 2)) return;      // Read 0x5D_Reg  
  if(Val[0] == 0x80){ 
    if(__I2C_Read(TP_ID, TP_DATA, CTP_Buf+1, 5)) return; // Read touch report
    *TPs  = 1;
  } else if(Val[0] == 0x40){
    if(__I2C_Read(TP_ID, TP_DATA, CTP_Buf+6, 2)) return; // Read gestrue report 
    *TPs |= 2;
  } else if(Val[0] == 0x00){                             // Finger leave
    *TPs |= 4;
    *TEx = *TPx;
    *TEy = *TPy;
  }
}

/*******************************************************************************
CTP Reset function                                  
*******************************************************************************/
void CTP_Soft_Reset(void)
{
  __I2C_Write(TP_ID, 0x7B, 0x55);
  Wait_mS(5);
  __I2C_Write(TP_ID, 0x7B, 0x99);
  Wait_mS(100);
}


u8  Click_Flag = 0, Motion   = 0, Drag_Cnt = 1;
s16 Last_TPn   = 1, Last_TGn = 1, TpCnt    = 0;
u16 Last_TPs   = 0;
s16 Key_x, Key_y, End_x, End_y, Last_x, Last_y, Last_z;

/******************************************************************************
检测触摸屏手势操作                                          Return: Key Code
*******************************************************************************/
u16 Touch_Motion(void)
{
  u16 KeyCode = 0;
  u16 i,index = 0;
  
  if(menu.current==Oscillo){                          //page 1
    i = menu.menu_index[Oscillo];
    index = PopMenu1_Base[i]+Cur_PopItem-1;           //当前子窗口选项
  }
  
//===============================有新的触摸动作===============================//  
  if((Last_TPn != *TPn)){                    
    Last_TPn = *TPn;
    Last_TGn = *TGn;
    Last_TPs = *TPs;
    Last_x   = *TPx;
    Last_y   = *TPy;
    Key_x    = *TPx;
    Key_y    = *TPy;
    Motion   = 0;
  }
  
//===============================同一触摸动作=================================//   
  else{ 
    //+++++++++++++++++++++++++++++手指按下（滑动）+++++++++++++++++++++++++++//
    if((Last_TPs == 1) ){                           
      //-----------------------------向上滑动---------------------------------//
      if((*TPy > (Last_y+6*8)) && (fabs(*TPx-Last_x) < 3*16)){            
                                                       //隐藏菜单时滑动
        if((*TPy > (Last_y+8*8)) && (Drag_Cnt) && (menu.menu_flag == 0)){
          if((Last_x < 16*16)&&(Last_x > 6*16)&&(Last_y < 480)&&(Last_y > 160)){
            if((PopMenu3_Value[SYS_Standy]!=0) &&(PD_Cnt == 0)){ 
                __Ctrl(B_LIGHT, PopMenu3_Value[SYS_BKLight]*10);
                __Ctrl(PM_CTRL, CHG_ACT);              //充电模式 LED长亮 
              }
            if(PopMenu1_Value[CH1_Vol]<Popmenu1_Limit1[CH1_Vol])
              PopMenu1_Value[CH1_Vol]++;
            Show_Title();
            Update_Proc_All();
            Beep(50);
          }
          KeyCode = 0xFFFF;
        }
                                                       //开启快速滑动
        else if((!PopMenu3_Value[SYS_PosiSlide])&&(menu.current==Oscillo)
                &&(Last_x<7*16)){          
          Vertical_Slide(add);
        }
                                                       //一般滑动
        else {
          KeyCode = K_UP;
        }
      }
      
      //-----------------------------向下滑动---------------------------------//
      else if ((*TPy < (Last_y-6*8))&&(fabs(*TPx-Last_x)<640)) {    
                                                       //全屏时滑动
        if((*TPy < (Last_y-8*8)) &&(Drag_Cnt)&&(menu.menu_flag ==0) ){
          if((Last_x < 16*16)&&(Last_x > 6*16)&&(Last_y < 480)&&(Last_y > 160)){
            if((PopMenu3_Value[SYS_Standy]!=0) &&(PD_Cnt == 0)){ 
                __Ctrl(B_LIGHT, PopMenu3_Value[SYS_BKLight]*10);
                __Ctrl(PM_CTRL, CHG_ACT);              //充电模式 LED长亮 
              }
            if(PopMenu1_Value[CH1_Vol]>PopMenu1_Limit2[CH1_Vol])
              PopMenu1_Value[CH1_Vol]--;
            Show_Title();
            Update_Proc_All();
            Beep(50); 
          }
          KeyCode = 0xFFFF;
        }
                                                       //开启快速滑动
        else if((!PopMenu3_Value[SYS_PosiSlide])&&(menu.current==Oscillo)
                &&(Last_x<7*16)){          
          Vertical_Slide(dec);
        }
                                                       //一般滑动
        else {
          KeyCode = K_DOWN;
        }
      }  
      
      //-----------------------------向右滑动---------------------------------//
      else if((About_Flag==0)&&(*TPx > (Last_x+4*16))&&(fabs(*TPy-Last_y)<640)){    
                                                       //无子窗口右滑
        if((*TPx > (Last_x+5*16))&&(Drag_Cnt)
           &&!(PopType & (LIST_POP|DAILOG_POP|FILE_POP)) ){   
                                                       //翻页      
          if(!(PopType & (LIST_POP|DAILOG_POP|FILE_POP))
             &&((Last_y < 16*16)&&(Last_y > 0)||(Last_y > 23*16))){
            if((menu.menu_flag == 1)){
              ParamTab[M_WX] = 251;
              if(menu.menu_flag == 1){
                if(menu.current >= MENU_MAX)menu.current = 0;
                else menu.current++;
              }
              menu.menu_flag = 1;
              Show_Menu();
              menu.mflag |= UPD;  
              Drag_Cnt = 0;
              Beep(100);
              MenuCnt = 6000;  
            }
            KeyCode = 0xFFFF;
          }
                                                       //改变当前选项参数
          else if(menu.menu_flag == 1){
            KeyCode = K_RIGHT;
          }
                                                       //隐藏菜单右滑 快速操作
          else if((menu.menu_flag == 0)&&(*TPx > (Last_x+7*16))&&(Last_y < 23*16)
                  &&(Last_y > 16*16) ){
            if((PopMenu3_Value[SYS_Standy]!=0) &&(PD_Cnt == 0)){
                                                       //唤醒省电模式
              __Ctrl(B_LIGHT, PopMenu3_Value[SYS_BKLight]*10);
              __Ctrl(PM_CTRL, CHG_ACT);                //充电模式 LED长亮 
            }
            if(PopMenu1_Value[TIM_Base]<Popmenu1_Limit1[TIM_Base])
              PopMenu1_Value[TIM_Base]++;
            Show_Title();
            Update_Proc_All();
            Beep(50);
            KeyCode = 0xFFFF;
          }
        } 
                                                        //子窗口右滑 
        else if ((PopType & (LIST_POP|DAILOG_POP|FILE_POP))&&(Last_y < 23*16)
                 &&(Last_y > 16*16))
          KeyCode = K_RIGHT;
        Last_y = *TPy;                                  //左右滑动时忽略上下抖动
      }
      
      //-----------------------------向左滑动---------------------------------//        
      else if((About_Flag==0)&&(*TPx < (Last_x-4*16))&&(fabs(*TPy-Last_y)<3*16)){           
                                                        //无窗口左滑
        if((*TPx < (Last_x-5*16))&&(Drag_Cnt)
           &&!(PopType & (LIST_POP|DAILOG_POP|FILE_POP))){   
                                                        //翻页 
          if(!(PopType & (LIST_POP|DAILOG_POP|FILE_POP))
             &&((Last_y < 16*16)&&(Last_y > 0)||(Last_y > 23*16))){
            if(menu.menu_flag == 1){
              ParamTab[M_WX] = 251;
              if(menu.menu_flag == 1){
                if(menu.current <= 0)menu.current = MENU_MAX;       
                else menu.current--;
              }
              menu.menu_flag = 1;
              Show_Menu();
              menu.mflag |= UPD;
              Drag_Cnt = 0; 
              Beep(100);
              MenuCnt = 6000;      
            }
            KeyCode = 0xFFFF;
          }                                             //改变当前选项参数
          else if(menu.menu_flag == 1){
            KeyCode = K_LEFT;
          }
                                                        //隐藏菜单左滑 快速操作
          else if((menu.menu_flag == 0)&&(*TPx < (Last_x-7*16))
                  &&(Last_y < 23*16)&&(Last_y > 16*16)){
            if((PopMenu3_Value[SYS_Standy]!=0) &&(PD_Cnt == 0)){
                                                        //唤醒省电模式
              __Ctrl(B_LIGHT, PopMenu3_Value[SYS_BKLight]*10);
              __Ctrl(PM_CTRL, CHG_ACT);                 //充电模式 LED长亮 
            }
            if(PopMenu1_Value[TIM_Base]>PopMenu1_Limit2[TIM_Base])
              PopMenu1_Value[TIM_Base]--;
            Show_Title();
            Update_Proc_All();
            Beep(50);
            KeyCode = 0xFFFF;
          }     
        }
                                                         //子窗口左滑 
        else if ((PopType & (LIST_POP|DAILOG_POP|FILE_POP))
                 &&(Last_y < 23*16)&&(Last_y > 16*16))
          KeyCode = K_LEFT;
        Last_y = *TPy;                        //左右滑动时忽略上下抖动       
      }
      
      
      if(KeyCode != 0){                       //有滑动时，x，y轴坐标更新
        if((PopType & (LIST_POP|DAILOG_POP|FILE_POP))
           &&((index ==CH1_Posi)||(index ==CH2_Posi)||(index ==CH3_Posi)
              ||(index ==TRI_Ch1)||(index ==CUR_T1-1)||(index ==CUR_T2-1)
              ||(index ==CUR_V1-1)||(index ==CUR_V2-1)||(index ==WIN_Posi-1)));   
                                              //移动位置时，不抬手则连续移动
        else Last_x = *TPx;
        Last_y = *TPy;
        Motion = 1;
        Click_Flag = 0;
      }else{             
        if( Motion ==0 )Click_Flag = 1;       //无滑动时，单击使能标志 
      } 
    }  
    //+++++++++++++++++++++++++++++手指抬起（单击）+++++++++++++++++++++++++++//
    else  {               
      if((Last_TPs != 1 && Click_Flag == 1)&&(*TGk!=0x01) ){
       
        if((Key_x > 6*16)&&(Key_x < 16*16)&&(Key_y <  8*16))
          KeyCode = K_ACT;                    //提高“M”键优先级
           
        if((Key_x > 7*16)&&(Key_x < 15*16)){
          if     (Key_y > 31*16) KeyCode = K_OK;
          else if(Key_y > 23*16) KeyCode = K_UP; 
          else if(Key_y <  8*16) ;            //KeyCode = K_ACT;
          else if(Key_y < 16*16) KeyCode = K_DOWN; 
        } else {
          if((Key_y < 23*16)&&(Key_y > 16*16)){
            if(Key_x > 14*16) KeyCode = K_RIGHT;
            if(Key_x <  7*16) KeyCode = K_LEFT;
          }
        }
        Click_Flag = 0;
        Motion = 1; 
      }
      Drag_Cnt = 1; 
    } 
  }
//===============================有新手势动作===============================//  
  if((Last_TGn != *TGn))   
  {
    if(*TGk==TG_HOLD){                        //长按
      if((Key_x > 7*16)&&(Key_x < 15*16)){
        if     (Key_y > 31*16) ;
        else if(Key_y > 23*16) ; 
        else if(Key_y <  8*16) ;
        else if(Key_y < 16*16) ;
        else KeyCode = HOLD_K;                //手势类别为长按 
      } 
      else if((Key_y < 23*16)&&(Key_y > 16*16)){
        if(Key_x > 14*16) ;
        else if(Key_x <  7*16) ;
        else KeyCode = HOLD_K;                //手势类别为长按  
      }
      else KeyCode = HOLD_K;                  //手势类别为长按 
    }
    
    else if(*TGk==TG_DOUBLE) {                //双击
      if((Key_x > 7*16)&&(Key_x < 15*16)){
        if     (Key_y > 31*16) ;
        else if(Key_y > 23*16) ; 
        else if(Key_y <  8*16) ;
        else if(Key_y < 16*16) ;
        else KeyCode = DOUBLE_K;              //手势类别为双击  
      } 
      else if((Key_y < 23*16)&&(Key_y > 16*16)){
        if(Key_x > 14*16) ;
        else if(Key_x <  7*16) ;
        else KeyCode = DOUBLE_K;              //手势类别为双击  
      }
      else KeyCode = DOUBLE_K;                //手势类别为双击 
    }
  }
  
  Last_TPn = *TPn;
  Last_TGn = *TGn;
  Last_TPs = *TPs;
  if(KeyCode && (KeyCode != 0xFFFF))MenuCnt = 6000;          
  return KeyCode; 
}


/*******************************************************************************
 CTP upgrade function
*******************************************************************************/
#define CTP_TRUE                 0x00
#define CTP_FALSE                0x01

//---------------------- Registers Address Definitions -----------------------//

//      register name           address      default data
#define PRODUCT_REV              0x00         // 0x65
#define HW_FW_REV                0x01         // 0x2F
#define WD_DISABLE               0x02     
#define FLASH_EN                 0x03     
#define BOOT_STAT                0x04        
#define ERROR_ID                 0x05         // 0x00
#define DEBUG                    0x3d
#define DOWNLOAD_CTL             0x52         // N/A
#define DOWNLOAD_DATA            0x53         // N/A
#define FLASH_CTL                0x67
#define SHUTDOWN                 0x7B         // 0x55 shutdown
                                              // 0x99 hard reset
                                              // 0x77 patrial reset
                                              // 0x66 resume

u16 CTP_Upgrade(void)
{ 
  s32 i,j,Fw_Size,Config_Size,Haptic_Size,Button_Slider_Size,Delta_Table_Size;
  u8  Drive_Sense_Size,Reg_Size,Ex_Reg_Size;
  u8  Val,Val1,Retry,Down_Fail,Buf_1, Buf_2; 
  u8  *Fw_Base,*Reg_Data,*Config_Tail,*Config_Data,*Ex_Reg_Data;
  
  u8 Cmd[] = {0x55,0x77,0xAA,0x00,0xe3,0x08,0x2e,0xA0,0x90,0x84,0x88,0x82};
  
  u8 Reg_Addr_List[]={
    0x73,0x15,0x14,0x13,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x25,0x24,0x27,0x26,0x1a,
    0x1b,0x1c,0x2f,0x30,0x31,0x32,0x33,0x1e,0x1f,0x22,0x23,0x11,0x12,0x7f,0x7d,
    0x7e,0x5b,0x68,0x17,0x16,0x19,0x18,0x08,0x09,0x0a,0x0b,0x0d,0x0c,0x0f,0x0e,
    0x10,0x40,0x41,0x44,0x43,0x42,0x48,0x47,0x46,0x4d,0x4b,0x4a,0x5c,0x34,0x35,
    0x36,0x37,0x5e,0x61,0x60,0x6c,0x79,0x78,0x70};
  
  u8 Ex_Reg_Addr_List2[]={  
    0x17,0x18,0x1a,0x19,0x1c,0x1b,0x1d,0x06,0x08,0x07,0x1f,0x1e,0x09,0x0a,0x0b,
    0x15,0x20,0x0c,0x16,0x21,0x22,0x23,0x24,0x0d,0x0e,0x26,0x25,0x2a,0x29,0x2c,
    0x2b,0x0f,0x10,0x11,0x12,0x2d,0x2e,0x13,0x14,0x2f,0x30,0x32,0x31,0x34,0x33,
    0x37,0x36,0x01,0x00,0x05,0x04,0x35,0x38,0x39,0x43,0x42,0x45,0x44,0x47};

  u8 Ex_Reg_Addr_List1[]={ 0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26};
  
  Button_Slider_Size = 0;
  Config_Size = 0;
  Config_Tail = 0;
  Haptic_Size = 0;
  Down_Fail   = 0;
  
  __I2C_Read(TP_ID, 0x75, &Val, 1);
  __I2C_Read(TP_ID, 0x73, &Val1, 1);
  /*判断升级固件版本号，升级固件版本号小于当前号则不更新*/
  if(Val > FW_DATA[0]) return CTP_FALSE; 
  /*如果升级固件版本号等于当前号，则进一步判断寄存器版本号*/ 
  else if((Val == FW_DATA[0]) && (Val1 >= FW_DATA[1])) return CTP_FALSE;   

  SetColor(BLK, WHT);
  DispStr(8, 90, PRN, "                                        ");
  DispStr(8, 70, PRN, "                                        ");
  DispStr(8, 90, PRN, "      Touch program  is upgrading...    ");
  DispStr(8, 70, PRN, "        Please wait a few seconds       ");
  
  Fw_Size = (((s32)FW_DATA[2]) << 8) | ((s32)FW_DATA[3]);
  Fw_Base = (u8*)&FW_DATA[4];
  if(Fw_Size){
    for(Retry=0; Retry<3; Retry++){
      __I2C_Write(TP_ID, SHUTDOWN, Cmd[0]);
      Wait_mS(1);                          
      __I2C_Write(TP_ID, SHUTDOWN, Cmd[1]);
      Wait_mS(40);
      __I2C_Read(TP_ID, HW_FW_REV, &Val, 1);
      while(Val != 0x21){
        __I2C_Write(TP_ID, SHUTDOWN, Cmd[0]);
        Wait_mS(10);
        __I2C_Write(TP_ID, SHUTDOWN, Cmd[1]);
        Wait_mS(300);
        __I2C_Read(TP_ID, HW_FW_REV, &Val, 1);
      }
      __I2C_Write(TP_ID, WD_DISABLE,   Cmd[2]);
      __I2C_Write(TP_ID, BOOT_STAT,    Cmd[3]);
      __I2C_Read(TP_ID, ERROR_ID, &Val, 1);   
      __I2C_Write(TP_ID, FLASH_EN,     Cmd[4]);
      __I2C_Write(TP_ID, DEBUG,        Cmd[5]);
      __I2C_Write(TP_ID, DOWNLOAD_CTL, Cmd[6]);
      Wait_mS(100);
      __I2C_Read(TP_ID, DOWNLOAD_CTL, &Val, 1);
      if(Val != 0x2e)  return CTP_FALSE;         // CTP erase fail!
      __I2C_Read(TP_ID, ERROR_ID,     &Val, 1); 
      
      for(i=0; i<Fw_Size; i++){                  // flash downloading
        if(__I2C_Write(TP_ID, DOWNLOAD_DATA, *(Fw_Base+i))) return CTP_FALSE;  
        // CTP flash write fail! 
      }
      Wait_mS(800);
      __I2C_Read(TP_ID, BOOT_STAT, &Val, 1);  
      if((Val & 0x02) != 0x02) return CTP_FALSE; // CTP flash check fail!
      __I2C_Read(TP_ID, HW_FW_REV, &Val, 1);  
      if(Val == 0x2F) break;                     // CTP FW version ok!
    }
  }
  
  //==========================使用非固化触摸升级程序============================

  /*************  prepare to download config file  ***************/
  Config_Size = (((s32)FW_DATA[Fw_Size + 3+1])<<8)|((s32)FW_DATA[Fw_Size+4+1]);
  if(Config_Size)
  {
    for(Retry=0;Retry<3;Retry++){
      Config_Data = Fw_Base + Fw_Size + 2;
      Config_Tail = Config_Data + Config_Size;
      __I2C_Write(TP_ID, WD_DISABLE,Cmd[2]);
      __I2C_Write(TP_ID, BOOT_STAT,Cmd[3]);
      __I2C_Write(TP_ID, FLASH_EN,Cmd[4]);
      
      for(j=0;j<4;j++){
        if(Config_Data == Config_Tail){
          break;
        }
	
        switch(*(Config_Data)){
        case 0x29:{
            Haptic_Size |= *(Config_Data+3);
            Haptic_Size <<= 8;
            Haptic_Size |= *(Config_Data+2);
            __I2C_Write(TP_ID, DOWNLOAD_CTL,*Config_Data);
            for(i=1;i<Haptic_Size+1;i++)
              __I2C_Write(TP_ID, DOWNLOAD_DATA,*(Config_Data+i));
            __I2C_Write(TP_ID, FLASH_CTL,Cmd[7]);
            Config_Data += Haptic_Size+1;
          }
          break;
        case 0x2A:{
            Button_Slider_Size = ((*(Config_Data+3)) << 8) |(*(Config_Data+2));
            __I2C_Write(TP_ID, DOWNLOAD_CTL,*Config_Data);
            for(i=1;i<Button_Slider_Size+1;i++)
              __I2C_Write(TP_ID, DOWNLOAD_DATA,*(Config_Data+i));
            __I2C_Write(TP_ID, FLASH_CTL,Cmd[8]);
            Config_Data += Button_Slider_Size+1;
          }
          break;
        case 0x2C:{
            Drive_Sense_Size = 28;
            __I2C_Write(TP_ID, DOWNLOAD_CTL,*Config_Data);
            for(i=1;i<Drive_Sense_Size;i++)
              __I2C_Write(TP_ID, DOWNLOAD_DATA,*(Config_Data+i));
            __I2C_Write(TP_ID, FLASH_CTL,Cmd[9]);
            Config_Data += Drive_Sense_Size;
          }
          break;
        case 0x2D:{
            Delta_Table_Size = 482;
            __I2C_Write(TP_ID, DOWNLOAD_CTL,*Config_Data);
            for(i=1;i<Delta_Table_Size;i++)
              __I2C_Write(TP_ID, DOWNLOAD_DATA,*(Config_Data+i));
            __I2C_Write(TP_ID, FLASH_CTL,Cmd[10]);
            Config_Data += Delta_Table_Size;
          }
          break;
        default:
          break;
        }
        
        Wait_mS(100);
        __I2C_Read(TP_ID, BOOT_STAT, &Val,1);
        if((Val & 0x40) == 0x40){
        }
        else{
          Down_Fail = 1;
        }
      }
      if(!Down_Fail)
        break;
    }
  }
  
  /*************  prepare to download IO register  ***************/

  Wait_mS(300);
  Reg_Size = FW_DATA[Fw_Size + Config_Size + 5+1];
  Reg_Data = Config_Tail +1;
  
  if(Reg_Size){
    __I2C_Write(TP_ID, WD_DISABLE,Cmd[2]);
    __I2C_Write(TP_ID, BOOT_STAT,Cmd[3]);
    __I2C_Write(TP_ID, FLASH_EN,Cmd[4]);
    for(i=0;i<Reg_Size;i++){
      __I2C_Write(TP_ID, Reg_Addr_List[i],*(Reg_Data+i));
    }
    __I2C_Write(TP_ID, FLASH_CTL,Cmd[11]);
    Wait_mS(200);
  }
  
  /************  prepare to download extend register  **************/
  Ex_Reg_Size = FW_DATA[Fw_Size + Config_Size + Reg_Size + 6+1];
  Ex_Reg_Data = Reg_Data +Reg_Size +1;
  if(Ex_Reg_Size){
    Buf_1 = 0x17;
    Buf_2 = 0x19;	
    __I2C_Write(TP_ID, WD_DISABLE,Cmd[2]);
    __I2C_Write(TP_ID, BOOT_STAT,Cmd[3]);
    __I2C_Write(TP_ID, FLASH_EN,Cmd[4]);
    for(i=0;i<10;i++){	
    	__I2C_Write(TP_ID, 0x50,Ex_Reg_Addr_List1[i]);
      __I2C_Write(TP_ID, 0x51,*(Ex_Reg_Data+i));
    }
    for(i=10;i<Ex_Reg_Size;i++){	
      __I2C_Write(TP_ID, 0x50,Buf_1);
      __I2C_Write(TP_ID, 0x51,Ex_Reg_Addr_List2[i-10]);
      __I2C_Write(TP_ID, 0x50,Buf_2);
      __I2C_Write(TP_ID, 0x51,*(Ex_Reg_Data+i));
    }
    __I2C_Write(TP_ID, FLASH_CTL,Cmd[11]);
    Wait_mS(200); 
  }
  
  __I2C_Write(TP_ID, FLASH_EN,   Cmd[3]);
  __I2C_Write(TP_ID, WD_DISABLE, Cmd[3]);
  CTP_Soft_Reset(); 
  Wait_mS(300);
  
  return CTP_TRUE;                   // Upgrade data download sucess!
}
/******************************** END OF FILE *********************************/
