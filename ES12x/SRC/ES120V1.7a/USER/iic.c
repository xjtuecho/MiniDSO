/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      iic.c
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
2017/06/20       新增不同硬件版本下的iic定义配置;
2017/10/21       优化汇总iic配置;    
*******************************************************************************/
#include "app.h"
#include "iic.h"

extern u8 version_number;
/*******************************************************************************
* Function Name  : I2C_delay
* Description    : Simulation IIC Timing series delay
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_delay(void)
{
    u8 i=20; //这里可以优化速度	，经测试最低到5还能写入
    while(i) {
        i--;
    }
}

/*******************************************************************************
* Function Name  : delay5ms
* Description    : Simulation IIC 5 series delay
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void delay5ms(void)
{
    int i=50;
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
*******************************************************************************/
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
* Function Name  : Write_IIC_Byte
* Description    : None
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Write_IIC_Byte(unsigned char IIC_Byte)
{
    unsigned char i;
    
    if(version_number)//1.3版本
    {
        for(i=0; i<8; i++) {
            OLED_SCLK_Clr_1();
            if(IIC_Byte & 0x80) {
                OLED_SDIN_Set_1();
            } else OLED_SDIN_Clr_1();
            IIC_Byte<<=1;
            OLED_SCLK_Set_1();
        }
        OLED_SCLK_Clr_1();
        OLED_SCLK_Set_1() ;
    }
    else//1.4版本
    {
        for(i=0; i<8; i++) {
            OLED_SCLK_Clr();//接口时钟信号
            if(IIC_Byte & 0x80) {
                OLED_SDIN_Set();//接口数据信号
            } else OLED_SDIN_Clr();
            IIC_Byte<<=1;
            OLED_SCLK_Set();
        }
        OLED_SCLK_Clr();
        OLED_SCLK_Set() ;
    }
}
/*******************************************************************************
* Function Name  : Write_IIC_Command
* Description    : None
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
    I2C_Start();
    Write_IIC_Byte(0x78);       //Slave address,SA0=0
    Write_IIC_Byte(0x00);       //write command
    Write_IIC_Byte(IIC_Command);
    I2C_Stop();
}
/*******************************************************************************
* Function Name  : Write_IIC_Data
* Description    : None
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
    I2C_Start();
    Write_IIC_Byte(0x78);       //D/C#=0; R/W#=0
    Write_IIC_Byte(0x40);       //write data
    Write_IIC_Byte(IIC_Data);
    I2C_Stop();
}
/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : Master Stop Simulation IIC Communication
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
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
*******************************************************************************/
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
*******************************************************************************/
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
*******************************************************************************/
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
*******************************************************************************/
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
*******************************************************************************/
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
/*******************************************************************************
* Function Name  : Single_Write
* Description    : Single byte writing
* Input          : None
* Output         : None
* Return         : TURE OR FALSE
*******************************************************************************/
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

/*******************************************************************************
* Function Name  : Single_Read
* Description    : Single byte reading
* Input          : None
* Output         : None
* Return         : URE OR FALSE
*******************************************************************************/
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

