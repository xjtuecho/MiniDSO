/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      adc.c
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
2017/11/11       添加AD看门狗;
*******************************************************************************/
#include "adc.h"
#include "delay.h"
#include "stm32f10x_dma.h"

vu16 ADC1ConvertedValue[3];
vu32 adc_avg[3], adc_count[3];
vu16 AWD_entry;
 
/*******************************************************************************
函数名: Get_AdcValue
函数作用:获取ADC 转换后的读数
输入参数:转换后的AD
返回参数:NULL
*******************************************************************************/
u16 Get_AdcValue(u8 i)
{
    return ADC1ConvertedValue[i];
}

/*******************************************************************************
函数名: Adc_Init
函数作用:初始化ADC
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void  Adc_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef   DMA_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//使能DMA时钟  ////
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );//使能ADC1通道时钟
    ////RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

    //PA3 Vcur ADC1 channel3转动//PA0 Vin ADC1 channel0//PA2 Vbty ADC1 channel2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_0|GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
     
    ADC_DeInit(ADC1);  //ADC1的全部寄存器重设为缺省值

    ADC_InitStructure.ADC_Mode                = ADC_Mode_Independent;//ADC工作模式:ADC1和ADC2工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode        = ENABLE;//模数转换工作在单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode  = ENABLE;//模数转换工作在单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv    = ADC_ExternalTrigConv_None;//转换由软件而不是外部触发启动
    ADC_InitStructure.ADC_DataAlign           = ADC_DataAlign_Right;//ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel        = 3;//顺序进行规则转换的ADC通道的数目
    ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_239Cycles5 );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3, ADC_SampleTime_239Cycles5);//28 or 55   2017.8.30通道7改为4
    

    
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);	                //使能指定的ADC1

    ADC_ResetCalibration(ADC1);	                //使能复位校准

    while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束

    ADC_StartCalibration(ADC1);	                //开启AD校准

    while(ADC_GetCalibrationStatus(ADC1));	//等待校准结束

   /* DMA1 channel1 configuration -------------------------------------------*/
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC1ConvertedValue;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize =3;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    /* Enable DMA1 channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE); //使能DMA传输完成中断
	
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}
/*******************************************************************************
函数名: Set_CurLimit
函数作用:ADC看门狗
输入参数:电流上下限
返回参数:NULL
*******************************************************************************/
void Set_CurLimit(u16 high_ma,u16 low_ma)
{
  // Configure high and low analog watchdog thresholds 
  ADC_AnalogWatchdogThresholdsConfig(ADC1, high_ma*4096/60000, low_ma*4096/60000);
  // Configure channel14 as the single analog watchdog guarded channel 
  ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_Channel_3);
  // Enable analog watchdog on one regular channel 
  ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_SingleRegEnable);
  // Enable AWD interupt 
  ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);
}
/*******************************************************************************
函数名:ADC1_2_IRQHandler
函数作用:ADC1中断
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void ADC1_2_IRQHandler(void)    
{
   if(ADC_GetITStatus(ADC1, ADC_IT_AWD))
   {
       AWD_entry++;
       // Clear ADC1 AWD pending interrupt bit */
       ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
   } 

}
/*******************************************************************************
函数名:filter
函数作用:计算ADC平均值
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void filter(void)
{
   #define N (16)//平均次数
   static u8 FirstEntry = 1;
   u8 i;
   if(FirstEntry)
   {
   	for(i=0;i<2;i++)
       	adc_count[i] =  ADC1ConvertedValue[i] * N;
	FirstEntry = 0;
   }
      
   for(i=0;i<3;i++)
   {
        adc_count[i] = adc_count[i] -adc_count[i] /N  + ADC1ConvertedValue[i];
        adc_avg[i] =  adc_count[i] /N;
   }		
   
}
/*******************************************************************************
函数名:DMA1_Channel1_IRQHandler
函数作用:DMAz中断
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
    {
        filter();
        DMA_ClearITPendingBit(DMA1_IT_TC1);
    }
}
/*******************************************************************************
函数名:Get_Adc
函数作用:获取AD值
输入参数:通道:channel
返回参数:AD值:adc_avg[channel]
*******************************************************************************/
u16 Get_Adc(u8 channel)
{
    return adc_avg[channel];    
}
/******************************** END OF FILE *********************************/
