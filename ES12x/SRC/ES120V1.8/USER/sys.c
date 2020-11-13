/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      Sys.c
Version :
Description:
Author :         Celery
Data:            2016/12/22
History:

*******************************************************************************/
#include "App.h"
#include "sys.h"
#include "stm32f10x_flash.h"

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
}

/*******************************************************************************
函数名: RCC_Config
函数作用:时钟初始化
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void RCC_Config(void)
{
    RCC_DeInit();
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    FLASH_SetLatency(FLASH_Latency_1);   // Flash 1 wait state for 48MHz
    RCC_CFGR_CFG();
    RCC_PLL_EN();
    RCC_HSICmd(ENABLE);
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {}
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08) {}

    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div1);       // USBCLK = 48MHz
}
/******************************** END OF FILE *********************************/
