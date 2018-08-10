/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      Hardware.c
Version :
Description:
Author :         Celery
Data:            2016/12/22
History:

*******************************************************************************/
#include "STM32F10x.h"
#include "app.h"
#include "Hardware.h"
#include "Oled.h"
#include "stdio.h"
#include "delay.h"
#include "Disk.h"
#include "Adc.h"
#include "L3G4200D.h"



u32 gKey_state;
u32 gKey_gap;
vu32 gMs_timeout;
volatile u32 gTimer[6];
vu32 Timer_Counter;
vu32 Moto_TimerCounter;
vu8 gKey_Press = 0;

extern u8 Version_jud;

#define KEY_READ     (GPIOB->IDR & GPIO_Pin_7) //按下为1  1.4版本
#define KEY_READ_1   (GPIOA->IDR & GPIO_Pin_1) //按下为1  1.3版本


/*******************************************************************************
函数名: Init_GTIME
函数作用:初始化计时器
输入参数:NULL 
返回参数:NULL
*******************************************************************************/
void Init_Gtime(void)
{
    u8 i;
    
    for(i = 0; i < 6; i++)   gTimer[i] = 0;
    gTimer[2] = SHOTDOWN_TIME;//关机时间
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
    if(state == DISABLE) {
        USB_DN_OUT();
        USB_DP_OUT();
    } else {
        USB_DN_EN();
        USB_DP_EN();
    }
}
/*******************************************************************************
函数名: Timer_StartValue
函数作用:记录开始的时间始值
输入参数:timer
返回参数:timer
*******************************************************************************/
u32 Timer_StartValue(void)
{
    // 返回初始值
    return Timer_Counter;
}
/*******************************************************************************
函数名:   Elapse_Value
函数作用: 从开始时刻到此时经过的时间
输入参数: start 开始时间，cur目前时间。
返回参数: 经过的时间。
*******************************************************************************/
u32 Elapse_Value(u32 start,u32 cur)
{
    // 根据初始值计算流逝时间。以 ms 为单位
    if (cur < start) {
        // 计时器已经回滚
        return 0xffffffff - start + cur;
    } else {
        return cur - start;
    }
}

/*******************************************************************************
函数名: GPIO_Config
函数作用:GPIO初始化
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_PD01 , ENABLE);
    
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;//OLED Rst
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;  //VE
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_9;//InA,InB
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;  //Pump
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;//VK
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;//Chrg  充电0 不充电1 充满1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
        
    GPIO_InitStructure.GPIO_Pin = SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SCL;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);    

    POWER_ON();
    TURBO_OFF();
}
/*******************************************************************************
函数名: GPIO_Config_1
函数作用:1.3硬件版本的GPIO初始化
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void GPIO_Config_1(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_PD01 , ENABLE);
    
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;//ncr
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;  //ldo_on
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_9;  //InA,InB
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;  //Pump
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//k1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;//Chrg  充电0 不充电1 充满1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_SetBits(GPIOD,GPIO_Pin_0);
    GPIO_ResetBits(GPIOA,GPIO_Pin_10);
}


/*******************************************************************************
 函数名: FLASH_Prog
 函数作用:写东西到flash
 输入参数:NULL
 返回参数:NULL

 PWM 频率计算公式:F = (定时器频率(48M))/((arr + 1)*(psc + 1))     ..TIM_Period = arr;
*******************************************************************************/
void PWM_Init(u16 arr,u16 psc,u8 version)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    if(version)
    {
        RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);
      
        GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);
    }
    else
    {
        RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    }    
   
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9; //PA8 TIM1_CH1,PA9 TIM1_CH2
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period        = arr - 1;
    TIM_TimeBaseStructure.TIM_Prescaler     = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//TIM_CKD_DIV1
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    /*
        PWM模式1(TIM_OCMode_PWM1)－ 在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为有效电平，否则为
        无效电平；在向下计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为无效电平(OC1REF=0)，否
        则为有效电平(OC1REF=1)。
        PWM模式2(TIM_OCMode_PWM2)－ 在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为
        有效电平；在向下计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为有效电平，否则为无效电
        平。
    */
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; 
    TIM_OCInitStructure.TIM_Pulse       = 0;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High; 
 
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
  
}
/*******************************************************************************
函数名: Init_Timer2
函数作用:初始化定时器2 时间中断
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Init_Timer2(void)
{
    NVIC_InitTypeDef         NVIC_InitStructure;//中断结构体
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//定时器结构体

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2  , ENABLE);//时钟使能(打开电源)
    TIM_TimeBaseStructure.TIM_Prescaler     = 48-1;// (48MHz)/48 = 1MHz   预分频值
    TIM_TimeBaseStructure.TIM_Period        = 10000-1;// Interrupt per 10mS 下次活动周期
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//时间分割
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;//TIM向上计数模式
    TIM_TimeBaseInit    (TIM2, &TIM_TimeBaseStructure);//根据结构体初始化TIM2
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_ITConfig        (TIM2, TIM_IT_Update, ENABLE);//使能TIM2，中断源
    TIM_Cmd             (TIM2, ENABLE);//使能TIM(打开开关)

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;//
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;//使能
    NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
函数名: Set_gKey
函数作用:设置按键状态
输入参数:要设置的按键状态
返回参数:NULL
*******************************************************************************/
void Set_gKey(u32 key)
{
    gKey_state = key;
}
/*******************************************************************************
函数名: Get_gKey
函数作用:获取按键状态
输入参数:NULL
返回参数:按键状态
*******************************************************************************/
u32 Get_gKey(void)
{
    return gKey_state;
}
/*******************************************************************************
函数名: Get_kGap
函数作用:获取按键时间间隔
输入参数:NULL
返回参数:按键时间间隔
*******************************************************************************/
u32 Get_kGap(void)
{
    return gKey_gap;
}
/*******************************************************************************
函数名: Set_kGp
函数作用:设置按键时间间隔
输入参数:按键时间间隔
返回参数:NULL
*******************************************************************************/
void Set_kGap(u32 key_gap)
{
    gKey_gap = key_gap;
}
/*******************************************************************************
函数名: KeyRead
函数作用:读取按键值
输入参数:NULL
返回参数:NULL
*******************************************************************************/
#define SI_DATA 25
extern u8 version_number;

