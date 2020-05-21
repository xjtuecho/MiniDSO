/******************** (C) COPYRIGHT 2017 e-Design Co.,Ltd. *********************
 File Name : Menu.c
 Version   : DS212                                                Author : bure
*******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "Version.h"
#include "Process.h"
#include "Menu.h"
#include "Draw.h"
#include "LCD.h"
#include "BIOS.h"
#include "Func.h"
#include "Drive.h"
#include "File.h"
#include "math.h"

#define Menu_X    263        
#define Menu_Y    183
#define ITEM_X    Menu_X
#define ITEM_Y    Menu_Y-1
#define TITLE_H   223
#define TITLE_L   3
#define RowHeight 14
#define MAX_FREQ  10-1                             /*输出模拟波形最大频率为20KHz   10KHz*/



MAINMENU  menu;
extern    u8   Menu_Temp[5];
static    u16  rowh;
static    void MeasureStr(u8 Source, u8 Item);

uc8  XSTR[][6] = {"uS", "mS", "S "};
uc8  YSTR[][6] = {"uV", "mV", "V "};
uc8  FSTR[][6] = {"Hz", "Hz", "KHz", "MHz"};
uc8  PCNT[]    =  "%";
uc8  NSTR[][6] = {"  ", "  ", "  "};
u8   Cur_Limit, Cur_Item, Cur_PopItem;
u8   Vbat_Flag = 1;
u8   Battery   = 0, Unit = 60; 
u8   Label_Flag, Windows_Pop = 0, Windows_Flag = 0;
u8   Status    = RUN;
u16  Depth;
u16  Background_Col = GRAY;  
u16  Vbat_Temp=0;
u8   Update[9]    = {1,1,1,1,1,1,1,1,1};
uc8  Menu_Limit[] = {7,6,6};            
uc8  Item_Base[8] = {0,7,13};
u16  CHA_Col = RED_, CHB_Col = DAR;

//==============+======+======+======+======+======+======+======+======+======+
//  电池状态    | 报警 | 全空 | 半空 | 半满 | 全满 | 充电 | 充满 |             |
//--------------+------+------+------+------+------+------+------+------+------+
uc8  BT_S[][5] ={ ";<=", ";<=", ";&@", ";*@", ">`@", "\"#$", "\"#$"};// 符号串 |
uc16 V_BT[]    ={  3250,  3300,  3400,  3700,  3900,  4200,  4200};  // 电压值 |
uc16 BT_C[]    ={  RED,   YEL,   GRN,   GRN,   GRN,   CYN,   GRN };  // 颜色   |
//==============+======+======+======+======+======+======+======+======+======+

//页面标题颜色
uc16 Page_BackCol =  RED;
uc16 Page_ForeCol =  WHT;
/*============================页面选项颜色====================================*/
//Page1_Menu        | CH_A  | CH_B | CH_C |TimeBase|Trigger|Cursors|X_Wondows|            
//------------------+-------+------+------+--------+-------+-------+---------+
uc16 CH_Col[] =     {  CYN,   YEL,   PUR,    ORN,     CYN,    RED,     CYN};

//Page3_Menu        | File  | WaveOut| System  | Calibra | Product |   About |    
//                  | Manage| Option | Setting | tion    | Info    |         |
//------------------+-------+--------+---------+---------+---------+---------+
uc16 P3Menu_Col[] = {  CYN,    YEL,      PUR,       ORN,     BLU,       RED};
/*============================================================================*/

uc16 SIN_DATA[36] =  // Sine wave data                                 
  {0x000,0x027,0x08E,0x130,0x209,0x311,0x441,0x58F,0x6F0,    // 90
   0x85A,0x9C0,0xB19,0xC59,0xD76,0xE68,0xF26,0xFAB,0xFF3,    // 180
   0xFFF,0xFD7,0xF70,0xECE,0xDF5,0xCED,0xBBD,0xA6F,0x90E,    // 270
   0x7A4,0x63E,0x4E5,0x3A5,0x288,0x196,0x0D8,0x053,0x00B,};  // 360

uc16 TRG_DATA[36] =  // triangle wave data
  {0x000,0x0E3,0x1C6,0x2AA,0x38D,0x471,0x554,0x638,0x71B,    // 90
   0x7FF,0x8E2,0x9C6,0xAA9,0xB8D,0xC70,0xD54,0xE37,0xF1B,    // 180
   0xFFE,0xF1B,0xE37,0xD54,0xC70,0xB8D,0xAA9,0x9C6,0x8E2,    // 270
   0x7FF,0x71B,0x638,0x554,0x471,0x38D,0x2AA,0x1C6,0x0E3};   // 360

uc16 SAW_DATA[36] =  // Sawtooth wave data                            
  {0x000,0x075,0x0EA,0x15F,0x1D4,0x249,0x2BE,0x333,0x3A8,    // 90
   0x41D,0x492,0x507,0x57C,0x5F1,0x666,0x6DB,0x750,0x7C5,    // 180
   0x83A,0x8AF,0x924,0x999,0xA0E,0xA83,0xAF8,0xB6D,0xBE2,    // 270
   0xC57,0xCCC,0xD41,0xDB6,0xE2B,0xEA0,0xF15,0xF8A,0xFFF};   // 360

//----------------+------------------------+-----------------+
uc8 FnNote[][14]= {"File R/W OK!",          /*"文件操作完成"   */
                   "File Ver Err",          /*"文件版本错误"   */
                   "Not Found!  ",          /*"文件名不存在"   */
                   "File R/W Err",          /*"文件操作错误"   */
                   "Disk Busy!  ",          /*"磁盘读写错误"   */
};
//----------------+------------------------+-----------------+

//------------FILE manage-----------------------------------------------------// 
uc8 FileStr[][20] = {
 "Save File  .Pam","Save File  .BMP","Save File  .DAT","Save File  .BUF",
 "Save File  .CSV","Load File  .DAT","Load File  .BUF","Save File  .SVG",};

uc8 MeasStr[][8]  = {"Freq:","Duty:","Vrms:","Vavg:",
                     " Vpp:","Vmax:","Vmin:","Vbat:"};

//-----------菜单项目标题-----------------------------------------------------//
uc8 Menu_Str[][8] = {" Page1 ", "Oscillo", " Page2 ",
                     "Measure", " Page3 ", "Options"};
//-----------菜单选项---------------------------------------------------------//
uc8 Item_Str[][10]= {
  "  CH_A   ",  "  CH_B   ",  "  CH_C   ", 
  "TimeBase ",  "Trigger  ",  "Cursors   ",  "X_Window  ",
  "         ",  "         ",  "         ",   "         ",  
  "         ",  "          ",
  " File    ",  " WaveOut ",  " System  ",   " Calibra ",
  " Product ",  " About   ",
};

uc8 Options_Str[][10]={
  " Manage  ",  " Option  ",  " Setting ",  " tion    ",  " Info    ",
};

//-------------特定标示符-----------------------------------------------------//
uc8  Cps_SYMB[][6]      = {"b", "a","_"};               //'~','-'
uc8  Scale_SYMB[][6]    = {"c", "d"};                   //x1,x10
uc8  Num_SYMB[][6]      = {"e", "f" ,"g" , "i" };       //"1", "2", "3", "F"
uc8  SYMB_2[][6]        = {"f"   , "i"};                //方框‘2’ 和‘F’
uc8  SYMB_T[][6]        = {"h"   , "i"};                //方框‘T’ 和‘F’
uc8  Math_Str[][6]      = {" -A ", " -B ", "A+B ", "A-B ",
                           "RecA", "RecB", "RecC"};
uc8  Probe_Str[][7]     ={" X1   "," X10   "};
uc8  Swintch[][7]       ={" OFF  "," ON  "};
uc8  Swintch1[][7]      ={"OFF   ","ON   "};
uc8  Swintch_V[][7]     ={" OFF "," CH_A"," CH_B"};
uc8  T0_Str[][7]        ={"  1  ","  2  ","  3  "};
uc8  Adc_Str[][7]       ={" AC  "," DC  "};
uc8  Math[][7]          ={" -A  ", " -B  ", " A+B ", " A-B ",
                          " RecA", " RecB", " RecC"};
uc8  Trig_SYMB[][6]     ={ "H",   "L" };
uc8  Trig_Str[][6]      ={ " {   ", " }   " };
uc8  TrigCH_Str[][8]    ={"CH_A  ","CH_B  "};
uc16 TRG_COLOR[]        ={RED,GRN,GRN,GRN,ORN};
uc8  Mode[][7]          ={"AUTO", "NORM" , "SNGL" , "SCAN" , "NONE" ,"STOP"};
uc8  Mode_Str[][7]      ={"AUTO ", "NORM " , "SNGL " ,
                          "SCAN " , "NONE " ,"STOP "};
uc8  Depth_Str[][7]     ={" 1K  ", " 2K  " , " 4K  " , " 8K  "};
uc8  Source_Str[][8]    ={"CH_A  ","CH_B  ","CH_3 "};
uc8  Wave_Str[][10]     ={"Squar   ","Sine    ","Triangle","Sawtooth"};
uc8  Freq_Str[][10]     ={" 10Hz    "," 20Hz    "," 50Hz    "," 100Hz   ",
                          " 200Hz   "," 500Hz   "," 1KHz    "," 2KHz    ",
                          " 5KHz    "," 10KHz   "," 20KHz   "," 50KHz   ",
                          " 100KHz  "," 200KHz  "," 500KHz  "," 1MHz    "};
uc8  Duty_Str [][10]    ={"  0%     "," 10%     "," 20%     "," 30%     ",
                          " 40%     "," 50%     "," 60%     "," 70%     ",
                          " 80%     "," 90%     "," 100%    "};
uc8  Vol_Str[][8]       ={" 10mV  "," 20mV  "," 50mV  "," 0.1V  "," 0.2V  ",
                          " 0.5V  "," 1.0V  "," 2.0V  "," 5.0V  "," 10V   "};
uc8  Vol_10X[][8]       ={" 0.1V  "," 0.2V  "," 0.5V  "," 1.0V  "," 2.0V  ",
                          " 5.0V  "," 10V   "," 20V  "," 50V  "," 100V  ",};
uc8  VolMenu_Str[][10]  ={"  10mV   ","  20mV   ","  50mV   ","  0.1V   ",
                          "  0.2V   ","  0.5V   ","  1.0V   ","  2.0V   ",
                          "  5.0V   ","   10V   "};
uc8  VolMenu_10X[][10]  ={"  0.1V   ","  0.2V   ","  0.5V   ","  1.0V   ",
                          "  2.0V   ","  5.0V   ","   10V   ","   20V   ",
                          "   50V   ","  100V   ",};// 垂直档字符串
uc8  VScale_Str[][6]    ={"10",   "20",   "50",   "100",  "200", 
                          "500",  "1000", "2000", "5000", "10000"};
uc8  VScale_Str_10X[][6]={"100",   "200",   "500",   "1000",  "2000", 
                          "5000",  "10000", "20000", "50000", "100000"};
uc8  TimeBase_Str[][8]  ={"1.0uS","2.0uS","5.0uS","10uS ","20uS ","50uS ",
                          "0.1mS","0.2mS","0.5mS","1.0mS", 
                          "2.0mS","5.0mS","10mS ","20mS ","50mS ","0.1S ",
                          "0.2S ","0.5S ","1.0S ","2.0S "};
uc8  TimeBaseMenu_Str[][10]=
                         {" 1.0uS   "," 2.0uS   "," 5.0uS   ","  10uS   ",
                          "  20uS   ","  50uS   "," 0.1mS   "," 0.2mS   ",
                          " 0.5mS   "," 1.0mS   "," 2.0mS   "," 5.0mS   ",
                          "  10mS   ","  20mS   ","  50mS   ","  0.1S   ",
                          "  0.2S   ","  0.5S   ","  1.0S   ","  2.0S   "};
uc8  TScale_Str[][8]   = { "1",    "2",     "5",     "10",    "20",   "50",   "100",                        
                           "200",  "500",   "1000",  "2000",  "5000", "10000","20000",
                           "50000","100000","200000","500000","1000000", "2000000"}; 

//--------------------------输出波形预分频--------------------------------------
//----------+-------+-------+-------+-------+-------+-------+-------+-------+---
uc16 FPSC[] = {  128,  128,   64,   64,   16,   16,    4,    4,
                   1,    1,    1,    1,    1,    1,    1,    1};

uc16 FARR[] = {56250,28125,22500,11250,22500, 9000, 18000,9000,
               14400, 7200, 3600, 1440,  720,  360,  144,   72};
//----------+-------+-------+-------+-------+-------+-------+-------+-------+---
//--------------------------输出模拟波形分频------------------------------------
uc16 Dac_Psc[12] = {4,    2,    2,   2,   2,    2,   2,   2,    2,   2,   2};
//uc16 Dac_Tim[12] = {10000,10000,4000,2000,1000, 400, 200, 100,  40,  100,  50};
uc16 Dac_Tim[12] = {10000,10000,4000,2000,1000, 400, 200, 100,  40,  20,  10};
//                   0-us                       9--ms                    18--1s
uc16 Ts[]        = {10,10,10,10,20,50,100,200,500,1,2,5,10,20,50,100,200,500,1,2};
uc16 DEPTH[]     = { 1024 , 2048 , 4096 , 8192};


uc32 VScale[]    = { 10,20, 50, 100, 200, 500, 1000, 2000, 5000,  10000};
uc64 VScale_10X[]= { 100, 200, 500, 1000, 2000, 5000,  10000,20000,50000, 100000,};
uc32 TScale[]    = { 1,     2,    5,   10,    20,     50,                        
                     100, 200,  500,  1000, 2000,    5000,  10000, 20000, 50000, 
                     100000, 200000, 500000, 1000000, 2000000};                

//---Title显示位置-----BAT| CH1| CH2| Ch3|  TB | TRI| AUTO--------------------//   
uc16 Title_Posi_X[] = {  6,  30,  85, 140, 180, 240, 289,};  



