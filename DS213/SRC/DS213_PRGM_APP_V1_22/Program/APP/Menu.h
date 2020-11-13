/********************* (C) COPYRIGHT 2010 e-Design Co.,Ltd. ********************
  DS213_APP Menu.h                                               Author : bure
*******************************************************************************/
#ifndef __MENU_H
#define __MENU_H

  #ifdef STM32F30X
    #include "STM32F30x.h"
  #elif STM32F10X_HD
    #include "STM32F10x.h"
  #endif

  typedef struct {
    uc8* STR;      // 该项显示的字符串指针
    uc16 X0;       // 该项显示的水平起始位置
    uc16 Y0;       // 该项显示的垂直起始位置
    uc16 Min;      // 该项变量的最小设置值
    uc16 Max;      // 该项变量的最大设置值
    u16  Val;      // 该项变量值
    u8   Src;      // 该项变量的来源编号
    u8   Flg;      // 控制标志
  } uimenu;

  typedef enum {
//  CHAR = 0x00,   // 该项内容正常显示
//  INVR = 0x01,   // 该项内容反衬显示
//  HIDE = 0x04,   // 不显示该项内容
    FLSH = 0x04,   // Flashing
    UPDT = 0x08,   // 刷新显示该项内容
    LOOP = 0x10,   // Value 值在调节到头时循环, 否则停止
    SLCT = 0x80,   // 根据 Value 选择显示字符串
    FNUM = 0x40,
    PCNT = 0x20,
    TCNT = 0x01,
  } menu_item_flg; // For Menu[Item].flg
  
  typedef enum {
    CPA, RNA, CPB, RNB, T3S, T4S, XNP, V_T, TRG, SYN,
    TIM, OUT, FRQ, YNP, V_1, V_2, T_0, T_1, T_2,
    CAL, TM2, TM1, VM2, VM1, RUN, BTY, ITEM_END,
  }umenu_itemu; // For Menu[Item]

  typedef enum {
    AUTO, NORM, SNGL, SLOW,
  } synchronous_mode; //  For Menu[SYN]

  typedef enum {
    MAX, MIN, VPP, RMS, AVG, D_V , VTH, B_V,
  }measure_v;   // For Measure V

  typedef enum {
    PTWH, PTWL, CYCL, FQRN, DUTY, D_T
  }measure_f_t; // For Measure F & T

  typedef enum {
    _1S,  _500mS, _200mS, _100mS,  _50mS,  _20mS, _10mS,  _5mS, 
    _2mS,   _1mS, _500uS, _200uS, _100uS,  _50uS, _20uS, _10uS, 
    _5uS,   _2uS,   _1uS, _500nS, _200nS, _100nS,
  }time_base; // For SetBase

  #define HOLD       0
  #define SMPL       1
  #define SINE       0
  #define TRIAN      1
  #define SAW        2
  #define DGTL       3

  extern uimenu Menu[], Pop[];
  extern uc8    DGTL_PSC[], ANLG_PSC[], XNP_MAX[], V_RANGE[][8];
  extern uc16   DGTL_ARR[], ANLG_ARR[], BASE_PSC[], BASE_ARR[], BASE_KP1[];
  extern u16    Diff[][10], Gain[][10], Slope[2], Yn[], Vt[], Item, Kind, Line;
  extern u8     PopUpdt;
  extern u16    MeasT[4][6];

  void UpdtMainMenu(void);
  u16  IndxInc(u16 Val, u16 Unit, u16 Indx);
  u16  IndxDec(u16 Val, u16 Unit, u16 Indx);
  void PopValInc(u16 Line, u16 Unit);
  void PopValDec(u16 Line, u16 Unit);

#endif

/********************************* END OF FILE ********************************/
