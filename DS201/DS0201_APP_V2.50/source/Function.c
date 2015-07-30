/******************** (C) COPYRIGHT 2009 e-Design Co.,Ltd. *********************
* File Name: Function.c
     Author: bure
   Hardware: DS0201V1.1~1.6
    Version: V1.0
*******************************************************************************/

#include "Function.h"
#include "Lcd.h"
#include "Calculate.h"
#include "stm32f10x_lib.h"
#include "HW_V1_Config.h"
#include "ASM_Funtion.h"

//------------输入电压量程相关参量定义------------

unsigned const char Item_V[20][11] = //各档位垂直灵敏度显示字符
//    0，        1，        2，        3，        4，       5，        6，        7，         8，        9,   探头衰减×1 时
{"10mV/Div","20mV/Div","50mV/Div","0.1V/Div","0.2V/Div","0.5V/Div"," 1V/Div "," 2V/Div "," 5V/Div "," 10V/Div",
//    10，       11，      12，       13，       14，      15，       16，       17，        18，       19，  探头衰减×10时
 "0.2V/Div","0.5V/Div"," 1V/Div "," 2V/Div "," 5V/Div "," 10V/Div"," 20V/Div"," 50V/Div","100V/Div"," -GND-  "};

unsigned const int V_Scale[20] =    //垂直灵敏度档位倍乘系数
// 0，  1，   2，   3，   4，    5，    6，    7，     8，     9,          探头衰减×1 时
{400, 800, 2000, 4000, 8000, 20000, 40000, 80000, 200000, 400000,
// 10，   11，   12，   13，    14，    15，    16，     17，   18， 19，  探头衰减×10时
 8000, 20000, 40000, 80000, 200000, 400000, 800000, 2000000, 4000000, 0 };

unsigned short Km[20]=    //各档电压补偿系数 K = Km/4069
//  0,    1,   2,   3,    4,   5,   6,    7,   8,   9,   10,  11,  12,   13,  14,  15,   16,  17,  18, 19
{2956, 1478, 591, 296, 1114, 446, 223, 1157, 463, 231, 1452, 581, 290, 1082, 433, 216, 1048, 419, 210, 231};

//        Y_POS校准数组1定义：10mV 20mV 50mV  .1V  .2V  .5V   1V   2V   5V  10V  .2V  .5V   1V   2V   5V  10V  20V  50V 100V default
unsigned short  Y_POSm[20]   ={ 20,  42, 104, 207,  55, 138, 275,  53, 132, 265,  42, 105, 212,  57, 142, 284,  59, 146, 294,   0};
//        Y_POS校准数组2定义：10mV 20mV 50mV  .1V  .2V  .5V   1V   2V   5V  10V  .2V  .5V   1V   2V   5V  10V  20V  50V 100V default
         short  Y_POSn[20]   ={881, 799, 566, 180, 750, 439, -75, 758, 461, -37, 799, 563, 161, 743, 424,-109, 735, 409,-146, 956};

//------------扫描时基量程相关参量定义------------

unsigned const char Item_T[22][12] =  //各档位水平扫描时基显示字符
//    0           1           2            3           4           5           6           7           8          9           10
{" 1uS/Div "," 2uS/Div "," 5uS/Div "," 10uS/Div"," 20uS/Div"," 50uS/Div","100uS/Div","200uS/Div","500uS/Div"," 1mS/Div "," 2mS/Div ",
//    11          12          13           14          15          16          17         18          19          20          21
 " 5mS/Div "," 10mS/Div"," 20mS/Div"," 50mS/Div"," 0.1S/Div"," 0.2S/Div"," 0.5S/Div","  1S/Div ","  2S/Div ","  5S/Div "," 10S/Div "};

unsigned const int T_Scale[22] =    //水平扫描时基档位倍乘系数
//    0           1           2            3           4           5           6           7           8          9           10
//{    1167,       1167,       1167,        1167,       1167,       2000,       4000,       8000,       20000,      40000,      80000,
{     40,         80,         200,        400,        800,        2000,       4000,      8000,       20000,     40000,      80000,
//    11          12          13           14          15          16          17         18          19          20          21
    200000,     400000,     800000,     2000000,    4000000,    8000000,    20000000,   40000000,   80000000,   200000000,  400000000};

unsigned const short Scan_PSC[22] =   //水平扫描时间计数器预分频数值-1
//    0           1           2            3           4           5           6           7           8          9           10
{11,/*40n*/  11,/*80n*/  11,/*200n*/  11,/*400n*/ 11,/*800n*/ 15,/*2u*/   15,/*4u*/   15,/*8u*/   15,/*20u*/  15,/*40u*/  15,/*80u*/
//    11          12          13           14          15          16          17         18          19          20          21
31,/*200u*/  63,/*400u*/ 63,/*800u*/  127,/*2m*/  255,/*4m*/  255,/*8m*/  255,/*20m*/ 511,/*40m*/ 511,/*80m*/511,/*0.2S*/1023,/*0.4S*/};

unsigned const short Scan_ARR[22] =   //水平扫描时间计数器分频数值-1
//    0           1           2            3           4           5           6           7           8          9           10
{ 6,/*40n*/    6,/*80n*/    6,/*200n*/   6,/*400n*/  6,/*800n*/ 8 ,/*2u*/   17,/*4u*/   35,/*8u*/   89,/*20u*/  179,/*40u*/ 359,/*80u*/
//    11          12          13           14          15          16          17         18          19          20          21
449,/*200u*/ 449,/*400u*/ 899,/*800u*/ 1124,/*2m*/ 1124,/*4m*/ 2249,/*8m*/ 5624,/*20m*/ 5624,/*40m*/11249,/*80m*/28124,/*.2*/28124,/*.4*/};

unsigned const short Ks[22] =   //水平扫描时间插值系数
// 0     1     2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21
{ 29860,14930,5972,2986,1493,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024,1024};

//------------基准频率输出相关参量定义------------

unsigned const char Item_F[16][7] =  //各档位频率输出显示字符
//   0       1        2        3         4        5        6        7       8         9       10       11       12       13       14       15
{" 1MHz ","500KHz","200KHz","100KHz"," 50KHz"," 20KHz"," 10KHz"," 5KHZ "," 2KHz "," 1KHz "," 500Hz"," 200Hz"," 100Hz"," 50Hz "," 20 Hz"," 10Hz "};

unsigned const int Fout_PSC[16] =
//   0       1        2        3         4        5        6        7       8         9       10       11       12       13       14       15
{    0,      0,       0,       0,        0,       0,       0,       0,     4-1,      4-1,    16-1,    16-1,    64-1,    64-1,    128-1,   128-1};

