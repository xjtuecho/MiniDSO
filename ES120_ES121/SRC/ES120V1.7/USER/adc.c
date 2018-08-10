/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      Adc.c
Version :
Description:
Author :         Celery
Data:            2016/05/17
History:

*******************************************************************************/
#include "adc.h"
#include "delay.h"

/*******************************************************************************
函数名: Adc_Init
函数作用:初始化ADC
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void  Adc_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );	  //使能ADC1通道时钟
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

    //PA3 Vcur ADC1 channel3//PA0 Vin ADC1 channel0//PA2 Vbty ADC1 channel2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_0|GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    ADC_DeInit(ADC1);  //ADC1的全部寄存器重设为缺省值

    ADC_InitStructure.ADC_Mode                = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode        = DISABLE;	//模数转换工作在单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode  = DISABLE;	//模数转换工作在单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv    = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
    ADC_InitStructure.ADC_DataAlign           = ADC_DataAlign_Right;	//ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel        = 1;	//顺序进行规则转换的ADC通道的数目
    ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器


    ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1

    ADC_ResetCalibration(ADC1);	//使能复位校准

    while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束

    ADC_StartCalibration(ADC1);	 //开启AD校准

    while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
}
/*******************************************************************************
函数名: Get_Adc
函数作用:读取ADC
输入参数:频道
返回参数:ADC转换值
*******************************************************************************/
u16 Get_Adc(u8 channel)
{
    //设置指定ADC的规则组通道，一个序列，采样时间
    if(channel == VIN)//Vin PA0
        ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 ); //ADC1,ADC通道,采样时间为239.5周期
    else if(channel == VBTY)//Vcur  PA2
        ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_239Cycles5 );   //ADC1,ADC通道,采样时间为239.5周期
    else if(channel == VCUR)//Vcur  PA3
        ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

    return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}
/*******************************************************************************
函数名: Get_AvgAdc
函数作用:读取ADC的平均值
输入参数:u8 channel
返回参数:ADC转换平均值
*******************************************************************************/
u16 Get_AvgAdc(u8 channel)
{
    u32 temp_val=0;
    u8 i;
    if(channel == 0)//pa0
    {
        for(i=0; i < 10; i++) {
            temp_val += Get_Adc(VCUR);
        }
    }else if(channel == 1)//pa2
    {
        for(i=0; i < 10; i++) {
            temp_val += Get_Adc(VBTY);
        }
    }
    return temp_val/10;
}
/******************************** END OF FILE *********************************/
