/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
File Name :      main.c
Version :        TS080 APP 1.02
Description:
Author :         Ning
Data:            2017/11/06
History:
2017/10/09       不同状态下选择不同电压；
2017/10/16       初始化待机时间
2017/11/24       加速计硬件判断;
*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include "APP_Version.h"
#include "Disk.h"
#include "Bios.h"
#include "USB_lib.h"
#include "I2C.h"
#include "Flash.h"
#include "MMA8652FC.h"
#include "UI.h"
#include "OLed.h"
#include "CTRL.h"
#include "HARDWARE.h"
#include "string.h"
extern u32 slide_data;
extern u8  MarkAd_pos;

u32 Resistance = 0;//内阻
/*******************************************************************************
函数名: main
函数作用:主循环
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void main(void)
{
    RCC_Config();
    NVIC_Config(0x4000);
    Init_Timer3();
    GPIO_Config();
    USB_Port(DISABLE);
    Delay_Ms(200);
    USB_Port(ENABLE);
    USB_Init();
    I2C_Configuration();
    Ad_Init();
    Is_ST_LIS2DH12();//加速计判断
    StartUp_Accelerated();
    System_Init();
    Disk_BuffInit();
    Config_Read();//启动虚拟U盘
    Init_Gtime();
    APP_Init();//开机状态初始化
    Init_Oled();
    Clear_Screen();

    Pid_Init();
    Start_Watchdog(3000);
    Set_CurLimit(MAX_VOL, MIN_VOL); //配置电压看门狗
    GPIO_Vol_Init(1);
    Vol_Set(0);
    Init_Timer2();
    GPIO_Vol_Init(0);
    Set_LongKeyFlag(1);
    Get_gKey();  //dummy read
    while (1)
    {
        Clear_MMA_INT();
        Clear_Watchdog();
        Status_Tran();                              //根据当前状态，配合按键与控制时间转换
    }
}
/******************************** END OF FILE *********************************/