unsigned const int Fout_ARR[16] =
//   0       1        2        3         4        5        6        7       8         9       10       11       12       13       14       15
{  72-1,   144-1,   360-1,   720-1,   1440-1,   3600-1,  7200-1,  14400-1, 9000-1,  18000-1, 9000-1,  22500-1, 11250-1, 22500-1, 28125-1, 56250-1};

//-----------------------------------------------------------------------------

unsigned short Scan_Buffer[0x1000]; //扫描采样缓冲区
unsigned char  Signal_Buffer[300];  //波形数据缓冲区：0-299为波形描述，300为Y量程、301为X量程
unsigned char  View_Buffer[300];    //显示波形缓冲区：0-299为波形描述，300为Y量程、301为X量程
unsigned char  Ref_Buffer [304];
/*=   //参考样板缓冲区：0-299为波形描述，300为Y量程、301为X量程
  {100,116,130,144,157,167,175,181,185,185,184,179,173,164,153,141,128,114,100,86, 73, 60, 49, 40, 33, 27, 24, 24, 25, 29,
   35, 43, 52, 63, 75, 87, 100,112,124,135,145,153,160,164,167,167,166,163,157,150,142,133,122,111,100,89, 79, 70, 61, 54,
   48, 44, 42, 42, 43, 46, 50, 57, 64, 72, 81, 90, 100,109,118,126,133,139,144,147,149,149,148,146,142,137,130,124,116,108,
   100,93, 85, 79, 73, 68, 64, 61, 60, 60, 61, 63, 66, 70, 75, 81, 87, 93, 100,106,112,117,122,125,128,130,131,131,131,129,
   126,123,119,115,110,105,100,96, 91, 88, 84, 81, 79, 78, 77, 78, 78, 80, 82, 84, 87, 90, 93, 97, 100,103,106,108,110,112,
   113,114,114,113,113,112,110,109,107,105,104,102,100,99, 97, 96, 96, 95, 95, 95, 95, 96, 96, 97, 97, 98, 99, 99, 100,100,
   100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,
   100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,115,129,141,151,160,165,169,170,169,167,162,156,148,139,130,
   120,110,100,91, 82, 75, 68, 63, 59, 57, 56, 56, 58, 61, 64, 69, 75, 81, 87, 94, 100,106,112,117,121,125,127,129,130,130,
   128,127,124,121,117,113,109,104,100,96, 92, 88, 85, 83, 81, 80, 79, 79, 80, 81, 83, 85, 88, 91, 94, 97, 100,103,106,108,
   108,108,6,7};
*/
//------------------------------------------开机时的初始值定义------------------------------------------------

//        Item_Index数组定义：模式 Y档 X档  V0  计算 电源 TR 极性 衰减 保存 读取 Fout  T2  T1  X.POS  -- Item V2   V1   VT
unsigned short Item_Index[23]={0,   6,  7,  80,   0,   4,  8,  0,   0,   1,   1,   9,  233, 68, 4096,  0, 0,  40, 199, 140, 0, 0, 1};
//        Hide_Index数组定义：模式 REF X档  V0  计算 电源 VS 极性 衰减 保存 读取 Fout  T2  T1   Tp    --  --  V2   V1   VT
unsigned char  Hide_Index[23]={1,   0,  1,   1,   1,  1,  1,  1,   1,   1,   1,   1,    0,  0,   1,    1,  1,  0,  0,   0, 0, 0, 1};

//unsigned short Item_Index[20]={0,  6,  7,  0,  0,   4,  8,  0,   0,  1,   10,  9,  0, 0,  4096, 0, 0, 0, 0, 0};
//unsigned short vt=140, v0=69, v1=199, v2=40, t0=0, t1=68, t2=233;
unsigned short t0=0, Tp;
//unsigned char  Hide_v2=0;
//unsigned char  Item=4;
//------------------------------------------------------------------------------------------------------------
int  Frequency, Cycle, Duty, Vram, Vavg, Vpp, DCV;

unsigned char Update[23]   ={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1};//各Item的刷新标志,数值=1时表示需要刷新
unsigned const char V_Unit[4][3]={"uV","mV","V ","kV"};
unsigned const char T_Unit[4][3]={"nS","uS","mS","S "};
unsigned const char F_Unit[4][4]={"Hz","Hz","KC","MC"};
unsigned const char Battery_Status[5][4]={"~`'","~`}","~|}","{|}","USB"};
unsigned const short Battery_Color [5]  ={RED,  YEL,  GRN,  GRN,  GRN  };
unsigned const char MODE_Unit[6][6]={"AUTO","NORM","SING","NONE","SCAN","!FIT!"};
unsigned const char TEST_Unit[7][8]={" FREQN "," CYCLE ","  DUTY "," V p-p "," V rms "," V avg ","  DC.V "};
unsigned char  FileNum[4]="000";

unsigned short Delay_Counter=0, X_Counter=0, Edge=0, First_Edge, Last_Edge, Wait_CNT;
unsigned char  Counter_20mS=0, Key_Repeat_Counter=0, Key_Wait_Counter=0, Cursor_Counter=0, Type, Key_Repeat_X10;
unsigned char  Key_Buffer=0, Toggle=0, Sync=0, Erase_Note=0, Frame=0, Stop=0;
unsigned int   vb_Sum=360*32, Battery=400;


I32STR_RES Num;

/*******************************************************************************
 delayms: 毫秒（mS）延时程序。 输入: 延时等待的毫秒数值（在72MHz主频情况下）
*******************************************************************************/
void Delayms(u16 mS)
{
	Delay_Counter=mS;
	while(Delay_Counter)
	{
	};
}

/*******************************************************************************
 Sync_Trig: 在扫描缓冲区内寻找第一个满足同步触发条件的点
*******************************************************************************/
void Sync_Trig(void)
{
  int Vs;
  for(t0=150; t0<(0x1000-X_SIZE);++t0){   //判别触发同步位置
    if(t0>=(0x1000-DMA_CNDTR1)) break;    //跳空一个周期，等待A/D转换结束
    Vs=120+(Km[Item_Index[Y_SENSITIVITY]]*(2048-Scan_Buffer[t0]))/4096;   //转换当前波形点的值
    if(Item_Index[TRIG_SLOPE]==0) {       //触发极性为上升沿触发
      if(Vs<(Item_Index[VT]-Item_Index[TRIG_SENSITIVITY])&&(Sync==0)) Sync=1; //低于触发阈值下限
      if(Vs>(Item_Index[VT]+Item_Index[TRIG_SENSITIVITY])&&(Sync==1)) Sync=2; //高于触发阈值上限
    } else {                              //触发极性为下降沿触发
      if(Vs>(Item_Index[VT]+Item_Index[TRIG_SENSITIVITY])&&(Sync==0)) Sync=1; //高于触发阈值上限
      if(Vs<(Item_Index[VT]-Item_Index[TRIG_SENSITIVITY])&&(Sync==1)) Sync=2; //低于触发阈值下限
    }
    if(Sync==2) break;
  }
  X_Counter=0;
  if(t0>=(0x1000-X_SIZE)) Sync=3;//触发同步不成功标志

}

