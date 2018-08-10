/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      Oled.c
Version :
Description:
Author :         Celery
Data:            2016/12/22
History:

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "Oled.h"
#include "HARDWARE.h"
#include "Delay.h"

#define OLED_VCC_ON()       GPIO_SetBits(GPIOB,GPIO_Pin_4)
#define OLED_RST_PIN        GPIO_Pin_0     //PB0
#define OLED_RST()          GPIO_ResetBits(GPIOB, OLED_RST_PIN)
#define OLED_ACT()          GPIO_SetBits  (GPIOB, OLED_RST_PIN)

extern u8 version_number;//版本标志
extern u8 Version_jud;//版本判断

void IIC_Start(void)
{
    if(version_number)
    {
        OLED_SCLK_Set_1() ;
        OLED_SDIN_Set_1();
        OLED_SDIN_Clr_1();
        OLED_SCLK_Clr_1();
    }
    else
    {
        OLED_SCLK_Set() ;
        OLED_SDIN_Set();
        OLED_SDIN_Clr();
        OLED_SCLK_Clr();
    }
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop(void)
{
    if(version_number)
    {
        OLED_SCLK_Clr_1();
        OLED_SDIN_Clr_1();
        OLED_SDIN_Set_1();
        OLED_SCLK_Set_1();
    }
    else
    {
        OLED_SCLK_Clr();
        OLED_SDIN_Clr();
        OLED_SDIN_Set();
        OLED_SCLK_Set();
    }
}
/**********************************************
// IIC Write byte
**********************************************/

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
/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
    IIC_Start();
    Write_IIC_Byte(0x78);            //Slave address,SA0=0
    Write_IIC_Byte(0x00);			//write command
    Write_IIC_Byte(IIC_Command);
    IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
    IIC_Start();
    Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
    Write_IIC_Byte(0x40);			//write data
    Write_IIC_Byte(IIC_Data);
    IIC_Stop();
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)//等待从机发送应答信号给主机
{
    u8 ucErrTime=0;
    OLED_SDIN_Clr();Delay_Ms(2);
      
    OLED_SCLK_Clr();Delay_Ms(2);
    while(SDA)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    OLED_SCLK_Set();//时钟输出0   
    return 0;  
} 

u8 MPU6050_ReadI2C(u8 REG_Address)
{
    u8 REG_data;
    IIC_Start();                  //起始信号
    Write_IIC_Byte(0xD0);  //发送设备地址+写信号
    REG_data=IIC_Wait_Ack();   
    Write_IIC_Byte(REG_Address);   //发送存储单元地址，从0开始
    REG_data=IIC_Wait_Ack();   
    IIC_Start();                  //起始信号
    Write_IIC_Byte(0xD0+1);//发送设备地址+读信号
    REG_data=IIC_Wait_Ack();   
    REG_data=IIC_read_byte(); //读取一个字节,不继续再读,发送NAK,读出寄存器数据
    IIC_Stop();                  //停止信号
return REG_data;
}
/*******************************************************************************
函数名: IIC_read_byte
函数作用:IIC读取一个byte
输入参数:NULL
返回参数:byte
*******************************************************************************/
u8 IIC_read_byte()
{//从IIC总线读取一个字节的数据函数
    u8 i;
    u8 Data; //定义一个缓冲寄存器。
    for(i=0;i<8;i++)//有8位数据
    {
        OLED_SCLK_Set();//拉高时钟线，为读取下一位数据做准备。
        Data=Data<<1;//将缓冲字节的数据左移一位，准备读取数据。

        if(SDA)//如果数据线为高平电平。
        Data=Data|0x1;//则给缓冲字节的最低位写1。
        OLED_SCLK_Clr();//拉低时钟线，为读取下一位数据做准备。
    }
    return Data;//返回读取的一个字节数据。
}

