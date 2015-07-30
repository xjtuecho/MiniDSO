/******************** (C) COPYRIGHT 2009 e-Design Co.,Ltd. *********************
* File Name: Funtion.h   Author: bure   Hardware:  DS0201V1.1   Version: Ver 1.0
*******************************************************************************/
#ifndef __Funtion_h
#define __Funtion_h

//#define _FLASH_PROG

#define KEYCODE_VOID    0
#define KEYCODE_PLAY    1
#define KEYCODE_MANU    2
#define KEYCODE_UP      3
#define KEYCODE_DOWN    4
#define KEYCODE_LEFT    5
#define KEYCODE_RIGHT   6
#define KEYCODE_B       7

#define LCD_X1    0
#define LCD_X2    319
#define LCD_Y1    0
#define LCD_Y2    239
#define X_SIZE    300
#define Y_SIZE    200
#define MIN_X     1
#define MIN_Y     19
#define MAX_X     (X_SIZE + MIN_X)
#define MAX_Y     (Y_SIZE + MIN_Y)

#define SYNC_MODE         0
#define Y_SENSITIVITY     1
#define X_SENSITIVITY     2
#define Y_POSITION        3
#define MEASUR_KIND       4
#define POWER_INFOMATION  5
#define TRIG_SENSITIVITY  6
#define TRIG_SLOPE        7
#define INPUT_ATTENUATOR  8
#define SAVE_WAVE_CURVE   9
#define LOAD_WAVE_CURVE   10
#define OUTPUT_FREQUENCY  11
#define X_VERNIER_2       12
#define X_VERNIER_1       13
#define X_POSITION        14
#define TP                14
#define RUNNING_STATUS    15
#define DELTA_T           16
#define CURRENT_ITEM      16   //Item_Index[CURRENT_ITEM]
#define Y_VERNIER_2       17
#define Y_VERNIER_1       18
#define TRIG_LEVEL        19
#define VERNIERS          20
#define WINDOW_AREA       21
#define SAVE_WAVE_IMAGE   22

#define RUN                     0
#define HOLD                    1
#define RISING                  0

#define REF                1
#define V0                 3
#define VS                 6
#define T2                12
#define T1                13
#define V2                17
#define V1                18
#define VT                19

extern char                 Test[2]; 
extern unsigned short       Scan_Buffer[0x1000]; 
extern unsigned char        View_Buffer[300], Ref_Buffer[304];
extern unsigned char        Signal_Buffer[300], Key_Buffer;
extern unsigned const char  Item_V[20][11], Item_T[22][12];
extern unsigned const int   V_Scale[20],  T_Scale[22], Fout_ARR[16];
extern unsigned const char  V_Unit[4][3], T_Unit[4][3];
extern unsigned const short Scan_PSC[22], Scan_ARR[22];
extern unsigned short       Y_POSm[20], Km[20];
extern          short       Y_POSn[20];
extern unsigned short Delay_Counter, X_Counter;  
extern unsigned char  Counter_20mS, Key_Repeat_Counter, Key_Wait_Counter, Key_Repeat_X10; 
extern unsigned char  Cursor_Counter,Erase_Note, Stop, Type, Sync, Frame;
extern unsigned int   vb_Sum, Battery;
extern unsigned short temp, Lcd_X, Lcd_Y, Tp;
extern unsigned char  Update[23];
extern unsigned char  F_Buff[512];
extern unsigned char  FileNum[4];
extern unsigned short Item_Index[23];
extern unsigned char  Hide_Index[23];
extern unsigned short t0;    
extern           int  Frequency, Cycle, Duty, Vram, Vavg, Vpp, DCV;
void Delayms(unsigned short mS);
void Scan_Wave(void);
void Display_Item(void);
void Update_Item(void);
void Sync_Trig(void);
void Signal_Process(void);
void Set_Grey_Buf(unsigned char *s, unsigned char *r, unsigned char *p);
void Erase_Draw(void);
void Measure_Wave(void);
void Battery_Detect(void);
void Test_Display(int a, int b); 

#endif
/******************************** END OF FILE *********************************/
