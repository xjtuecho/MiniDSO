/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      L3G4200D.c
Version :
Description:
Author :         Celery
Data:            2016/12/22
History:

*******************************************************************************/
#include "stdio.h"
#include "L3G4200D.h"
#include "Oled.h"
#include "app.h"
#include "Hardware.h"
#include "UI.h"

float gAg_data;

//unsigned char TX_DATA[4];
//unsigned char BUF[8];                         //接收数据缓存区
//char  test=0;
//float T_X,T_Y,T_Z;

extern u8 version_number;
#define FALSE  (0)
#define TRUE   (1)
//************************************
/*1.4版本模拟IIC端口输出输入定义*/
#define SCL_H         (GPIOA->BSRR = GPIO_Pin_15)
#define SCL_L         (GPIOA->BRR  = GPIO_Pin_15)

#define SDA_H         (GPIOB->BSRR = GPIO_Pin_3)
#define SDA_L         (GPIOB->BRR  = GPIO_Pin_3)

#define SCL_read      (GPIOA->IDR  & GPIO_Pin_15)
#define SDA_read      (GPIOB->IDR  & GPIO_Pin_3)

/*1.3版本模拟IIC端口输出输入定义*/
#define SCL_H_1         GPIOB->BSRR = GPIO_Pin_3
#define SCL_L_1         GPIOB->BRR  = GPIO_Pin_3

#define SDA_H_1         GPIOB->BSRR = GPIO_Pin_4
#define SDA_L_1         GPIOB->BRR  = GPIO_Pin_4

