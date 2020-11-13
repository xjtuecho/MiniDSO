/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      L3G4200D.c
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:    
*******************************************************************************/
#include "stdio.h"
#include "L3G4200D.h"
#include "Oled.h"
#include "app.h"
#include "Hardware.h"
#include "UI.h"
#include "iic.h"

float gAg_data;
extern u8 version_number;
/*******************************************************************************
������: Init_L3G4200D
��������:��ʼ��L3G4200D
�������:NULL
���ز���:NULL
*******************************************************************************/
void Init_L3G4200D(void)
{   
    if(version_number)
    {
        GPIO_InitTypeDef    GPIO_InitStructure;

        /*����SMBUS_SCK��SMBUS_SDAΪ���缫��©���*/
        GPIO_InitStructure.GPIO_Pin = SCL | SDA;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
    
    Single_Write(L3G4200_Addr, CTRL_REG1, 0x0F);
    Single_Write(L3G4200_Addr, CTRL_REG2, 0x00);
    Single_Write(L3G4200_Addr, CTRL_REG3, 0x00);
    Single_Write(L3G4200_Addr, CTRL_REG4, 0x00);//0x30);	//+-2000dps
    Single_Write(L3G4200_Addr, CTRL_REG5, 0x02);//|0x10);//0x00);
}
/*******************************************************************************
������: READ_L3G4200D
��������:��ȡL3G4200D����
�������:NULL
���ز���:NULL
*******************************************************************************/
void READ_L3G4200D(void)
{
    short dat;
    unsigned char buf[8];//�������ݻ�����
    
    if(Single_Read(L3G4200_Addr, STATUS_REG) && 0x01 == 1)
    {
        buf[2] = Single_Read(L3G4200_Addr, OUT_Y_L);
        buf[3] = Single_Read(L3G4200_Addr, OUT_Y_H);
        dat = (buf[3] << 8) | buf[2];
        gAg_data = (float)dat;
    }
    else        gAg_data = 0;
}
/*******************************************************************************
������: Get_Angle
��������:��ȡת�������ĽǶ�
�������:NULL
���ز���:�Ƕ�
*******************************************************************************/
float Get_Angle(void)
{
    static unsigned long cur_timer, last_timer, dt;
    float angle;

    cur_timer = Timer_StartValue(); //��ȡ��ǰʱ��
    dt = Elapse_Value(last_timer, cur_timer); //������ʱ��
    last_timer = cur_timer;
    
    READ_L3G4200D();
    angle = GAIN * (gAg_data) * (float)dt / 100.0;//����ת���ĽǶ�

    return angle;
}
/******************************** END OF FILE *********************************/
