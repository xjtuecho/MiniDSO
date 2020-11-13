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

extern u8 version_number;//�汾��־
extern u8 Version_jud;//�汾�ж�

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
    
    if(version_number)//1.3�汾
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
    else//1.4�汾
    {
        for(i=0; i<8; i++) {
            OLED_SCLK_Clr();//�ӿ�ʱ���ź�
            if(IIC_Byte & 0x80) {
                OLED_SDIN_Set();//�ӿ������ź�
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
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)//�ȴ��ӻ�����Ӧ���źŸ�����
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
    OLED_SCLK_Set();//ʱ�����0   
    return 0;  
} 

u8 MPU6050_ReadI2C(u8 REG_Address)
{
    u8 REG_data;
    IIC_Start();                  //��ʼ�ź�
    Write_IIC_Byte(0xD0);  //�����豸��ַ+д�ź�
    REG_data=IIC_Wait_Ack();   
    Write_IIC_Byte(REG_Address);   //���ʹ洢��Ԫ��ַ����0��ʼ
    REG_data=IIC_Wait_Ack();   
    IIC_Start();                  //��ʼ�ź�
    Write_IIC_Byte(0xD0+1);//�����豸��ַ+���ź�
    REG_data=IIC_Wait_Ack();   
    REG_data=IIC_read_byte(); //��ȡһ���ֽ�,�������ٶ�,����NAK,�����Ĵ�������
    IIC_Stop();                  //ֹͣ�ź�
return REG_data;
}
/*******************************************************************************
������: IIC_read_byte
��������:IIC��ȡһ��byte
�������:NULL
���ز���:byte
*******************************************************************************/
u8 IIC_read_byte()
{//��IIC���߶�ȡһ���ֽڵ����ݺ���
    u8 i;
    u8 Data; //����һ������Ĵ�����
    for(i=0;i<8;i++)//��8λ����
    {
        OLED_SCLK_Set();//����ʱ���ߣ�Ϊ��ȡ��һλ������׼����
        Data=Data<<1;//�������ֽڵ���������һλ��׼����ȡ���ݡ�

        if(SDA)//���������Ϊ��ƽ��ƽ��
        Data=Data|0x1;//��������ֽڵ����λд1��
        OLED_SCLK_Clr();//����ʱ���ߣ�Ϊ��ȡ��һλ������׼����
    }
    return Data;//���ض�ȡ��һ���ֽ����ݡ�
}

/*******************************************************************************
������: Set_ShowPos
��������:Ҫ��ʾ���ݵ�λ��
�������:x:������,y:������(0,8,16,24)
���ز���:NULL
*******************************************************************************/
void OLED_Set_Pos(u8 x, u8 y)
{
    x +=0;
    Write_IIC_Command(0xb0+y);
    Write_IIC_Command(((x&0xf0)>>4)|0x10);
    Write_IIC_Command(x&0x0f);//Write_IIC_Command((x&0x0f)|0x01);
}
/*******************************************************************************
������: Disp_Point
��������:��ʾ��
�������:x:������,y:������
���ز���:NULL
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

        OLED_SCLK_Clr();//�ӿ�ʱ���ź�
        OLED_SDIN_Clr();//�ӿ������ź�
        OLED_SCLK_Set();   
        OLED_SCLK_Clr();
        OLED_SCLK_Set();
    }
    IIC_Stop();
}

/*******************************************************************************
������: Oled_DrawArea
��������:��ʾһ������
�������: x0:��ʼ������
          y0:��ʼ������(0,8,16,24)
          wide:��ʾ���ݿ��
          high:��ʾ���ݸ߶�
          ptr:��ʾ�����ݿ�ָ��
���ز���:��һ��ָ��
*******************************************************************************/
u8* Oled_DrawArea(u8 x0,u8 y0,u8 wide, u8 high,u8 *ptr)
{
    u8 m,n,y,x;
    
    n = y0 + high;
    if(y0 % 8 == 0) m = y0 / 8;
    else            m = y0 / 8 + 1;

    if(n % 8 == 0)  y = n / 8;
    else            y = n / 8 + 1;
    
    //y���ж�����.
    //m�Ƕ��ٸ�8��,ÿ��һ��mֵ��һ.
    for(; m < y; m++) {
        OLED_Set_Pos(x0,m);
        for(x=0; x<wide; x++) {
            Write_IIC_Data(*ptr++);
        }
    }
    return ptr;
}

/*******************************************************************************
������: Init_Oled
��������:��ʼ��LED����
�������:NULL
���ز���:NULL
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
    Write_IIC_Command(0x14);       //VCC Generated by Internal DC/DC circuit ������ 0x14
    Write_IIC_Command(0xDA);       //Set COM Pins Hardware Configuration 
    Write_IIC_Command(0x02);       //0x02
    Write_IIC_Command(0x81);       //Set Contrast Control 
    Write_IIC_Command(0xe0);       //�Աȶȣ�1-256)//0x56 0xe0
    Write_IIC_Command(0xD9);       //Set Pre-charge Period 
    Write_IIC_Command(0xf1);       //VCC Supplied Externally 0xf1
    Write_IIC_Command(0xDB);       //Set VCOMH Deselect Level (��1��������
    Write_IIC_Command(0x30);       //0,0x20,0x30
    Write_IIC_Command(0xA4);       //Output follows RAM content 
    Write_IIC_Command(0xA6);       //Set Normal Display 
    Write_IIC_Command(0xAF);       //Set Display ON (normal mode) 
}


/******************************** END OF FILE *********************************/
