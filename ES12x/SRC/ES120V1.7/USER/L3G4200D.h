/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      L3G4200D.h
Version :
Description:
Author :         Celery
Data:            2016/12/22
History:

*******************************************************************************/
#ifndef __L3G4200D_H
#define __L3G4200D_H

#include "stm32f10x.h"

#define	L3G4200_Addr   0xD6	 //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改

//**********L3G4200D内部寄存器地址*********
#define WHO_AM_I 0x0F
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24
#define REFERENCE 0x25
#define OUT_TEMP 0x26
#define STATUS_REG 0x27
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D
#define FIFO_CTRL_REG 0x2E
#define FIFO_SRC_REG 0x2F
#define INT1_CFG 0x30
#define INT1_SRC 0x31
#define INT1_TSH_XH 0x32
#define INT1_TSH_XL 0x33
#define INT1_TSH_YH 0x34
#define INT1_TSH_YL 0x35
#define INT1_TSH_ZH 0x36
#define INT1_TSH_ZL 0x37
#define INT1_DURATION 0x38
//#define GAIN   (0.07)     // 满量程2000dps时灵敏度(dps/digital)
//#define GAIN   (0.0175)     // 满量程500dps时灵敏度(dps/digital)
#define GAIN   (0.00875)     // 满量程250dps时灵敏度(dps/digital)

#define bool u8

void Init_L3G4200D(void);
void READ_L3G4200D(void);
float Get_Angle(void);

void I2C_delay(void);
void delay5ms(void);
u8 I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NoAck(void);
u8 I2C_WaitAck(void);
void I2C_SendByte(u8 SendByte);
u8 I2C_RadeByte(void);
u8 Single_Write(u8 SlaveAddress,u8 REG_Address,u8 REG_data);
u8 Single_Read(u8 SlaveAddress,u8 REG_Address);

#endif
/******************************** END OF FILE *********************************/