//-------popmenu1 OSCILOSCOPE item--------------------------------------------//
uc8 PM1_str[][13] = {
 /*CH1*/        "Voltage:  ", "Post:     ", "AC/DC:    ",
                "Enable:   ", "Probe:    ", 
 /*CH2*/        "Voltage:  ", "Post:     ", "AC/DC:    ",
                "Enable:   ", "Probe:    ",
 /*CH3*/        "Math:     ", "Post:     ", "Enable:   ",         
 /*Time*/       "TimeBase: ", "Fit:      ",
 /*Trig*/       "SyncMode: ", "TrigMode: ", "Source:   ",
                "Threshold:", "Enable:   ", "Auto Fit: ", 
 /*Cursor*/     "T1.Post:  ", "T2.Post:  ", "Enable.T: ",
                "V1.Post:  ", "V2.Post:  ", "Enable.V: ",
 /*Window*/     "Post:     ", "Depth:    ", "Enable:   ", "T0:       "
};
//-------popmenu3 SETTING item------------------------------------------------//
uc8 PM3_Str[][13]={
 "Save Param  ", "Save   .BMP ", "Save   .DAT ", "Save   .BUF ", "Save   .CSV ",
 "Load   .DAT ", "Load   .BUF ", "Save   .SVG ",
 "Type:      ",  "Freq:      ",  "Duty:      ",
 "Volume:    ",  "BKlight:    ", "Standby:    ", "PowerOff:  ",  "MenuCycle:",
 "ItemCycle:",   "PostCycle:",
 "Calibrate ",   "Restore   ",
 "          ",   "          ",
 "About:    ",   "          ", 
};
//====================Popmenu选项数目=========================
//---Popmenu_1选项数目---CH1| CH2| CH3| TIME| TRI | CURS| WIN
uc8  PopMenu1_Base[]  = { 0,   5,  10,   13,   15,   21,  27};
uc8  PopMenu1_Limit[] = { 5,   5,   3,    1,    6,    6,   3};//关闭T0 Fit
//---Popmenu_3选项数目---FILE| WAVE| SYS | CAL | PRO| ABOUT
uc8  PopMenu3_Base[]  = { 0,    8,   11,    18,   20,   22}; 
uc8  PopMenu3_Limit[] = { 8,    3,    7,     2,    2,    2};

u8   PopMenu1_Len = 33;
u8   PopMenu2_Len = 18;
u8   PopMenu3_Len = 18;

//====================Menu1_Popmenu========================= 
//--------------Popmenu选项上限------------------------

sc16 Popmenu1_Limit1[]={
//======+====+=====+======+=====+=====+=======+   
//CH1    VOL | POST| AC/DC| ENABLE | PROBE
          9,   195,    1,    1,     1,   
//CH2    VOL | POST| AC/DC| ENABLE | PROBE   
          9,   195,    1,    1,     1,
//CH3   TYPE | POTS| SWITCH        
          6,   195,    1,
//TIME  TIME | FIT
         19,     1,                           
//TRIG  MODE | TYPE| OURCE| THR1  FIT | ENABLE |THR2
          4,     1,    1,   95,   1,     1,    95,
//CURS   T1  |  T2 |  EN_T|  V1 |  V2 | EN_V        
        248,   248,    1,   198, 198,     2, 
//WIN   POSI |DEPTH| EN | T0_Str
        7800,     3,    1,  2,                         
};
//--------------Popmenu选项下限------------------------
sc16 PopMenu1_Limit2[]={ 
  1,  5,  0,   0,  0,           //CH1     VOL | POST| AC/DC| ENABLE | PROBE
  1,  5,  0,   0,  0,           //CH2     VOL | POST| AC/DC| ENABLE | PROBE
  0,  5,  0,                    //CH3    TYPE | POTS| SWITCH 
  0,  0,                        //Time   TIME | FIT
  0,  0,  0, -95,  0,  0,  -95, //Trig   MODE | TYPE| OURCE| THRI| FIT |ENABLE|THR2
  5,  5,  0,   2,  2,  0,       //Curs    T1  |  T2 |  EN_T|  V1 |  V2 |EN_V 
  0,  0,  0,   0,               //Win    POSI |DEPTH| EN   |   T0_Str
};
//--------------Popmenu选项默认值------------------------
s16 PopMenu1_Value[]={
  6,  100,  0,  1,  0,        //CH1     VOL | POST| AC/DC| ENABLE | PROBE
  6,   90,  0,  1,  0,        //CH2     VOL | POST| AC/DC| ENABLE | PROBE
  2,   30,  0,                //CH3    TYPE | POTS| SWITCH
  4,    1,                    //Time   TIME | FIT
  0,    0,  0, 10,   1,  0, 8,//Trig   MODE | TYPE| CH_N | THRI| FIT |ENABLE|THR2
  50, 200,  1,175,  25,  1,   //Curs    T1  |  T2 | EN_T |  V1 |  V2 |EN_V 
  0,    2,  1,  0,            //Win    POSI |DEPTH| EN           
};
sc16 PopMenu1_CValue[]={
  6,  100,  0,  1,  0,        //CH1     VOL | POST| AC/DC| ENABLE | PROBE
  6,   90,  0,  1,  0,        //CH2     VOL | POST| AC/DC| ENABLE | PROBE
  2,   30,  0,                //CH3    TYPE | POTS| SWITCH
  4,    1,                    //Time   TIME | FIT
  0,    0,  0, 10,   1,  0, 8,//Trig   MODE | TYPE| CH_N | THRI| FIT |ENABLE|THR2
  50, 200,  1,175,  25,  1,   //Curs    T1  |  T2 | EN_T |  V1 |  V2 |EN_V 
  0,    2,  1,  0,            //Win    POSI |DEPTH| EN           
};

//--------------Popmenu_Posi窗口位置------------------------ 
uc8 PopMenu1_Posi_Y[]={
   100,  70, 70,  60,  30,  20,  10,    
}; 
uc8 PopMenu3_Posi_Y[]={
   60,  70, 70,  60,  30,  20,  10, 
}; 
uc8 PopMenu1_Posi_X[]={
   145,   145,  145,  145,  145,  145,  145,  
};
uc8 PopMenu3_Posi_X[]={ 
   163,  145, 145,  145,  120,  120,  120,  
};

//------PopMenu2 measurement item---------------------------------------------//

uc8 PopMenu2[][13]      = {"Source:   ", "Type:     ","Enable:   "};
uc8 MeasItem[][9]       = {"FREQ   ", "DUTY   ", "RMS    ", "Vavg  ", "VPP    ",
                           "Max    ", "Min    ", "VBAT   "};
uc8 MeasItem_Menu[][10] = {"  FREQ   ", "  DUTY   ", "  RMS    ", "  Vavg   ",
                           "  VPP    ", "  Max    ", "  Min    ", "  VBAT   "};

uc16 PopMenu2_Limit1[]={1,  6,  1,    //上限  // 第二选项：7 ，6的话关闭VBat
                        1,  6,  1,
                        1,  6,  1,
                        1,  6,  1,
                        1,  6,  1,
                        1,  6,  1,
};
uc16 PopMenu2_Limit2[]={0,  0,  0,        
                        0,  0,  0,
                        0,  0,  0,
                        0,  0,  0,
                        0,  0,  0,
                        0,  0,  0,
};
u8  PopMenu2_Value[]={               
                        0,  0,  1,                        //source CH1
                        1,  1,  1,                        //type
                        0,  2,  1,                        //enable
                        0,  3,  1,                        //source CH1
                        0,  4,  1,                        //type
                        0,  5,  1,                        //enable
};
uc8  PopMenu2_CValue[]={               
                        0,  0,  1,                        //source CH1
                        1,  1,  1,                        //type
                        0,  2,  1,                        //enable
                        0,  3,  1,                        //source CH1
                        0,  4,  1,                        //type
                        0,  5,  1,                        //enable
};

//**************Menu3_Pop**************************************** 
uc16 PopMenu3_Limit1[]={//上限 
  99, 99, 99, 99, 99, 99, 99, 99, //File   Pam | BMP | DAT | BUF | CSV | DAT |BUF 
  3,  15,  9,                     //Wave   TYPE| FRQ | DUTY| 
  9,  10, 60, 60, 1, 1, 1,        //Sys    VOL | BK  | STANDBY   | PowerOff
  1,  1,
  1,  1,
  1,  1,
};
uc16 PopMenu3_Limit2[]={// 下限
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  1,  
  0,  1,  0, 0, 0, 0, 0,
  0,  0,
  0,  0,
  0,  0,
};
u8 PopMenu3_Value[]={   //默认
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  9,  5,  
  5,  5, 10, 20,1, 0, 1,
  0,  0,
  0,  0,
  0,  0,
};
uc8 PopMenu3_CValue[]={ //默认
  0,  0,  0,  0,  0,  0,  0,  0,
  0,  9,  5,  
  5,  5, 10, 20, 1, 0, 1,
  0,  0,
  0,  0,
  0,  0,
};  



/*******************************************************************************
show menu: 显示主菜单标题
*******************************************************************************/
void Show_Menu(void)
{
  u16 k, n, item;
  u8* ptr; 
  if (!(menu.mflag & UPD) && !(menu.iflag &UPD)) return;
  //------show menu------------------------------------  
  if(menu.mflag & UPD){
    menu.mflag &=~UPD;
    menu.iflag |= UPD;
    Background=CYN; 
    Foreground=WHT;
    Clear_Label_R(DAR);                                        //擦除右栏  
    Draw_Circle_D(Page_BackCol,  Menu_X, Menu_Y, 38, 53, 56);  //画菜单标题页数矩形框
    ptr=(u8*)&Menu_Str[menu.current*2];
    Background=Page_BackCol; 
    Foreground=Page_ForeCol;     
    //------显示主页名称------------ 
    Sy =Menu_Y+20 ; 
    Sx =Menu_X;
    PrintStr(PRN, ptr);
    Sy =Menu_Y+6 ; 
    Sx =Menu_X;
    ptr+=8;
    PrintStr(PRN, ptr);
  }
  if (!(menu.iflag & UPD)) return;
  menu.iflag &=~UPD;
  //------show item------------------------------------------// 
  if(menu.current==Measure){
    Show_Measure();
  }
  else{
    item=Item_Base[menu.current];
    ptr=(u8*)&Item_Str[item];   
    rowh=0;
    for(n=0; n<Menu_Limit[menu.current]; n++){
      if(menu.menu_index[menu.current]==n)Background=GRN;
      else  Background=WHT;      
      Foreground=BLK;
      Sx=Menu_X+8;             
      rowh+=RowHeight;
      Sy=ITEM_Y-rowh;
      Draw_Rectangle(Background, Menu_X+8, ITEM_Y-rowh+11, 3, 49); 
      PrintStr6x8(PRN, ptr);
      ptr+=10;                
      Sx=Menu_X+8;
      k = item+n;
      if((k!=CH3)&&(k!=COURSOR)&&(k!=WINDOWS)&&(k!=ABOUT)){
        rowh+=RowHeight;
        Draw_Rectangle(Background, Menu_X+8, ITEM_Y-rowh, 3, 49); 
      }
      Show_Item(k);                      
    }
  }
}

