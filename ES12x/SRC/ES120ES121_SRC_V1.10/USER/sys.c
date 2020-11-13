/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      sys.c
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
*******************************************************************************/
#include "App.h"
#include "sys.h"
#include "stm32f10x_flash.h"

u8 frequency_pos = 1;//48MHz/8MHz
/*******************************************************************************
函数名: NVIC_Configuration
函数作用:NVIC初始化
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel                   = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);          // Enable the DMA Interrupt 

    /* Configure and enable ADC interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
}
/*******************************************************************************
函数名: RCC_Config
函数作用:时钟初始化
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void RCC_Config(void)
{
    u8 RCC_Getclk = 0x00;
    RCC_DeInit();
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    
    if(frequency_pos)
    {
        FLASH_SetLatency(FLASH_Latency_1);   // Flash 1 wait state for 48MHz
        RCC_Getclk = 0x08;
    }
    else
    {
        FLASH_SetLatency(FLASH_Latency_0);   // 8MHz
        RCC_Getclk = 0x00;
    }
    
    RCC_CFGR_CFG();
    RCC_PLL_EN();
    RCC_HSICmd(ENABLE);
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {}
    if(frequency_pos)           RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    else                        RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
    while(RCC_GetSYSCLKSource() != RCC_Getclk) {}

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SRAM | RCC_AHBPeriph_DMA1, ENABLE);

    if(frequency_pos)   RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div1);       // USBCLK = 48MHz
}
/******************************** END OF FILE *********************************/
