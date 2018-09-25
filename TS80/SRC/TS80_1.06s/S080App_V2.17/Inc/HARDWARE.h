/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
File Name :      HARDWARE.H
Version :        TS200 APP Test 1.0
Description:
Author :         Ning
Data:            2017/11/06
History:
*******************************************************************************/
#ifndef _HARDWARE_H
#define _HARDWARE_H

#include "STM32F10x.h"

#define CAL_AD          250
#define G_VOL           1000
#define SI_COE          8       //56
#define SI_THRESHOLD    60
#define BENCKMARK_AD    0     //基准AD值 
#define SI_DATA 25
#define KEYA_READ     ((~GPIOB->IDR) & KEY1_PIN) //按下为1
#define KEYB_READ     ((~GPIOB->IDR) & KEY2_PIN) //按下为1

typedef enum VOL_CLASS
{
    H_ALARM = 0,
    VOL_24,
    VOL_19,
    VOL_12,
    VOL_5,
    L_ALARM,
} VOL_CLASS;

extern s32 gZerop_ad;
extern u8 gCalib_flag;
extern u32 gTurn_offv;

u32 Get_gKey(void);
//void Set_gKey(u32 key);
void Set_LongKeyFlag(u32 flag);
void Zero_Calibration(void);
int Read_Vb(void);
u32 Get_SlAvg(u32 avg_data);
int Get_TempSlAvg(int avg_data);
u32 Get_AvgAd(void);
int Get_SensorTmp(u8 pos);
s32 Get_Temp(void);
u32 Clear_Watchdog(void);
u32 Start_Watchdog(u32 ms);
u8 Get_AlarmType(void);
u32 Get_CalFlag(void);
void Key_Read( void );
void Clear_LongKey(void);
#endif
/******************************** END OF FILE *********************************/