/*******************************************************************************
 show item : 显示主菜单子项表    输入参数 ：主菜单当前子项
*******************************************************************************/
void Show_Item(u8 k)
{

  u8  wink;
  u8  *Str1,*Str2;
  u16 x,y,j;
    switch (k){
    case CH1:
      wink = PRN;
      Sy=ITEM_Y-rowh+3;
      Str1 = (u8*)VolMenu_10X[PopMenu1_Value[CH1_Vol]];
      Str2 = (u8*)VolMenu_Str[PopMenu1_Value[CH1_Vol]];
      PrintStr6x8(wink, PopMenu1_Value[CH1_Probe]? Str1 : Str2);
      Sy=ITEM_Y-rowh;
      Draw_Circle_S(CH_Col[k], Menu_X, Sy, 28, 6);  //画项目栏颜色
      wink = (PopMenu1_Value[CH1_Probe])? INV : INV;  //F模式时，字体半消隐
      Draw_Circle_D(CH_Col[k], Title_Posi_X[k+1]+8, TITLE_H+2, 11, 2, 7*6);
      SetColor(DAR,CH_Col[k]);
      DispStr10x14(Title_Posi_X[k+1]-3, TITLE_H+2, SYMB,
                  (PopMenu1_Value[CH1_Probe]?(u8*)("A"):(u8*)("B")));
      Str1 = (u8*)(Vol_10X[PopMenu1_Value[CH1_Vol]]+1);
      Str2 = (u8*)(Vol_Str[PopMenu1_Value[CH1_Vol]]+1);
      DispStr6x8(Title_Posi_X[k+1]+11, TITLE_H+2, wink,(PopMenu1_Value[CH1_Enable]?
                (PopMenu1_Value[CH1_Probe]?Str1:Str2):(u8*)("Hide ")));
      DispStr(Title_Posi_X[k+1]+40, TITLE_H+2, wink+SYMB,
             (u8*)(Cps_SYMB[PopMenu1_Value[CH1_Coupl]]));
      break;  
    case CH2:
      wink = PRN;
      Sy=ITEM_Y-rowh+3;
      Str1 = (u8*)VolMenu_10X[PopMenu1_Value[CH2_Vol]];
      Str2 = (u8*)VolMenu_Str[PopMenu1_Value[CH2_Vol]];
      PrintStr6x8(wink, PopMenu1_Value[CH2_Probe]? Str1 : Str2);
      Sy=ITEM_Y-rowh;
      Draw_Circle_S(CH_Col[k], Menu_X, Sy, 28, 6);  //画项目栏颜色
      SetColor(DAR,CH_Col[k]);
      wink = (PopMenu1_Value[CH2_Probe])? INV : INV;  //F模式时，字体半消隐
      Draw_Circle_D(CH_Col[k], Title_Posi_X[k+1]+8, TITLE_H+2, 11, 2, 7*6);
      DispStr10x14(Title_Posi_X[k+1]-3, TITLE_H+2, SYMB,
                  (PopMenu1_Value[CH2_Probe]?(u8*)("A"):(u8*)("B")));
      Str1 = (u8*)(Vol_10X[PopMenu1_Value[CH2_Vol]]+1);
      Str2 = (u8*)(Vol_Str[PopMenu1_Value[CH2_Vol]]+1);
      DispStr6x8(Title_Posi_X[k+1]+ 7+4, TITLE_H+2, wink,(PopMenu1_Value[CH2_Enable]?
                (PopMenu1_Value[CH2_Probe]?Str1:Str2): (u8*)("Hide ")));
      DispStr(Title_Posi_X[k+1]+40, TITLE_H+2, wink+SYMB, 
             (u8*)(Cps_SYMB[PopMenu1_Value[CH2_Coupl]]));               
      break;
    case CH3: 
      Draw_Circle_S(CH_Col[k], Menu_X, Sy, 14, 6);  //画项目栏颜色
      SetColor(DAR,CH_Col[k]);    
      Draw_Circle_D(CH_Col[k], Title_Posi_X[k+1]+8, TITLE_H+2, 11, 2, 5*6-1);
      DispStr6x8(Title_Posi_X[k+1]+12, TITLE_H+2, INV, (PopMenu1_Value[CH3_Enable]?
                (u8*)(Math_Str[PopMenu1_Value[CH3_Type]]): (u8*)("Hide")));
      break;
    case TIMEBASE:        
      wink = (PopMenu1_Value[TIM_Fit])? PRN : VOID; //F模式时，字体半消隐
      Sy=ITEM_Y-rowh+3;
      PrintStr6x8(wink, (u8*)&TimeBaseMenu_Str[PopMenu1_Value[TIM_Base]]);
      Sy=ITEM_Y-rowh;
      Draw_Circle_S(CH_Col[k], Menu_X, Sy, 28, 6);  //画项目栏颜色
      SetColor(DAR,CH_Col[k]);
      DispStr(Title_Posi_X[k+1], TITLE_H+2, SYMB,
             (PopMenu1_Value[TIM_Fit]?(u8*)("_"):(u8*)("i")));
      Draw_Circle_D(CH_Col[k], Title_Posi_X[k+1]+10, TITLE_H+2, 11, 2, 6*6-1);  
      DispStr6x8(Title_Posi_X[k+1]+13, TITLE_H+2, INV,
                (u8*)&TimeBase_Str[PopMenu1_Value[TIM_Base]]);
      break;
    case TRIGGER:
      Sx=Menu_X+8;
      Sy=ITEM_Y-rowh;
      PrintStr(PRN, "       ");
      Sx=Menu_X+24;
      
      Draw_Circle_S(CH_Col[PopMenu1_Value[TRI_Ch]], Menu_X, Sy, 28, 6);
      Sy=ITEM_Y-rowh+1;
      SetColor(Background, BLK);
      PrintStr(PRN+SYMB, (u8*)&Trig_SYMB[PopMenu1_Value[TRI_Mode]]);
      
      SetColor(DAR,CH_Col[PopMenu1_Value[TRI_Ch]]);
      Sx=Title_Posi_X[k+1];
      Sy=TITLE_H+2;
      Draw_Circle_D(CH_Col[PopMenu1_Value[TRI_Ch]], 
                    Title_Posi_X[k+1]-3, TITLE_H+2, 11, 2, 6*6+1);
      if(PopMenu1_Value[TRI_Fit])PrintStr6x8(INV, "Trig");
      else{ 
        PrintStr6x8(INV, "Trig");
      }  
      DispStr(Title_Posi_X[k+1]+24, TITLE_H+2,INV+ SYMB, 
             (u8*)&Trig_SYMB[PopMenu1_Value[TRI_Mode]]);
      DispStr(Title_Posi_X[k+1]-12, TITLE_H+2, SYMB,
              (PopMenu1_Value[TRI_Fit]?(u8*)("i"):(u8*)("_")));
      Update_Status();
      break;
    case COURSOR:
      Draw_Circle_S(CH_Col[k], Menu_X, Sy, 14, 6);//画项目栏颜色
      break;
    case WINDOWS:
      Draw_Circle_S(CH_Col[k], Menu_X, Sy, 14, 6);//画项目栏颜色
      break;
 //-------------------第三页------------------------------------------
    case FILE:
      Sx=Menu_X+8;                                //菜单标题
      Sy=ITEM_Y-rowh+3;
      PrintStr6x8(PRN, (u8*)&Options_Str[k-FILE]);
      Sy=ITEM_Y-rowh;
      Draw_Circle_S(P3Menu_Col[0], Menu_X, Sy, 28, 6);//画项目栏颜色
      break;
    case WAVE:
      Sx=Menu_X+8;                                //菜单标题
      Sy=ITEM_Y-rowh+3; 
      PrintStr6x8(PRN, (u8*)&Options_Str[k-FILE]);
      Sy=ITEM_Y-rowh;
      Draw_Circle_S(P3Menu_Col[1], Menu_X, Sy, 28, 6);//画项目栏颜色
      break;
    case SYSTEM:
      Sx=Menu_X+8;                                //菜单标题
      Sy=ITEM_Y-rowh+3;      
      PrintStr6x8(PRN, (u8*)&Options_Str[k-FILE]);
      Sy=ITEM_Y-rowh;
      Draw_Circle_S(P3Menu_Col[2], Menu_X, Sy, 28, 6);//画项目栏颜色
      break;
    case CALIBRATION:
      Sx=Menu_X+8;                                //菜单标题
      Sy=ITEM_Y-rowh+3;      
      PrintStr6x8(PRN, (u8*)&Options_Str[k-FILE]);
      Sy=ITEM_Y-rowh;
      Draw_Circle_S(P3Menu_Col[3], Menu_X, Sy, 28, 6);      //画项目栏颜色
      break;
    case PRO:
      Sy=ITEM_Y-rowh+3;
      PrintStr6x8(PRN, (u8*)&Options_Str[k-FILE]);
      Sy=ITEM_Y-rowh;
      Draw_Circle_S(P3Menu_Col[4], Menu_X, Sy, 28, 6);      //画项目栏颜色
      break;
    case ABOUT:
      Draw_Circle_S(P3Menu_Col[5], Menu_X, Sy, 14, 6);      //画项目栏颜色
      break;
    }
    rowh++;
    x=Menu_X+2;
    y=ITEM_Y-rowh;
    for(j=0; j<58; j++){                          //黑线间隔
      SetPosi(x++, y);
      SetPixel(BLK);
    }
}
/*******************************************************************************
Show_Item_One : 显示主菜单子项表    输入参数 ：主菜单当前子项
*******************************************************************************/
void Show_Item_One(u8 k)
{

  u8  wink;
  u8  *Str1,*Str2;
  switch (k){
    case CH1:
      wink = PRN;
      rowh = 2*RowHeight;
      Sx=Menu_X+8;
      Sy=ITEM_Y-rowh+3;
      if(Menu_Temp[1]== CH1)Background=GRN;
      else  Background=WHT;     
      Foreground=BLK;
      Str1 = (u8*)VolMenu_10X[PopMenu1_Value[CH1_Vol]];
      Str2 = (u8*)VolMenu_Str[PopMenu1_Value[CH1_Vol]];
      PrintStr6x8(wink, PopMenu1_Value[CH1_Probe]? Str1 : Str2);
      Sy=ITEM_Y-rowh;
      Draw_Circle_S(CH_Col[k], Menu_X, Sy, 28, 6);  //画项目栏颜色
      
      break;  
    case CH2:
      wink = PRN;
      rowh = 4*RowHeight;
      Sx=Menu_X+8;
      Sy=ITEM_Y-rowh+2;
      if(Menu_Temp[1]== CH2)Background=GRN;
      else  Background=WHT;      
      Foreground=BLK;
      Str1 = (u8*)VolMenu_10X[PopMenu1_Value[CH2_Vol]];
      Str2 = (u8*)VolMenu_Str[PopMenu1_Value[CH2_Vol]];
      PrintStr6x8(wink, PopMenu1_Value[CH2_Probe]? Str1 : Str2);
      Sy=ITEM_Y-rowh-1;
      Draw_Circle_S(CH_Col[k], Menu_X, Sy, 28, 6);  //画项目栏颜色              
      break;
  case TIMEBASE:
      wink = PRN;
      rowh = 7*RowHeight;
      Sx=Menu_X+8;
      Sy=ITEM_Y-rowh;
      if(Menu_Temp[1]== TIMEBASE)Background=GRN;
      else  Background=WHT;      
      Foreground=BLK;
      PrintStr6x8(wink, (u8*)&TimeBaseMenu_Str[PopMenu1_Value[TIM_Base]]);
      Sy=ITEM_Y-rowh-3;
      Draw_Circle_S(CH_Col[k], Menu_X, Sy, 28, 6);  //画项目栏颜色
      break;
  case TRIGGER:
      wink = PRN;
      rowh = 9*RowHeight;
      Sx=Menu_X+8;
      Sy=ITEM_Y-rowh-4;
      if(Menu_Temp[1]== TRIGGER)Background=GRN;
      else  Background=WHT;      
      Foreground=BLK;
      PrintStr(PRN, "       ");
      Sx=Menu_X+24;
      
      Sy=ITEM_Y-rowh-4;
      Draw_Circle_S(CH_Col[PopMenu1_Value[TRI_Ch]], Menu_X, Sy, 28, 6);
      Sy=ITEM_Y-rowh+1;
      //SetColor(DAR, CH_Col[PopMenu1_Value[TRI_Ch]]);
      SetColor(Background,BLK);
      PrintStr(PRN+SYMB, (u8*)&Trig_SYMB[PopMenu1_Value[TRI_Mode]]);
      break;  
    }
    
}
/*******************************************************************************
 Show_Title: 显示标题栏    
*******************************************************************************/
void Show_Title(void)
{
  u8  wink;
  u8 *Str1, *Str2;
  u16 k;
  for(k=0;k<7;k++){
    switch (k){
    case CH1:
      wink = (PopMenu1_Value[CH1_Probe])? INV : INV; //F模式时，字体半消隐
      Draw_Circle_D(CH_Col[k], Title_Posi_X[k+1]+8, TITLE_H+2, 11, 2, 7*6);
      SetColor(DAR,CH_Col[k]);
      DispStr10x14(Title_Posi_X[k+1]-3, TITLE_H+2, SYMB,
                  (PopMenu1_Value[CH1_Probe]?(u8*)("A"):(u8*)("B")));
      if(menu.menu_flag == 0)SetColor(CHA_Col,CH_Col[k]);
      Str1 = (u8*)(Vol_10X[PopMenu1_Value[CH1_Vol]]+1);
      Str2 = (u8*)(Vol_Str[PopMenu1_Value[CH1_Vol]]+1);
      DispStr6x8(Title_Posi_X[k+1]+11, TITLE_H+2, wink,(PopMenu1_Value[CH1_Enable]?
                (PopMenu1_Value[CH1_Probe]?Str1:Str2):(u8*)("Hide ")));
      
      DispStr(Title_Posi_X[k+1]+40, TITLE_H+2, wink+SYMB, 
             (u8*)(Cps_SYMB[PopMenu1_Value[CH1_Coupl]]));             
      break;  
    case CH2:
      SetColor(DAR,CH_Col[k]);
      wink = (PopMenu1_Value[CH2_Probe])? INV : INV; 
      Draw_Circle_D(CH_Col[k], Title_Posi_X[k+1]+8, TITLE_H+2, 11, 2, 7*6);
      DispStr10x14(Title_Posi_X[k+1]-3, TITLE_H+2, SYMB,
                  (PopMenu1_Value[CH2_Probe]?(u8*)("A"):(u8*)("B")));
      if(menu.menu_flag == 0)SetColor(CHB_Col,CH_Col[k]);
      Str1 = (u8*)(Vol_10X[PopMenu1_Value[CH2_Vol]]+1);
      Str2 = (u8*)(Vol_Str[PopMenu1_Value[CH2_Vol]]+1);
      DispStr6x8(Title_Posi_X[k+1]+ 7+4, TITLE_H+2, wink,(PopMenu1_Value[CH2_Enable]?
                (PopMenu1_Value[CH2_Probe]?Str1:Str2):(u8*)("Hide ")));
      DispStr(Title_Posi_X[k+1]+40, TITLE_H+2, wink+SYMB, 
             (u8*)(Cps_SYMB[PopMenu1_Value[CH2_Coupl]]));                   
      break;
    case CH3:
      SetColor(DAR,CH_Col[k]);    
      Draw_Circle_D(CH_Col[k], Title_Posi_X[k+1]+8, TITLE_H+2, 11, 2, 5*6-1);
      DispStr6x8(Title_Posi_X[k+1]+12, TITLE_H+2, INV, (PopMenu1_Value[CH3_Enable]?
                (u8*)(Math_Str[PopMenu1_Value[CH3_Type]]):(u8*)("Hide")));
      break;
    case TIMEBASE:        //TimeBs
      SetColor(DAR,CH_Col[k]);
      DispStr(Title_Posi_X[k+1], TITLE_H+2, SYMB,
              (PopMenu1_Value[TIM_Fit]?(u8*)("_"):(u8*)("i")));
      Draw_Circle_D(CH_Col[k], Title_Posi_X[k+1]+10, TITLE_H+2, 11, 2, 6*6-1); 
      DispStr6x8(Title_Posi_X[k+1]+13, TITLE_H+2, INV, 
                (u8*)&TimeBase_Str[PopMenu1_Value[TIM_Base]]);
      break;
    case TRIGGER:
      SetColor(DAR,CH_Col[PopMenu1_Value[TRI_Ch]]);
      Sx=Title_Posi_X[k+1];
      Sy=TITLE_H+2;
      Draw_Circle_D(CH_Col[PopMenu1_Value[TRI_Ch]], 
                    Title_Posi_X[k+1]-3, TITLE_H+2, 11, 2, 6*6+1);//前景边框 
      if(PopMenu1_Value[TRI_Fit])PrintStr6x8(INV, "Trig");
      else{ 
        PrintStr6x8(INV, "Trig");
      }  
      DispStr(Title_Posi_X[k+1]+24, TITLE_H+2,INV+ SYMB, 
             (u8*)&Trig_SYMB[PopMenu1_Value[TRI_Mode]]);
      DispStr(Title_Posi_X[k+1]-12, TITLE_H+2, SYMB,
              (PopMenu1_Value[TRI_Fit]?(u8*)("i"):(u8*)("_")));
      Update_Status();
      break;
    case COURSOR:
      break;
    case WINDOWS:
      break;
    case FILE:
      break;
    case WAVE:
      break;
    case SYSTEM:
      break;
    case CALIBRATION:
      break;
    case PRO:
      break;
    case ABOUT:
      break;
    }
  }
}
/*******************************************************************************
显示子菜单窗口目录 
*******************************************************************************/
void Show_PopMenu(u8 Cur_Item)
{
  #define LEN 9
  u16 i,j,index;
  u8  *ptr1,*ptr2;
  u8  ptr[64],ptr3[6], item = 0;
  
  memset(ptr,0,64);
  switch (menu.current){
    //----------------------------第一页菜单----------------------------------//
  case Oscillo:
    j=menu.menu_index[menu.current];
    Cur_Limit=PopMenu1_Limit[j];
    index=PopMenu1_Base[j];
    ptr1=(u8*)&PM1_str[index];
    switch (j){
    case CH1:
      memcpy(&ptr," ",1);
      ptr2=PopMenu1_Value[CH1_Probe]?(u8*)&Vol_10X[PopMenu1_Value[CH1_Vol]]
           :(u8*)&Vol_Str[PopMenu1_Value[CH1_Vol]];                //Ch_range
      memcpy(&ptr[1],&ptr2[1],LEN-5);
      memcpy(&ptr[LEN]," ",1);
      sprintf((char*)ptr3,"%-3d",PopMenu1_Value[CH1_Posi]);       //Ch_posi
      strcat((char*)ptr3," ");
      memcpy(&ptr[LEN+1],ptr3,5);
      memcpy(&ptr[2*LEN],&Adc_Str[PopMenu1_Value[CH1_Coupl]],5);  //Ch_AD~DC
      memcpy(&ptr[3*LEN],&Swintch[PopMenu1_Value[CH1_Enable]],5); //Ch_Enable 
      memcpy(&ptr[4*LEN],&Probe_Str[PopMenu1_Value[CH1_Probe]],5);//Ch_Probe
      break;
    case CH2:
      memcpy(&ptr," ",1);
      ptr2=PopMenu1_Value[CH2_Probe]?(u8*)&Vol_10X[PopMenu1_Value[CH2_Vol]]
           :(u8*)&Vol_Str[PopMenu1_Value[CH2_Vol]];                //Ch_range
      memcpy(&ptr[1],&ptr2[1],LEN-5);
      memcpy(&ptr[LEN]," ",1);
      sprintf((char*)ptr3,"%-3d",PopMenu1_Value[CH2_Posi]);       //Ch_posi
      strcat((char*)ptr3," ");
      memcpy(&ptr[LEN+1],ptr3,5);
      memcpy(&ptr[2*LEN],&Adc_Str[PopMenu1_Value[CH2_Coupl]],5);  //Ch_AD~DC
      memcpy(&ptr[3*LEN],&Swintch[PopMenu1_Value[CH2_Enable]],5); //Ch_Enable 
      memcpy(&ptr[4*LEN],&Probe_Str[PopMenu1_Value[CH2_Probe]],5);//Ch_Probe
      break;
    case CH3:
      memcpy(&ptr,(u8*)&Math[PopMenu1_Value[CH3_Type]],LEN-3);
      memcpy(&ptr[LEN]," ",1);
      sprintf((char*)ptr3,"%-3d",PopMenu1_Value[CH3_Posi]);
      strcat((char*)ptr3," ");
      memcpy(&ptr[LEN+1],ptr3,5);
      memcpy(&ptr[2*LEN],&Swintch[PopMenu1_Value[CH3_Enable]],5);
      break;
    case TIMEBASE:
      ptr2=(u8*)&TimeBaseMenu_Str[PopMenu1_Value[TIM_Base]];   //TimeBase_range
      memcpy(&ptr,&ptr2[1],LEN-4);
      memcpy(&ptr[LEN],&Swintch[PopMenu1_Value[TIM_Fit]],5);   //TimeBase_Fit 
      break;          
    case TRIGGER:
      memcpy(&ptr,(u8*)&Mode_Str[PopMenu1_Value[TRI_Sync]],LEN-3);
      memcpy(&ptr[LEN],(u8*)&Trig_Str[PopMenu1_Value[TRI_Mode]],LEN-4);        
      memcpy(&ptr[2*LEN],(u8*)&TrigCH_Str[PopMenu1_Value[TRI_Ch]],LEN-4);
      sprintf((char*)ptr3,"%-3d",(PopMenu1_Value[TRI_Ch])?
              PopMenu1_Value[TRI_Ch2]:PopMenu1_Value[TRI_Ch1]);
      strcat((char*)ptr3,"  ");
      memcpy(&ptr[3*LEN],ptr3,5);
      memcpy(&ptr[4*LEN],&Swintch1[PopMenu1_Value[TRI_Enable]],5);
      memcpy(&ptr[5*LEN],&Swintch1[PopMenu1_Value[TRI_Fit]],5);
      break;
    case COURSOR:
      index++;
      for(i=0;i<6;i++){
        if     (i==2)memcpy(&ptr[i*LEN],Swintch[PopMenu1_Value[index++]],5);
        else if(i==5)memcpy(&ptr[i*LEN],Swintch_V[PopMenu1_Value[index++]],5);
        else{
          memcpy(&ptr[i*LEN]," ",1);
          sprintf((char*)ptr3,"%-3d",PopMenu1_Value[index++]);
          strcat((char*)ptr3," ");
          memcpy(&ptr[i*LEN+1],ptr3,5);
        }
      }
      break;
    case WINDOWS:
      index++;
      Windows_Flag |=UPD;
      Update_Windows();  //窗口位置处理，popmenu1_value有更新
      sprintf((char*)ptr3,"%3d",PopMenu1_Value[WIN_Posi]);
      strcat((char*)ptr3,"  ");
      memcpy(&ptr,ptr3,5);
      memcpy(&ptr[LEN],(u8*)&Depth_Str[PopMenu1_Value[WIN_Depth]],LEN-3);
      memcpy(&ptr[2*LEN],&Swintch[PopMenu1_Value[WIN_Enable]],5);
      memcpy(&ptr[3*LEN],&T0_Str[PopMenu1_Value[WIN_T0]],5);
      break; 
    }
    PMenu_Pop(Cur_Limit,Cur_Item,ptr1,10,ptr);
    break;
    //---------------------------第二页菜单----------------------------------//
  case Measure:
    j=menu.menu_index[menu.current];
    Cur_Limit=3;
    index=j*3;
    ptr1=(u8*)&PopMenu2;
    memcpy(&ptr,(u8*)(Source_Str[PopMenu2_Value[index++]]),LEN-4);
    memcpy(&ptr[LEN],(u8*)&MeasItem[PopMenu2_Value[index++]],LEN-4);
    ptr[2*LEN-2]=0;
    memcpy(&ptr[2*LEN],(u8*)&Swintch1[PopMenu2_Value[index]],LEN-4);
    if(j<5) //最后一项，电池电压项目无子窗口
    PMenu_Pop(Cur_Limit,Cur_Item,ptr1,10,ptr);
    break;
    //---------------------------第三页菜单-----------------------------------//
  case Option:
    item=menu.menu_index[menu.current]; 
    j=Item_Base[menu.current]+item;
    Cur_Limit=PopMenu3_Limit[item];
    index=PopMenu3_Base[item];
    ptr1=(u8*)&PM3_Str[index];
    switch(j){
    case FILE:
      FMenu_Pop(Cur_Limit,Cur_Item,ptr1);
      break;
    case WAVE:
      ptr2=(u8*)&Wave_Str[PopMenu3_Value[WAVE_Type]]; 
      memcpy(&ptr,&ptr2[0],LEN-1);
      ptr2=(u8*)&Freq_Str[PopMenu3_Value[WAVE_Freq]]; 
      memcpy(&ptr[LEN],&ptr2[1],LEN-1); 
      ptr2=(u8*)&Duty_Str[PopMenu3_Value[WAVE_Duty]]; 
      memcpy(&ptr[LEN*2],&ptr2[1],LEN-1);         
      sprintf((char*)ptr3,"%2d",PopMenu3_Value[WAVE_Duty]);  
      memcpy(&ptr[LEN*3],ptr3,2);
      PMenu_Pop(Cur_Limit,Cur_Item,ptr1,7,ptr);
      break;
    case SYSTEM:
      ptr2=(u8*)&Duty_Str[PopMenu3_Value[SYS_Volume]]; 
      memcpy(&ptr,&ptr2[0],LEN-3);
      ptr2=(u8*)&Duty_Str[PopMenu3_Value[SYS_BKLight]]; 
      memcpy(&ptr[LEN],&ptr2[0],LEN-3);
      sprintf((char*)ptr3,"%2d",PopMenu3_Value[SYS_Standy]);
      strcat((char*)ptr3,"min ");
      memcpy(&ptr[LEN*2],ptr3,6);
      sprintf((char*)ptr3,"%2d",PopMenu3_Value[SYS_PowerOff]);
      strcat((char*)ptr3,"min ");
      memcpy(&ptr[LEN*3],ptr3,6);
      memcpy(&ptr[LEN*4],":",1);
      memcpy(&ptr[LEN*4+1],(u8*)&Swintch1[PopMenu3_Value[SYS_MenuCyc]],5);
      memcpy(&ptr[LEN*5],":",1);
      memcpy(&ptr[LEN*5+1],(u8*)&Swintch1[PopMenu3_Value[SYS_ItemCyc]],5);
      memcpy(&ptr[LEN*6],":",1);
      memcpy(&ptr[LEN*6+1],(u8*)&Swintch1[PopMenu3_Value[SYS_PosiCyc]],5);
      PMenu_Pop(Cur_Limit,Cur_Item,ptr1,9,ptr);
      break;
    case CALIBRATION:
      memcpy(&ptr,     " Zero ",6);
      memcpy(&ptr[LEN]," Data ",6);
      PMenu_Pop(Cur_Limit,Cur_Item,ptr1,9,ptr);
      break;
    case PRO:
      Product_Pop("Product Info", 105, 20, 96, 23*6);
      break;
    case ABOUT:  
      if(About_Flag){
        About_Flag =0;
      }
      else About_Flag =1;
      if(About_Flag){
        Open_Bmp(0);
      }
      else{
        Label_Flag  |= UPD;
        Update_Label();
      } 
      break;
    }
    break;
  }
}

