/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      olde.c
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
2017/06/20       ������ͬӲ���汾�µĶ�������;
2017/10/21       �Ż�����iic����;    
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "Oled.h"
#include "HARDWARE.h"
#include "Delay.h"
#include "iic.h"

u8 gOled_param[23] = {  0xAE, 0xD5, 0x52, 0xA8, 0x0f,
                        0xC8, 0xD3, 0x00, 0x40, 0xA1,
                        0x8D, 0x14, 0xDA, 0x02, 0x81, 
                        0x33, 0xD9, 0xF1, 0xDB, 0x30,
                        0xA4, 0XA6, 0xAF
                     };

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
    Write_IIC_Command(((x&0xf0) >> 4) | 0x10);
    Write_IIC_Command(x&0x0f);//Write_IIC_Command((x&0x0f)|0x01);
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
    u8 m,n,y;
    
    n = y0 + high;
    if(y0 % 8 == 0)     m = y0 / 8;
    else                m = y0 / 8 + 1;

    if(n % 8 == 0)      y = n / 8;
    else                y = n / 8 + 1;
    
    //y���ж�����.
    //m�Ƕ��ٸ�8��,ÿ��һ��mֵ��һ.
    for(; m < y; m++) {
        OLED_Set_Pos(x0,m);
        Write_IIC_PageData(ptr, wide);
        ptr+=wide;
        //for(x=0; x<wide; x++) {
        //    Write_IIC_Data(*ptr++);
        //}
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
    u32 i;
  
    OLED_RST();
    Delay_Ms(2);
    OLED_ACT();
    Delay_Ms(2);
    
    Delay_Ms(105);
    for(i = 0; i < 23; i++)
        Write_IIC_Command(gOled_param[i]);
}
/******************************** END OF FILE *********************************/
