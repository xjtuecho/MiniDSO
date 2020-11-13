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
������: Adc_Init
��������:��ʼ��ADC
�������:NULL
���ز���:NULL
*******************************************************************************/
void  Adc_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

    //PA3 Vcur ADC1 channel3//PA0 Vin ADC1 channel0//PA2 Vbty ADC1 channel2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_0|GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    ADC_DeInit(ADC1);  //ADC1��ȫ���Ĵ�������Ϊȱʡֵ

    ADC_InitStructure.ADC_Mode                = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
    ADC_InitStructure.ADC_ScanConvMode        = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode  = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv    = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
    ADC_InitStructure.ADC_DataAlign           = ADC_DataAlign_Right;	//ADC�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel        = 1;	//˳����й���ת����ADCͨ������Ŀ
    ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���


    ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1

    ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼

    while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����

    ADC_StartCalibration(ADC1);	 //����ADУ׼

    while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
}
/*******************************************************************************
������: Get_Adc
��������:��ȡADC
�������:Ƶ��
���ز���:ADCת��ֵ
*******************************************************************************/
u16 Get_Adc(u8 channel)
{
    //����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
    if(channel == VIN)//Vin PA0
        ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 ); //ADC1,ADCͨ��,����ʱ��Ϊ239.5����
    else if(channel == VBTY)//Vcur  PA2
        ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_239Cycles5 );   //ADC1,ADCͨ��,����ʱ��Ϊ239.5����
    else if(channel == VCUR)//Vcur  PA3
        ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

    return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}
/*******************************************************************************
������: Get_AvgAdc
��������:��ȡADC��ƽ��ֵ
�������:u8 channel
���ز���:ADCת��ƽ��ֵ
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