/*******************************************************************************
显示第二页测量菜单 
*******************************************************************************/
void Show_Measure(void)
{
  u8 *ptr;
  int j,k;
  s16 x,y;
  u8  source,type,enable;
  rowh=0;
  
  if(Vbat_Flag == 1){
    Vbat_Temp = Vbat;
    Vbat_Flag = 0;
  }
  for(k=0; k<6; k++){
    source = PopMenu2_Value[k*3];
    type   = PopMenu2_Value[k*3+1];
    enable = PopMenu2_Value[k*3+2];
    Foreground=BLK;
    if(menu.menu_index[menu.current]==k)Background=GRN;
    else {
      if(enable==1){ Background=WHT;  Foreground=BLK;}
      else          {Background=GRY; }
    }
    Sx=Menu_X+8;
    rowh+=RowHeight;
    Sy=ITEM_Y-rowh;
    ptr=(u8*)&MeasItem_Menu[type];
    if(k==5){
      DispStr6x8(Sx+2, Sy,  PRN, "         ");
      Sx=Menu_X+6;
      DispStr6x8(Sx+2, Sy, SYMB, "':");         //显示电池符号 
      Sx=Menu_X+21;
      Sy=ITEM_Y-rowh;
      if(__Info(P_VUSB)){
        if((Vbat > Vbat_Temp) && ((Vbat - Vbat_Temp) < 500))Vbat_Temp = Vbat;
      }else{
        if((Vbat < Vbat_Temp) && ((Vbat_Temp - Vbat) < 500))Vbat_Temp = Vbat;
      }
      Value2Str(NumStr, Vbat_Temp*1000, (u8*)YSTR, 3, STD);
      ptr = NumStr;
    }
    DispStr6x8(Sx, Sy, PRN, ptr); 
    Draw_Rectangle(Background, Menu_X+8, ITEM_Y-rowh+11, 3, 49); 
    Sx=Menu_X+8;
    rowh+=RowHeight;
    Sy=ITEM_Y-rowh;
    if(k<5)PrintStr(PRN, "       ");
    MeasureStr(source,type);
    //===画通道颜色====可调用Dreaw_Circle_S()画圆角  
    Sx=Menu_X+8;Sy=ITEM_Y-rowh+3;
    if(k<5){
      DispStr6x8(Sx, Sy, PRN, NumStr);    
      Draw_Circle_S(CH_Col[source], Menu_X, ITEM_Y-rowh, 28, 6);//画项目栏颜色
    }
    else if(k==5) Draw_Circle_S(CH_Col[source], Menu_X, ITEM_Y-rowh+14, 14, 6);
    //==============
    rowh++;
    x=Menu_X+2; //Menu间隔黑线
    y=ITEM_Y-rowh;
    if(k<5)
      for(j=0; j<58; j++){
        SetPosi(x++, y);
        SetPixel(BLK);
      }
  }
  Foreground=BLK;
}

