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
//unsigned char BUF[8];                         //�������ݻ�����
//char  test=0;
//float T_X,T_Y,T_Z;

extern u8 version_number;
#define FALSE  (0)
#define TRUE   (1)
//************************************
/*1.4�汾ģ��IIC�˿�������붨��*/
#define SCL_H         (GPIOA->BSRR = GPIO_Pin_15)
#define SCL_L         (GPIOA->BRR  = GPIO_Pin_15)

#define SDA_H         (GPIOB->BSRR = GPIO_Pin_3)
#define SDA_L         (GPIOB->BRR  = GPIO_Pin_3)

#define SCL_read      (GPIOA->IDR  & GPIO_Pin_15)
#define SDA_read      (GPIOB->IDR  & GPIO_Pin_3)

/*1.3�汾ģ��IIC�˿�������붨��*/
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

    u8 i=30; //��������Ż��ٶ�	����������͵�5����д��
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
        if(!SDA_read_1)return FALSE;	//SDA��Ϊ�͵�ƽ������æ,�˳�
        SDA_L_1;
        I2C_delay();
        if(SDA_read_1) return FALSE;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
        SDA_L_1;
        I2C_delay();
        return TRUE;
    }
    else
    {
        SDA_H;
        SCL_H;
        I2C_delay();
        if(!SDA_read)return FALSE;	//SDA��Ϊ�͵�ƽ������æ,�˳�
        SDA_L;
        I2C_delay();
        if(SDA_read) return FALSE;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
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
u8 I2C_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
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
void I2C_SendByte(u8 SendByte) //���ݴӸ�λ����λ//
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
u8 I2C_RadeByte(void)  //���ݴӸ�λ����λ//
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
//���ֽ�д��*******************************************
u8 Single_Write(u8 SlaveAddress,u8 REG_Address,u8 REG_data)		   
{
    if(!I2C_Start())  return FALSE;
    I2C_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�//I2C_SendByte(((REG_Address & 0x0700) >>7) | SlaveAddress & 0xFFFE);//���ø���ʼ��ַ+������ַ
    if(!I2C_WaitAck()) {
        I2C_Stop();
        return FALSE;
    }
    I2C_SendByte(REG_Address );   //���õ���ʼ��ַ
    I2C_WaitAck();
    I2C_SendByte(REG_data);
    I2C_WaitAck();
    I2C_Stop();
    delay5ms();
    return TRUE;
}

//���ֽڶ�ȡ*****************************************
u8 Single_Read(u8 SlaveAddress,u8 REG_Address)
{
    u8 REG_data;
    if(!I2C_Start())return FALSE;
    I2C_SendByte(SlaveAddress); //I2C_SendByte(((REG_Address & 0x0700) >>7) | REG_Address & 0xFFFE);//���ø���ʼ��ַ+������ַ
    if(!I2C_WaitAck()) {
        I2C_Stop();
        return FALSE;
    }
    I2C_SendByte((u8) REG_Address);   //���õ���ʼ��ַ
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
    
    Single_Write(L3G4200_Addr,CTRL_REG1, 0x0F);
    Single_Write(L3G4200_Addr,CTRL_REG2, 0x00);
    Single_Write(L3G4200_Addr,CTRL_REG3, 0x00);
    Single_Write(L3G4200_Addr,CTRL_REG4, 0x00);//0x30);	//+-2000dps
    Single_Write(L3G4200_Addr,CTRL_REG5, 0x02);//|0x10);//0x00);
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
������: Get_Angle
��������:��ȡת�������ĽǶ�
�������:NULL
���ز���:�Ƕ�
*******************************************************************************/
float Get_Angle(void)
{
    static unsigned long cur_timer,last_timer,dt;
    float angle;

    cur_timer = Timer_StartValue(); //��ǰʱ��
    dt = Elapse_Value(last_timer,cur_timer); //������ʱ��
    last_timer = cur_timer;
    
    READ_L3G4200D();
    
    angle = GAIN * (gAg_data) * (float)dt / 100.0;//����ת���ĽǶ�

    return angle;

}
/******************************** END OF FILE *********************************/