#define SCL_read_1      GPIOB->IDR  & GPIO_Pin_3
#define SDA_read_1      GPIOB->IDR  & GPIO_Pin_4
/*******************************************************************************
* Function Name  : I2C_delay
* Description    : Simulation IIC Timing series delay
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_delay(void)
{

    u8 i=30; //这里可以优化速度	，经测试最低到5还能写入
    while(i) {
        i--;
    }
}

void delay5ms(void)
{

    int i=5000;
    while(i) {
        i--;
    }
}
/*******************************************************************************
* Function Name  : I2C_Start
* Description    : Master Start Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : Wheather	 Start
****************************************************************************** */
u8 I2C_Start(void)
{   
    if(version_number)
    {
        SDA_H_1;
        SCL_H_1;
        I2C_delay();
        if(!SDA_read_1)return FALSE;	//SDA线为低电平则总线忙,退出
        SDA_L_1;
        I2C_delay();
        if(SDA_read_1) return FALSE;	//SDA线为高电平则总线出错,退出
        SDA_L_1;
        I2C_delay();
        return TRUE;
    }
    else
    {
        SDA_H;
        SCL_H;
        I2C_delay();
        if(!SDA_read)return FALSE;	//SDA线为低电平则总线忙,退出
        SDA_L;
        I2C_delay();
        if(SDA_read) return FALSE;	//SDA线为高电平则总线出错,退出
        SDA_L;
        I2C_delay();
        return TRUE;
    }
}
/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : Master Stop Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Stop(void)
{   
    if(version_number)
    {
        SCL_L_1;
        I2C_delay();
        SDA_L_1;
        I2C_delay();
        SCL_H_1;
        I2C_delay();
        SDA_H_1;
        I2C_delay();
    }
    else
    {
        SCL_L;
        I2C_delay();
        SDA_L;
        I2C_delay();
        SCL_H;
        I2C_delay();
        SDA_H;
        I2C_delay();
    }
}
/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : Master Send Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_Ack(void)
{
    if(version_number)
    {
        SCL_L_1;
        I2C_delay();
        SDA_L_1;
        I2C_delay();
        SCL_H_1;
        I2C_delay();
        SCL_L_1;
        I2C_delay();
    }
    else
    {
        SCL_L;
        I2C_delay();
        SDA_L;
        I2C_delay();
        SCL_H;
        I2C_delay();
        SCL_L;
        I2C_delay();
    }
}
/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : Master Send No Acknowledge Single
* Input          : None
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_NoAck(void)
{   
    if(version_number)
    {
        SCL_L_1;
        I2C_delay();
        SDA_H_1;
        I2C_delay();
        SCL_H_1;
        I2C_delay();
        SCL_L_1;
        I2C_delay();
    }
    else
    {
        SCL_L;
        I2C_delay();
        SDA_H;
        I2C_delay();
        SCL_H;
        I2C_delay();
        SCL_L;
        I2C_delay();
    }
}
/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : Master Reserive Slave Acknowledge Single
* Input          : None
* Output         : None
* Return         : Wheather	 Reserive Slave Acknowledge Single
****************************************************************************** */
u8 I2C_WaitAck(void) 	 //返回为:=1有ACK,=0无ACK
{    
    if(version_number)
    {
        SCL_L_1;
        I2C_delay();
        SDA_H_1;
        I2C_delay();
        SCL_H_1;
        I2C_delay();
        if(SDA_read_1) {
            SCL_L_1;
            I2C_delay();
            return FALSE;
        }
        SCL_L_1;
        I2C_delay();
        return TRUE;
    }
    else
    {
        SCL_L;
        I2C_delay();
        SDA_H;
        I2C_delay();
        SCL_H;
        I2C_delay();
        if(SDA_read) {
            SCL_L;
            I2C_delay();
            return FALSE;
        }
        SCL_L;
        I2C_delay();
        return TRUE;
    }
}
/*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : Master Send a Byte to Slave
* Input          : Will Send Date
* Output         : None
* Return         : None
****************************************************************************** */
void I2C_SendByte(u8 SendByte) //数据从高位到低位//
{
    u8 i=8;
    if(version_number)
    {
        while(i--) {
            SCL_L_1;
            I2C_delay();
            if(SendByte&0x80)
                SDA_H_1;
            else
                SDA_L_1;
            SendByte<<=1;
            I2C_delay();
            SCL_H_1;
            I2C_delay();
        }
        SCL_L_1;
    }
    else
    {
        while(i--) {
            SCL_L;
            I2C_delay();
            if(SendByte&0x80)
                SDA_H;
            else
                SDA_L;
            SendByte<<=1;
            I2C_delay();
            SCL_H;
            I2C_delay();
        }
        SCL_L;
    }
}
/*******************************************************************************
* Function Name  : I2C_RadeByte
* Description    : Master Reserive a Byte From Slave
* Input          : None
* Output         : None
* Return         : Date From Slave
****************************************************************************** */
u8 I2C_RadeByte(void)  //数据从高位到低位//
{
    u8 i=8;
    u8 ReceiveByte=0;
    
    if(version_number)
    {   
        SDA_H_1;
        while(i--) {
            ReceiveByte<<=1;
            SCL_L_1;
            I2C_delay();
            SCL_H_1;
            I2C_delay();
            if(SDA_read_1) {
                ReceiveByte|=0x01;
            }
        }
        SCL_L_1;
        return ReceiveByte;
    }
    else
    {
        SDA_H;
        while(i--) {
            ReceiveByte<<=1;
            SCL_L;
            I2C_delay();
            SCL_H;
            I2C_delay();
            if(SDA_read) {
                ReceiveByte|=0x01;
            }
        }
        SCL_L;
        return ReceiveByte;
    }
}
//ZRX
//单字节写入*******************************************
u8 Single_Write(u8 SlaveAddress,u8 REG_Address,u8 REG_data)		   
{
    if(!I2C_Start())  return FALSE;
    I2C_SendByte(SlaveAddress);   //发送设备地址+写信号//I2C_SendByte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//设置高起始地址+器件地址
    if(!I2C_WaitAck()) {
        I2C_Stop();
        return FALSE;
    }
    I2C_SendByte(REG_Address );   //设置低起始地址
    I2C_WaitAck();
    I2C_SendByte(REG_data);
    I2C_WaitAck();
    I2C_Stop();
    delay5ms();
    return TRUE;
}

//单字节读取*****************************************
u8 Single_Read(u8 SlaveAddress,u8 REG_Address)
{
    u8 REG_data;
    if(!I2C_Start())return FALSE;
    I2C_SendByte(SlaveAddress); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//设置高起始地址+器件地址
    if(!I2C_WaitAck()) {
        I2C_Stop();
        return FALSE;
    }
    I2C_SendByte((u8) REG_Address);   //设置低起始地址
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(SlaveAddress+1);
    I2C_WaitAck();

    REG_data= I2C_RadeByte();
    I2C_NoAck();
    I2C_Stop();
    //return TRUE;
    return REG_data;

}
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