/*******************************************************************************
窗口目录处理 
*******************************************************************************/
void PMenu_Proc(u8 mode,u8 Cur_Item,u8 step)
{
  s16 j,index,limit;
  u16 temp = 0; 
  u8 item=0;
  if(Windows_Pop==0){
    menu.iflag|=UPD;
    Label_Flag|=UPD;
  }
  switch (menu.current){
  //------------------------------第一页菜单----------------------------------//
  case Oscillo:
    j=menu.menu_index[menu.current];
    index=PopMenu1_Base[j]+Cur_PopItem-1;
    //---------------加模式---------------
    if (mode==add){ 
     // +++++++++index不为Threshold时++++++++++++++++
      if(index != TRI_Ch1){   
        if(index>TRI_Fit)index++; //因为过了tri2，所以index+1
        limit=Popmenu1_Limit1[index]-step;
        if(index==CH1_Posi){     //Ch1位置上调模式
          if((PopMenu1_Value[CH1_Posi]<195)
             &&((PopMenu1_Value[TRI_Ch1]+PopMenu1_Value[CH1_Posi])>195+step)){
            PopMenu1_Value[TRI_Ch1]--;
            PopMenu1_Value[CH1_Posi]++;
          }else if(PopMenu1_Value[CH1_Posi]<limit)
            PopMenu1_Value[CH1_Posi]++;
        }
        else if(index==CH2_Posi){//Ch2位置上调模式
          if((PopMenu1_Value[CH2_Posi]<195)
             &&((PopMenu1_Value[TRI_Ch2]+PopMenu1_Value[CH2_Posi])>195+step)){
            PopMenu1_Value[TRI_Ch2]--;
            PopMenu1_Value[CH2_Posi]++;
          }else if(PopMenu1_Value[CH2_Posi]<limit)
            PopMenu1_Value[CH2_Posi]++;
        }
       else if(index==WIN_Posi){
         switch(PopMenu1_Value[WIN_Depth]){ //不同采样深度WIN_Posi的限制
         case 0:
           limit = 774-55; 
           break;
         case 1:
           limit = 1798-55;
           break;
         case 2:
           limit = 3846-55;
           break;
         case 3:
           limit = 7800;//7939-55;
           break;
         }
         if((PopMenu1_Value[TRI_Sync]!= NONE)
            &&(PopMenu1_Value[TRI_Sync]!= SCAN)){
           if(step!=0) PopMenu1_Value[index]+=step;
           else if(PopMenu1_Value[index]<limit)PopMenu1_Value[index]+=5;
         }
       } 
        else if(PopMenu1_Value[index]<limit){   //其他选项上调模式
          temp = PopMenu1_Value[TIM_Base];
          if(step!=0) PopMenu1_Value[index]+=step;
          else        PopMenu1_Value[index]++;
          if((temp==1)&&(PopMenu1_Value[TIM_Base]==2))__Ctrl(SMPL_MODE, SIMULTANEO);
          if(PopMenu1_Value[TRI_Fit])Key_S_Time = 300;
          else                       Key_S_Time = 0;
        }else if(PopMenu3_Value[SYS_ItemCyc]) {//选项循环模式
          if((index!=CH1_Vol)&&(index!=CH2_Vol)&&(index!=TIM_Base))
            PopMenu1_Value[index]=PopMenu1_Limit2[index];
        }
      }
      //++++++++index为Threshold时+++++++++++
      else{           
        if(PopMenu1_Value[TRI_Ch]){   //trigger为ch2是，对应的值处理  
          if((PopMenu1_Value[TRI_Ch2]+PopMenu1_Value[CH2_Posi])<195+step){
            if(step!=0) PopMenu1_Value[TRI_Ch2]+=step;
            else        PopMenu1_Value[TRI_Ch2]++;
          }
        }
        else{                         //trigger为ch1是，对应的值处理
          if((PopMenu1_Value[TRI_Ch1]+PopMenu1_Value[CH1_Posi])<195+step){
            if(step!=0) PopMenu1_Value[TRI_Ch1]+=step;
            else        PopMenu1_Value[TRI_Ch1]++;
          }
        }
      }     
    }
    //-------------减模式 ------------------
    else{
      if(index != TRI_Ch1){           // index不为Threshold时
        if(index>TRI_Fit)index++;
        limit=PopMenu1_Limit2[index]+step;
        
        if(index==CH1_Posi){
          if((PopMenu1_Value[CH1_Posi]>5)
             &&((PopMenu1_Value[TRI_Ch1]+PopMenu1_Value[CH1_Posi])<5+step)){
            PopMenu1_Value[TRI_Ch1]++;
            PopMenu1_Value[CH1_Posi]--;
          }else if(PopMenu1_Value[CH1_Posi]>limit)
            PopMenu1_Value[CH1_Posi]--;
        }
        else if(index==CH2_Posi){
          if((PopMenu1_Value[CH2_Posi]>5)
             &&((PopMenu1_Value[TRI_Ch2]+PopMenu1_Value[CH2_Posi])<5+step)){
            PopMenu1_Value[TRI_Ch2]++;
            PopMenu1_Value[CH2_Posi]--;
          }else if(PopMenu1_Value[CH2_Posi]>limit)
            PopMenu1_Value[CH2_Posi]--;
        } 
        else if(index==WIN_Posi){
          if((PopMenu1_Value[TRI_Sync] != NONE)
             &&(PopMenu1_Value[TRI_Sync] != SCAN)){
            if(step!=0) PopMenu1_Value[index]-=step;
            else if(PopMenu1_Value[index]>limit) PopMenu1_Value[index]-=5;
          }
        }
        else if(PopMenu1_Value[index]>limit){
          temp = PopMenu1_Value[TIM_Base];
          if(step!=0) PopMenu1_Value[index]-=step;
          else        PopMenu1_Value[index]--;
          if((temp==2)&&(PopMenu1_Value[TIM_Base]==1))__Ctrl(SMPL_MODE, INTERLEAVE);
          if(PopMenu1_Value[TRI_Fit])Key_S_Time = 300;
          else                       Key_S_Time = 0;
          
        }else if(PopMenu3_Value[SYS_ItemCyc]){
          if((index!=CH1_Vol)&&(index!=CH2_Vol)&&(index!=TIM_Base)) 
            PopMenu1_Value[index]=Popmenu1_Limit1[index];
        }
      }
      else{                                // index为Threshold时
        if(PopMenu1_Value[TRI_Ch]){        //trigger为ch2是，对应的值处理  
          if((PopMenu1_Value[TRI_Ch2]+PopMenu1_Value[CH2_Posi])>5+step){
            if(step!=0) PopMenu1_Value[TRI_Ch2]-=step;
            else        PopMenu1_Value[TRI_Ch2]--;
          }
        }
        else{
          if((PopMenu1_Value[TRI_Ch1]+PopMenu1_Value[CH1_Posi])>5+step){
            if(step!=0) PopMenu1_Value[TRI_Ch1]-=step;
            else        PopMenu1_Value[TRI_Ch1]--;
          }
        }         
      } 
    }
    if(index==CH1_Vol || index == CH2_Vol) Update[V1F] |= UPD ;  //更新V1-V2
    if(index==TIM_Base)                    Update[T1F] |= UPD ;
    break;
 //------------------------------第二页菜单----------------------------------//
 case Measure:
      j=menu.menu_index[menu.current];
      index=3*j+Cur_PopItem-1;               //popmenu2_base[j]
      if (mode==add){
        limit=PopMenu2_Limit1[Cur_PopItem-1];//-stepindex
        if(PopMenu2_Value[index]<limit)PopMenu2_Value[index]++;
        else{
          if(PopMenu3_Value[SYS_ItemCyc]) 
            PopMenu2_Value[index]=PopMenu2_Limit2[index];
        }
      }
      else{
        limit=0;                             //PopMenu2_Limit2[index]+step;
        if(PopMenu2_Value[index]>limit)PopMenu2_Value[index]--;
        else{
          if(PopMenu3_Value[SYS_ItemCyc]) 
            PopMenu2_Value[index]=PopMenu2_Limit1[index];
        }
      }
    break;
 //------------------------------第三页菜单----------------------------------//   
  case Option:
      item=menu.menu_index[menu.current];  
      index=PopMenu3_Base[item]+Cur_PopItem-1;
     
      if (mode==add){
        if(PopMenu3_Value[WAVE_Type]>0 && index==WAVE_Freq)
          limit=PopMenu3_Limit1[index]-step-5; 
        else limit=PopMenu3_Limit1[index]-step;
        if(PopMenu3_Value[index]<limit){
          if(step!=0) PopMenu3_Value[index]+=step;
          else        PopMenu3_Value[index]++;
        }
        else{
          if(PopMenu3_Value[SYS_ItemCyc]) 
            PopMenu3_Value[index]=PopMenu3_Limit2[index];
        }
      }
      else{
        limit=PopMenu3_Limit2[index]+step;
        if(PopMenu3_Value[index]>limit){
          if(step!=0) PopMenu3_Value[index]-=step;
          else        PopMenu3_Value[index]--;
        }else{
          if(PopMenu3_Value[SYS_ItemCyc]) 
            PopMenu3_Value[index]=PopMenu3_Limit1[index];
        }
      }
    break;
  }
  Show_PopMenu(Cur_PopItem);
  Update_Proc();
}
/*******************************************************************************
主菜单项目处理，单击按键处理
*******************************************************************************/
void Item_Proc(u8 mode)
{
  u16 temp = 0;
  
  u16 j, index;
  menu.iflag|=UPD;  
  switch (menu.current){                  //当面菜单页
//------------------------------第一页菜单------------------------------------//  
  case Oscillo:
    j=menu.menu_index[menu.current];      //当前菜单项目
    index=PopMenu1_Base[j];               //当前子窗口第一项
    
    switch (j){    
    case TRIGGER: 
      index+=2;
    case CH1:       Update[V1F] |= UPD ;  //更新V1-V2
    case CH2:       Update[V1F] |= UPD ;  //更新V1-V2
    case TIMEBASE:  Update[T1F] |= UPD ;
      if(mode==add){
        
        temp = PopMenu1_Value[TIM_Base];
        
        if(PopMenu1_Value[index]<Popmenu1_Limit1[index])
          PopMenu1_Value[index]++;
        else if(PopMenu3_Value[SYS_MenuCyc]){
          if((index!=CH1_Vol)&&(index!=CH2_Vol)&&(index!=TIM_Base))
          PopMenu1_Value[index] = PopMenu1_Limit2[index];
        }
        
        if((temp==1)&&(PopMenu1_Value[TIM_Base]==2))__Ctrl(SMPL_MODE, SIMULTANEO);
      }
      else{
        
        temp = PopMenu1_Value[TIM_Base];
        
        
        if(PopMenu1_Value[index]>PopMenu1_Limit2[index])
          PopMenu1_Value[index]--;
        else if(PopMenu3_Value[SYS_MenuCyc]){
          if((index!=CH1_Vol)&&(index!=CH2_Vol)&&(index!=TIM_Base))
          PopMenu1_Value[index] = Popmenu1_Limit1[index];
        }
        
        if((temp==2)&&(PopMenu1_Value[TIM_Base]==1))__Ctrl(SMPL_MODE, INTERLEAVE);
      }
      break;  
    default:
      break;  
    }
    break;
//------------------------------第二页菜单------------------------------------//     
  case Measure:
    j=menu.menu_index[menu.current];        //当前菜单项目
    index = j*3+1;
    if(mode==add){
      if(PopMenu2_Value[index]<PopMenu2_Limit1[1])
        PopMenu2_Value[index]++;
      else if(PopMenu3_Value[SYS_MenuCyc])
          PopMenu2_Value[index] = PopMenu2_Limit2[index];
    }
    else{
      if(PopMenu2_Value[index]>PopMenu2_Limit2[index])
        PopMenu2_Value[index]--;
      else if(PopMenu3_Value[SYS_MenuCyc])
          PopMenu2_Value[index] = PopMenu2_Limit1[index];
    }    
    if(menu.menu_flag == 1)Show_Measure();                         //刷新显示
    break;
 //------------------------------第三页菜单-----------------------------------//    
  case Option:
    break;
  default:
    break;
  }
  Update_Proc();                            //刷新单击操作
}

/*******************************************************************************
子菜单垂直滑动处理
*******************************************************************************/
void Vertical_Slide(u8 mode)
{
  u16 j, index;
  
  switch (menu.current){                    //当面菜单页
//------------------------------第一页菜单------------------------------------//  
  case Oscillo:
    j=menu.menu_index[menu.current];       //当前菜单项目
    index=PopMenu1_Base[j]+1;              //当前子窗口第一项
    
    switch (j){    
    case TRIGGER: 
      index = (PopMenu1_Value[TRI_Ch])?TRI_Ch2:TRI_Ch1;
    case CH1:              
    case CH2:     
    case CH3:        
      if(mode==add){
        if(index==CH1_Posi){  
          if((PopMenu1_Value[CH1_Posi]<195)
             &&((PopMenu1_Value[TRI_Ch1]+PopMenu1_Value[CH1_Posi])>195)){
            PopMenu1_Value[TRI_Ch1]--;
            PopMenu1_Value[CH1_Posi]++;
          }else if(PopMenu1_Value[CH1_Posi]<Popmenu1_Limit1[index])
            PopMenu1_Value[CH1_Posi]++;
        }
        else if(index==CH2_Posi){
          if((PopMenu1_Value[CH2_Posi]<195)
             &&((PopMenu1_Value[TRI_Ch2]+PopMenu1_Value[CH2_Posi])>195)){
            PopMenu1_Value[TRI_Ch2]--;
            PopMenu1_Value[CH2_Posi]++;
          }else if(PopMenu1_Value[CH2_Posi]<Popmenu1_Limit1[index])
            PopMenu1_Value[CH2_Posi]++;
        }
        else if((index==TRI_Ch1)||(index==TRI_Ch2)){
          if(PopMenu1_Value[TRI_Ch]){    
            if((PopMenu1_Value[TRI_Ch2]+PopMenu1_Value[CH2_Posi])<195){
              PopMenu1_Value[TRI_Ch2]++;
            }
          }
          else{        
            if((PopMenu1_Value[TRI_Ch1]+PopMenu1_Value[CH1_Posi])<195){
              PopMenu1_Value[TRI_Ch1]++;
            }
          }
        }
        else{
          if(PopMenu1_Value[index]<Popmenu1_Limit1[index])
            PopMenu1_Value[index]++;
        }
      }
      //----dec mode----
      else{
        if(index==CH1_Posi){
          if((PopMenu1_Value[CH1_Posi]>5)
             &&((PopMenu1_Value[TRI_Ch1]+PopMenu1_Value[CH1_Posi])<5)){
            PopMenu1_Value[TRI_Ch1]++;
            PopMenu1_Value[CH1_Posi]--;
          }else if(PopMenu1_Value[CH1_Posi]>PopMenu1_Limit2[index])
            PopMenu1_Value[CH1_Posi]--;
        }
        else if(index==CH2_Posi){
          if((PopMenu1_Value[CH2_Posi]>5)
             &&((PopMenu1_Value[TRI_Ch2]+PopMenu1_Value[CH2_Posi])<5)){
            PopMenu1_Value[TRI_Ch2]++;
            PopMenu1_Value[CH2_Posi]--;
          }else if(PopMenu1_Value[CH2_Posi]>PopMenu1_Limit2[index])
            PopMenu1_Value[CH2_Posi]--;
        } 
        else if((index==TRI_Ch1)||(index==TRI_Ch2)){
          if(PopMenu1_Value[TRI_Ch]){         
            if((PopMenu1_Value[TRI_Ch2]+PopMenu1_Value[CH2_Posi])>5){
              PopMenu1_Value[TRI_Ch2]--;
            }
          }
          else{
            if((PopMenu1_Value[TRI_Ch1]+PopMenu1_Value[CH1_Posi])>5){
              PopMenu1_Value[TRI_Ch1]--;
            }
          } 
        }
        else{
          if(PopMenu1_Value[index]>PopMenu1_Limit2[index])
            PopMenu1_Value[index]--;
        }
      }
      
      Update_Proc_All();
      Label_Flag  |= UPD;
      Update_Label();
      Beep(50); 
      
      break;  
    default:
      break;  
    }
    break;
  default:
    break;
  }
}

