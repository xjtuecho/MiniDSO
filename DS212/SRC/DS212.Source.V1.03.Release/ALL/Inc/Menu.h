/******************** (C) COPYRIGHT 2017 e-Design Co.,Ltd. *********************
 File Name : Menu.h
 Version   : DS212                                                Author : bure
*******************************************************************************/
#include "STM32F30x.h"

#define   MENU_MAX  2

#define   add       1    
#define   dec       2 

#define   RUN       0
#define   STOP      2
#define   UPD       1  

typedef  uint64_t     u64;
typedef  int64_t      s64;
typedef const uint64_t     uc64;

typedef struct 
{
  u8  menu_flag;         //win
  u8  mflag;             //menu
  u8  iflag;             //item
  u8  current;           //当前菜单页
  u8  menu_index[12];    //当前菜单项目
} MAINMENU;

extern MAINMENU menu;

extern u8    Srceen_Flag;
extern u8    TFlg[], LFlg[], TVal[], SVal[], Vn[], LVal[];
extern uc8   TATTR[], T_VH[], S_VH[], LATTR[];
extern uc16  V_BT[], L_VH[];
extern uc8   Vol_Str[][8];
extern uc8   Vol_10X[][8];
extern uc8   TimeBase_Str[][8];
extern uc8   Scale[][6];
extern u8    Battery, Unit;
extern u8    Status,Windows_Pop;
extern u8    Label_Flag,Update[];
extern u8    Cur_PopItem,Cur_Limit;
extern uc8   Menu_Limit[];
extern uc8   Item_Base[];
extern uc8   item_limit[];
extern s16   PopMenu1_Value[];
extern sc16  PopMenu1_CValue[];
extern sc16  Popmenu1_Limit1[], PopMenu1_Limit2[];
extern u8    PopMenu2_Value[],PopMenu3_Value[];
extern uc8   PopMenu2_CValue[],PopMenu3_CValue[];
extern uc8   PopMenu1_Base[], PopMenu3_Base[]; 
extern u8    Windows_Flag;  
extern u8    NumStr[];
extern sc16* pFNP[]; 
extern uc16  DEPTH[];
extern uc32  VScale[];
extern uc64  VScale_10X[];
extern uc32  TScale[];
extern uc8   TScale_Str[][8];
extern uc8   VScale_Str[][6];
extern uc8   VScale_Str_10X[][6];
extern u8    PopMenu1_Len ;
extern u8    PopMenu2_Len ;
extern u8    PopMenu3_Len ;
extern u16   CHA_Col, CHB_Col;

enum            {  CH_A,    CH_B,    CH_C};
//==============+========+========+========+========+========+========+========+
//   SR_C选项   | -CH_A  | -CH_B  | CH_A+B | CH_A-B |  MEM_1 |  MEM_2 |  MEM_2  |
//==============+========+========+========+========+========+========+========+
enum            {  aNEG,    bNEG,     ADD,     SUB,     MR1,     MR2,    MR3, };
//==============+========+========+========+========+========+========+========+
enum            { Oscillo, Measure,  Option};

enum{               //第一页子菜单选项
  CH1_Vol,  CH1_Posi, CH1_Coupl, CH1_Enable, CH1_Probe,
  CH2_Vol,  CH2_Posi, CH2_Coupl, CH2_Enable, CH2_Probe, 
  CH3_Type, CH3_Posi, CH3_Enable,
  TIM_Base, TIM_Fit,
  TRI_Sync, TRI_Mode, TRI_Ch,    TRI_Ch1,    TRI_Enable,  TRI_Fit,  TRI_Ch2,
  CUR_T1,   CUR_T2,   CUR_T,     CUR_V1,     CUR_V2,      CUR_V,
  WIN_Posi, WIN_Depth,           WIN_Enable, WIN_T0         
};

enum{             //第三页之菜单选项
  SAVE_Pam,   SAVE_Bmp,    SAVE_Dat,   SAVE_Buf,     SAVE_Csv,    LOAD_Dat,   LOAD_Buf,   SAVE_Svg,
  WAVE_Type,  WAVE_Freq,   WAVE_Duty, 
  SYS_Volume, SYS_BKLight, SYS_Standy, SYS_PowerOff, SYS_MenuCyc, SYS_ItemCyc,SYS_PosiCyc, 
};

enum{
  SAVE_PAM = 1,   SAVE_BMP,    SAVE_DAT,   SAVE_BUF,  SAVE_CSV,  LOAD_DAT,  LOAD_BUT, SAVE_SVG
};

enum{             
  CAL_ZERO = 1,   RES_DATA,   
};

enum{ 
  CH1,   CH2,   CH3,    TIMEBASE,    TRIGGER, COURSOR, WINDOWS, 
  MEA_1, MEA_2, MEA_3,  MEA_4,       MEA_5,   MEA_BAT,  
  FILE,  WAVE,  SYSTEM, CALIBRATION, PRO,     ABOUT,           
};

void Update_Info(void);
void Close_Pop(void);
void PwrOff_Pop(void);
void Demo_Pop(void);
void Param_Pop(u8* str);
void Menu_Pop(void);
void Exit_Pop(void);
void SetColor(u16 BgColor, u16 FgColor);
void TouchPad(void);
void Print_dV_Info(u8 Wink);
void Print_dT_Info(u8 Wink);
void Print_dM_Info(u8 Wink , u8 Num, u16 Posi_x, u16 Posi_y);
void Show_Menu(void);
void Show_Item(u8 item);
void Item_Proc(u8 mode);
void Show_PopMenu(u8 current);
void PMenu_Pop(u8 item,u8 current,u8* str1, u8 str_num, u8* str2);
void FMenu_Pop(u8 item,u8 current,u8* str1);
void PMenu_Proc(u8 mode,u8 Cur_Item,u8 step);
void Update_Label(void);
void Update_Status(void);
void Update_Windows(void);
void Show_Measure(void);
void Menu_Hide(void);
void DispFileInfo(u8 Info);
void Draw_Circle_S(u16 Col, u16 Posi_x, u16 Posi_y, u16 High, u16 Width);
void Draw_Circle_D(u16 Col, u16 Posi_x, u16 Posi_y, u16 High, u16 Width, u16 Distance);
void Draw_Rectangle(u16 Col, u16 Posi_x, u16 Posi_y, u16 High, u16 Width);
void Draw_RECT(u16 Col, u16 Posi_x, u16 Posi_y, u16 High, u16 width, u8 R );
void Clear_Label_R(u16 Col);
void Clear_Label_L(u16 Col);
void Update_Proc(void);
void Show_Title(void);
void Update_Proc_All(void);
void Battery_update(void);
void Battery_Show(void);
void Vertical_Slide(u8 mode);
u32  WaveOut_Date(u16 type);
void Show_Item_One(u8 k);
/******************************** END OF FILE *********************************/
