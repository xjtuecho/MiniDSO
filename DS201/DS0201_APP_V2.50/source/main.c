/******************** (C) COPYRIGHT 2009 e-Design Co.,Ltd. *********************
  File Name: main.c
     Author: bure
   Hardware: DS0201V1.1~1.6
    Version: Ver 2.01
********************************************************************************
 Ver 2.01 版本修改说明
 1.修改了SCAN模式中在低速时显示花屏的BUG
 2.增加了保存现场配置参数的功能，参数保存在CONFIG.DAT文件中
 3.增加了触发点标识显示
 4.修改了Vp-p读数出错的BUG
 5.修改了电池容量测量及显示出错的BUG
 6.修改了2G的SD卡读写出错的BUG
********************************************************************************
 Ver 2.02 版本修改说明
 1.修改电池容量为FULL时，电压为3.9V
********************************************************************************
 Ver 2.20 版本修改说明
 1. 将应用程序分为LIB和APP两大部分，每一部分代码均小于32KB，方便使用正版的IAR（32K限制版）开发软件
 2. 将工作参数存放区从SD卡上改到机内的FLASH上，在没有SD卡时仍然可以保存工作参数
 3. 增加LCD屏幕块写入功能，提高显示效率
 4. 修改了参考波形的显示和消隐方式：按M键切换“显示/消隐”
 5. 修改了调整触发灵敏度（回差）时的表示方式
********************************************************************************
 Ver 2.21 版本修改说明
 1. 增加用波形亮度表示信号变化速率的功能
********************************************************************************
 Ver 2.30 版本修改说明
 1. 修改背景坐标网格的式样
 2. 修改了开机提示信息中的语法错误
 3. UI操作方式改动：统一以左右键选择菜单，上下键改变菜单选项
 4. 在扫描同步方式中，增加了“FIT”自适应功能档，可以根据所测量的信号频率，自动选择合适的扫描时时间
********************************************************************************
 Ver 2.31 版本修改说明
 1. 在“FIT”自适应功能档中，增加了可以根据所测量的信号幅度，
    自动选择合适的Y轴灵敏度档的功能
********************************************************************************
 Ver 2.40 版本修改说明
 1. SIGN模式改名为SING
 2. 修改在SINGLE Mode下，捕捉不到触发的BUG
 3. 修改在NORM Mode下，不能连续触发的BUG
 4. 改善操作响应，切换菜单项的时候先反色，延迟半秒后再开始闪烁
 5. 改移动X.POS时候每次按键移动1点为1格（25点）
 6. 取消用波形亮度表示信号变化速率的功能
 7. 按“暂停/运行”键恢复运行时重新刷新屏幕，消除偶然产生的残留像素
********************************************************************************
 Ver 2.50 版本修改说明
 1. 增加了屏幕当前显示图像的BMP文件保存功能
 2. 修改在RUN/HOLD模式转换时，同步锁定偏移的BUG
 3. 增加了在同步模式状态下，按“M”键可以“显示/消隐”参考波形功能
********************************************************************************
校对：yijian 2010.08.26
*******************************************************************************/

#include "Function.h"
#include "Lcd.h"
#include "Calculate.h"
#include "stm32f10x_lib.h"
#include "HW_V1_Config.h"
#include "File.h"
#include "string.h"
#include "ASM_Funtion.h"

int main(void)
{
  int i,j;
/*--------------硬件电路初始化-----------*/
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xC000);
  Timer_Configuration();

/*----------显示开机提示信息页面----------*/
  Display_Str(8, 71, GRN,PRN, "        System Initializing...       ");
  Display_Str(8, 23, YEL,PRN, "   DS0201 Oscilloscope APP Ver2.50   ");
  for(i=0; i<300; ++i)  Signal_Buffer[i]=120;
  for(j=0; j<0x1000; ++j) Scan_Buffer[j]=1940;
  for(i=0; i<304; ++i)  Ref_Buffer [i]=__Get_Ref_Wave(i);
  Read_Parameter();
  Delayms(2000);
  Battery_Detect();