u8 keypress_cnt = 0;

void Key_Read( void )
{
    static u16 key_cnt = 0;
    static u16 kgap = 0; //key gap 两次按键的空隙
    static u16 press_key = 0;
    u16 read_data = 0;
    
    if(version_number == 0 )        read_data = KEY_READ;  //PB7
    else                            read_data = KEY_READ_1;
    press_key = read_data;//长按

    if(version_number)
    {
        if(press_key == 0x0002){
        kgap = 0;
        key_cnt++;
        }
    }
    else
    {
        if(press_key == 0x0080 ){
            kgap = 0;
            key_cnt++;
        }
    }
    if(key_cnt >= SI_DATA) Set_gKey(KEY_CN | KEY_V1);//设置为长按
    
    if(press_key == 0){

        if(key_cnt < SI_DATA && key_cnt != 0){
            Set_gKey(KEY_V1);//设置案件
            if(kgap < 10)  keypress_cnt++;//10*20ms
        }

        kgap++;
        if(kgap >= 10){
            keypress_cnt = 0;//清除案件计数
        }
        key_cnt = 0;
    }
    Set_kGap(kgap*20);//设置按键时间间隔
}
/*******************************************************************************
函数名: TIM2_IRQHandler
函数作用:定时器2中断处理函数
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void TIM2_IRQHandler(void)
{
    static u8 sk = 0; //scan key
    u8 i;
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	   // 中断标志复位
    if(sk++ == 2) {//20ms一次
        Key_Read();
        sk = 0;
    }
    if(version_number == 0)
    {
        if(KEY_READ)    gKey_Press = 1;
        else            gKey_Press = 0;
    }
    else
    {
        if(KEY_READ_1)  gKey_Press = 1;
        else            gKey_Press = 0;
    }

    for(i=0; i<6; i++) if(gTimer[i] > 0) gTimer[i]--;
    Timer_Counter++;
}
/*******************************************************************************
函数名: Start_Watchdog
函数作用:初始化开门狗
输入参数:ms 开门狗计数
返回参数:返回1
*******************************************************************************/
u32 Start_Watchdog(u32 ms)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /*IWDG counter clock: 40KHz(LSI)/ 32 = 1.25 KHz(min:0.8ms -- max:3276.8ms */
    IWDG_SetPrescaler(IWDG_Prescaler_32);

    /* Set counter reload value to XXms */
    IWDG_SetReload(ms*10/8);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
    return 1;
}
/*******************************************************************************
函数名: Clear_Watchdog
函数作用:重置开门狗计数
输入参数:NULL
返回参数:返回1
*******************************************************************************/
u32 Clear_Watchdog(void)
{
    IWDG_ReloadCounter();
    return 1;
}

/*******************************************************************************
函数名: version_Init
函数作用:1.3版本配置修改
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Version_Modify(void)
{
    RCC_Config();
    Delay_Init();
    GPIO_Config_1();
    NVIC_Configuration();
    Adc_Init();

    if(Get_Adc(VIN) > 500) {//有USB
        USB_Port(DISABLE);
        Delay_Ms(200);
        USB_Port(ENABLE);
        USB_Init();
    }

    Disk_BuffInit();            //U盘内容读取
    Config_Analysis();          //启动虚拟U盘
    Init_L3G4200D();            //初始化L3G4200D
    Init_Timer2();
    PWM_Init(2400,0,1);//20k
    Init_Oled();			//初始化OLED
    Clear_Screen();
    Read_MtCnt();//读取电机转动时间
    
    Init_Gtime();   //初始化Gtime[]
    Set_gKey(NO_KEY);
    Start_Watchdog(3000);
}
/******************************** END OF FILE *********************************/