/*******************************************************************************
函数名: Set_ShowPos
函数作用:要显示内容的位置
输入参数:x:横坐标,y:纵坐标(0,8,16,24)
返回参数:NULL
*******************************************************************************/
void OLED_Set_Pos(u8 x, u8 y)
{
    x +=0;
    Write_IIC_Command(0xb0+y);
    Write_IIC_Command(((x&0xf0)>>4)|0x10);
    Write_IIC_Command(x&0x0f);//Write_IIC_Command((x&0x0f)|0x01);
}
/*******************************************************************************
函数名: Disp_Point
函数作用:显示点
输入参数:x:横坐标,y:纵坐标
返回参数:NULL
*******************************************************************************/
void Disp_Point(void)
{
    
    IIC_Start();
    Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
    Write_IIC_Byte(0x40);			//write data

    if(version_number)
    {

        OLED_SCLK_Clr_1();
        OLED_SDIN_Clr_1();
        OLED_SCLK_Set_1();
        OLED_SCLK_Clr_1();
        OLED_SCLK_Set_1();
    }
    else
    {

        OLED_SCLK_Clr();//接口时钟信号
        OLED_SDIN_Clr();//接口数据信号
        OLED_SCLK_Set();   
        OLED_SCLK_Clr();
        OLED_SCLK_Set();
    }
    IIC_Stop();
}

/*******************************************************************************
函数名: Oled_DrawArea
函数作用:显示一个区域
输入参数: x0:起始横坐标
          y0:起始纵坐标(0,8,16,24)
          wide:显示内容宽度
          high:显示内容高度
          ptr:显示的内容库指针
返回参数:下一库指针
*******************************************************************************/
u8* Oled_DrawArea(u8 x0,u8 y0,u8 wide, u8 high,u8 *ptr)
{
    u8 m,n,y,x;
    
    n = y0 + high;
    if(y0 % 8 == 0) m = y0 / 8;
    else            m = y0 / 8 + 1;

    if(n % 8 == 0)  y = n / 8;
    else            y = n / 8 + 1;
    
    //y是有多少行.
    //m是多少个8行,每多一个m值加一.
    for(; m < y; m++) {
        OLED_Set_Pos(x0,m);
        for(x=0; x<wide; x++) {
            Write_IIC_Data(*ptr++);
        }
    }
    return ptr;
}

/*******************************************************************************
函数名: Init_Oled
函数作用:初始化LED设置
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Init_Oled(void)
{
    OLED_RST();
    Delay_Ms(2);
    OLED_ACT();
    Delay_Ms(2);
    
    Delay_Ms(105);

    Write_IIC_Command(0xAE);       //Display OFF (sleep mode) 
    Write_IIC_Command(0xD5);       //Set Display Clock Divide Ratio/Oscillator Frequency
    Write_IIC_Command(0x52);       //parem
    Write_IIC_Command(0xA8);       //Set Multiplex Ratio 
    Write_IIC_Command(0x0f);       //0x0f A[5:0]
    Write_IIC_Command(0xC8);       //Set COM Output Scan Direction  0xC0 
    Write_IIC_Command(0xD3);       //Set Display Offset 
    Write_IIC_Command(0x00); 
    Write_IIC_Command(0x40);       //Set Display Start Line 0x40
    Write_IIC_Command(0xA1);       //Set Segment Re-map 0xA0
    Write_IIC_Command(0x8D);       //Set Display Offset 
    Write_IIC_Command(0x14);       //VCC Generated by Internal DC/DC circuit 调亮度 0x14
    Write_IIC_Command(0xDA);       //Set COM Pins Hardware Configuration 
    Write_IIC_Command(0x02);       //0x02
    Write_IIC_Command(0x81);       //Set Contrast Control 
    Write_IIC_Command(0xe0);       //对比度（1-256)//0x56 0xe0
    Write_IIC_Command(0xD9);       //Set Pre-charge Period 
    Write_IIC_Command(0xf1);       //VCC Supplied Externally 0xf1
    Write_IIC_Command(0xDB);       //Set VCOMH Deselect Level (带1个参数）
    Write_IIC_Command(0x30);       //0,0x20,0x30
    Write_IIC_Command(0xA4);       //Output follows RAM content 
    Write_IIC_Command(0xA6);       //Set Normal Display 
    Write_IIC_Command(0xAF);       //Set Display ON (normal mode) 
}


/******************************** END OF FILE *********************************/