/*******************************************************************************
 Signal_Process: 计算处理数据缓冲区
*******************************************************************************/
void Signal_Process(void)
{
  int i, p, q; int Vs, Vr;
  if(Sync==3) t0=150; //若同步不成功，设定默认起始位置
  p=(Frame*(1024*X_SIZE)/Ks[Item_Index[X_SENSITIVITY]])+t0+Item_Index[X_POSITION]-(4096+150);
  for(i=X_Counter; i<(X_SIZE); ++i){
    Sync=5;           //部分转换完成后转去显示扫描波形
    q=p+(i*1024)/Ks[Item_Index[X_SENSITIVITY]];
    if(q<0) {
      q=0;
      Item_Index[X_POSITION]++;
    }
    if(q>=(0x1000-DMA_CNDTR1)) break;  //跳空一个周期，等待A/D转换结束
    X_Counter=i+1;
    Vr=Km[Item_Index[Y_SENSITIVITY]]*(Scan_Buffer[q+1]-Scan_Buffer[q])/4096;
    Vs=(Km[Item_Index[Y_SENSITIVITY]]*(2048-Scan_Buffer[q]))/4096+120               //当前波形点的主值
      -(((i*1024)%Ks[Item_Index[X_SENSITIVITY]])*Vr)/Ks[Item_Index[X_SENSITIVITY]]; //当前波形点的插值
    if(Vs>MAX_Y)  Vs=MAX_Y;
    else if(Vs<MIN_Y) Vs=MIN_Y;
    Signal_Buffer[i]=Vs;
    Sync=4;        //全部转换完成后转去显示扫描波形
  }
  if(DMA_CNDTR1==0) {
    Measure_Wave();//若采样完成，计算波形的各个测量值
     if(Item_Index[RUNNING_STATUS]==RUN) ADC_Start(); //若在"RUN"模式下，重新采样
  }
}

/*******************************************************************************
 Erase_Draw: 先擦除后显示扫描波形
*******************************************************************************/
void Erase_Draw(void)
{
  unsigned short i;
  unsigned char  y1, y2, y3, y4,y5, y6;

  y1=View_Buffer[0];
  y3=Signal_Buffer[0];
  y5=Ref_Buffer[0];

  for(i=0; i<X_Counter; ++i){
    y2=View_Buffer[i];
    y4=Signal_Buffer[i];
    View_Buffer[i]=y4;
    y6=Ref_Buffer[i];
    Erase_SEG(i,y1,y2,CH1_COLOR);
    Erase_SEG(i,y5,y6,CH3_COLOR);
    Draw_CH1_SEG(i,y3,y4);
    if(Hide_Index[REF]==0) Draw_CH3_SEG(i,y5,y6);
    y1 = y2;
    y3 = y4;
    y5 = y6;
  }

  if(X_Counter>=X_SIZE-1) {//一帧显示完成
    Stop=1;
    X_Counter=0; //窗口指针复零
    Battery_Detect();//检测电池电压
    if((((Frame+2)*X_SIZE)+t0+Item_Index[X_POSITION]-4096)<0x1000) Frame++;//指向下一帧
    else {
      Frame=0;
      ADC_Start();
      Item_Index[X_POSITION]=4096;
    }
    Delay_Counter=100;        //维持波形显示100mS
    if(Item_Index[0]!=4) Sync=0;//非"SCAN"模式下，重新开始处理显示波形
    else Erase_Wave();//在"SCAN"模式下，擦除已经显示的波形，准备下一帧的显示
  } else {
    Sync=2;
    Stop=0;//扫描数据显示未完成，则继续对应帧的数据转换处理和显示
  }
}
/*******************************************************************************
 Scan_Wave: 扫描同步处理，按设定模式显示波形
*******************************************************************************/

