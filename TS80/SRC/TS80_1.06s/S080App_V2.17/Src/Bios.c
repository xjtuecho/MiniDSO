/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
File Name :      Bios.c
Version :        TS080 APP 1.02
Description:
Author :         Ning
Data:            2017/11/06
History:
2017/12/01   修改读取ad在TIM3中断
2018/02/01   看门狗监控电压
*******************************************************************************/

#include "APP_Version.h"
#include "usb_lib.h"
#include "Bios.h"
#include "HARDWARE.h"
#include "I2C.h"
#include "CTRL.h"

u8 frequency_pos = 1;//48MHz/8MHz

vu32 gTime[8];
vu16 ADC1ConvertedValue[2];
vu16 AWD_entry;//2018.2.1

static vu32 gMs_timeout;
static vu32 gHeat_cnt = 0;
extern u8 MarkAd_pos;//2017.12.1
extern u32 slide_data;//2017.12.5
extern vu32 Timer_Counter;
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
函数名: Set_HeatingTime
函数作用:设置加热时间
输入参数:heating_time 加热时间
返回参数:NULL
*******************************************************************************/
void Set_HeatingTime(u32 heating_time)
{
    gHeat_cnt = heating_time;
}
/*******************************************************************************
函数名: Get_HeatingTime
函数作用:读取加热时间
输入参数:NULL
返回参数:加热时间
*******************************************************************************/
u32 Get_HeatingTime(void)
{
    return gHeat_cnt;
}
/*******************************************************************************
函数名: Init_GTIME
函数作用:初始化计时器
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Init_Gtime(void)
{
    u8 i;

    for(i = 0; i < 8; i++)      gTime[i] = 0;
}
/*******************************************************************************
函数名: Delay_Ms
函数作用:延时程序。
输入参数:延时等待的毫秒数值
返回参数:NULL
*******************************************************************************/
void Delay_Ms(u32 ms)
{
    if(frequency_pos)   gMs_timeout = ms * 20;
    else                gMs_timeout = ms * 20;
    while(gMs_timeout); // {if(Scan_key()!=0)break;}
}
/*******************************************************************************
函数名: Delay_HalfMs
函数作用:每单位为0.5毫秒的延时程序。
输入参数:延时等待的0.5毫秒数
返回参数:NULL
*******************************************************************************/
void Delay_HalfMs(u32 ms)
{
    if(frequency_pos)   gMs_timeout = ms * 10;
    else                gMs_timeout = ms * 10;
    while(gMs_timeout); // {if(Scan_key()!=0)break;}
}