/*******************************************************************************
单一对应更新处理 
*******************************************************************************/
void Update_Proc(void)
{
  s16 j,k,l,index;
  if(Windows_Pop==0){
    menu.iflag|=UPD;
    Label_Flag|=UPD;
  }
  switch (menu.current){
//------------------------------第一页菜单------------------------------------//
  case Oscillo:
    j=menu.menu_index[menu.current];
    index=PopMenu1_Base[j];
    switch (j){
    case CH1:  
      if((Status != STOP)){

      GainA  = PopMenu1_Value[CH1_Vol];   
        
        if(PopMenu1_Value[CH1_Vol]>3)
          KindA  = HV;
        else   
          KindA  = LV;                //Ch1选择低压或高压量程
        StateA =  (PopMenu1_Value[CH1_Vol]>6)?ACT: GND; 
        
        CouplA = (PopMenu1_Value[CH1_Coupl])? AC : DC;                      // AC);  //AC/DC耦合方式
        
      __Ctrl(AiRANGE, KindA+CouplA+StateA);             //Ch1状态刷新
      }
      ParamTab[P1x2]=2*(PopMenu1_Value[CH1_Posi]);      //Ch1游标位置 
      ParamTab[W1F]=PopMenu1_Value[CH1_Enable]? L_HID : B_HID;  //Ch1 show_hide 
      AiPosi(PopMenu1_Value[CH1_Posi]);                 //Ch1硬件对应位置
      k=(PopMenu1_Value[TRI_Ch])?PopMenu1_Value[TRI_Ch2]:PopMenu1_Value[TRI_Ch1];
      l=(PopMenu1_Value[TRI_Ch])?ParamTab[P2x2]:ParamTab[P1x2]; 
      ParamTab[VTx2]=l+2*k;
      ParamTab[VTC]=CH_Col[PopMenu1_Value[TRI_Ch]];
      ParamTab[VTF]=(PopMenu1_Value[TRI_Enable])? W_HID : L_HID;
      if(Cur_PopItem==2)Update[W1F]|=UPD;               //游标刷新标志
      break;
    case CH2:
      if((Status != STOP)){
        GainB  = PopMenu1_Value[CH2_Vol];   
        
        if(PopMenu1_Value[CH2_Vol]>3)
          KindB  = HV;
        else   
          KindB  = LV;                //Ch1选择低压或高压量程
        StateB =  (PopMenu1_Value[CH2_Vol]>6)?ACT: GND; 
        
        CouplB = (PopMenu1_Value[CH2_Coupl])? AC : DC;                      // AC);  //AC/DC耦合方式
        
      __Ctrl(BiRANGE, KindB+CouplB+StateB);
      }
      ParamTab[P2x2]=2*(PopMenu1_Value[CH2_Posi]);
      ParamTab[W2F]=PopMenu1_Value[CH2_Enable]? L_HID : B_HID;
      BiPosi(PopMenu1_Value[CH2_Posi]);
      k=(PopMenu1_Value[TRI_Ch])?PopMenu1_Value[TRI_Ch2]:PopMenu1_Value[TRI_Ch1];
      l=(PopMenu1_Value[TRI_Ch])?ParamTab[P2x2]:ParamTab[P1x2]; 
      ParamTab[VTx2]=l+2*k;
      ParamTab[VTC]=CH_Col[PopMenu1_Value[TRI_Ch]];
      ParamTab[VTF]=(PopMenu1_Value[TRI_Enable])? W_HID : L_HID;
      if(Cur_PopItem==2)Update[W2F]|=UPD;
      break;
    case CH3:
      ParamTab[W3F]=PopMenu1_Value[CH3_Enable]? L_HID : B_HID;
      ParamTab[P3x2]=2*(PopMenu1_Value[CH3_Posi]);
      if(Cur_PopItem==2)Update[W3F]|=UPD;
      break;
    case TIMEBASE:
      Set_Base(PopMenu1_Value[TIM_Base]);              //硬件设置扫描时基档位
      break;
    case TRIGGER:
      k=(PopMenu1_Value[TRI_Ch])?PopMenu1_Value[TRI_Ch2]:PopMenu1_Value[TRI_Ch1];
      l=(PopMenu1_Value[TRI_Ch])?ParamTab[P2x2]:ParamTab[P1x2]; 
      ParamTab[VTx2]=l+2*k;
      ParamTab[VTC]=CH_Col[PopMenu1_Value[TRI_Ch]];
      ParamTab[VTF]=(PopMenu1_Value[TRI_Enable])? W_HID : L_HID;
      break;
    case COURSOR:
      ParamTab[T1x1]=PopMenu1_Value[CUR_T1]+1;
      ParamTab[T2x1]=PopMenu1_Value[CUR_T2]+1;
      if(PopMenu1_Value[CUR_T]==1)
        ParamTab[T1F]=SHOW;
      else ParamTab[T1F]=L_HID;
      ParamTab[T2F]=ParamTab[T1F];
      ParamTab[V1x2]=2*(PopMenu1_Value[CUR_V1]+1);   
      ParamTab[V2x2]=2*(PopMenu1_Value[CUR_V2]+1);    
      if(PopMenu1_Value[CUR_V]==0)                     // CH1,Ch2,OFF
        ParamTab[V1F]=L_HID;
      else ParamTab[V1F]=SHOW;
      ParamTab[V2F]=ParamTab[V1F];
      if(Cur_PopItem==4)Update[V1F]|=UPD;
      if(Cur_PopItem==5)Update[V2F]|=UPD;
      if(Cur_PopItem==1)Update[T1F]|=UPD;
      if(Cur_PopItem==2)Update[T2F]|=UPD;
      if(Cur_PopItem==6)Update[V1F]|=UPD ;            //更新V1-V2
      break;
    case WINDOWS:
      index++;
      switch(PopMenu1_Value[WIN_T0]){
        case 0:
          ParamTab[T0x1]= 125 - PopMenu1_Value[WIN_Posi];
          if(Cur_PopItem==4 || Cur_PopItem==2){
            T0_PerCnt =  PRE_SMPL;} 
          break;
        case 1:
          ParamTab[T0x1]= Depth/2+125 - PopMenu1_Value[WIN_Posi];
          if(Cur_PopItem==4 || Cur_PopItem==2){T0_PerCnt =  Depth/2 - PRE_SMPL;}
          break;
        case 2:
          ParamTab[T0x1]= Depth-125 - PopMenu1_Value[WIN_Posi];
          if(Cur_PopItem==4 || Cur_PopItem==2){T0_PerCnt =  Depth - PRE_SMPL ;}
          break;  
        }
      ParamTab[T0F] =!PopMenu1_Value[WIN_Enable];
      if(Status != STOP)ADC_Start();                    //重新开始ADC扫描采样
      break;
    default:
      break;
    }
    break;
//------------------------------第二页菜单------------------------------------//   
  case Measure:
    j=menu.menu_index[menu.current];
    switch(j){
    case MEA_1:
      break;
    }
    break;
//------------------------------第三页菜单------------------------------------//    
  case Option: 
    j= 7+6+menu.menu_index[menu.current];
    index=PopMenu3_Base[menu.menu_index[menu.current]];
    switch(j){
    case FILE: 
      break;
    case WAVE:      
      if(Cur_PopItem==1){                                    //波形类型选项
        if(PopMenu3_Value[WAVE_Type]==0){                    //脉冲
          __Ctrl(OUT_PSC, FPSC[PopMenu3_Value[WAVE_Freq]]);  // 72MHz/72 = 1MHz 
          __Ctrl(OUT_ARR, FARR[PopMenu3_Value[WAVE_Freq]]);  // 1MHz/100 = 10KHz 
          __Ctrl(OUT_WTH, FARR[PopMenu3_Value[WAVE_Freq]]*PopMenu3_Value[WAVE_Duty]/10); 
          __Ctrl(OUT_MOD, PULSED);                           // 设定脉冲输出模式
        }
        else{                                         //模拟波形输出
         if(PopMenu3_Value[WAVE_Freq] > MAX_FREQ){    //模拟波形输出不超过20KHz
             PopMenu3_Value[WAVE_Freq] = MAX_FREQ ;   //20KHz
          }
          __Ctrl(OUT_MOD, DISABLE);
          {
            __Ctrl(OUT_CNT, 180);       
            TIM_DA->PSC = Dac_Psc[PopMenu3_Value[WAVE_Freq]] - 1;
            __Ctrl(DAC_TIM, Dac_Tim[PopMenu3_Value[WAVE_Freq]]);  
            __Ctrl(OUT_BUF, __Info(22+PopMenu3_Value[WAVE_Type]));
          }
          __Ctrl(OUT_MOD, ANALOG);
          PopMenu3_Value[WAVE_Duty] = 5; //Duty =50% 
          Show_PopMenu(Cur_PopItem);     //当输出波形不为方波时，Duty刷新为50%   
        }
      }
      if(Cur_PopItem==2){
        if(PopMenu3_Value[WAVE_Type]==0){
          __Ctrl(OUT_PSC, FPSC[PopMenu3_Value[WAVE_Freq]]);  // 72MHz/72 = 1MHz  
          __Ctrl(OUT_ARR, FARR[PopMenu3_Value[WAVE_Freq]]);  // 1MHz/100 = 10KHz 
          __Ctrl(OUT_WTH, FARR[PopMenu3_Value[WAVE_Freq]]*PopMenu3_Value[WAVE_Duty]/10);   
        }
        else{
          if(PopMenu3_Value[WAVE_Freq] > MAX_FREQ){         
             PopMenu3_Value[WAVE_Freq] = MAX_FREQ ;         
          }
          __Ctrl(OUT_MOD, DISABLE);
          {
            __Ctrl(OUT_CNT, 180);                 // 模拟输出周期(每周期180点)
            TIM_DA->PSC = Dac_Psc[PopMenu3_Value[WAVE_Freq]] - 1;
            __Ctrl(DAC_TIM, Dac_Tim[PopMenu3_Value[WAVE_Freq]]);  
            __Ctrl(OUT_BUF, __Info(22+PopMenu3_Value[WAVE_Type]));    
          }
          __Ctrl(OUT_MOD, ANALOG);
          PopMenu3_Value[WAVE_Duty] = 5;   //Duty =50% 
          Show_PopMenu(Cur_PopItem);      //当输出波形不为方波时，Duty刷新为50% 
        }
      }
      if(Cur_PopItem==3){
        if(PopMenu3_Value[WAVE_Type]==0){
          __Ctrl(OUT_WTH, FARR[PopMenu3_Value[WAVE_Freq]]*PopMenu3_Value[WAVE_Duty]/10); 
        }
      }
      break;
    case SYSTEM:
      if(Cur_PopItem==1){
        __Ctrl(BUZZVOL, PopMenu3_Value[SYS_Volume]*10);                 
        Beep(100);}
      if(Cur_PopItem==2)__Ctrl(B_LIGHT, PopMenu3_Value[SYS_BKLight]*10);
      if(Cur_PopItem==3)PD_Cnt = PopMenu3_Value[SYS_Standy]*Unit;
      if(Cur_PopItem==4)AutoPwr_Cnt = PopMenu3_Value[SYS_PowerOff]*Unit;
      
      break;
    case CALIBRATION:
      break;
    case PRO:
      break;
    case ABOUT:
      break;  
    }
    break;  
  }  
}  
/*******************************************************************************
显示菜单窗口 
*******************************************************************************/
void PMenu_Pop(u8 item,u8 current,u8* str1, u8 str_num, u8* str2)
{
  u8  Wink, i, j;
  u16 Px,Py;
  u8* ptr1,*ptr2;
  j=menu.menu_index[menu.current];
  item=Cur_Limit;
  PopColor(WHT, DAR); 
  PopType |= LIST_POP;
  OpenPop(PopMenu1_Posi_X[j], PopMenu1_Posi_Y[j], (item)*12+12,  16*6+2, COVER);
  ptr1=str1;
  ptr2=str2;
  Wink = INV;
   
  PopColor(WHT, DAR);
  for(i = 1; i <= item; i++){
    Px = 4;
    Py = 4+13*(item-i);
    if(i==current){
      Wink = INV;}
    else  {
      Wink = PRN;
    }
    Pop_STR6x8(Px, Py, Wink, ptr1);
    Px = 4+str_num*6;   
    Pop_STR6x8(Px, Py, Wink, ptr2);
    ptr1+=13;
    ptr2+=9;    
  }
  PopCnt = POP_TIME;        
}
/*******************************************************************************
文件菜单窗口 
*******************************************************************************/
void FMenu_Pop(u8 item,u8 current,u8* str1)
{
  u8  Wink, i, j;
  u16 Px,Py;
  u8* ptr1;
  j=menu.menu_index[menu.current];
  item=Cur_Limit;
  PopColor(WHT, DAR); 
  PopType |= FILE_POP;
  OpenPop(PopMenu3_Posi_X[j], PopMenu3_Posi_Y[j],(item)*16+4,  14*6,  COVER); 
  ptr1=str1;
  Wink = INV;
  for(i = 1; i <= item; i++){
    Px = 6;
    Py = 4+16*(item-i);
    if(i==current)Wink = INV;
    else  Wink = PRN;
    Pop_STR6x8(Px, Py, Wink, ptr1);
    Px = 6+5*6;
    if(i>1)
    {
      sprintf((char*)NumStr,"%02d",
              PopMenu3_Value[PopMenu3_Base[menu.menu_index[menu.current]]+i-1]);   
      Pop_STR6x8(Px, Py, Wink, NumStr);
    }
    ptr1+=13;   
  }
  PopCnt = POP_TIME;        
}

