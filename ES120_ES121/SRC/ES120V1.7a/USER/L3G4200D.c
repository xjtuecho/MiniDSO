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

//unsigned char TX_DATA[4];
//unsigned char BUF[8];                         //接收数据缓存区
//char  test=0;
//float T_X,T_Y,T_Z;

/*******************************************************************************
函数名: Init_L3G4200D
函数作用:初始化L3G4200D
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Init_L3G4200D(void)
{   
    if(version_number)
    {
        GPIO_InitTypeDef    GPIO_InitStructure;

        /*配置SMBUS_SCK、SMBUS_SDA为集电极开漏输出*/
        GPIO_InitStructure.GPIO_Pin = SCL | SDA;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
    
    Single_Write(L3G4200_Addr,CTRL_REG1, 0x0F);
    Single_Write(L3G4200_Addr,CTRL_REG2, 0x00);
    Single_Write(L3G4200_Addr,CTRL_REG3, 0x00);
    Single_Write(L3G4200_Addr,CTRL_REG4, 0x00);//0x30);	//+-2000dps
    Single_Write(L3G4200_Addr,CTRL_REG5, 0x02);//|0x10);//0x00);
}
/*******************************************************************************
函数名: READ_L3G4200D
函数作用:读取L3G4200D数据
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void READ_L3G4200D(void)
{
    short dat;
    unsigned char buf[8];//接收数据缓存区
    
    if(Single_Read(L3G4200_Addr,STATUS_REG)&&0x01 == 1){
        buf[2]=Single_Read(L3G4200_Addr,OUT_Y_L);
        buf[3]=Single_Read(L3G4200_Addr,OUT_Y_H);
        dat=	(buf[3]<<8)|buf[2];
        gAg_data = (float)dat;
    }else{
        gAg_data = 0;
    }
}

/*******************************************************************************
函数名: Get_Angle
函数作用:获取转动经过的角度
输入参数:NULL
返回参数:角度
*******************************************************************************/
float Get_Angle(void)
{
    static unsigned long cur_timer,last_timer,dt;
    float angle;

    cur_timer = Timer_StartValue(); //当前时间
    dt = Elapse_Value(last_timer,cur_timer); //经过的时间
    last_timer = cur_timer;
    
    READ_L3G4200D();
    
    angle = GAIN * (gAg_data) * (float)dt / 100.0;//计算转过的角度

    return angle;

}
/******************************** END OF FILE *********************************/
