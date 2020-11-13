/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      iic.h
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
2017/06/20       ������ͬӲ���汾�µ�iic��������;
*******************************************************************************/
#ifndef __IIC_H
#define __IIC_H
/*---------------1.4�汾-----------------*/
#define SDA      GPIO_Pin_3
#define	SCL      GPIO_Pin_15
/*---------------1.3�汾-----------------*/
#define SDA_1   GPIO_Pin_4
#define	SCL_1	  GPIO_Pin_3

#define FALSE  (0)
#define TRUE   (1)

/*1.4�汾ģ��IIC�˿�������붨��*/
#define SCL_H         (GPIOA->BSRR = GPIO_Pin_15)
#define SCL_L         (GPIOA->BRR  = GPIO_Pin_15)

#define SDA_H         (GPIOB->BSRR = GPIO_Pin_3)
#define SDA_L         (GPIOB->BRR  = GPIO_Pin_3)

#define SCL_read      (GPIOA->IDR  & GPIO_Pin_15)
#define SDA_read      (GPIOB->IDR  & GPIO_Pin_3)

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_15)//SCL IIC�ӿڵ�ʱ���ź�
#define OLED_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_15)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_3)//SDA IIC�ӿڵ������ź�
#define OLED_SDIN_Set() GPIO_SetBits(GPIOB,GPIO_Pin_3)

/*1.3�汾ģ��IIC�˿�������붨��*/
#define SCL_H_1         GPIOB->BSRR = GPIO_Pin_3
#define SCL_L_1         GPIOB->BRR  = GPIO_Pin_3

#define SDA_H_1         GPIOB->BSRR = GPIO_Pin_4
#define SDA_L_1         GPIOB->BRR  = GPIO_Pin_4

#define SCL_read_1      GPIOB->IDR  & GPIO_Pin_3
#define SDA_read_1      GPIOB->IDR  & GPIO_Pin_4

#define OLED_SCLK_Clr_1() GPIO_ResetBits(GPIOB,GPIO_Pin_3)//SCL IIC�ӿڵ�ʱ���ź�
#define OLED_SCLK_Set_1() GPIO_SetBits(GPIOB,GPIO_Pin_3)

#define OLED_SDIN_Clr_1() GPIO_ResetBits(GPIOB,GPIO_Pin_4)//SDA IIC�ӿڵ������ź�
#define OLED_SDIN_Set_1() GPIO_SetBits(GPIOB,GPIO_Pin_4)

u8 I2C_Start(void);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Command(unsigned char IIC_Command);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NoAck(void);
u8 I2C_WaitAck(void) ;
void I2C_SendByte(u8 SendByte);
u8 I2C_RadeByte(void) ;
u8 Single_Write(u8 SlaveAddress,u8 REG_Address,u8 REG_data);
u8 Single_Read(u8 SlaveAddress,u8 REG_Address);
	
#endif