/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      Main.c
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
2017/06/20       ���Ӳ���汾�ж�;
2017/11/11       ���AD���Ź�;
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
������: main
��������:������
�������:NULL
���ز���:NULL
*******************************************************************************/
int main(void)
{
    Setup();/*Setup*/
    while(1) 
    {
        Clear_Watchdog();//���ÿ��Ź�     
        Mode_Switching();//״̬ת��
    }
}
/*******************************************************************************
������: Setup
��������:����
�������:NULL
���ز���:NULL
*******************************************************************************/
void Setup(void)
{     
    RCC_Config();//ʱ�ӳ�ʼ��
    Delay_Init();//��ʼ���ӳٺ���
    GPIO_Config();
    NVIC_Configuration();//NVIC��ʼ��
    Adc_Init();
    Delay_Ms(10);
    if(Get_Adc(VIN) > 500)
    {//��USB
        USB_Port(DISABLE);
        Delay_Ms(200);
        USB_Port(ENABLE);
        USB_Init();
    }

    Disk_BuffInit();//U�����ݶ�ȡ
    Config_Analysis();//��������U��
    Init_L3G4200D();//��ʼ��L3G4200D
    Init_Timer2();//��ʼ����ʱ��2
    PWM_Init(2400,0,0);//20k
    Init_Oled();//��ʼ��OLED
    Clear_Screen();
    Read_MtCnt();//��ȡ���ת��ʱ��
   
    Init_Gtime();//��ʼ��Gtime[]
    Set_gKey(NO_KEY);
    Start_Watchdog(3000);
   
    if(Hardware_version())//����Ӳ���ж�      
    {
        version_number = 0;//1.4�汾
    }
    else
    {
        version_number = 1;//1.3�汾
        Version_Modify();
    }
    
    if(info_def.torque_level != 1 && info_def.torque_level != 0)
    {
        current_limt = (MAX_ROTATE_I - ((info_def.torque_level - 2)) * GEARS_UNIT_I);//���ݵ�λ�������
    }
    else        current_limt = 1800;
    Set_CurLimit(current_limt, 0);//adc���Ź�����
}
/*********************************END OF FILE*********************************/