/*******************************************************************************
菜单隐藏标志 
*******************************************************************************/
void Menu_Hide(void)
{
  u16 x, y, i, j, high, Width, col;
  col = WHT;
  high = 50; Width = 8;
  x = 313; y = 50;
  
  SetPosi(x,y+2);
  for(j=0;j<high-4;j++)SetPixel(col);
  SetPosi(x+1,y+1);
  for(j=0;j<high-2;j++)SetPixel(col);
  for(i=0;i<Width;i++){ 
    SetPosi(x+2,y); x++;
    for(j=0;j<high;j++)SetPixel(col); 
  } 
}
/*******************************************************************************
 Update_Label: 游标位置显示
*******************************************************************************/
void Update_Label(void)
{
  s16 x,y;
  u16 i,j;
  if(!(Label_Flag & UPD))return;
  Windows_Pop=0;
  Label_Flag &=~ UPD;
  x=0;y=19;   //左栏擦除
  for(i=0;i<7;i++){
    SetPosi(x++, y);
    for(j=0; j<204; j++)SetPixel(DAR);//
  }  
 //--------tmep process-------------------------------//  
  if(((Update[VTF] & UPD)==UPD) ){
    Sx = 0;
    Sy =ParamTab[VTx2]/2+16;
    SetColor(DAR, ParamTab[VTC]);
    PrintStr_Cur(SYMB,"4"); //T
  }
  if(((Update[W1F] & UPD)==UPD)){
    Sx = 0;
    Sy =ParamTab[P1x2]/2+16;  
    //SetColor(DAR,ParamTab[W1C]);
    SetColor(DAR,ParamTab[W1C]);
    PrintStr_Cur(SYMB,"1"); //A
  }
  if(((Update[W2F] & UPD)==UPD) ){
    Sx = 0;
    Sy =ParamTab[P2x2]/2+16;  
    SetColor(DAR, ParamTab[W2C]);
    PrintStr_Cur(SYMB,"2");//B
  }
  if(((Update[W3F] & UPD)==UPD) ){
    Sx = 0;
    Sy =ParamTab[P3x2]/2+16;  
    SetColor(DAR, ParamTab[W3C]);
    PrintStr_Cur(SYMB,"3");//C
  } 
  if((((Update[T1F] & UPD)==UPD) )&& (ParamTab[T1F]==SHOW)){
    SetColor(DAR, ORN);
    Print_dT_Info(INV);
    Update[T1F] &=~ UPD ;
  }
   if((((Update[T2F] & UPD)==UPD) )&& (ParamTab[T2F]==SHOW)){
    SetColor(DAR, ORN);
    Print_dT_Info(INV);
    Update[T2F] &=~ UPD ;
  }  
  if(((Update[V1F] & UPD)==UPD) &&(ParamTab[V1F]==SHOW)){
    Sx = 0;
    Sy =ParamTab[V1x2]/2+13;  
    SetColor(DAR, CYN);
    Print_dV_Info(INV); 
    Sx=12*8;   
    Update[V1F] &=~ UPD ;
  }
  if(((Update[V2F] & UPD)==UPD) &&(ParamTab[V2F]==SHOW)){
    Sx = 0;
    Sy =ParamTab[V2x2]/2+13;  
    SetColor(DAR, CYN);
    Print_dV_Info(INV); 
    Sx=12*8; 
    Update[V2F] &=~ UPD ;
  }
    //NORM没触发时，底端测量值不更新
  if((PopMenu1_Value[TRI_Sync] == NORM)&&(Trigger_k < 0));  
  else{
    Print_dM_Info(INV, 0, 12*6+5 , TITLE_L);
    Print_dM_Info(INV, 1, 12*6+14*6+10 , TITLE_L);
  }
 
}

/*******************************************************************************
运行、暂停状态更新 
*******************************************************************************/
void Update_Status(void)
{
  u8  index;
  Sx = Title_Posi_X[6];
  Sy=TITLE_H+2;
  
  if(Status==STOP){
    Draw_Circle_D(RED, Title_Posi_X[6]-3, TITLE_H+2, 11, 2, 5*6-1);//前景边框
    SetColor(DAR,RED);
    PrintStr6x8(INV,(u8*)&Mode[5] );
  }
  else{
    Draw_Circle_D(GRN, Title_Posi_X[6]-3, TITLE_H+2, 11, 2, 5*6-1);//前景边框
    index=PopMenu1_Base[TRIGGER];
    SetColor(DAR,GRN);
    PrintStr6x8(INV,(u8*)&Mode[PopMenu1_Value[index]] );//"AUTO"
  }
}
/*******************************************************************************
更新缓冲区窗口位置 
*******************************************************************************/
void Update_Windows(void)
{
  u16  x0,x,y,i,j,Width;
  Depth =  DEPTH[PopMenu1_Value[WIN_Depth]];
  Width =(215*300)/Depth;
  if(menu.menu_index[menu.current]==6 && Cur_PopItem==2){  //Depth选项
    if(KeyIn == K_RIGHT || KeyIn == K_LEFT){  //选项有变动时改变预触发位置
      switch(PopMenu1_Value[WIN_T0]){
      case 0:
        PopMenu1_Value[WIN_Posi]= 0;
        x0=10+(PopMenu1_Value[WIN_Posi])*250/Depth;
        break;
      case 1:
        PopMenu1_Value[WIN_Posi]= Depth/2;
        x0=10+(Depth/2-125)*247/Depth;
        break;
      case 2:
        PopMenu1_Value[WIN_Posi]= Depth-300-5; //-5 超出采样深度
        x0=10+(Depth-300-5)*250/Depth;
        break;
      }
    }
  }
  if( PopMenu1_Value[WIN_T0]==0){
    if(menu.menu_index[menu.current]==6 && Cur_PopItem==4){
      if(KeyIn == K_RIGHT || KeyIn == K_LEFT)
      PopMenu1_Value[WIN_Posi]= 0;
      x0=10+(PopMenu1_Value[WIN_Posi])*250/Depth;
    }
    else x0=10+(PopMenu1_Value[WIN_Posi])*250/Depth;    
  }
  else if( PopMenu1_Value[WIN_T0]==1){
    if(menu.menu_index[menu.current]==6 && Cur_PopItem==4){
      if(KeyIn == K_RIGHT || KeyIn == K_LEFT)
        PopMenu1_Value[WIN_Posi]= Depth/2;
      x0=10+(Depth/2-125)*247/Depth;
    }
    else x0=10+(Depth/2-125+(PopMenu1_Value[WIN_Posi]-Depth/2))*247/Depth;  
  }
  else if( PopMenu1_Value[WIN_T0]==2){
    if(menu.menu_index[menu.current]==6 && Cur_PopItem==4){
      if(KeyIn == K_RIGHT || KeyIn == K_LEFT)
        PopMenu1_Value[WIN_Posi]= Depth-300-5;//-5 超出采样深度
      x0=10+(Depth-300-5)*250/Depth;
    }
    else x0=10+(PopMenu1_Value[WIN_Posi])*250/Depth;
  }
  if(Windows_Flag & UPD){  //刷新窗口
    Windows_Flag&=~UPD;
    x=10;
    y=15;
    for(i=0;i<250;i++){    //缓冲区条
      SetPosi(x++, y);
      SetPixel(DAR);
      for(j=0; j<2; j++)SetPixel(RED);
      SetPixel(DAR);     
    }
    x = x0;               //当前窗口在缓冲区位置
    y=15;   
    for(i=0;i<Width;i++){
      SetPosi(x++, y);
      for(j=0; j<4; j++)SetPixel(GRN);
    }
  }
  else if(Windows_Flag == 0){    //清除小窗口
    Clr_WavePosi(DAR);
  }
}
/*******************************************************************************
测量
*******************************************************************************/
void MeasureStr(u8 Source, u8 Item)
{
  u16 Dpth;
  s64 scale;
  s64 Temp;
  u64 FTmp;
  s32 Ak = (KgA[KindA+(StateA?1:0)]*4)/GK[GainA];
  s32 Bk = (KgB[KindB+(StateB?1:0)]*4)/GK[GainB]; // 8192~409
  
  if(((PopMenu1_Value[TRI_Sync] == NONE)||(PopMenu1_Value[TRI_Sync] == SCAN))
     &&(PopMenu1_Value[TIM_Base]>11)){
    Dpth = 300;
  }else Dpth = DEPTH[PopMenu1_Value[WIN_Depth]];
  
  switch (Item){
  case 0: // Freq  存为误差
    if(Source == CH1)FTmp=EdgeA*25*1000/2;
    else             FTmp=EdgeB*25*1000/2;
    if(PopMenu1_Value[TIM_Base]<9)FTmp= FTmp*1000000;
    else if(PopMenu1_Value[TIM_Base]<18)FTmp= FTmp*1000;
    FTmp=FTmp/Ts[PopMenu1_Value[TIM_Base]]/Dpth;
    if(PopMenu1_Value[TIM_Base]<2) Temp=2*FTmp; //交错工作模式 Depth/2 所以*2
    else Temp=FTmp;
    Value2Str(NumStr, Temp, (u8*)FSTR, 3, UNSIGN); break;
  case 1: // Duty
    if(Source == CH1)Temp = (HighA*100)/(HighA + LowA);
    else             Temp = (HighB*100)/(HighB + LowB);
    Value2Str(NumStr, Temp, (u8*)PCNT, 2, UNSIGN); break;
  case 2: // Vrms 
    if(Source == CH1){
      scale = (PopMenu1_Value[CH1_Probe]?
               VScale_10X[PopMenu1_Value[CH1_Vol]]:VScale[PopMenu1_Value[CH1_Vol]])*40;
      Temp  = ((((PrmsA)*Ak)>>12))*scale;
    }else{
      scale = (PopMenu1_Value[CH2_Probe]?
               VScale_10X[PopMenu1_Value[CH2_Vol]]:VScale[PopMenu1_Value[CH2_Vol]])*40;
      Temp  = ((((PrmsB)*Bk)>>12))*scale;
    }
    if(fabs(Temp)<(scale*25*10/100)){
      Temp=0;
      Value2Str(NumStr, Temp, (u8*)NSTR, 3, SIGN); } 
    else Value2Str(NumStr, Temp, (u8*)YSTR, 3, SIGN); 
    break;
  case 3: // Vavg
    if(Source == CH1){
      scale = (PopMenu1_Value[CH1_Probe]?
               VScale_10X[PopMenu1_Value[CH1_Vol]]:VScale[PopMenu1_Value[CH1_Vol]])*40;
      Temp  = ((((PavgA-2048)*(Ak))>>12)+100-PopMenu1_Value[CH1_Posi])*scale;
    }else{
      scale = (PopMenu1_Value[CH2_Probe]?
               VScale_10X[PopMenu1_Value[CH2_Vol]]:VScale[PopMenu1_Value[CH2_Vol]])*40;
      Temp  = ((((PavgB-2048)*(Bk))>>12)+100-PopMenu1_Value[CH2_Posi])*scale;
    }
    if(fabs(Temp)<(scale*25*10/100)){
      Temp=0;
      Value2Str(NumStr, Temp, (u8*)NSTR, 3, SIGN);} 
    else Value2Str(NumStr, Temp, (u8*)YSTR, 3, SIGN); 
    break;
  case 4: // Vp-p
    if(Source == CH1){
      scale = (PopMenu1_Value[CH1_Probe]?
               VScale_10X[PopMenu1_Value[CH1_Vol]]:VScale[PopMenu1_Value[CH1_Vol]])*40;
      Temp  = ((((PmaxA-2048)*(Ak))>>12)-(((PminA-2048)*(Ak))>>12))*scale;
    }else{
      scale = (PopMenu1_Value[CH2_Probe]?
               VScale_10X[PopMenu1_Value[CH2_Vol]]:VScale[PopMenu1_Value[CH2_Vol]])*40;
      Temp  = ((((PmaxB-2048)*(Bk))>>12)-(((PminB-2048)*(Bk))>>12))*scale;
    }
    if(fabs(Temp)<(scale*25*20/100)){
      Temp=0;
      Value2Str(NumStr, Temp, (u8*)NSTR, 3, SIGN);} 
    else Value2Str(NumStr, Temp, (u8*)YSTR, 3, SIGN); 
    break;
  case 5: // Vmax
    if(Source == CH1){
      scale = (PopMenu1_Value[CH1_Probe]?
               VScale_10X[PopMenu1_Value[CH1_Vol]]:VScale[PopMenu1_Value[CH1_Vol]])*40;
      Temp  = ((((PmaxA-2048)*(Ak))>>12)+100-PopMenu1_Value[CH1_Posi])*scale;
    }else{
      scale = (PopMenu1_Value[CH2_Probe]?
               VScale_10X[PopMenu1_Value[CH2_Vol]]:VScale[PopMenu1_Value[CH2_Vol]])*40;
      Temp  = ((((PmaxB-2048)*(Bk))>>12)+100-PopMenu1_Value[CH2_Posi])*scale;
    }
    if(fabs(Temp)<(scale*25*10/100)){
      Temp=0;
      Value2Str(NumStr, Temp, (u8*)NSTR, 3, SIGN);}
    else Value2Str(NumStr, Temp, (u8*)YSTR, 3, SIGN);
    break;
  case 6: // Vmin
    if(Source == CH1){
      scale = (PopMenu1_Value[CH1_Probe]?
               VScale_10X[PopMenu1_Value[CH1_Vol]]:VScale[PopMenu1_Value[CH1_Vol]])*40;
      Temp  = ((((PminA-2048)*(Ak))>>12)+100-PopMenu1_Value[CH1_Posi])*scale;
    }else{
      scale = (PopMenu1_Value[CH2_Probe]?
               VScale_10X[PopMenu1_Value[CH2_Vol]]:VScale[PopMenu1_Value[CH2_Vol]])*40;
      Temp  = ((((PminB-2048)*(Bk))>>12)+100-PopMenu1_Value[CH2_Posi])*scale;
    }
    if(fabs(Temp)<(scale*25*10/100)){
      Temp=0;
      Value2Str(NumStr, Temp, (u8*)NSTR, 3, SIGN);}
    else Value2Str(NumStr, Temp, (u8*)YSTR, 3, SIGN);
    break;
  case 7: // Vbty
    Temp =  Vbat*1000;
    Value2Str(NumStr, Temp, (u8*)YSTR, 3, UNSIGN); break;
  }  
}
/*******************************************************************************
电池电量更新
*******************************************************************************/
void Battery_update(void)
{
  u8 str[5];

  SetColor(DAR, BT_C[Battery]);
  memcpy(&str[0],&BT_S[Battery],4);   
  Draw_Circle_D(Foreground, Title_Posi_X[0]-3, TITLE_H+2, 11, 2, 4*6-2);
  DispStr6x8(Title_Posi_X[0], TITLE_H+2, INV+SYMB, str);
}
/*******************************************************************************
电池电量显示
*******************************************************************************/
void Battery_Show(void)
{
  SetColor(DAR, GRN);  
  Draw_Circle_D(Foreground, Title_Posi_X[0]-3, TITLE_H+2, 11, 2, 4*6-2);
  DispStr6x8(Title_Posi_X[0], TITLE_H+2, INV+SYMB, ";*@");           //电池半满
}
/*******************************************************************************
  显示保存参数提示弹出窗
*******************************************************************************/
void Param_Pop(u8* str)
{
  u16 Px,Py;
  PopColor(DAR, WHT);
  PopType &=~(MENU_POP|LIST_POP);
  PopType |= DAILOG_POP;           //LIST_POP;PWR_POP;
  OpenPop(80, 80, 40, 144,COVER);
  Px =8; Py = 23;
  Pop_STR6x8(Px,Py,PRN,str );      //"Save Parameter ?"
  Px=3*6; Py = 5;
  Pop_STR6x8(Px,Py,INV, " Yes "); 
  Px=11*6;Py = 5;
  Pop_STR(Px,Py,INV, " No "); 
}
/*******************************************************************************
 T1-T2的差值
*******************************************************************************/
void Print_dT_Info(u8 Wink)
{
  s32  Tmp,scale; 
  scale=TScale[PopMenu1_Value[TIM_Base]];
  Tmp =scale *(ParamTab[T2x1]-ParamTab[T1x1])/25;
  Value2Str(NumStr, Tmp, (u8*)XSTR, 3, SIGN);
  Sx=0;
  Sy=0;
  SetColor(DAR, ORN);  
  Draw_Circle_D(ORN, 255-3, TITLE_L, 11, 2, 11*6-2);
  DispStr6x8(255+6*8, TITLE_L, INV, "  ");
  DispStr6x8(255, TITLE_L, INV+SYMB, "]T:");
  DispStr6x8(270, TITLE_L, Wink, NumStr);
}
/*******************************************************************************
 V1-V2的差值
*******************************************************************************/
void Print_dV_Info(u8 Wink)
{
  s32  Tmp,scale;
  u8 index;
  index=(PopMenu1_Value[CUR_V]-1)?PopMenu1_Value[CH2_Vol]:PopMenu1_Value[CH1_Vol];
  scale=(PopMenu1_Value[CUR_V]-1)?(PopMenu1_Value[CH2_Probe]?VScale_10X[index]:VScale[index])
        :(PopMenu1_Value[CH1_Probe]?VScale_10X[index]:VScale[index]);
  Tmp =scale *((ParamTab[V1x2]-ParamTab[V2x2])/2)/25;
  Value2Str(NumStr, Tmp, (u8*)&YSTR[1], 3, SIGN);
  Sx=0,Sy=0;
  SetColor(Background, PopMenu1_Value[CUR_V]?CH_Col[PopMenu1_Value[CUR_V]-1]:CH_Col[0]); 
  Draw_Circle_D(PopMenu1_Value[CUR_V]?CH_Col[PopMenu1_Value[CUR_V]-1]:CH_Col[0], 1*6-3, TITLE_L, 11, 2, 11*6-2);
  DispStr6x8(6+6*8, TITLE_L, INV, "  ");
  DispStr6x8(1*6, TITLE_L, INV+SYMB, "]V:");
  DispStr6x8(4*6, TITLE_L, Wink, NumStr);
}
/*******************************************************************************
底栏显示测量值
*******************************************************************************/
void Print_dM_Info(u8 Wink , u8  Num, u16 Posi_x, u16 Posi_y)
{
  u8  type,source;
  u8* ptr;
  source = PopMenu2_Value[3*Num];
  type=PopMenu2_Value[3*Num+1];
  ptr=(u8*)&MeasStr[type];   
  MeasureStr(source,type);
  SetColor(Background,CH_Col[source]);
  Sx=Posi_x,Sy=Posi_y;   
  Draw_Circle_D(CH_Col[source], Posi_x-3, Posi_y, 11, 2, 14*6-2);
  DispStr6x8(Posi_x,     Posi_y, Wink, ptr);
  DispStr6x8(Posi_x+5*6, Posi_y, Wink, "        ");
  DispStr6x8(Posi_x+5*6, Posi_y, Wink, NumStr);
}
/*******************************************************************************
文件存储提示
*******************************************************************************/
void DispFileInfo(u8 Info)
{ 
  SetColor(DAR, RED);
  DispStr(252, TITLE_L-1, PRN, "             ");
  DispStr6x8(252, TITLE_L, PRN, ((u8*)&FnNote+14*Info));
  __Ctrl(DELAYmS, 1000);
  DispStr(252, TITLE_L-1, PRN, "             ");
}
/*******************************************************************************
画项目栏单边圆角框
*******************************************************************************/
void Draw_Circle_S(u16 Col, u16 Posi_x, u16 Posi_y, u16 High, u16 Width)
{
  u16 x,y,i,j;
  x=Posi_x;
  y=Posi_y;
  SetPosi(x, y);
  SetPixel(DAR);SetPixel(DAR);
  for(j=0;j<High-4;j++)SetPixel(Col);
  SetPixel(DAR);SetPixel(DAR);
  x=Posi_x+1;SetPosi(x, y);
  SetPixel(DAR);      
  for(j=0;j<High-2;j++)SetPixel(Col);
  SetPixel(DAR); 
  x=Posi_x+2; y=Posi_y; SetPosi(x, y);      
  for(i=0;i<Width;i++){
    SetPosi(x++, y);
    for(j=0; j<High; j++)SetPixel(Col);
  }
}
/*******************************************************************************
画项目栏双边圆角框
*******************************************************************************/
void Draw_Circle_D(u16 Col, u16 Posi_x, u16 Posi_y, u16 High, u16 Width, u16 Distance)
{
  u16 x, y, i, j;
  
  x = Posi_x;
  y = Posi_y + 2;            
  SetPosi(x, y);
  for(j=0;j<High-4;j++ )SetPixel(Col);
  x = Posi_x + Distance;
  SetPosi(x, y);
  for(j=0;j<High-4;j++ )SetPixel(Col);
  x = Posi_x + 1;
  y = Posi_y + 1;
  SetPosi(x, y);
  for(j=0;j<High-2;j++ )SetPixel(Col);
  x = Posi_x + Distance-1;
  SetPosi(x, y);
  for(j=0;j<High-2;j++ )SetPixel(Col);
  x = Posi_x + Distance-2;
  y = Posi_y;
  SetPosi(x, y);
  for(j=0;j<High;j++ )SetPixel(Col);
  x = Posi_x + 2;
  y = Posi_y;
  for(i=0;i<Width;i++){
    SetPosi(x++, y);
    for(j=0; j<High; j++)SetPixel(Col);
  }
}
/*******************************************************************************
画项矩形
*******************************************************************************/
void Draw_Rectangle(u16 Col, u16 Posi_x, u16 Posi_y, u16 High, u16 width)
{
  u16 x,y,i,j;
  x=Posi_x;
  y=Posi_y;
  SetPosi(x, y);     
  for(i=0;i<width;i++){
    SetPosi(x++, y);
    for(j=0; j<High; j++)SetPixel(Col);
  }
}
/*******************************************************************************
画空矩形
*******************************************************************************/
void Draw_RECT(u16 Col, u16 Posi_x, u16 Posi_y, u16 High, u16 width, u8 R )
{
  u16 x,y,i,j;
  x=Posi_x;
  y=Posi_y;
  SetPosi(x, y);     
  for(i=0;i<R;i++){
    SetPosi(x++, y);
    for(j=0; j<High; j++)SetPixel(Col);
  }
  SetPosi(Posi_x+R, y);
  for(i=0;i<width-2*R;i++){
    SetPosi(x, y);
    for(j=0; j<R; j++)SetPixel(Col);
    SetPosi(x++, y+High-R);
    for(j=0; j<R; j++)SetPixel(Col);
  }
  x=Posi_x+width-R;
  SetPosi(x, y); 
  for(i=0;i<R;i++){
    SetPosi(x++, y);
    for(j=0; j<High; j++)SetPixel(Col);
  }
}

