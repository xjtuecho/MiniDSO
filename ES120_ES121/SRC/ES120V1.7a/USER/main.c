/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      Main.c
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
2017/06/20       添加硬件版本判断;
2017/11/11       添加AD看门狗;
*******************************************************************************/
#include "stdio.h"
#include "delay.h"
#include "ctrl.h"
#include "sys.h"
#include "oled.h"
#include "Disk.h"
#include "app.h"
#include "Hardware.h"
#include "Adc.h"
#include "L3G4200D.h"
#include "UI.h"

void Setup(void);
extern u8 version_number;
extern const u8 Start_picture[];
extern u32 current_limt;
/*******************************************************************************
函数名: main
函数作用:主函数
输入参数:NULL
返回参数:NULL
*******************************************************************************/
int main(void)
{
    Setup();/*Setup*/
    while(1) {
        Clear_Watchdog();//重置看门狗     
        Mode_Switching();//状态转换
    }
}
/*******************************************************************************
函数名: Setup
函数作用:设置
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Setup(void)
{     
    RCC_Config();
    Delay_Init();
    GPIO_Config();
    NVIC_Configuration();
    Adc_Init();
    Delay_Ms(10);	
    if(Get_Adc(VIN) > 500) {//有USB
        USB_Port(DISABLE);
        Delay_Ms(200);
        USB_Port(ENABLE);
        USB_Init();
    }

    Disk_BuffInit();//U盘内容读取
    Config_Analysis();//启动虚拟U盘
    Init_L3G4200D();//初始化L3G4200D
    Init_Timer2();
    PWM_Init(2400,0,0);//20k
    Init_Oled();//初始化OLED
    Clear_Screen();
    Read_MtCnt();//读取电机转动时间
   
    Init_Gtime();//初始化Gtime[]
    Set_gKey(NO_KEY);
    Start_Watchdog(3000);
   
    if(Hardware_version())//板子硬件判断      
    {
        version_number = 0;//1.4版本
    }
    else
    {
        version_number = 1;//1.3版本
        Version_Modify();   
    }
    
    current_limt = (MAX_ROTATE_I - ((info_def.torque_level - 1))*GEARS_UNIT_I);//根据挡位计算电流
    Set_CurLimit(current_limt,0);//adc看门狗界限
}

/*********************************END OF FILE*********************************/