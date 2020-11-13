/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      Main.c
Version :
Description:
Author :         Celery
Data:            2016/12/22
History:
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
/*******************************************************************************
������: main
��������:������
�������:NULL
���ز���:NULL
*******************************************************************************/
int main(void)
{
    Setup();/*Setup*/
    while(1) {
        Clear_Watchdog();//���ÿ��Ź�     
        Mode_Switching();//״̬ת��
    }
}
void Setup(void)
{     
    RCC_Config();
    Delay_Init();
    GPIO_Config();
    NVIC_Configuration();
    Adc_Init();

    if(Get_Adc(VIN) > 500) {//��USB
        USB_Port(DISABLE);
        Delay_Ms(200);
        USB_Port(ENABLE);
        USB_Init();
    }

    Disk_BuffInit();            //U�����ݶ�ȡ
    Config_Analysis();          //��������U��
    Init_L3G4200D();            //��ʼ��L3G4200D
    Init_Timer2();
    PWM_Init(2400,0,0);//20k
    Init_Oled();			//��ʼ��OLED
    Clear_Screen();
    Read_MtCnt();//��ȡ���ת��ʱ��
   
    Init_Gtime();   //��ʼ��Gtime[]
    Set_gKey(NO_KEY);
    Start_Watchdog(3000);
   
    if(Hardware_version())      
    {
        version_number = 0;//1.4�汾
    }
    else
    {
        version_number = 1;//1.3�汾
        Version_Modify();
        
    }
}
/*********************************END OF FILE*********************************/