/*******************************************************************************
擦除右栏
*******************************************************************************/
void Clear_Label_R(u16 Col)
{
    u16 x, y ,i, j;  
    x = Menu_X-2;
    y = 20; 
    for(i=0;i<59;i++){
      SetPosi(x++, y);
      for(j=0; j<204; j++)SetPixel(Col);
    }
}
/*******************************************************************************
擦除左栏
*******************************************************************************/
void Clear_Label_L(u16 Col)
{
    u16 x, y ,i, j;  
    x=0;y=19;  
    for(i=0;i<7;i++){
      SetPosi(x++, y);
      for(j=0; j<204; j++)SetPixel(Col);
    } 
  }
/*******************************************************************************
波形输出表
*******************************************************************************/
u32 WaveOut_Date(u16 type)
{
  switch (type){
  case 1:
    return (u32)SIN_DATA; break;
  case 2:
    return (u32)TRG_DATA; break;
  case 3:
    return (u32)SAW_DATA; break;
  default:     
    return 0; 
  }
}
/*******************************************************************************
Update_Proc_All  全部刷新
*******************************************************************************/
void Update_Proc_All(void)
{
  s16 i,k,l,index;

  //------------------------------第一页菜单------------------------------------//
  for(i=0;i<ABOUT;i++){
    index=PopMenu1_Base[i];
    switch (i){
    case CH1:  

      GainA  = PopMenu1_Value[CH1_Vol];   
        
        if(PopMenu1_Value[CH1_Vol]>3)
          KindA  = HV;
        else   
          KindA  = LV;                //Ch1选择低压或高压量程
        StateA =  (PopMenu1_Value[CH1_Vol]>6)?ACT: GND; 
        
        CouplA = (PopMenu1_Value[CH1_Coupl])? AC : DC;                      // AC);  //AC/DC耦合方式
        
        
      
      __Ctrl(AiRANGE, KindA+CouplA+StateA);             //Ch1状态刷新
      ParamTab[P1x2]=2*(PopMenu1_Value[CH1_Posi]);      //Ch1游标位置 
      ParamTab[W1F]=PopMenu1_Value[CH1_Enable]? L_HID : B_HID;//Ch1 show_hide 
      AiPosi(PopMenu1_Value[CH1_Posi]);                 //Ch1硬件对应位置
      break;
    case CH2:

      GainB  = PopMenu1_Value[CH2_Vol];   
        
        if(PopMenu1_Value[CH2_Vol]>3)
          KindB  = HV;
        else   
          KindB  = LV;                //Ch1选择低压或高压量程
        StateB =  (PopMenu1_Value[CH2_Vol]>6)?ACT: GND; 
        
        CouplB = (PopMenu1_Value[CH2_Coupl])? AC : DC;                      // AC);  //AC/DC耦合方式
        
      __Ctrl(BiRANGE, KindB+CouplB+StateB);
      ParamTab[P2x2]=2*(PopMenu1_Value[CH2_Posi]);
      ParamTab[W2F]=PopMenu1_Value[CH2_Enable]? L_HID : B_HID;
      BiPosi(PopMenu1_Value[CH2_Posi]);
      break;
    case CH3:
      ParamTab[W3F]=PopMenu1_Value[CH3_Enable]? L_HID : B_HID;
      ParamTab[P3x2]=2*(PopMenu1_Value[CH3_Posi]);   
      break;
    case TIMEBASE:
      Set_Base(PopMenu1_Value[TIM_Base]);               // 硬件设置扫描时基档位
      break;
    case TRIGGER:
      k=(PopMenu1_Value[TRI_Ch])?PopMenu1_Value[TRI_Ch2]:PopMenu1_Value[TRI_Ch1];
      l=(PopMenu1_Value[TRI_Ch])?ParamTab[P2x2]:ParamTab[P1x2]; 
      ParamTab[VTx2]=l+2*k;
      ParamTab[VTC]=CH_Col[PopMenu1_Value[TRI_Ch]];
      ParamTab[VTF]=(PopMenu1_Value[TRI_Enable])? W_HID : L_HID;
      break;
    case COURSOR:      
      ParamTab[T1x1]=PopMenu1_Value[CUR_T1]+1;
      ParamTab[T2x1]=PopMenu1_Value[CUR_T2]+1;
      if(PopMenu1_Value[CUR_T]==1)
        ParamTab[T1F]=SHOW;
      else ParamTab[T1F]=L_HID;
      ParamTab[T2F]=ParamTab[T1F];
      ParamTab[V1x2]=2*(PopMenu1_Value[CUR_V1]+1);   
      ParamTab[V2x2]=2*(PopMenu1_Value[CUR_V2]+1);    
      if(PopMenu1_Value[CUR_V]==0)                        // CH1,Ch2,OFF
        ParamTab[V1F]=L_HID;
      else ParamTab[V1F]=SHOW;
      ParamTab[V2F]=ParamTab[V1F];
      if(Cur_PopItem==4)Update[V1F]|=UPD;
      if(Cur_PopItem==5)Update[V2F]|=UPD;
      if(Cur_PopItem==1)Update[T1F]|=UPD;
      if(Cur_PopItem==2)Update[T2F]|=UPD;
      if(Cur_PopItem==6)Update[V1F]|=UPD ;                //更新V1-V2  
      break;
    case WINDOWS:
      index++;
      switch(PopMenu1_Value[WIN_T0]){
      case 0:
        ParamTab[T0x1]= 125 - PopMenu1_Value[WIN_Posi];
        break;
      case 1:
        ParamTab[T0x1]= Depth/2+125 - PopMenu1_Value[WIN_Posi];
        break;
      case 2:
        ParamTab[T0x1]= Depth-125 - PopMenu1_Value[WIN_Posi];
        break;  
      }
      ParamTab[T0F] =!PopMenu1_Value[WIN_Enable];
      if(Status != STOP)ADC_Start();                     //重新开始ADC扫描采样
      break;
    case WAVE:
      if(PopMenu3_Value[WAVE_Type]==0){         
        __Ctrl(OUT_PSC, FPSC[PopMenu3_Value[WAVE_Freq]]);  
        __Ctrl(OUT_ARR, FARR[PopMenu3_Value[WAVE_Freq]]);  
        __Ctrl(OUT_WTH, FARR[PopMenu3_Value[WAVE_Freq]]*PopMenu3_Value[WAVE_Duty]/10); 
        __Ctrl(OUT_MOD, PULSED); 
      }
      else{                                  
        if(PopMenu3_Value[WAVE_Freq] > MAX_FREQ){  
          PopMenu3_Value[WAVE_Freq] = MAX_FREQ ; 
        }
        __Ctrl(OUT_MOD, DISABLE);
        {
          __Ctrl(OUT_CNT, 180);       
          TIM_DA->PSC = Dac_Psc[PopMenu3_Value[WAVE_Freq]] - 1;
          __Ctrl(DAC_TIM, Dac_Tim[PopMenu3_Value[WAVE_Freq]]); 
          __Ctrl(OUT_BUF, __Info(22+PopMenu3_Value[WAVE_Type]));    
        } 
        __Ctrl(OUT_MOD, ANALOG);
        PopMenu3_Value[WAVE_Duty] = 5;   
      }
      break;   
    case SYSTEM:
      __Ctrl(BUZZVOL, PopMenu3_Value[SYS_Volume]*10);               
      __Ctrl(B_LIGHT, PopMenu3_Value[SYS_BKLight]*10);             
      PD_Cnt = PopMenu3_Value[SYS_Standy]*Unit;
      AutoPwr_Cnt = PopMenu3_Value[SYS_PowerOff]*Unit;
      break;  
    }
  }
}

/******************************** END OF FILE *********************************/
