/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      iic.h
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
2017/06/20       新增不同硬件版本下的iic定义配置;
*******************************************************************************/
#ifndef __IIC_H
#define __IIC_H

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

/*---------------1.4版本-----------------*/
#define SDA      GPIO_Pin_3
#define	SCL      GPIO_Pin_15
/*---------------1.3版本-----------------*/
#define SDA_1   GPIO_Pin_4
#define	SCL_1	  GPIO_Pin_3

#define FALSE  (0)
#define TRUE   (1)

/*1.4版本模拟IIC端口输出输入定义*/
#define SCL_H         PAout(15)=1       //(GPIOA->BSRR = GPIO_Pin_15)
#define SCL_L         PAout(15)=0       //(GPIOA->BRR  = GPIO_Pin_15)

#define SDA_H         PBout(3)=1       //(GPIOB->BSRR = GPIO_Pin_3)
#define SDA_L         PBout(3)=0      //(GPIOB->BRR  = GPIO_Pin_3)

#define SCL_read      (GPIOA->IDR  & GPIO_Pin_15)
#define SDA_read      (GPIOB->IDR  & GPIO_Pin_3)

#define OLED_SCLK_Clr() PAout(15)=0; \
                        asm("NOP"); \
                        asm("NOP")  


//GPIO_ResetBits(GPIOA,GPIO_Pin_15) //GPIO_ResetBits(GPIOA,GPIO_Pin_15)//SCL IIC接口的时钟信号
#define OLED_SCLK_Set() PAout(15)=1; \
                        asm("NOP"); \
                        asm("NOP")    

//GPIO_SetBits(GPIOA,GPIO_Pin_15) //GPIO_SetBits(GPIOA,GPIO_Pin_15)

#define OLED_SDIN_Clr() PBout(3)=0; \
                        asm("NOP"); \
                        asm("NOP")


//GPIO_ResetBits(GPIOB,GPIO_Pin_3)  //GPIO_ResetBits(GPIOB,GPIO_Pin_3)//SDA IIC接口的数据信号
#define OLED_SDIN_Set() PBout(3)=1; \
                        asm("NOP"); \
                        asm("NOP")      

//GPIO_SetBits(GPIOB,GPIO_Pin_3) //GPIO_SetBits(GPIOB,GPIO_Pin_3)

/*1.3版本模拟IIC端口输出输入定义*/
#define SCL_H_1         GPIOB->BSRR = GPIO_Pin_3 //PBout(3)=1       //GPIOB->BSRR = GPIO_Pin_3
#define SCL_L_1         GPIOB->BRR  = GPIO_Pin_3 //PBout(3)=0       //GPIOB->BRR  = GPIO_Pin_3

#define SDA_H_1         GPIOB->BSRR = GPIO_Pin_4 //PBout(4)=1       //GPIOB->BSRR = GPIO_Pin_4
#define SDA_L_1         GPIOB->BRR  = GPIO_Pin_4 //PBout(4)=0       //GPIOB->BRR  = GPIO_Pin_4

#define SCL_read_1      GPIOB->IDR  & GPIO_Pin_3
#define SDA_read_1      GPIOB->IDR  & GPIO_Pin_4

#define OLED_SCLK_Clr_1() PBout(3)=0       //GPIO_ResetBits(GPIOB,GPIO_Pin_3)//SCL IIC接口的时钟信号
#define OLED_SCLK_Set_1() PBout(3)=1       //GPIO_SetBits(GPIOB,GPIO_Pin_3)

#define OLED_SDIN_Clr_1() PBout(4)=0       //GPIO_ResetBits(GPIOB,GPIO_Pin_4)//SDA IIC接口的数据信号
#define OLED_SDIN_Set_1() PBout(4)=1       //GPIO_SetBits(GPIOB,GPIO_Pin_4)

u8 I2C_Start(void);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_PageData(unsigned char *buf, u8 cnt);
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