/*--------示波器工作页面显示初始化--------*/
  Clear_Screen(BACKGROUND);       //清屏幕
  Display_Grid();                 //显示背景网格
  Display_Frame();

/*------------示波器工作主循环------------*/
  while (1){
    Update_Item();
    Scan_Wave();
    if(Key_Buffer!=0) {
      Cursor_Counter=25;
      Type=0;
      Update[Item_Index[CURRENT_ITEM]]=1;
      Update[VERNIERS]=1;
      if(Key_Buffer==KEYCODE_PLAY){
        Update[15]=1;
        if(Item_Index[RUNNING_STATUS]==RUN)  Item_Index[RUNNING_STATUS]=HOLD;
        else {
          ADC_Start();
	  Item_Index[RUNNING_STATUS]=RUN;
	  Sync=0;
          Clear_Screen(BACKGROUND);       //清屏幕
          Display_Grid();                 //显示背景网格
          Display_Frame();
          Display_Item();
        }
        Key_Buffer=0;
      }
      if(((~GPIOA_IDR) & 0x0010)&&(Key_Buffer==KEYCODE_MANU)){ // KEY_PLAY & KEY_MANU is push on
        Item_Index[RUNNING_STATUS]=RUN;                        // RUNNING_STATUS always RUN
        if(Write_Parameter()!=FLASH_COMPLETE) Display_Str(89,2,WHITE,PRN,"!Write Error !");
        else                                  Display_Str(89,2,WHITE,PRN,"!Write Finish!");
        Delayms(500);
        Update[Y_VERNIER_1]=Update[Y_VERNIER_2]=1;
        Key_Buffer=0;
      }
      if(((~GPIOA_IDR) & 0x0008)&&(Key_Buffer==KEYCODE_MANU)){ // KEY_B & KEY_MANU is push on
        if(Write_Parameter()!=FLASH_COMPLETE) Display_Str(89,2,WHITE,PRN,"!Write Error !");
        else                                  Display_Str(89,2,WHITE,PRN,"!Write Finish!");
        Delayms(500);
        Update[Y_VERNIER_1]=Update[Y_VERNIER_2]=1;
        Key_Buffer=0;
      }
      if(((~GPIOA_IDR) & 0x0008)&&(Key_Buffer==KEYCODE_LEFT)){ // KEY_B & KEY_LEFT is push on
        Item_Index[CURRENT_ITEM]=X_SENSITIVITY;
        Update[Item_Index[CURRENT_ITEM]]=1;
        if(Item_Index[X_SENSITIVITY]>0){
          Item_Index[X_SENSITIVITY]--;
          ADC_Start();
          Frame=0;
          X_Counter=0;
          Item_Index[X_POSITION]=4096;
          Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
          Sync=0;
        }
        Key_Buffer=0;
      }
      if(((~GPIOA_IDR) & 0x0008)&&(Key_Buffer==KEYCODE_RIGHT)){ // KEY_B & KEY_RIGHT is push on
        Item_Index[CURRENT_ITEM]=X_SENSITIVITY;
        Update[Item_Index[CURRENT_ITEM]]=1;
        if(Item_Index[X_SENSITIVITY]<21){
          Item_Index[X_SENSITIVITY]++;
          ADC_Start();
          Frame=0;
          X_Counter=0;
          Item_Index[X_POSITION]=4096;
          Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
          Sync=0;
        }
        Key_Buffer=0;
      }
      if(((~GPIOA_IDR) & 0x0008)&&(Key_Buffer==KEYCODE_UP)){ // KEY_B & KEY_UP is push on
        Item_Index[CURRENT_ITEM]=Y_SENSITIVITY;
        Update[Item_Index[CURRENT_ITEM]]=1;
        ADC_Start();
        Frame=0;
        X_Counter=0;
        Item_Index[X_POSITION]=4096;
        Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
        Sync=0;
        if((Item_Index[INPUT_ATTENUATOR]==0)&&(Item_Index[Y_SENSITIVITY]<9))
          Item_Index[Y_SENSITIVITY]++; //输入探头衰减=1时
        if((Item_Index[INPUT_ATTENUATOR]==1)&&(Item_Index[Y_SENSITIVITY]<18))
          Item_Index[Y_SENSITIVITY]++; //输入探头衰减=10时
        Key_Buffer=0;
      }
      if(((~GPIOA_IDR) & 0x0008)&&(Key_Buffer==KEYCODE_DOWN)){ // KEY_B & KEY_DOWN is push on
        Item_Index[CURRENT_ITEM]=Y_SENSITIVITY;
        Update[Item_Index[CURRENT_ITEM]]=1;
        ADC_Start();
        Frame=0;
        X_Counter=0;
        Item_Index[X_POSITION]=4096;
        Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
        Sync=0;
        if((Item_Index[INPUT_ATTENUATOR]==0)&&(Item_Index[Y_SENSITIVITY]>0))
          Item_Index[Y_SENSITIVITY]--; //输入探头衰减=1时
        if((Item_Index[INPUT_ATTENUATOR]==1)&&(Item_Index[Y_SENSITIVITY]>11))
          Item_Index[Y_SENSITIVITY]--; //输入探头衰减=10时
        Key_Buffer=0;
      }
      if(Item_Index[Y_SENSITIVITY]!=Ref_Buffer [302]) Hide_Index[REF]=1;
      if(Item_Index[X_SENSITIVITY]!=Ref_Buffer [303]) Hide_Index[REF]=1;

      switch (Item_Index[CURRENT_ITEM]) {
        case SYNC_MODE:
          if(Key_Buffer==KEYCODE_LEFT)  Item_Index[CURRENT_ITEM]=TRIG_LEVEL;
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=Y_SENSITIVITY;
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[REF]=1-Hide_Index[REF];                //（显示/消隐）参考波形。
          }
          if(Key_Buffer==KEYCODE_DOWN){             //指向下一种扫描同步方式  Char_to_Str
            if(Item_Index[SYNC_MODE]>0) Item_Index[SYNC_MODE]--;
            else                        Item_Index[SYNC_MODE]=5;
            ADC_Start();
            Sync=0;
            Frame=0;
            Item_Index[RUNNING_STATUS]=RUN;
            Update[RUNNING_STATUS]=1;
          }
          if(Key_Buffer==KEYCODE_UP){               //指向上一种扫描同步方式
            if(Item_Index[SYNC_MODE]<5) Item_Index[SYNC_MODE]++;
            else                        Item_Index[SYNC_MODE]=0;
            ADC_Start();
	    Sync=0;
            Frame=0;
            Item_Index[RUNNING_STATUS]=RUN;
            Update[RUNNING_STATUS]=1;
          }
          if(Item_Index[SYNC_MODE]==5){
            Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Item_Index[TRIG_SENSITIVITY]=3;
            Hide_Index[VT]=1;
            Update[VERNIERS]=1;
          }

        break;

        case Y_SENSITIVITY:
          if(Key_Buffer==KEYCODE_LEFT)  Item_Index[CURRENT_ITEM]=SYNC_MODE;
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=X_SENSITIVITY;
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[REF]=1-Hide_Index[REF];                //（显示/消隐）参考波形。
          }
          if(Key_Buffer==KEYCODE_UP){
            ADC_Start();
            Frame=0;
            X_Counter=0;
            Item_Index[X_POSITION]=4096;
            Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
            Sync=0;
            if((Item_Index[INPUT_ATTENUATOR]==0)&&(Item_Index[Y_SENSITIVITY]<9))
              Item_Index[Y_SENSITIVITY]++; //输入探头衰减=1时
            if((Item_Index[INPUT_ATTENUATOR]==1)&&(Item_Index[Y_SENSITIVITY]<18))
              Item_Index[Y_SENSITIVITY]++; //输入探头衰减=10时
          }
          if(Key_Buffer==KEYCODE_DOWN){
            ADC_Start();
            Frame=0;
            X_Counter=0;
            Item_Index[X_POSITION]=4096;
            Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
            Sync=0;
            if((Item_Index[INPUT_ATTENUATOR]==0)&&(Item_Index[Y_SENSITIVITY]>0))
              Item_Index[Y_SENSITIVITY]--; //输入探头衰减=1时
            if((Item_Index[INPUT_ATTENUATOR]==1)&&(Item_Index[Y_SENSITIVITY]>11))
              Item_Index[Y_SENSITIVITY]--; //输入探头衰减=10时
          }
          if(Item_Index[Y_SENSITIVITY]!=Ref_Buffer [302]) Hide_Index[REF]=1;
        break;

        case X_SENSITIVITY:
          if(Key_Buffer==KEYCODE_LEFT)  Item_Index[CURRENT_ITEM]=Y_SENSITIVITY;
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=Y_POSITION;
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[REF]=1-Hide_Index[REF];                //（显示/消隐）参考波形。
          }
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[X_SENSITIVITY]<21)){
            Item_Index[X_SENSITIVITY]++;
            ADC_Start();
            Frame=0;
            X_Counter=0;
            Item_Index[X_POSITION]=4096;
            Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
            Sync=0;
          }
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[X_SENSITIVITY]>0)){
            Item_Index[X_SENSITIVITY]--;
            ADC_Start();
            Frame=0;
            X_Counter=0;
            Item_Index[X_POSITION]=4096;
            Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
            Sync=0;
          }
          if(Item_Index[X_SENSITIVITY]!=Ref_Buffer [303]) Hide_Index[REF]=1;
        break;

        case Y_POSITION:
          if(Key_Buffer==KEYCODE_LEFT)  Item_Index[CURRENT_ITEM]=X_SENSITIVITY;
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=MEASUR_KIND;
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[V0]<MAX_Y)){
            Draw_Dot_Vn(Item_Index[V0],ERASE,CH1_COLOR);
            Item_Index[V0]++;
            Hide_Index[V0]=0;
          }                                    //'垂直位移'基线上移，显示基线。
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[V0]>(MIN_Y+1))){
            Draw_Dot_Vn(Item_Index[V0],ERASE,CH1_COLOR);
            Item_Index[V0]--;
            Hide_Index[V0]=0;
          }                                   //'垂直位移'基线下移，显示基线。
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[V0]=1-Hide_Index[V0];                //（显示/消隐）垂直位移基线。
            if(Hide_Index[V0]==0) Hide_Index[TP]=1;
          }
        break;

        case MEASUR_KIND:
          if(Key_Buffer==KEYCODE_LEFT) Item_Index[CURRENT_ITEM]=Y_POSITION;
          if(Key_Buffer==KEYCODE_RIGHT){
            Hide_Index[VS]=0;
            Item_Index[CURRENT_ITEM]=TRIG_SENSITIVITY;
          }
          if(Key_Buffer==KEYCODE_UP){
            if(Item_Index[MEASUR_KIND]<6) Item_Index[MEASUR_KIND]++;  //选择下一种测量类型
            else                          Item_Index[MEASUR_KIND]=0;
          }
          if(Key_Buffer==KEYCODE_DOWN){
            if(Item_Index[MEASUR_KIND]>0) Item_Index[MEASUR_KIND]--;  //选择上一种测量类型
            else                          Item_Index[MEASUR_KIND]=6;
          }
        break;  //测量类型共有：FREQN、CYCLE、DUTY、Vp-p、Vrms、Vavg DCV 7种

        case TRIG_SENSITIVITY:
          if(Key_Buffer==KEYCODE_LEFT){
            Hide_Index[VS]=1;
            Item_Index[CURRENT_ITEM]=MEASUR_KIND;
            Update[Y_VERNIER_2]=1;
          }
          if(Key_Buffer==KEYCODE_RIGHT){
            Item_Index[CURRENT_ITEM]++;//=TRIG_SLOPE;
            Hide_Index[VS]=1;
          }
          if(Key_Buffer==KEYCODE_MANU)  Hide_Index[VT]=1-Hide_Index[VT];  //（显示/消隐）触发电平基线
          if((Key_Buffer==KEYCODE_UP)&&((Item_Index[TRIG_SENSITIVITY]+Item_Index[VT])<(MAX_Y-1))
             &&((Item_Index[VT]-Item_Index[TRIG_SENSITIVITY])>(MIN_Y+3))){
            Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Item_Index[TRIG_SENSITIVITY]++;                      //降低触发灵敏度
          }
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[TRIG_SENSITIVITY]>0)){
            Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Item_Index[TRIG_SENSITIVITY]--;                      //提高触发灵敏度
          }
        break;

        case TRIG_SLOPE:
          if(Key_Buffer==KEYCODE_LEFT){
            Hide_Index[VS]=0;
            Item_Index[CURRENT_ITEM]=TRIG_SENSITIVITY;
          }
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=INPUT_ATTENUATOR;
          if((Key_Buffer==KEYCODE_UP)||(Key_Buffer==KEYCODE_DOWN)){
            Item_Index[TRIG_SLOPE]=1-Item_Index[TRIG_SLOPE];  //选择（上升沿触发/下降沿触发）
          }
        break;

        case INPUT_ATTENUATOR:
          if(Key_Buffer==KEYCODE_LEFT) Item_Index[CURRENT_ITEM]=TRIG_SLOPE;
          if(Key_Buffer==KEYCODE_RIGHT){
            if(SD_Card_ON()) Item_Index[CURRENT_ITEM]=SAVE_WAVE_IMAGE;
            else Item_Index[CURRENT_ITEM]=OUTPUT_FREQUENCY;        //若无SD卡，则指向'频率输出设定'
          }
          if((Key_Buffer==KEYCODE_UP)||(Key_Buffer==KEYCODE_DOWN)){
            Item_Index[INPUT_ATTENUATOR]=1-Item_Index[INPUT_ATTENUATOR];//（×1/×0.1）输入衰减选择
          }
          if((Item_Index[INPUT_ATTENUATOR]==0)&&(Item_Index[Y_SENSITIVITY]>9))
            Item_Index[Y_SENSITIVITY]-=9;
          if((Item_Index[INPUT_ATTENUATOR]==1)&&(Item_Index[Y_SENSITIVITY]<10))
            Item_Index[Y_SENSITIVITY]+=9;
        break;

        case SAVE_WAVE_IMAGE:
          if(SD_Card_ON())
          {
            if(Key_Buffer==KEYCODE_LEFT) Item_Index[CURRENT_ITEM]=INPUT_ATTENUATOR;
            if(Key_Buffer==KEYCODE_RIGHT)  Item_Index[CURRENT_ITEM]=SAVE_WAVE_CURVE;
            if((Key_Buffer==KEYCODE_UP)&&(Item_Index[SAVE_WAVE_IMAGE]<255))
              Item_Index[SAVE_WAVE_IMAGE]++; //波形文件序号号+1
            if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[SAVE_WAVE_IMAGE]>1))
              Item_Index[SAVE_WAVE_IMAGE]--; //波形文件序号号-1
            if(Key_Buffer==KEYCODE_MANU){
              Update[SAVE_WAVE_IMAGE]=0;
              Cursor_Counter=0;
              if(FAT_Info()==0){
                __Char_to_Str(FileNum, Item_Index[SAVE_WAVE_IMAGE]);
                if(Open_BMP_File(FileNum)==0){
                  Display_Str(89,2,WHITE,PRN, "Save Image...");
          if(Writ_BMP_File()==0){
                    if(Item_Index[SAVE_WAVE_IMAGE]<255)
                      Item_Index[SAVE_WAVE_IMAGE]++;
                  } else Display_Str(89,2,WHITE,PRN,"ImageWriteErr");
                } else                Display_Str(89,2,WHITE,PRN,"!File NoPlace!");
              } else                  Display_Str(89,2,WHITE,PRN,"!Micro SD Err!");
            }
          } else {
            Item_Index[CURRENT_ITEM]=OUTPUT_FREQUENCY;                              //若无SD卡，则指向'频率输出设定'
          }
        break;

        case SAVE_WAVE_CURVE:
          if(SD_Card_ON())
          {
            if(Key_Buffer==KEYCODE_LEFT) Item_Index[CURRENT_ITEM]=SAVE_WAVE_IMAGE;
            if(Key_Buffer==KEYCODE_RIGHT){
              Item_Index[CURRENT_ITEM]=LOAD_WAVE_CURVE;
              Hide_Index[REF]=1;                     // 消隐参考波形。
            }
            if((Key_Buffer==KEYCODE_UP)&&(Item_Index[SAVE_WAVE_CURVE]<255))
              Item_Index[SAVE_WAVE_CURVE]++; //波形文件序号号+1
            if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[SAVE_WAVE_CURVE]>1))
              Item_Index[SAVE_WAVE_CURVE]--; //波形文件序号号-1
            if(Key_Buffer==KEYCODE_MANU){
              Update[SAVE_WAVE_CURVE]=0;
              Cursor_Counter=0;
              if(FAT_Info()==0){
                __Char_to_Str(FileNum, Item_Index[SAVE_WAVE_IMAGE]);
                if(Open_DAT_File(FileNum)==0){
		  F_Buff[0]=1;  F_Buff[1]=0;//Ver0001
                  memcpy(F_Buff+2, View_Buffer, 300);
                  memcpy(F_Buff+302, Item_Index, 40);
                  memcpy(F_Buff+342, Hide_Index, 20);
                  if(Write_File()==0){
                    Item_Index[LOAD_WAVE_CURVE]=Item_Index[SAVE_WAVE_CURVE]; //将当前写入的文件序号定为读取文件序号缺省值
                    if(Item_Index[SAVE_WAVE_CURVE]<255){
                      Item_Index[SAVE_WAVE_CURVE]++;
                    }
                  Update[SAVE_WAVE_CURVE]=1;
                  } else  Display_Str(89,2,WHITE,PRN,"!FileWriteErr!");
                } else    Display_Str(89,2,WHITE,PRN,"!File NoPlace!");
              } else      Display_Str(89,2,WHITE,PRN,"!Micro SD Err!");
            }
          } else {
            Item_Index[CURRENT_ITEM]=OUTPUT_FREQUENCY;                              //若无SD卡，则指向'频率输出设定'
          }
        break;

        case LOAD_WAVE_CURVE:
          if(SD_Card_ON()){
            if(Key_Buffer==KEYCODE_LEFT)   Item_Index[CURRENT_ITEM]=SAVE_WAVE_CURVE;
            if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=OUTPUT_FREQUENCY;
            if((Key_Buffer==KEYCODE_UP)&&(Item_Index[LOAD_WAVE_CURVE]<255))
              Item_Index[LOAD_WAVE_CURVE]++; //波形文件序号号+1
            if((Key_Buffer==KEYCODE_DOWN)&&((Item_Index[LOAD_WAVE_CURVE])>1))
              Item_Index[LOAD_WAVE_CURVE]--; //波形文件序号号-1
            if(Key_Buffer==KEYCODE_MANU) {        //按'M'键
              Cursor_Counter=0;
              if(FAT_Info()==0){
                __Char_to_Str(FileNum, Item_Index[SAVE_WAVE_IMAGE]);
                if(Open_DAT_File(FileNum)==0){
                  if(Read_File()==0){
                    memcpy(Ref_Buffer, F_Buff+2, 300);
                    if((F_Buff[0]==1)&&(F_Buff[1]==0)){ //Ver0001
                      memcpy(Item_Index, F_Buff+302, 40);
                      memcpy(Hide_Index, F_Buff+342, 20);
                      memset(Update, 1, 24);
                    }
                  } else  Display_Str(89,2,WHITE,PRN,"!File ReadErr!");
                } else    Display_Str(89,2,WHITE,PRN,"!File NoFound!");
              } else      Display_Str(89,2,WHITE,PRN,"!Micro SD Err!");
              Hide_Index[REF]=0;//;显示新参考样本波形
            }
          } else  Item_Index[CURRENT_ITEM]=OUTPUT_FREQUENCY; //若无SD卡，则指向'频率输出设定'
        break;

        case OUTPUT_FREQUENCY:
          if(Key_Buffer==KEYCODE_LEFT) {
            if(SD_Card_ON()){
              Item_Index[CURRENT_ITEM]=LOAD_WAVE_CURVE;
              Hide_Index[REF]=1;                     // 消隐参考波形。
            }
            else Item_Index[CURRENT_ITEM]=INPUT_ATTENUATOR;  //若无SD卡，则指向'输入衰减选择'
          }
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=X_VERNIER_2;
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[OUTPUT_FREQUENCY]<15))
            Item_Index[OUTPUT_FREQUENCY]++;  //基准频率输出降低一档
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[OUTPUT_FREQUENCY]>0))
            Item_Index[OUTPUT_FREQUENCY]--;  //基准频率输出提高一档
        break;

        case X_VERNIER_2:
          if(Key_Buffer==KEYCODE_LEFT)   Item_Index[CURRENT_ITEM]=OUTPUT_FREQUENCY;
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=X_VERNIER_1;
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[T2]=1-Hide_Index[T2];                  //（显示/消隐）水平测量游标T2
            Draw_Ti(Item_Index[T2],ERASE,LN2_COLOR);
          }
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[T2]<MAX_X)) { //时间测量游标线2右移
            Draw_Dot_Ti(Item_Index[T2],ERASE,LN2_COLOR);
            Draw_Ti(Item_Index[T2],ERASE,LN2_COLOR);
            Item_Index[T2]++;
            Hide_Index[T1]=0;
            Hide_Index[T2]=0;
          }
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[T2]>MIN_X+1)) { //时间测量游标线2左移
            Draw_Dot_Ti(Item_Index[T2],ERASE,LN2_COLOR);
            Draw_Ti(Item_Index[T2],ERASE,LN2_COLOR);
            Item_Index[T2]--;
            Hide_Index[T1]=0;
            Hide_Index[T2]=0;
          }
          Update[DELTA_T]=1;
          Update[VERNIERS]=1;
        break;

        case X_VERNIER_1:
          if(Key_Buffer==KEYCODE_LEFT) Item_Index[CURRENT_ITEM]=X_VERNIER_2;
          if(Key_Buffer==KEYCODE_RIGHT) {
            Item_Index[CURRENT_ITEM]=X_POSITION;
            Draw_View_Area();
          }
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[T1]=1-Hide_Index[T1];                  //（显示/消隐）水平测量游标T1
            Draw_Ti(Item_Index[T1],ERASE,LN2_COLOR);
          }
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[T1]<MAX_X)) { //时间测量游标线1右移
            Draw_Dot_Ti(Item_Index[T1],ERASE,LN2_COLOR);
            Draw_Ti(Item_Index[T1],ERASE,LN2_COLOR);
            Item_Index[T1]++;
            Hide_Index[T1]=0;
            Hide_Index[T2]=0;
          }
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[T1]>MIN_X+1)) { //时间测量游标线1左移
            Draw_Dot_Ti(Item_Index[T1],ERASE,LN2_COLOR);
            Draw_Ti(Item_Index[T1],ERASE,LN2_COLOR);
            Item_Index[T1]--;
            Hide_Index[T1]=0;
            Hide_Index[T2]=0;
          }
          Update[DELTA_T]=1;
          Update[VERNIERS]=1;
        break;

        case X_POSITION:

          Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[TP]=1-Hide_Index[TP];                  //（显示/消隐）触发指示游标Tp
            Draw_Ti(Tp,ERASE,CH2_COLOR);
            if(Hide_Index[TP]==0) Hide_Index[V0]=1;
          }

          if(Key_Buffer==KEYCODE_LEFT) {
            Item_Index[CURRENT_ITEM]=X_VERNIER_1;
            Erase_View_Area();
          }
          if(Key_Buffer==KEYCODE_RIGHT){
            Item_Index[CURRENT_ITEM]=Y_VERNIER_2;
            Update[17]=1;
            Erase_View_Area();
          }
          if(Key_Buffer==KEYCODE_UP){
            Draw_Ti(Tp,ERASE,CH2_COLOR);
            Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
            Stop=0;
            Frame=0;
            Item_Index[X_POSITION]+=25;   //水平显示起始位置右移10
            Draw_View_Area();
          }
          if(Key_Buffer==KEYCODE_DOWN){
            Draw_Ti(Tp,ERASE,CH2_COLOR);
            Draw_Dot_Ti(Tp,ERASE,CH2_COLOR);
            Stop=0;
            Frame=0;
            Item_Index[X_POSITION]-=25; //水平显示起始位置左移25
            Draw_View_Area();
          }
          Update[VERNIERS]=1;
        break;

        case Y_VERNIER_2:
          if(Key_Buffer==KEYCODE_LEFT) {
            Item_Index[CURRENT_ITEM]=X_POSITION;
            Draw_View_Area();
          }
          if(Key_Buffer==KEYCODE_RIGHT) Item_Index[CURRENT_ITEM]=Y_VERNIER_1;
            if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[V2]=1-Hide_Index[V2];                  //（显示/消隐）垂直测量游标V2。
            Draw_Vi(Item_Index[V2],ERASE,LN2_COLOR);
          }
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[V2]<MAX_Y)){
            Draw_Dot_Vn(Item_Index[V2],ERASE,LN2_COLOR);
            Draw_Vi(Item_Index[V2],ERASE,LN2_COLOR);
            Item_Index[V2]++;                               //垂直测量游标V2上移
          }
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[V2]>MIN_Y+1)){
            Draw_Dot_Vn(Item_Index[V2],ERASE,LN2_COLOR);
            Draw_Vi(Item_Index[V2],ERASE,LN2_COLOR);
            Item_Index[V2]--;                               //垂直测量游标V2下移
          }
          Update[VERNIERS]=1;
        break;

        case Y_VERNIER_1:
          if(Key_Buffer==KEYCODE_LEFT) Item_Index[CURRENT_ITEM]=Y_VERNIER_2; //指向'垂直测量游标V2调整'模式
          if(Key_Buffer==KEYCODE_RIGHT)  Item_Index[CURRENT_ITEM]=TRIG_LEVEL; //指向'触发电平设定'模式
          if(Key_Buffer==KEYCODE_MANU){
            Hide_Index[V1]=1-Hide_Index[V1];                  //（显示/消隐）垂直测量游标V1。
            Draw_Vi(Item_Index[V1],ERASE,LN2_COLOR);
          }
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[V1]<MAX_Y)){
            Draw_Dot_Vn(Item_Index[V1],ERASE,LN2_COLOR);
            Draw_Vi(Item_Index[V1],ERASE,LN2_COLOR);
            Item_Index[V1]++;                               //垂直测量游标V1上移
          }
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[V1]>MIN_Y+1)){
            Draw_Dot_Vn(Item_Index[V1],ERASE,LN2_COLOR);
            Draw_Vi(Item_Index[V1],ERASE,LN2_COLOR);
            Item_Index[V1]--;                               //垂直测量游标V1下移
          }
          Update[VERNIERS]=1;
          break;

        case TRIG_LEVEL:
          if(Key_Buffer==KEYCODE_LEFT) Item_Index[CURRENT_ITEM]=Y_VERNIER_1;
          if(Key_Buffer==KEYCODE_RIGHT)  Item_Index[CURRENT_ITEM]=SYNC_MODE;
          if(Key_Buffer==KEYCODE_MANU)  Hide_Index[VT]=1-Hide_Index[VT];  //（显示/消隐）触发电平基线
          if((Key_Buffer==KEYCODE_UP)&&(Item_Index[VT]<(MAX_Y-Item_Index[TRIG_SENSITIVITY]))){
            Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Item_Index[VT]++;                               //触发电平游标Vt上移
          }
          if((Key_Buffer==KEYCODE_DOWN)&&(Item_Index[V1]>(MIN_Y+5+Item_Index[TRIG_SENSITIVITY]))){
            Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
            Draw_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
            Item_Index[VT]--;                               //触发电平游标Vt下移
          }
          Update[VERNIERS]=1;
          break;
      }
      Key_Buffer=0;
    }
  }

  return 0;
}
/********************************* END OF FILE ********************************/
