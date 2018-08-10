/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      Delay.c
Version :
Description:
Author :         Celery
Data:            2016/12/22
History:

*******************************************************************************/
#include "Delay.h"

static u8  fac_us=0;//us延时倍乘数
static u16 fac_ms=0;//ms延时倍乘数
/*******************************************************************************
 函数名: Delay_Init
 函数作用:初始化延迟函数
 输入参数:NULL
 返回参数:NULL
*******************************************************************************/
void Delay_Init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
    fac_us=72/8;		    
    fac_ms=(u16)fac_us*1000;
}								    
/*******************************************************************************
 函数名: Delay_Ms
 函数作用:延迟函数
 输入参数:延时ms 毫秒
 返回参数:NULL
*******************************************************************************/
void Delay_Ms(u16 nms)
{	 		  	  
    u32 temp;

    SysTick->LOAD = (u32)nms*fac_ms;//时间加载(SysTick->LOAD为24bit)
    SysTick->VAL  = 0x00;           //清空计数器
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;          //开始倒数  
    do{
              temp = SysTick->CTRL;
    }while(temp & 0x01 && !(temp & (1<<16)));//等待时间到达   
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
    SysTick->VAL = 0X00;       //清空计数器	  	    
}
/******************************** END OF FILE *********************************/
