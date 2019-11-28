/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      olde.c
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
2017/06/20       新增不同硬件版本下的定义配置;
2017/10/21       优化汇总iic配置;    
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
函数名: Set_ShowPos
函数作用:要显示内容的位置
输入参数:x:横坐标,y:纵坐标(0,8,16,24)
返回参数:NULL
*******************************************************************************/
void OLED_Set_Pos(u8 x, u8 y)
{
    x +=0;
    Write_IIC_Command(0xb0+y);
    Write_IIC_Command(((x&0xf0) >> 4) | 0x10);
    Write_IIC_Command(x&0x0f);//Write_IIC_Command((x&0x0f)|0x01);
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
    u8 m,n,y;
    
    n = y0 + high;
    if(y0 % 8 == 0)     m = y0 / 8;
    else                m = y0 / 8 + 1;

    if(n % 8 == 0)      y = n / 8;
    else                y = n / 8 + 1;
    
    //y是有多少行.
    //m是多少个8行,每多一个m值加一.
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
函数名: Init_Oled
函数作用:初始化LED设置
输入参数:NULL
返回参数:NULL
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