void Scan_Wave(void)//6 kinds of SYNC MODE:  AUTO、NORM、SIGN、NONE、SCAN、FIT
{

  //---------------------扫描同步方式为 AUTO or FIT --------------------------
  if((Item_Index[SYNC_MODE]==0)||(Item_Index[SYNC_MODE]>4)) {
    Frame=0;
    if(Item_Index[RUNNING_STATUS]==RUN){
      Stop=0;
      if((Sync==0)||(Sync==1)) Sync_Trig();            //同步触发
      if((Sync==2)||(Sync==3)) Signal_Process();       //计算处理数据缓冲区
      if((Sync>3)&&(Delay_Counter==0)) Erase_Draw();   //刷新显示扫描波形
    } else {                //在'HOLD'状态下
      Sync=2;
      X_Counter=0;
    }
  }

//--------------------扫描同步方式为 NORM-----------------------------
  if(Item_Index[SYNC_MODE]==1){
    Frame=0;
    if(Item_Index[RUNNING_STATUS]==RUN){
      Stop=0;
      if((Sync==0)||(Sync==1)) Sync_Trig();            //同步触发
      if((Sync==2)||(Sync==3)) Signal_Process();       //计算处理数据缓冲区
      if((Sync>3)&&(Delay_Counter==0)) Erase_Draw();   //刷新显示扫描波形
    } else {                //在'HOLD'状态下
      Sync=2;
      X_Counter=0;
    }
    if(Sync==3) {
      Erase_Wave();                        //擦除旧的扫描波形
      if(DMA_CNDTR1==0) {//若采样完成，重新开始采样
        ADC_Start();
        Item_Index[RUNNING_STATUS]=RUN;
        Sync=0;
      }
    }
  }
//--------------------扫描同步方式为 SING--------------------------
  if(Item_Index[SYNC_MODE]==2){
    Frame=0;
    if(Item_Index[RUNNING_STATUS]==RUN){  //在'RUN'状态下
      Stop=0;
      if((Sync==0)||(Sync==1)) Sync_Trig();            //同步触发
    } else {                //在'HOLD'状态下
      Sync=0;
      X_Counter=0;
    }
    if(Sync==3) {
      Erase_Wave();                        //擦除旧的扫描波形
      if(DMA_CNDTR1==0) {//若采样完成，重新开始采样
        ADC_Start();
        Item_Index[RUNNING_STATUS]=RUN;
        Sync=0;
      }
    }
    if(Sync==2) Signal_Process();       //计算处理数据缓冲区
    if((Sync>3)&&(Delay_Counter==0)){
      Erase_Draw();                     //刷新显示扫描波形
      Item_Index[RUNNING_STATUS]=HOLD;
      Update[RUNNING_STATUS]=1;
    }
  }
//-----------------------扫描同步方式为 NONE------------------------
  if(Item_Index[SYNC_MODE]==3){
    Frame=0; X_Counter=0;
	if(Item_Index[RUNNING_STATUS]==RUN){  //在'RUN'状态下
      Sync=3; Stop=0;
	} else {                //在'HOLD'状态下
      Sync=2;
	}
    if((Sync==2)||(Sync==3)) Signal_Process();                    //计算处理数据缓冲区

    if((Sync>3)&&(Delay_Counter==0)) Erase_Draw(); //刷新显示扫描波形
  }
//-----------------------扫描同步方式为 SCAN------------------------
  if(Item_Index[SYNC_MODE]==4){
    X_Counter=0;
    if(Item_Index[RUNNING_STATUS]==RUN){  //在'RUN'状态下
      Stop=0;
      if(Sync==0) Erase_Wave();
      Sync=3;
      if((Sync==2)||(Sync==3)) Signal_Process();       //计算处理数据缓冲区
      if((Sync>3)&&(Delay_Counter==0)) Erase_Draw();   //刷新显示扫描波形
    } else {                //在'HOLD'状态下
      Sync=2;
      X_Counter=0;
    }
  }
}
/*******************************************************************************
 Measure_Wave: 计算波形的频率、周期、峰峰值、平均值、有效值、占空比
*******************************************************************************/
void Measure_Wave(void)
{
  unsigned short i=0, Vmax=0xFFFF, Vmin=0, Trig=0;
  unsigned int Threshold0, Threshold1, Threshold2, Threshold3;
  int  Vk=0, Vn=0, Vm, Vp, Vq, Tmp1, Tmp2;
  Edge=0,
  First_Edge=0;
  Last_Edge=0;
  Threshold0=2048-((Item_Index[V0]-120)*4096)/Km[Item_Index[Y_SENSITIVITY]];
  Threshold1=2048-((Item_Index[VT]-Item_Index[TRIG_SENSITIVITY]-120)*4096)
    /Km[Item_Index[Y_SENSITIVITY]];
  Threshold2=2048-((Item_Index[VT]+Item_Index[TRIG_SENSITIVITY]-120)*4096)
    /Km[Item_Index[Y_SENSITIVITY]];
  Threshold3=2048-((Item_Index[VT]-120)*4096)/Km[Item_Index[Y_SENSITIVITY]];
  for(i=0;i<0x1000;++i){
    Vk += Scan_Buffer[i];
    if((i>t0)&&(i<t0+300)){
      if(Scan_Buffer[i]<Vmax) Vmax=Scan_Buffer[i];
      if(Scan_Buffer[i]>Vmin) Vmin=Scan_Buffer[i];
    }
    if((Scan_Buffer[i]>Threshold1)&&(Trig==0))  Trig=1;//低于触发阈值下限
    if((Scan_Buffer[i]<Threshold2)&&(Trig==1)) {
      Trig=0;
      if(First_Edge==0) {
        First_Edge=i;
        Last_Edge=i;
        Edge=0;
      } else {
        Last_Edge=i;
        Edge++;
      }
    }
  }
  Vk >>= 12;

  if(Edge!=0){

    Vm = 0;
    Vq = 0;
    for(i=First_Edge; i<Last_Edge; ++i){
      if(Scan_Buffer[i]<Threshold3) Vm++;
      Vp=(4096-Scan_Buffer[i])-Threshold0;
      Vn +=(Vp*Vp)/8;                       //为了防止超界先预除8
      if(Scan_Buffer[i]<Threshold0) Vq+=(Threshold0-Scan_Buffer[i]);
      else                          Vq+=(Scan_Buffer[i]-Threshold0);
    }
    if(Type==PRN) {
      if(Item_Index[X_SENSITIVITY]<5) Frequency =(Edge*(1000000000/1167)/(Last_Edge-First_Edge))*1000;
      else Frequency = (Edge*(1000000000/T_Scale[Item_Index[X_SENSITIVITY]])/(Last_Edge-First_Edge))*1000;
      Cycle = ((Last_Edge-First_Edge)*T_Scale[Item_Index[X_SENSITIVITY]])/Edge;
      Duty = 100000*Vm/(Last_Edge-First_Edge);
      Vram=(((Km[Item_Index[Y_SENSITIVITY]])*__Int_sqrt((Vn/(Last_Edge-First_Edge))*8))>>12)
        *V_Scale[Item_Index[Y_SENSITIVITY]];
      Vavg=(((Km[Item_Index[Y_SENSITIVITY]])*(Vq/(Last_Edge-First_Edge)))>>12)
        *V_Scale[Item_Index[Y_SENSITIVITY]];
    }
  } else {
      Frequency = 0;
      Cycle     = 0;
      Duty      = 0;
      Vram      = 0;
      Vavg      = 0;
  }
  if(Vmin<Vmax) Vmin=Vmax;
  Tmp1 =((Km[Item_Index[Y_SENSITIVITY]]*(Vmin-Vmax))>>12);
  Vpp = Tmp1 * V_Scale[Item_Index[Y_SENSITIVITY]];

  Tmp2 = 120+(Km[Item_Index[Y_SENSITIVITY]]*(2048-Vk))/4096;
  DCV  =(Tmp2-Item_Index[V0]) * V_Scale[Item_Index[Y_SENSITIVITY]];
  if((Item_Index[SYNC_MODE]==5)&&(Wait_CNT==0)){
    if((Edge<20)&&(Item_Index[X_SENSITIVITY]<15)) {
      Item_Index[X_SENSITIVITY]++;
      Update[X_SENSITIVITY]=1;
    }
    if((Edge>60)&&(Item_Index[X_SENSITIVITY]>0 )) {
      Item_Index[X_SENSITIVITY]--;
      Update[X_SENSITIVITY]=1;

    }
    if((Tmp1 <50)&&(Item_Index[INPUT_ATTENUATOR]==0)&&(Item_Index[Y_SENSITIVITY]>0)){
      Item_Index[Y_SENSITIVITY]--;
      Update[Y_SENSITIVITY]=1;
    }
    if((Tmp1 <50)&&(Item_Index[INPUT_ATTENUATOR]==1)&&(Item_Index[Y_SENSITIVITY]>11)){
      Item_Index[Y_SENSITIVITY]--;
      Update[Y_SENSITIVITY]=1;
    }
    if((Tmp1 >150)&&(Item_Index[INPUT_ATTENUATOR]==0)&&(Item_Index[Y_SENSITIVITY]<9 )){
      Item_Index[Y_SENSITIVITY]++;
      Update[Y_SENSITIVITY]=1;
    }
    if((Tmp1 >150)&&(Item_Index[INPUT_ATTENUATOR]==1)&&(Item_Index[Y_SENSITIVITY]<18)){
      Item_Index[Y_SENSITIVITY]++;
      Update[Y_SENSITIVITY]=1;
    }
      Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS], ERASE, LN1_COLOR);
      Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS], ERASE, LN1_COLOR);
      Item_Index[VT]=Tmp2;
      Update[VERNIERS]=1;
  }
  if(Wait_CNT>5) Wait_CNT=0;
  else Wait_CNT++;
}
/*******************************************************************************
 Display_Item: 显示各项目栏内容
*******************************************************************************/
void Display_Item(void)
{
  Display_Str(3,224,GRN,PRN,MODE_Unit[Item_Index[SYNC_MODE]]);//显示扫描同步方式
  Display_Str(40,224,YEL,PRN,Item_V[Item_Index[Y_SENSITIVITY]]);//显示(输入电压)量程
  Display_Str(109,224,YEL,PRN,Item_T[Item_Index[X_SENSITIVITY]]); //显示(扫描速率)量程
  Display_Str(186,224,CH1_COLOR,PRN,"Y.POS");//显示(垂直位移)调节选项
  Display_Str(231,224,WHITE,PRN,TEST_Unit[Item_Index[MEASUR_KIND]]);//显示(测量类型
  if(Test_USB_ON())
    Display_Str(292,224,Battery_Color[4],PRN,Battery_Status[4]);//在外接USB电源时，显示'USB'供电状态
  else
    Display_Str(292,224,Battery_Color[Item_Index[5]],PRN,
                Battery_Status[Item_Index[POWER_INFOMATION]]);//显示(电池状态)
  Display_Str(303, 205, YEL,PRN, "TR");//显示(触发灵敏度选择
  if(Item_Index[TRIG_SLOPE]==RISING) Display_Str(303, 185, YEL,PRN, "^S");//显示上升沿触发极性
  else                               Display_Str(303, 185, YEL,PRN, "_S");//显示下降沿触发极性
  if(Item_Index[INPUT_ATTENUATOR]==0) Display_Str(303, 165, YEL,PRN, "*1");//显示探头衰减×1
  else                                Display_Str(303, 165, YEL,PRN, "10");//显示探头衰减×0.1
  Display_Str(303, 145, YEL,PRN, "SI");//显示(图像保存)
  Display_Str(303, 125, YEL,PRN, "FS");//显示(文件保存)
  Display_Str(303, 105, YEL, PRN,"FL");//显示(文件装入)
  Display_Str(303, 85, YEL,PRN, "Fo");//显示(基准频率输出)
  Display_Str(303, 65,  YEL,PRN, "T2");//显示(测量游标T2)
  Display_Str(303, 45,  YEL,PRN, "T1");//显示(测量游标T1)
  Display_Str(303, 25,  YEL,PRN, "T0");//显示(水平位移)
  Display_Str(284,2,GRN,PRN,"!RUN!");//显示(扫描状态)
  Int32String_sign(&Num, (Item_Index[T2]-Item_Index[T1])*T_Scale[Item_Index[X_SENSITIVITY]], 3);//
  Display_Str(199,2,YEL,PRN,"[T=");//显示(时间测量)
  Display_Str(224,2,YEL,PRN,(unsigned const char *)Num.str);
  Display_Str(224+Num.len*8,2,YEL,PRN," ");
  Display_Str(224+40,2,YEL,PRN,T_Unit[Num.decPos]);
  Int32String_sign(&Num, (Item_Index[V1]-Item_Index[V2])*V_Scale[Item_Index[Y_SENSITIVITY]], 3);//
  Display_Str(88,2, WHITE,PRN,"V1");
  Display_Str(105,2,WHITE,PRN,"-");
  Display_Str(114,2,WHITE,PRN,"V2");
  Display_Str(131,2,WHITE,PRN,"=");
  Display_Str(139,2,WHITE,PRN,(unsigned const char *)Num.str);//显示(电压测量游标1~2)
  Display_Str(139+40,2,WHITE,PRN,V_Unit[Num.decPos]);
  Int32String_sign(&Num, (Item_Index[VT]-Item_Index[V0])*V_Scale[Item_Index[Y_SENSITIVITY]], 3);//
  Display_Str(3,2,YEL,PRN,"Vt"); //显示(触发电压)
  Display_Str(20,2,YEL,PRN,"=");
  Display_Str(28,2,YEL,PRN,(unsigned const char *)Num.str);
  Display_Str(28+40,2,YEL,PRN,V_Unit[Num.decPos]);
  if(Hide_Index[T1]) Draw_Ti(Item_Index[T1],ERASE,LN2_COLOR);
  else        Draw_Ti(Item_Index[T1],ADD,LN2_COLOR);
  if(Hide_Index[T2]) Draw_Ti(Item_Index[T2],ERASE,LN2_COLOR);
  else        Draw_Ti(Item_Index[T2],ADD,LN2_COLOR);
  if(Hide_Index[V1]) Draw_Vi(Item_Index[V1],ERASE,LN2_COLOR);
  else        Draw_Vi(Item_Index[V1],ADD,LN2_COLOR);
  if(Hide_Index[V2]) Draw_Vi(Item_Index[V2],ERASE,LN2_COLOR);
  else        Draw_Vi(Item_Index[V2],ADD,LN2_COLOR);
  if(Hide_Index[VT]){
    Draw_Vn(Item_Index[VT]+Item_Index[VS],ERASE,LN1_COLOR);
    Draw_Vn(Item_Index[VT]-Item_Index[VS],ERASE,LN1_COLOR);
  }else{
    Draw_Vn(Item_Index[VT]+Item_Index[VS],ADD,LN1_COLOR);
    Draw_Vn(Item_Index[VT]-Item_Index[VS],ADD,LN1_COLOR);
  }
  Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS],ADD,LN1_COLOR);
  Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS],ADD,LN1_COLOR);
  Draw_Dot_Vn(Item_Index[V1],ADD,LN2_COLOR);
  Draw_Dot_Vn(Item_Index[V2],ADD,LN2_COLOR);
  Draw_Dot_Ti(Item_Index[T1],ADD,LN2_COLOR);
  Draw_Dot_Ti(Item_Index[T2],ADD,LN2_COLOR);
  if((Tp!=MIN_X)&&(Tp!=MAX_X))  Draw_Dot_Ti(Tp,ADD,CH2_COLOR);            //Draw_Dot_Tp
}
/*******************************************************************************
 Update_Item: 根据Update[x]和Erase[x]的标志，刷新显示各项目栏内容
*******************************************************************************/
void Update_Item(void)
{
  short Tmp;
  if( Update[SYNC_MODE]!=0){                 //刷新(扫描同步方式选择)模式
    Update[SYNC_MODE]=Type;
    Display_Str(3,224,GRN,Type,MODE_Unit[Item_Index[SYNC_MODE]]);
  }
  if( Update[Y_SENSITIVITY]!=0){   //刷新(输入电压)量程档的显示和设置
    Update[Y_SENSITIVITY]=Type;
    Display_Str(40,224,YEL,Type,Item_V[Item_Index[Y_SENSITIVITY]]);
    Update[TRIG_LEVEL]=1;
    Update[Y_VERNIER_1]=1;
    Set_Range(Item_Index[Y_SENSITIVITY]);
    Set_Y_Pos(Item_Index[Y_SENSITIVITY], Item_Index[V0]);
  }
  if( Update[X_SENSITIVITY]!=0){   //刷新(扫描速率)量程档的显示
    Update[X_SENSITIVITY]=Type;
    Display_Str(109,224,YEL,Type,Item_T[Item_Index[X_SENSITIVITY]]);
    Set_Base(Item_Index[X_SENSITIVITY]);
    Update[DELTA_T]=1;
  }
  if( Update[Y_POSITION]!=0){   //刷新(垂直位移)调节选项 Y.Position
    Update[Y_POSITION]=Type;
    Display_Str(186,224,CH1_COLOR,Type,"Y.POS");
    Draw_Dot_Vn(Item_Index[V0],ADD,CH1_COLOR);
    Set_Y_Pos(Item_Index[Y_SENSITIVITY], Item_Index[V0]);
    Update[VERNIERS]=1;
  }
  if( Update[MEASUR_KIND]!=0){   //刷新(测量类型)显示
    Update[MEASUR_KIND]=Type;
    if(Type==0) Display_Str(231,224,WHITE,PRN,TEST_Unit[Item_Index[MEASUR_KIND]]);
    else {
      switch (Item_Index[4]){
        case 0://显示频率
          if(Frequency==0){
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,INV,TEST_Unit[Item_Index[MEASUR_KIND]]);
            } else {
              Display_Str(231,224,WHITE,PRN,TEST_Unit[Item_Index[MEASUR_KIND]]);
            }
          } else {
            Int32String(&Num, Frequency, 4);
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,Type,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,Type,F_Unit[Num.decPos]);
            } else{
              Display_Str(231,224,WHITE,PRN,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,PRN,F_Unit[Num.decPos]);
            }
          }
          break;
        case 1://显示周期
          if(Cycle==0)
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,INV,TEST_Unit[Item_Index[MEASUR_KIND]]);
            } else {
              Display_Str(231,224,WHITE,PRN,TEST_Unit[Item_Index[MEASUR_KIND]]);
            }
          else {
            Int32String(&Num, Cycle, 4);
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,Type,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,Type,T_Unit[Num.decPos]);
            } else{
              Display_Str(231,224,WHITE,PRN,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,PRN,T_Unit[Num.decPos]);
            }
          }
          break;
        case 2://显示占空比
          if(Duty==0)
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,INV,TEST_Unit[Item_Index[MEASUR_KIND]]);
            } else {
              Display_Str(231,224,WHITE,PRN,TEST_Unit[Item_Index[MEASUR_KIND]]);
            }
          else {
            Int32String(&Num, Duty, 4);
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,Type,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,Type,"% ");
            } else{
              Display_Str(231,224,WHITE,PRN,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,PRN,"% ");
            }
          }
          break;
        case 3://显示交流峰峰值
          Int32String(&Num, Vpp, 4);
          if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
            Display_Str(231,224,WHITE,Type,(unsigned const char *)Num.str);
            Display_Str(231+40,224,WHITE,Type,V_Unit[Num.decPos]);
          } else{
            Display_Str(231,224,WHITE,PRN,(unsigned const char *)Num.str);
            Display_Str(231+40,224,WHITE,PRN,V_Unit[Num.decPos]);
          }
          break;
        case 4://显示交流有效值
          if(Vram==0)
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,INV,TEST_Unit[Item_Index[MEASUR_KIND]]);
            } else {
              Display_Str(231,224,WHITE,PRN,TEST_Unit[Item_Index[MEASUR_KIND]]);
            }
          else {
            Int32String(&Num, Vram, 4);
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,Type,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,Type,V_Unit[Num.decPos]);
            } else{
              Display_Str(231,224,WHITE,PRN,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,PRN,V_Unit[Num.decPos]);
            }
          }
          break;
        case 5://显示交流平均值
          if(Vavg==0)
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,INV,TEST_Unit[Item_Index[MEASUR_KIND]]);
            } else {
              Display_Str(231,224,WHITE,PRN,TEST_Unit[Item_Index[MEASUR_KIND]]);
            }
          else {
            Int32String(&Num, Vavg, 4);
            if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
              Display_Str(231,224,WHITE,Type,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,Type,V_Unit[Num.decPos]);
            } else{
              Display_Str(231,224,WHITE,PRN,(unsigned const char *)Num.str);
              Display_Str(231+40,224,WHITE,PRN,V_Unit[Num.decPos]);
            }
          }
          break;
        case 6://显示直流平均值
          Int32String_sign(&Num, DCV, 3);
          if(Item_Index[CURRENT_ITEM]==MEASUR_KIND){
            Display_Str(231,224,WHITE,Type,(unsigned const char *)Num.str);
            Display_Str(231+40,224,WHITE,Type,V_Unit[Num.decPos]);
          } else{
            Display_Str(231,224,WHITE,PRN,(unsigned const char *)Num.str);
            Display_Str(231+40,224,WHITE,PRN,V_Unit[Num.decPos]);
          }
          break;
      }
    }
  }

  if( Update[POWER_INFOMATION]!=0){   //刷新(电池状态)显示
    Update[POWER_INFOMATION]=Type;
    if(Test_USB_ON())
      Display_Str(292,224,Battery_Color[4],PRN,Battery_Status[4]);//在外接USB电源时，供电状态栏显示'USB'
    else
      Display_Str(292,224,Battery_Color[Item_Index[5]],PRN,
                  Battery_Status[Item_Index[POWER_INFOMATION]]);
  }

  if( Update[TRIG_SENSITIVITY]!=0){   //刷新(触发灵敏度选择)显示
    Update[TRIG_SENSITIVITY]=Type;
    Display_Str(303, 205, YEL,Type, "TR");
    if(Item_Index[CURRENT_ITEM]==TRIG_SENSITIVITY){
      Display_Str(88,2,WHITE,Type,"Trig!Sensitive");
    }
  }

  if( Update[TRIG_SLOPE]!=0){   //刷新(触发极性)显示
    Update[TRIG_SLOPE]=Type;
    if(Item_Index[TRIG_SLOPE]==RISING) Display_Str(303, 185, YEL,Type, "^S");//上升沿触发
    else                               Display_Str(303, 185, YEL,Type, "_S");//下降沿触发
    if(Item_Index[CURRENT_ITEM]==TRIG_SLOPE){
      if(Item_Index[TRIG_SLOPE]==0) Display_Str(88,2,WHITE,Type,"!Trig.Slope=!^!");
      else                          Display_Str(88,2,WHITE,Type,"!Trig.Slope=!_!");
    }
  }

  if( Update[INPUT_ATTENUATOR]!=0){   //刷新(输入探头衰减)显示
    Update[INPUT_ATTENUATOR]=Type;
    if(Item_Index[INPUT_ATTENUATOR]==0) Display_Str(303, 165, YEL,Type, "*1");//输入×1
    else                                Display_Str(303, 165, YEL,Type, "10");//输入×0.1
    if(Item_Index[CURRENT_ITEM]==INPUT_ATTENUATOR){
      if(Item_Index[INPUT_ATTENUATOR]==0) Display_Str(88,2,WHITE,Type,"!INP.Scale!=*1!");
      else                                Display_Str(88,2,WHITE,Type,"!INP.Scale=*10");
    }
    Update[Y_SENSITIVITY]=1;
  }
  if( Update[SAVE_WAVE_IMAGE]!=0){   //刷新(图像保存)显示
    Update[SAVE_WAVE_IMAGE]=Type;
    Display_Str(303, 145, YEL,Type, "SI");
    if(Item_Index[CURRENT_ITEM]==SAVE_WAVE_IMAGE){
      __Char_to_Str(FileNum,Item_Index[SAVE_WAVE_IMAGE]);
      Display_Str(88,2,WHITE,Type,"!Save Image");
      Display_Str(172,2,WHITE,Type,FileNum);
    }
  }
  if( Update[SAVE_WAVE_CURVE]!=0){   //刷新(文件保存)显示
    Update[SAVE_WAVE_CURVE]=Type;
    Display_Str(303, 125, YEL,Type, "FS");
    if(Item_Index[CURRENT_ITEM]==SAVE_WAVE_CURVE){
      __Char_to_Str(FileNum,Item_Index[SAVE_WAVE_CURVE]);
      Display_Str(88,2,WHITE,Type,"!Save File");
      Display_Str(164,2,WHITE,Type,FileNum);
      Display_Str(188,2,WHITE,Type," ");
    }
  }
  if( Update[LOAD_WAVE_CURVE]!=0){   //刷新(文件装入)显示
    Update[LOAD_WAVE_CURVE]=Type;
    Display_Str(303, 105, YEL, Type,"FL");
    if(Item_Index[CURRENT_ITEM]==LOAD_WAVE_CURVE){
      __Char_to_Str(FileNum,Item_Index[LOAD_WAVE_CURVE]);
      Display_Str(88,2,WHITE,Type,"!Load File");
      Display_Str(164,2,WHITE,Type,FileNum);
      Display_Str(188,2,WHITE,Type," ");
    }
  }

  if( Update[OUTPUT_FREQUENCY]!=0){   //刷新(基准频率输出)显示
    Update[OUTPUT_FREQUENCY]=Type;
    TIM4_PSC = Fout_PSC[Item_Index[OUTPUT_FREQUENCY]];
    TIM4_ARR = Fout_ARR[Item_Index[OUTPUT_FREQUENCY]];
    TIM4_CCR1 = (Fout_ARR[Item_Index[OUTPUT_FREQUENCY]]+1)/2;
    Display_Str(303, 85, YEL,Type, "Fo");
    if(Item_Index[CURRENT_ITEM]==11){
      Display_Str(88,2,WHITE,Type," Fout!=");
      Display_Str(140,2,WHITE,Type,Item_F[Item_Index[OUTPUT_FREQUENCY]]);
      Display_Str(188,2,WHITE,Type," ");
    }
  }

  if( Update[X_VERNIER_2]!=0){   //刷新(测量游标T2)显示
    Update[X_VERNIER_2]=Type;
    Display_Str(303, 65,  YEL,Type, "T2");
    if(Item_Index[CURRENT_ITEM]==X_VERNIER_2){
      Display_Str(88,2,WHITE,Type,"!Time Cursor2 ");
      Draw_Dot_Ti(Item_Index[T1],ADD,LN2_COLOR);
    }
  }

  if( Update[X_VERNIER_1]!=0){   //刷新(测量游标T1)显示
    Update[X_VERNIER_1]=Type;
    Display_Str(303, 45,  YEL,Type, "T1");
    if(Item_Index[CURRENT_ITEM]==X_VERNIER_1){
      Display_Str(88,2,WHITE,Type,"!Time Cursor1 ");
      Draw_Dot_Ti(Item_Index[T1],ADD,LN2_COLOR);
    }
  }

  if( Update[X_POSITION]!=0){   //刷新(水平位移调节)显示
    Update[X_POSITION]=Type;
    Display_Str(303, 25,  YEL,Type, "T0");
    if(Item_Index[CURRENT_ITEM]==X_POSITION){
      Display_Str(88,2,WHITE,Type,"!X.POS Adjust ");
    }
  }

  if( Update[RUNNING_STATUS]!=0){   //刷新(扫描状态)显示
    Update[RUNNING_STATUS]=0;
      if(Item_Index[RUNNING_STATUS]==0) Display_Str(284,2,GRN,PRN,"!RUN!");
      else  Display_Str(284,2,RED,PRN,"HOLD");
  }

  if( Update[DELTA_T]!=0){   //刷新(时间测量)显示
    Update[DELTA_T]=0;
    Int32String_sign(&Num, (Item_Index[T2]-Item_Index[T1])*T_Scale[Item_Index[X_SENSITIVITY]], 3);//
    Display_Str(199,2,YEL,PRN,"[T=");
    Display_Str(224,2,YEL,PRN,(unsigned const char *)Num.str);
    Display_Str(224+Num.len*8,2,YEL,PRN," ");
    Display_Str(224+40,2,YEL,PRN,T_Unit[Num.decPos]);
  }

  if( Update[Y_VERNIER_2]!=0){   //刷新(电压测量游标2)显示
    Update[Y_VERNIER_2]=Type;
    if(Item_Index[CURRENT_ITEM]!=Y_VERNIER_2) Tmp=PRN;
    else                  Tmp=Type;
    if((Item_Index[CURRENT_ITEM]<6)||(Item_Index[CURRENT_ITEM]>14)){
      Int32String_sign(&Num, (Item_Index[V1]-Item_Index[V2])*V_Scale[Item_Index[Y_SENSITIVITY]], 3);//
      Display_Str(88,2, WHITE,PRN,"V1");
      Display_Str(105,2,WHITE,PRN,"-");
      Display_Str(114,2,WHITE,Tmp,"V2");
      Display_Str(131,2,WHITE,PRN,"=");
      Display_Str(139,2,WHITE,PRN,(unsigned const char *)Num.str);
      Display_Str(139+40,2,WHITE,PRN,V_Unit[Num.decPos]);
    }
  }

  if( Update[Y_VERNIER_1]!=0){   //刷新(电压测量游标1)显示
    Update[Y_VERNIER_1]=Type;
    if(Item_Index[CURRENT_ITEM]!=Y_VERNIER_1) Tmp=PRN;
    else                  Tmp=Type;
    if((Item_Index[CURRENT_ITEM]<6)||(Item_Index[CURRENT_ITEM]>14)){
      Int32String_sign(&Num, (Item_Index[V1]-Item_Index[V2])*V_Scale[Item_Index[Y_SENSITIVITY]], 3);//
      Display_Str(88,2,WHITE,Tmp,"V1");
      Display_Str(105,2,WHITE,PRN,"-");
      Display_Str(139,2,WHITE,PRN,(unsigned const char *)Num.str);
      Display_Str(139+40,2,WHITE,PRN,V_Unit[Num.decPos]);
    }
  }

  if( Update[TRIG_LEVEL]!=0){   //刷新(触发电压)显示
    Update[TRIG_LEVEL]=Type;
    Update[VERNIERS]=1;
    if(Item_Index[CURRENT_ITEM]!=TRIG_LEVEL) Tmp=PRN;
    else                 Tmp=Type;
    Int32String_sign(&Num, (Item_Index[VT]-Item_Index[V0])*V_Scale[Item_Index[Y_SENSITIVITY]], 3);//
    Display_Str(3,2,YEL,Tmp,"Vt");
    Display_Str(20,2,YEL,PRN,"=");
    Display_Str(28,2,YEL,PRN,(unsigned const char *)Num.str);
    Display_Str(28+40,2,YEL,PRN,V_Unit[Num.decPos]);
  }

  if(Hide_Index[VS]){
    Hide_Index[VS]=0;
  }

  if(Update[VERNIERS]){
    Update[VERNIERS]=0;
    Tp = MIN_X+150+Item_Index[X_POSITION]-4096;
    if(Tp > MIN_X+X_SIZE) Tp = MAX_X;
    if(Tp < MIN_X)        Tp = MIN_X;
    if(Hide_Index[TP]) Draw_Ti(Tp,ERASE,CH2_COLOR);
    else {
      if((Tp!=MIN_X)&&(Tp!=MAX_X))  Draw_Ti(Tp,ADD,CH2_COLOR);
    }
    if(Hide_Index[T1]) Draw_Ti(Item_Index[T1],ERASE,LN2_COLOR);
    else        Draw_Ti(Item_Index[T1],ADD,LN2_COLOR);
    if(Hide_Index[T2]) Draw_Ti(Item_Index[T2],ERASE,LN2_COLOR);
    else        Draw_Ti(Item_Index[T2],ADD,LN2_COLOR);
    if(Hide_Index[V1]) Draw_Vi(Item_Index[V1],ERASE,LN2_COLOR);
    else        Draw_Vi(Item_Index[V1],ADD,LN2_COLOR);
    if(Hide_Index[V2]) Draw_Vi(Item_Index[V2],ERASE,LN2_COLOR);
    else        Draw_Vi(Item_Index[V2],ADD,LN2_COLOR);
    if(Hide_Index[VT]){
      Draw_Vn(Item_Index[VT]+Item_Index[VS],ERASE,LN1_COLOR);
      Draw_Vn(Item_Index[VT]-Item_Index[VS],ERASE,LN1_COLOR);
    }else{
      Draw_Vn(Item_Index[VT]+Item_Index[VS],ADD,LN1_COLOR);
      Draw_Vn(Item_Index[VT]-Item_Index[VS],ADD,LN1_COLOR);
    }
    Draw_Dot_Vn(Item_Index[VT]+Item_Index[VS],ADD,LN1_COLOR);
    Draw_Dot_Vn(Item_Index[VT]-Item_Index[VS],ADD,LN1_COLOR);
    Draw_Dot_Vn(Item_Index[V1],ADD,LN2_COLOR);
    Draw_Dot_Vn(Item_Index[V2],ADD,LN2_COLOR);
    Draw_Dot_Ti(Item_Index[T1],ADD,LN2_COLOR);
    Draw_Dot_Ti(Item_Index[T2],ADD,LN2_COLOR);
    if((Tp!=MIN_X)&&(Tp!=MAX_X))  Draw_Dot_Ti(Tp,ADD,CH2_COLOR);            //Draw_Dot_Tp
  }
}

void Test_Display(int a, int b)
{
	__Char_to_Str(FileNum,a);
	Display_Str(232,87,WHITE,0,FileNum);
	Int32String_sign(&Num, b, 4);
	Display_Str(262,87,WHITE,0,(unsigned const char *)Num.str);
}

/******************************** END OF FILE *********************************/
