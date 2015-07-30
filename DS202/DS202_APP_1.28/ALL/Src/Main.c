/********************* (C) COPYRIGHT 2014 e-Design Co.,Ltd. ********************
File Name : main.c
Version   : DS202 Ver 1.0                                          Author : bure
*******************************************************************************/
#include "Version.h"
#include "TouchPad.h"
#include "Process.h"
#include "Drive.h"
#include "Func.h"
#include "Draw.h"
#include "Bios.h"
#include "Menu.h"
#include "Disk.h"
#include "LCD.h"
#include "FAT12.h"
#include "File.h"
#include "Math.h"

typedef void (*pFunc)(void);
void MSD_Disk_Config(void);
void Set_Licence(u16 x, u16 y);

//===============================APP版本号======================================
u8  APP_VERSION[] = "APP V1.28";

u8  Key_Flag = 0, CalPop_Flag = 1, ResPop_Flag = 1; 
u8  Menu_Temp[5], NumStr[20];
u16 FileInfo,     Label_Cnt;

void main(void)
{
  //===============================系统初始化===================================
  __Ctrl(SYS_CFG, RCC_DEV | TIM_DEV | GPIO_OPA | ADC_DAC  | I2C_SPI);
  __Ctrl(LCD_CLR, BLK);
  __Disp_Logo(LOGO_X, 140);
  /*-------生成不同分区APP时要更改此处和icf文件-------------------------------*/
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);      //第一分区APP
  // NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x20000);  //第二分区APP
  SysTick_Config(SystemCoreClock/1000);                 //SysTick = 1mS
  __Ctrl(B_LIGHT, 50);                                  //打开 LCD 背光照明 50%
  __Ctrl(PM_CTRL, CHG_ACT);                            //充电模式 LED长亮
  __Ctrl(BUZZVOL, 50);                                 //设定蜂鸣器音量(0~100%) 
  Beep(200);                                           //200mS                                     
  USB_MSD_Config();
  Init_Fat_Value();
  TIM6_Config(); 
  __Ctrl(SMPL_ST, DISABLE);
  __Ctrl(SMPL_ST, SIMULTANEO);
  __Ctrl(SMPLBUF, (u32)Smpl);
  __Ctrl(SMPLNUM, DEPTH[PopMenu1_Value[WIN_Depth]]);
  __Ctrl(SMPL_ST, ENABLE);
  
  //=============================显示开机提示信息页面===========================  
  SetColor(BLK, WHT);
  DispStr(8,      90, PRN, "      Oscilloscope DS202 ");
  DispStr(8+25*8, 90, PRN,                            APP_VERSION);
  DispStr(8,      70, PRN, "        System Initializing...       ");
  __Ctrl(DELAYmS, 500); 
  
  ADC_StartConversion(ADC1);  
  ADC_StartConversion(ADC3);
  memset(VRAM_PTR+TR1_pBUF, ~0,    900); 
  memcpy(VRAM_PTR+TAB_PTR,  PARAM,  90);  
  __Ctrl(SYS_CFG, CTP_DEV);
  Gesture_Enable();                            //触摸屏手势使能
  CTP_Upgrade();                               //触摸屏固件升级程序
  
  //=============================第一次写固件自动校准===========================   
  Read_CalFlag();
  if(Cal_Flag == 1){  
    Cal_Flag = 0;
    SetColor(BLK, WHT);
    DispStr(8, 90, PRN, "                                        ");
    DispStr(8, 70, PRN, "                                        ");
    DispStr(8, 90, PRN, "      Run the calibration program...    ");
    DispStr(8, 70, PRN, "        Please wait a few seconds       ");
    Zero_Align();                              //垂直位移零点校正 
    Save_Kpg();
  }
  //=============================主界面显示=====================================   
  Read_Kpg();                                   //读取校准参数
  Load_Param();                                 //从U盘读取保存参数
  ClrScrn(DAR);                                 //背景清屏
  menu.menu_flag = 1;                           //menu主菜单
  menu.mflag  |= UPD;                           //menu选项
  menu.iflag  |= UPD;                           //menu子菜单
  Label_Flag  |= UPD;                           //游标
  Show_Title();                                 //显示
  Show_Menu();                                  //主菜单和选项   
  Update_Proc_All();                            //操作刷新
  Update_Label();                               //游标显示
  Battery_Show();              
  MenuCnt = 5000;                               //Menu主菜单第一次待机时间
  PD_Cnt      = PopMenu3_Value[SYS_Standy]*Unit;   //屏幕待机时间
  AutoPwr_Cnt = PopMenu3_Value[SYS_PowerOff]*Unit; //自动关机时间
  
  //========================检查 Licence 正确则关闭DEMO窗=======================   
  if(__Info(LIC_OK) == 1){  
    PopType &= ~DEMO_POP;  
    ClosePop();
  }
  else Demo_Pop();
  
  //===================================主循环程序===============================
  while(1){
    //=================读ADC数据=====================   
    SetColor(BLK, WHT);
    Word2Hex(NumStr,  ADC1_2->CDR ); 
    Word2Hex(NumStr,  ADC3_4->CDR ); 
    //====================待机=======================
    if((PopMenu3_Value[SYS_Standy]!=0) && (PD_Cnt == 0)){
      __Ctrl(B_LIGHT,0);                   //关闭背光 
      __Ctrl(PM_CTRL, FLASH1HZ);           //LED闪烁
    }
    //==================自动关机=====================
    if((PopMenu3_Value[SYS_PowerOff] != 0) && (AutoPwr_Cnt == 0)){
      Beep(500);
      __Ctrl(DELAYmS, 500);                //关机前蜂鸣器响0.5s提示 
      __Ctrl(PM_CTRL, CHG_ACT);            //充电模式 LED长亮 
      __Ctrl(PM_CTRL, PWR_OFF);            //power off
    }
    
    if(menu.menu_flag == 1)Show_Menu();
    if((PopType & DEMO_POP)&&!(PopType&(DAILOG_POP|PWR_POP|LIST_POP|FILE_POP)))
      MovePop();                          //未解锁是显示Demo窗口    
    if(About_Flag == 0){                   //显示About时，不刷新波形窗口
      Process();
      __DrawWindow(VRAM_PTR);
    }
    Keys_Detect();                         //按键扫描
    
    //===========隐藏菜单时，只有物理按键有效和触摸“S”键有效===========
    if((menu.menu_flag == 1)||(KeyIn == K_OK)||(KeyIn == K_RUN)||(KeyIn == K_PWR)
       ||(KeyIn == RUN_HOLD)||(KeyIn == DOUBLE_K))
      Key_Flag = 1;
    else Key_Flag = 0;
    
    if(About_Flag == 1){                   //显示About时，只有“M”键和截图有效
      Key_Flag = 0;
      if((KeyIn == K_ACT)||(KeyIn == RUN_HOLD))
        Key_Flag = 1;
    }
    //=================================按键处理=================================     
    
    if(KeyIn && Key_Flag){       
      
      //==========有按键恢复待机和自动关机时间===============
      if((PopMenu3_Value[SYS_Standy] != 0) &&(PD_Cnt == 0)){
        __Ctrl(B_LIGHT, PopMenu3_Value[SYS_BKLight]*10);
        __Ctrl(PM_CTRL, CHG_ACT);          //充电模式 LED长亮 
      }
      PD_Cnt      = PopMenu3_Value[SYS_Standy]*Unit;   //待机时间 
      AutoPwr_Cnt = PopMenu3_Value[SYS_PowerOff]*Unit; //自动关机时间
      
      //=======================按键操作=====================
      switch (KeyIn){
        //-------------机械按键---------------- 
      case PWR_HOLD:
        break;  
        
      case RUN_HOLD:                       //长按RUN键，截图快捷按键
        Beep(50); 
        __Ctrl(DELAYmS, 100);
        Beep(50); 
        FileInfo = Save_Bmp(PopMenu3_Value[SAVE_Bmp]);
        DispFileInfo(FileInfo);
        
        SetColor(DAR, ORN);
        Print_dT_Info(INV);                //显示T1-T2
        Update[T1F] &=~ UPD ;
        break;
        
      case K_PWR:                          //电源键
        if(PopType & PWR_POP){             //在关机窗口下，按电源键关闭窗口
          PopType &= ~PWR_POP;
          ClosePop();
        } 
        break;
        
      case K_RUN:                         //RUN键
        if((PopType & PWR_POP)){          //在关机窗口下
          __Ctrl(B_LIGHT, 0);
          Save_One_Param(Option, SAVE_Bmp) ;
          __Ctrl(PM_CTRL, PWR_OFF);       //power off
        }
        else{                             //做暂停、运行
          if(Status == STOP) {
            Status &= ~STOP;
            if(PopMenu1_Value[TRI_Sync] == SNGL)ADC_Start();
            if(PopMenu1_Value[TRI_Sync] == NORM)ADC_Start();
            Norm_Clr = 1;
            SNGL_Kflag = 1;
            Update_Proc_All();
          }
          else  {
            Status  |=  STOP;
            Ch1_Posi = PopMenu1_Value[CH1_Posi];
            Ch2_Posi = PopMenu1_Value[CH2_Posi];
            Ch3_Posi = PopMenu1_Value[CH3_Posi];
          }
          Update_Status();
        }
        break; 
        
        //-------------触摸模块----------------  
      case HOLD_K:                        //长按
        if(PopType & (FILE_POP)){ 
          ClosePop(); 
          menu.current = Menu_Temp[0];
          menu.menu_index[menu.current] = Menu_Temp[1];
          break;          
        }
        else if(!(PopType & ( DAILOG_POP))){ 
          Menu_Temp[0] = menu.current;
          Menu_Temp[1] = menu.menu_index[menu.current];
          
          ClosePop();   
          PopCnt = 5000;                  //设定 Pop 自动关闭定时 5000mS
          menu.current = Option;
          menu.menu_index[menu.current] = 0;
          Cur_PopItem = 1;
          Show_PopMenu(Cur_PopItem);      //弹出子菜单_Pop
          List_Pop();
        }
        break;
      case DOUBLE_K:                      //双击
        if(!PopMenu1_Value[TRI_Fit]){
          Auto_Fit();
          menu.iflag |= UPD;
        }
        break;
        
      case K_OK:
        if((PopType & PWR_POP)){          //关机窗口
          __Ctrl(B_LIGHT, 0);
          Save_Param();
          __Ctrl(PM_CTRL, PWR_OFF);       //power off  
        }
        else if((PopType & FILE_POP)&&(menu.current == Option)  //文件子菜单窗口
                &&(menu.menu_index[menu.current] == 0)){
                  FileInfo = 1;
                  if(Cur_PopItem == SAVE_PAM) {
                    menu.current = Menu_Temp[0];
                    menu.menu_index[menu.current] = Menu_Temp[1];
                    Save_Param();
                    FileInfo = 0;
                    DispFileInfo(FileInfo);
                    menu.current = Option;
                    menu.menu_index[menu.current] = 0;
                    Show_PopMenu(Cur_PopItem);
                    break;
                  }
                  else if(Cur_PopItem == SAVE_BMP) {
                    ClosePop();
                    __DrawWindow(VRAM_PTR);
                    FileInfo = Save_Bmp(PopMenu3_Value[SAVE_Bmp]); 
                    List_Pop();
                  }
                  else if(Cur_PopItem == SAVE_DAT) {
                    FileInfo = Save_Dat(PopMenu3_Value[SAVE_Dat]);
                  }
                  else if(Cur_PopItem == SAVE_BUF) {
                    FileInfo = Save_Buf(PopMenu3_Value[SAVE_Buf]);
                  }
                  else if(Cur_PopItem == SAVE_CSV) {
                    FileInfo = Save_Csv(PopMenu3_Value[SAVE_Csv]);
                  }
                  else if(Cur_PopItem == LOAD_DAT) {
                    FileInfo = Load_Dat(PopMenu3_Value[LOAD_Dat]);
                  }
                  else if(Cur_PopItem == LOAD_BUT) {
                    FileInfo = Load_Buf(PopMenu3_Value[LOAD_Buf]);
                  }
                  Show_PopMenu(Cur_PopItem);
                  Show_Title();
                  DispFileInfo(FileInfo);
                  if(PopType & DAILOG_POP)  ClosePop();
                  break;
                }
        
        else if((menu.current == Option) && (menu.menu_index[menu.current] == 3)
                && (PopType & (LIST_POP |DAILOG_POP))){   //CAL校准选项
                  if(Cur_PopItem == CAL_ZERO) {
                    if(CalPop_Flag == 1){
                      Dialog_Pop("Auto Calibration?");
                      PopCnt = 5000;
                      CalPop_Flag = 0;
                      break;
                    }
                    if(CalPop_Flag == 0){
                      if(PopType & DAILOG_POP){          //DAILOG_POP对话框选择
                        Save_Kpg(); 
                        ClosePop();
                        CalPop_Flag = 1;
                      }
                      else if(PopType & LIST_POP){       //MENU_POP对话框选择
                        if(Cur_PopItem == CAL_ZERO) {
                          ClosePop();
                          Tips_Pop("Waiting for Calibration ...");
                          __DrawWindow(VRAM_PTR);        //刷新界面
                          Zero_Align();
                          Update_Proc_All();
                          ClosePop();
                          Dialog_CalPop("Cal completed,Save data?",48,110,32,26*6);
                          PopCnt = 5000;
                        }
                      }
                    }
                  }
                  else if(Cur_PopItem == RES_DATA) {
                    if(ResPop_Flag ==1){
                      Dialog_Pop("  Restore Data ?");
                      PopCnt = 5000;
                      ResPop_Flag = 0;
                      break;
                    }
                    if(ResPop_Flag == 0){
                      if(PopType & DAILOG_POP){
                        menu.current = Oscillo;
                        menu.menu_index[Oscillo] = 0;
                        Save_Param();
                        ClosePop();
                        menu.mflag |= UPD;               //menu选项
                        Show_Menu();                     //主菜单和选项 
                        ResPop_Flag = 1;
                      }
                      else if(PopType & LIST_POP){
                        Restore_OrigVal();
                        menu.current = Option;
                        menu.menu_index[Option] = 3;
                        Show_Title();                   //显示
                        Show_Menu();                    //主菜单和选项   
                        Update_Proc_All();              //操作刷新
                        ClosePop();
                        Dialog_CalPop(" Restored,Save Setting?", 90, 50, 32, 26*6);
                        PopCnt = 5000;                
                      }
                    }
                  }
                  break;
                }
        else if(!(PopType & LIST_POP)){ 
          if(menu.menu_flag){                          //隐藏菜单窗口
            MenuCnt = 0;
            menu.menu_flag = 0;
            ParamTab[M_WX] = 300;
            Clear_Label_R(DAR);                        //擦除右栏
          }
          else{                                        //弹出菜单窗口
            if(__Info(LIC_OK) == 1);                   //切换窗口时demo复位
            else if((ParamTab[PXx1]+ParamTab[PWx1]) >= (WIDTH_MINI+1))Demo_Pop();
            ParamTab[M_WX] = WIDTH_MINI;
            menu.menu_flag = 1;
            Show_Menu();
            menu.mflag |= UPD;
          }
        }
        break;
        
      case K_ACT:
        if(PopType & PWR_POP){           //在关机窗口下，按电源键关闭窗口
          PopType &= ~PWR_POP;
          ClosePop();
        }
        
        else if(!(PopType & (LIST_POP|DAILOG_POP|FILE_POP))){ 
          //无子窗口时，打开子窗口
          PopCnt = 5000;                 // 设定 Pop 自动关闭定时 5000mS
          Cur_PopItem = 1;               // 子窗口默认选项为第一选项
          Show_PopMenu(Cur_PopItem);     //弹出子菜单_Pop
          if(PopType & FILE_POP){//如果弹出文件管理子窗口，记录当前页和当前选项
            Menu_Temp[0] = menu.current;
            Menu_Temp[1] = menu.menu_index[menu.current];
          }
          if((menu.menu_index[menu.current] != 5)||(menu.current == 0))
            List_Pop();                  //电池电压和about不弹出窗口
        }
        
        else if(PopType & (LIST_POP|DAILOG_POP|FILE_POP)){ 
          //有子窗口时，关闭子窗口          
          if(PopType & FILE_POP){//文件管理子窗口,恢复打开时的当前页和选项
            menu.current = Menu_Temp[0];
            menu.menu_index[menu.current] = Menu_Temp[1];
          } 
          ClosePop();
          CalPop_Flag  = 1;             //Auto_Cal?
          ResPop_Flag  = 1;             //Restore?
          Windows_Flag = 0;             //关闭windows
          Update_Windows();       
        }
        break;
        
      case K_UP:
        if((PopType & LIST_POP)|| (Windows_Pop == 1)){   //子菜单Pop选择
          if((menu.current == Option) && (menu.menu_index[menu.current] == 1) 
             && (PopMenu3_Value[WAVE_Type] > 0)){
               if(Cur_PopItem <= 1)           //menu_key_chose
                 Cur_PopItem = Cur_Limit-1 ;  //模拟输出时，DUTY不循环
               else Cur_PopItem--;            
             }else{
               if(Cur_PopItem <= 1)           //menu_key_chose
                 Cur_PopItem = Cur_Limit ;
               else Cur_PopItem--;
             }
          Show_PopMenu(Cur_PopItem);
          menu.mflag &= ~UPD;
          menu.iflag &= ~UPD;
	  CalPop_Flag = 1;
          ResPop_Flag = 1;
        }
        else if(PopType & FILE_POP){       //文件管理Pop时按键选择
          menu.current = Option;
          menu.menu_index[menu.current] = 0;
          if(Cur_PopItem <= 1)             //menu_key_chose
            Cur_PopItem = Cur_Limit ;
          else Cur_PopItem--;
          Show_PopMenu(Cur_PopItem);
          menu.mflag &= ~UPD;
          menu.iflag &= ~UPD;
        }
        else if(!(PopType & (DAILOG_POP | PWR_POP))){     //主菜单选择
          if(menu.menu_index[menu.current] <= 0)          //menu_key_chose
            menu.menu_index[menu.current] = Menu_Limit[menu.current]-1;
          else 
            menu.menu_index[menu.current]--;
          menu.iflag |= UPD;
        }
        break;
        
      case K_DOWN:
        if((PopType & LIST_POP)|| (Windows_Pop == 1)){
          if((menu.current == Option)&&
             (menu.menu_index[menu.current] == 1)
               &&(PopMenu3_Value[WAVE_Type] > 0)){
                 if(Cur_PopItem >= Cur_Limit-1)           //menu_key_chose
                   Cur_PopItem = 1 ;
                 else Cur_PopItem++;
                 
               }else{
                 if(Cur_PopItem >= Cur_Limit  )           //menu_key_chose
                   Cur_PopItem = 1;
                 else 
                   Cur_PopItem++;
               }
          Show_PopMenu(Cur_PopItem);
          menu.mflag &= ~UPD;
          menu.iflag &= ~UPD;
          CalPop_Flag = 1;
          ResPop_Flag = 1;
        }
        else if(PopType & FILE_POP){                      //文件管理Pop时按键
          menu.current = Option;
          menu.menu_index[menu.current] = 0;
          if(Cur_PopItem >= Cur_Limit  )                  //menu_key_chose
            Cur_PopItem = 1;
          else 
            Cur_PopItem++;
          Show_PopMenu(Cur_PopItem);
          menu.mflag &= ~UPD;
          menu.iflag &= ~UPD;
        }
        else if(!(PopType & (DAILOG_POP | PWR_POP))){
          if(menu.menu_index[menu.current] >= Menu_Limit[menu.current]-1)
            menu.menu_index[menu.current] = 0;            //menu_key_chose
          else 
            menu.menu_index[menu.current]++;
          menu.iflag |= UPD;
        }
        break;        
        
      case K_LEFT:
        
        if(PopType & FILE_POP) {                          //文件管理Pop时按键
          PMenu_Proc(dec, Cur_PopItem, 0);
          menu.mflag &= ~UPD;
          menu.iflag &= ~UPD;
        }
        else {
          if((PopType & LIST_POP)|| (Windows_Pop == 1)) {
            PMenu_Proc(dec, Cur_PopItem, 0);
            CalPop_Flag = 1;
            ResPop_Flag = 1;
          }
          else if(!(PopType & (DAILOG_POP | PWR_POP)))  Item_Proc(dec);
        }
        break;
        
      case K_RIGHT:
        
        if(PopType & FILE_POP) {                      //文件管理Pop时按键
          PMenu_Proc(add, Cur_PopItem, 0);
          menu.mflag &= ~UPD;
          menu.iflag &= ~UPD;
        }
        else{
          if((PopType & LIST_POP) || (Windows_Pop == 1)) {
            PMenu_Proc(add, Cur_PopItem, 0);
            CalPop_Flag = 1;
            ResPop_Flag = 1;
          }
          else if(!(PopType & (DAILOG_POP | PWR_POP)))   Item_Proc(add);
        }
        break;
        
      }//----switch end-----//
      if((KeyIn == DOUBLE_K)){ 
        if(!PopMenu1_Value[TRI_Fit])Beep(50);        //开Auto_Fit时，双击响声
      }
      else Beep(50);
      
      KeyIn = 0;
      Update_Label();
    }//---Key_In end------
    
    // ===============================APP 关机处理==============================
    if(__Info(KEY_IN) != K_PWR) PwrCnt = 2000+1000;  //弹出关机提示窗定时 2000mS
    if(PwrCnt < 1000){                               //退出关机提示窗定时 1000mS
      if(PwrCnt) {
        Power_Pop("   Power Option  ", 75, 80, 56, 24*6);
      }
    }
    
    if(About_Flag == 0){                             //测量数据定时刷新
      if((Label_Cnt == 50)){
        Label_Cnt = 0;
        Label_Flag |= UPD;
        Update_Label();   
        Print_dT_Info(INV);
        Print_dV_Info(INV);
        Battery_update();
        if((menu.menu_flag == 1) && (menu.current == Measure))Show_Measure();
      }else Label_Cnt++;
    }
  }
}

/******************************** END OF FILE *********************************/
