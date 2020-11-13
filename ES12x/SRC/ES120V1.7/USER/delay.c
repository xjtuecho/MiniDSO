/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      Delay.c
Version :
Description:
Author :         Celery
Data:            2016/12/22
History:

*******************************************************************************/
#include "Delay.h"

static u8  fac_us=0;//us��ʱ������
static u16 fac_ms=0;//ms��ʱ������
/*******************************************************************************
 ������: Delay_Init
 ��������:��ʼ���ӳٺ���
 �������:NULL
 ���ز���:NULL
*******************************************************************************/
void Delay_Init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
    fac_us=72/8;		    
    fac_ms=(u16)fac_us*1000;
}								    
/*******************************************************************************
 ������: Delay_Ms
 ��������:�ӳٺ���
 �������:��ʱms ����
 ���ز���:NULL
*******************************************************************************/
void Delay_Ms(u16 nms)
{	 		  	  
    u32 temp;

    SysTick->LOAD = (u32)nms*fac_ms;//ʱ�����(SysTick->LOADΪ24bit)
    SysTick->VAL  = 0x00;           //��ռ�����
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;          //��ʼ����  
    do{
              temp = SysTick->CTRL;
    }while(temp & 0x01 && !(temp & (1<<16)));//�ȴ�ʱ�䵽��   
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
    SysTick->VAL = 0X00;       //��ռ�����	  	    
}
/******************************** END OF FILE *********************************/
