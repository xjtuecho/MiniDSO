/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      adc.c
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
2017/11/11       ���AD���Ź�;
*******************************************************************************/
#include "adc.h"
#include "delay.h"
#include "stm32f10x_dma.h"

vu16 ADC1ConvertedValue[3];
vu32 adc_avg[3], adc_count[3];
vu16 AWD_entry;
 
/*******************************************************************************
������: Get_AdcValue
��������:��ȡADC ת����Ķ���
�������:ת�����AD
���ز���:NULL
*******************************************************************************/
u16 Get_AdcValue(u8 i)
{
    return ADC1ConvertedValue[i];
}

/*******************************************************************************
������: Adc_Init
��������:��ʼ��ADC
�������:NULL
���ز���:NULL
*******************************************************************************/
void  Adc_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef   DMA_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//ʹ��DMAʱ��  ////
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );//ʹ��ADC1ͨ��ʱ��
    ////RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

    //PA3 Vcur ADC1 channel3ת��//PA0 Vin ADC1 channel0//PA2 Vbty ADC1 channel2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_0|GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
     
    ADC_DeInit(ADC1);  //ADC1��ȫ���Ĵ�������Ϊȱʡֵ

    ADC_InitStructure.ADC_Mode                = ADC_Mode_Independent;//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
    ADC_InitStructure.ADC_ScanConvMode        = ENABLE;//ģ��ת�������ڵ�ͨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode  = ENABLE;//ģ��ת�������ڵ���ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv    = ADC_ExternalTrigConv_None;//ת��������������ⲿ��������
    ADC_InitStructure.ADC_DataAlign           = ADC_DataAlign_Right;//ADC�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel        = 3;//˳����й���ת����ADCͨ������Ŀ
    ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_239Cycles5 );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3, ADC_SampleTime_239Cycles5);//28 or 55   2017.8.30ͨ��7��Ϊ4
    

    
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);	                //ʹ��ָ����ADC1

    ADC_ResetCalibration(ADC1);	                //ʹ�ܸ�λУ׼

    while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����

    ADC_StartCalibration(ADC1);	                //����ADУ׼

    while(ADC_GetCalibrationStatus(ADC1));	//�ȴ�У׼����

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
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE); //ʹ��DMA��������ж�
	
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}
/*******************************************************************************
������: Set_CurLimit
��������:ADC���Ź�
�������:����������
���ز���:NULL
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
������:ADC1_2_IRQHandler
��������:ADC1�ж�
�������:NULL
���ز���:NULL
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
������:filter
��������:����ADCƽ��ֵ
�������:NULL
���ز���:NULL
*******************************************************************************/
void filter(void)
{
   #define N (16)//ƽ������
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
������:DMA1_Channel1_IRQHandler
��������:DMAz�ж�
�������:NULL
���ز���:NULL
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
������:Get_Adc
��������:��ȡADֵ
�������:ͨ��:channel
���ز���:ADֵ:adc_avg[channel]
*******************************************************************************/
u16 Get_Adc(u8 channel)
{
    return adc_avg[channel];    
}
/******************************** END OF FILE *********************************/
