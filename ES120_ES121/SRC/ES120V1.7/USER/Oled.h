/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      Oled.h
Version :
Description:
Author :         Celery
Data:            2016/12/22
History:

*******************************************************************************/
#ifndef _OLED_SSD1306_H
#define _OLED_SSD1306_H
#include "stm32f10x.h"
/*---------------1.4版本-----------------*/
#define SDA      GPIO_Pin_3
#define	SCL      GPIO_Pin_15

#define DEVICEADDR_OLED  0x3c
#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_15)//SCL IIC接口的时钟信号
#define OLED_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_15)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_3)//SDA IIC接口的数据信号
#define OLED_SDIN_Set() GPIO_SetBits(GPIOB,GPIO_Pin_3)
/*---------------1.3版本-----------------*/
#define SDA_1   GPIO_Pin_4
#define	SCL_1	  GPIO_Pin_3

#define DEVICEADDR_OLED  0x3c
#define OLED_SCLK_Clr_1() GPIO_ResetBits(GPIOB,GPIO_Pin_3)//SCL IIC接口的时钟信号
#define OLED_SCLK_Set_1() GPIO_SetBits(GPIOB,GPIO_Pin_3)

#define OLED_SDIN_Clr_1() GPIO_ResetBits(GPIOB,GPIO_Pin_4)//SDA IIC接口的数据信号
#define OLED_SDIN_Set_1() GPIO_SetBits(GPIOB,GPIO_Pin_4)

void OLED_Set_Pos(u8 x, u8 y);
u8* Oled_DrawArea(u8 x0,u8 y0,u8 wide, u8 high,u8* ptr);
void Init_Oled(void);
void Clear_Screen(void);
u8 MPU6050_ReadI2C(u8 REG_Address);
void IIC_Start(void);
void IIC_Stop(void);
u8 IIC_read_byte();
void Write_IIC_Byte(unsigned char IIC_Byte);
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void Disp_Point(void);

#endif
/******************************** END OF FILE *********************************/
