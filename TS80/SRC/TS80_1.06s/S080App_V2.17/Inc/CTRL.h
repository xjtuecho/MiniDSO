/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
File Name :      CTRL.h
Version :        TS200 APP Test 1.0
Description:
Author :         Ning
Data:            2017/11/06
History:
*******************************************************************************/
#ifndef _CTRL_H
#define _CTRL_H

#include "STM32F10x.h"
#include "Bios.h"

#define TEMPSHOW_TIMER            gTime[0]/*TEMPSHOW_TIMER*/
#define HEATING_TIMER             gTime[1]/*HEATING_TIMER*/
#define DISP_TIMER                gTime[2]/*ENTER_WAIT_TIMER*/
#define EFFECTIVE_KEY_TIMER       gTime[3]/*EFFECTIVE_KEY_TIMER*/
#define LEAVE_WAIT_TIMER          gTime[4]/*LEAVE_WAIT_TIMER*/
#define G6_TIMER                  gTime[5]/*SWITCH_SHOW_TIMER*/
#define UI_TIMER                  gTime[6]/*UI_TIMER 图像界面左移时间控制*/
#define KD_TIMER                  gTime[7]/*按键延时*/

#define V_TIMER                   30

#define HEATINGCYCLE              10//30改成10 2018.3.16
//------------------------------ 按键定义------------------------------------//
#define   KEY_ST(KEY_PIN)      GPIO_ReadInputDataBit(GPIOA, KEY_PIN)
#define   NO_KEY    0x0
#define   KEY_V1    0x0100
#define   KEY_V2    0x0040
#define   KEY_CN    0X8000
#define   KEY_V3    0X0140

#define   QC3_0	(0)
#define	QC2_0	(1)
#define   HYSTERETIC_VAL		(18)
typedef enum WORK_STATUS
{
    START = 0,
    IDLE = 1,           //待机
    THERMOMETER,
    TEMP_CTR,           //温控模式
    WAIT,               //休眠
    TEMP_SET,           //温度设置
    CONFIG ,
    MODE_CNG,
    ALARM,              //警告
    VOLT,
} WORK_STATUS;

typedef enum WARNING_STATUS
{
    NORMAL_TEMP = 1,
    HIGH_TEMP,
    SEN_ERR,
    HIGH_VOLTAGE,
    LOW_VOLTAGE,
} WARNING_STATUS;

typedef enum SET_OPT
{
    WKT = 0,    //t_standby
    SDT,        //t_work
    WTT,        //wait_time
    IDT,        //idle_time
    STP,        //t_step
    TOV,        //turn of V
    DGC,        //dgc
    HD,         //handers
    PT,         //portable   
    WDJ,        //溫度計
    EXW,        //EX WORKS
    END_OF_SETUP_ITEM
} SET_OPT;

typedef enum _RUN_STATE
{
    HEATING = 0,
    COOLING,
    KEEPUP
} RUN_STATE;

typedef struct
{
    u8 ver[16];             //版本号
    int t_standby;          // 200°C=1800  2520,待机温度
    int t_work;             // 350°C=3362, 工作温度
    u32 t_step;             //设置步长
    u32 wait_time;          //3*60*100   3 mintute
    u32 rated_power;        //额定功率
    u32 handers;            //0 right 1 left
    u32 temp_flag;          //(0:Celsius ℃, 1:Fahrenheit ℉)
    u32 turn_offv;          //保护电压
    u32 zero_ad;            //零点AD
    u32 portable_flag;      //移动电源标志 0 OFF / 1 ON
} DEVICE_INFO_SYS;

extern DEVICE_INFO_SYS device_info;

double newSqrt(double n);
u8 Get_CtrlStatus(void);
void Set_CtrlStatus(u8 status);
void Frequency_Set (u8 pos);
void Vol_Calculation(u8 pos);
void Vol_Set(u8 pos);
u32 Vol_Get(void);
u32 Get_Resistance(void);
u16 Get_HtFlag(void);
void System_Init(void);
void Pid_Init(void);
u16 Pid_Realize(s16 temp);
u32 Heating_Time(s16 temp, s16 wk_temp);
void Status_Tran(void);
unsigned int Timer_StartValue(void);
unsigned int Timer_ElapseValue(const unsigned int start);
unsigned int Timer_ElapseSecond(const unsigned int start);
#endif
/******************************** END OF FILE *********************************/