/*******************************************************************************
函数名: USB_Port
函数作用:设置 USB 设备 IO 端口
输入参数:State = ENABLE / DISABLE
返回参数:NULL
*******************************************************************************/
void USB_Port(u8 state)
{
    USB_DN_LOW();
    USB_DP_LOW();
    if(state == DISABLE)
    {
        USB_DN_OUT();
        USB_DP_OUT();
    }
    else
    {
        USB_DN_EN();
        USB_DP_EN();
    }
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
        FLASH_SetLatency(FLASH_Latency_0);    //8MHz
        RCC_Getclk = 0x00;
    }
    
    RCC_CFGR_CFG();
    RCC_PLL_EN();
    RCC_HSICmd(ENABLE);
    RCC_PLLCmd(ENABLE);
    
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {}
    if(frequency_pos)   RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    else                RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
    while(RCC_GetSYSCLKSource() != RCC_Getclk) {}

    RCC_AHBPeriphClockCmd  (RCC_AHBPeriph_SRAM   | RCC_AHBPeriph_DMA1 , ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                            RCC_APB2Periph_ADC1  | RCC_APB2Periph_ADC2  |
                            RCC_APB2Periph_AFIO, ENABLE );
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM2  | RCC_APB1Periph_TIM3,
                            ENABLE);

    if(frequency_pos)   RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div1);// USBCLK = 48MHz
}
/*******************************************************************************
函数名: NVIC_Config
函数作用:中断初始化
输入参数:tab_offset
返回参数:NULL
*******************************************************************************/
void NVIC_Config(u16 tab_offset)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_SetVectorTable(NVIC_VectTab_FLASH, tab_offset);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel                   = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
函数名: GPIO_Config
函数作用:配置GPIO
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    //------ 2018-1-9 INT1 INT2设为输入模式 ------------------------------------------//
    GPIO_InitStructure.GPIO_Pin  = INT1_PIN;//
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; //GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin  = INT2_PIN;//
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);   //选择按键所在的GPIO管脚用作外部中断线路

    // Configure Button EXTI line
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;  //设置按键所有的外部线路
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;     //设外外部中断模式:EXTI线路为中断请求
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  ;  //外部中断触发沿选择:设置输入线路下降沿为中断请求
    EXTI_InitStructure.EXTI_LineCmd = ENABLE ;
    EXTI_Init(&EXTI_InitStructure);

    //------ PA7作为模拟通道Ai7输入引脚 ------------------------------------------//
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;//改为PA4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //------ OLED_RST_PIN(PB9) ---------------------------------------------------//
    GPIO_InitStructure.GPIO_Pin   = OLED_RST_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //------- 加热控制位 PB4------------------------------------------------------//
    //    GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST,ENABLE); // PB4=JNTRST

    GPIO_InitStructure.GPIO_Pin   = HEAT_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //------ PB0作为模拟通道Ai8输入引脚 ------------------------------------------//
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //---------- INPUT Voltage Detection Pin VB PB1(Ai9) -------------------------//
    GPIO_InitStructure.GPIO_Pin  = VB_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //-------- K1 = PA8, K2 = PA6 ------------------------------------------------//
    GPIO_InitStructure.GPIO_Pin  = KEY1_PIN | KEY2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //-------- PA10,PB3-Test-2017.9.5----------------------------------------------//
    GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//2017.11.6
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//2017.11.6
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//2017.11.6
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//2017.11.6
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //-------- D+-D-SWC-SWD-2017.9.5----------------------------------------------//

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //PA11 D-
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.11

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PA12 D+
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.12

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; //PA13 SWD
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.13

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;//PA14 SWC
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/*******************************************************************************
函数名: Ad_Init
函数作用: 初始化 AD
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Ad_Init(void)
{
    u32 timeout = 10 * 0x1000;
    ADC_InitTypeDef   ADC_InitStructure;
    DMA_InitTypeDef   DMA_InitStructure;

    /* DMA1 channel1 configuration -------------------------------------------*/
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC1ConvertedValue;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 2;
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
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    // ADC1 configuration ----------------------------------------------------//
    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode       = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel       = 2;
    ADC_Init(ADC1, &ADC_InitStructure);

    // ADC2 configuration ----------------------------------------------------//
    ADC_DeInit(ADC2);
    ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode       = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel       = 1;
    ADC_Init(ADC2, &ADC_InitStructure);

    // ADC1,2 regular channel7  channel9 and channel8 configuration ----------//
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 2, ADC_SampleTime_239Cycles5); //28 or 55   2017.8.30通道7改为4
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5); //28 or 55   2017.8.31通道8改为通道3
    ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 1, ADC_SampleTime_55Cycles5); //28 or 55   2017.8.30通道9改为通道2

    /* Enable ADC1、2 DMA */
    ADC_DMACmd(ADC1, ENABLE);

    ADC_Cmd(ADC1, ENABLE);                        /* Enable ADC1 */
    ADC_Cmd(ADC2, ENABLE);                        /* Enable ADC2 */

    ADC_ResetCalibration(ADC1);                   /* Enable ADC1 reset calibaration register */
    while(ADC_GetResetCalibrationStatus(ADC1)) 
        if(!timeout--) return ;   /* Check the end of ADC1 reset calibration register */

    ADC_ResetCalibration(ADC2);                   /* Enable ADC2 reset calibaration register */
    timeout = 10 * 0x1000;
    while(ADC_GetResetCalibrationStatus(ADC2)); //if(!timeout--) return ;   /* Check the end of ADC2 reset calibration register */

    ADC_StartCalibration(ADC2);	                //开启AD校准

    while(ADC_GetCalibrationStatus(ADC2));	//等待校准结束

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    ADC_SoftwareStartConvCmd(ADC2, ENABLE);
}
/*******************************************************************************
函数名: Init_Timer2
函数作用: 初始化 定时器2
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Init_Timer2(void)
{
    NVIC_InitTypeDef         NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    if(frequency_pos)    TIM_TimeBaseStructure.TIM_Prescaler     = 48 - 1;    // (48MHz)/48 = 1MHz
    else                 TIM_TimeBaseStructure.TIM_Prescaler     = 8 - 1;
    TIM_TimeBaseStructure.TIM_Period        = 10000 - 1;  // Interrupt per 10mS
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit    (TIM2, &TIM_TimeBaseStructure);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_ITConfig        (TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd             (TIM2, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
函数名: Init_Timer3
函数作用: 初始化 定时器3
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Init_Timer3(void)
{
    NVIC_InitTypeDef         NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    if(frequency_pos)
    {
        TIM_TimeBaseStructure.TIM_Prescaler     = 48 - 1;    //(48MHz)/48 = 1MHz
        TIM_TimeBaseStructure.TIM_Period        = 50 - 1;    // Interrupt per 50us
    }
    else
    {
        TIM_TimeBaseStructure.TIM_Prescaler     = 8 - 1;
        TIM_TimeBaseStructure.TIM_Period        = 50 - 1;    // Interrupt per 100us
    }
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit    (TIM3, &TIM_TimeBaseStructure);
    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_ITConfig        (TIM3, TIM_IT_Update, ENABLE);
    TIM_Cmd             (TIM3, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
函数名: TIM2_ISR
函数作用: 定时器2中断函数 扫描键盘
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void TIM2_ISR(void)
{
    static u8 invl;
    u8 i;

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);       // Clear interrupt flag
    for(i = 0; i < 8; i++) if(gTime[i] > 0)     gTime[i]--;
    Timer_Counter++;

    if(invl++ >= 2)  //20ms一次
    {
        Key_Read();
        invl = 0;
    }
}
/*******************************************************************************
函数名: TIM3_ISR
函数作用: 定时器3中断函数50us
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void TIM3_ISR(void)
{
    static u8 heat_flag = 0;

    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);       // Clear interrupt flag

    if(gMs_timeout > 0) gMs_timeout--;

    //停止加热后读取ad
    if(MarkAd_pos)      slide_data = Get_SlAvg(Get_AdcValue(0));

    if(gHeat_cnt > 0)
    {
        gHeat_cnt--;

        if(heat_flag)    HEAT_OFF();
        else             HEAT_ON();

        heat_flag = ~heat_flag;
    }
    if(gHeat_cnt == 0)
    {
        HEAT_OFF();
        heat_flag = 0;
    }
}
/*******************************************************************************
函数名: Set_CurLimit
函数作用:ADC看门狗
输入参数:电流上下限
返回参数:NULL
*******************************************************************************/
void Set_CurLimit(u16 high_ma, u16 low_ma)
{
    // Configure high and low analog watchdog thresholds
    ADC_AnalogWatchdogThresholdsConfig(ADC2, high_ma * 55296L / 22605L, low_ma * 55296L / 22605L); //high_ma*20475/7700, low_ma*20475/7700);
    // Configure channel14 as the single analog watchdog guarded channel
    ADC_AnalogWatchdogSingleChannelConfig(ADC2, ADC_Channel_2);
    // Enable analog watchdog on one regular channel
    ADC_AnalogWatchdogCmd(ADC2, ADC_AnalogWatchdog_SingleRegEnable);
    // Enable AWD interupt
    ADC_ITConfig(ADC2, ADC_IT_AWD, ENABLE);
}
/*******************************************************************************
函数名:ADC1_2_IRQHandler
函数作用:ADC1中断
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void ADC1_2_IRQHandler(void)
{
    if(ADC_GetITStatus(ADC2, ADC_IT_AWD))
    {
        if(ADC_GetConversionValue(ADC2) <=  ADC2->LTR)
            Set_HeatingTime(0);
        // Clear ADC2 AWD pending interrupt bit */
        ADC_ClearITPendingBit(ADC2, ADC_IT_AWD);
    }
}
/*******************************************************************************
函数名:GPIO_Vol_Init
函数作用:电压控制脚初始化
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void GPIO_Vol_Init(u8 pos)
{
    if(pos)
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_3);
        GPIO_ResetBits(GPIOA, GPIO_Pin_10);
        GPIO_ResetBits(GPIOA, GPIO_Pin_8);
        GPIO_ResetBits(GPIOA, GPIO_Pin_0);
        Delay_Ms(100);
    }
    else
    {
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//2017.11.6
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//2017.11.6
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_ResetBits(GPIOA, GPIO_Pin_8);
        GPIO_ResetBits(GPIOA, GPIO_Pin_10);
        Delay_Ms(100);
    }
}
/******************************** END OF FILE *********************************/
