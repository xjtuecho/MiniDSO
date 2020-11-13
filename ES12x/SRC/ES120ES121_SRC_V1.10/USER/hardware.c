/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      Hardware.c
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
2017/06/20       ����Ӳ���汾�жϹ���;
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
#include "iic.h"

extern u8 version_number;
extern u8 frequency_pos;

u32 gKey_state;
u32 gKey_gap;
vu32 gMs_timeout;
volatile u32 gTimer[7];
vu32 Timer_Counter;//��ʱ��2�жϼ���10ms,��Ϊ�������ʱ����
vu32 Moto_TimerCounter;
vu8 gKey_Press = 0;
u8 keypress_cnt = 0;

#define SI_DATA 25
#define KEY_READ     (GPIOB->IDR & GPIO_Pin_7) //����Ϊ1  1.4�汾
#define KEY_READ_1   (GPIOA->IDR & GPIO_Pin_1) //����Ϊ1  1.3�汾
/*******************************************************************************
������: Init_GTIME
��������:��ʼ����ʱ��
�������:NULL 
���ز���:NULL
*******************************************************************************/
void Init_Gtime(void)
{
    u8 i;
    for(i = 0; i < 6; i++)      gTimer[i] = 0;
    gTimer[2] = SHOTDOWN_TIME;//�ػ�ʱ��
}
/*******************************************************************************
������: USB_Port
��������:���� USB �豸 IO �˿� 
�������:State = ENABLE / DISABLE
���ز���:NULL
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
������: Timer_StartValue
��������:��¼��ʼ��ʱ���ʼֵ,��ȡ��ǰʱ��
�������:timer
���ز���:timer
*******************************************************************************/
u32 Timer_StartValue(void)
{
    return Timer_Counter;
}
/*******************************************************************************
������:   Elapse_Value
��������: �ӿ�ʼʱ�̵���ʱ������ʱ��
�������: start ��ʼʱ�䣬curĿǰʱ�䡣
���ز���: ������ʱ�䡣
*******************************************************************************/
u32 Elapse_Value(u32 start,u32 cur)
{// ���ݳ�ʼֵ��������ʱ�䡣�� ms Ϊ��λ
    if (cur < start) 
    {// ��ʱ���Ѿ��ع�
        return 0xffffffff - start + cur;
    }
    else        return cur - start;
}
/*******************************************************************************
������: GPIO_Config
��������:GPIO��ʼ��
�������:NULL
���ز���:NULL
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
	
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;//OLED Rst
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_2);
		
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

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;//Chrg  ���0 �����1 ����1
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
������: GPIO_Config_1
��������:1.3Ӳ���汾��GPIO��ʼ��
�������:NULL
���ز���:NULL
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

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;//Chrg  ���0 �����1 ����1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_SetBits(GPIOD,GPIO_Pin_0);
    GPIO_ResetBits(GPIOA,GPIO_Pin_10);
}
/*******************************************************************************
 ������: FLASH_Prog
 ��������:д������flash
 �������:NULL
 ���ز���:NULL
 PWM Ƶ�ʼ��㹫ʽ:F = (��ʱ��Ƶ��(48M))/((arr + 1)*(psc + 1)),   TIM_Period = arr;
*******************************************************************************/
void PWM_Init(u16 arr,u16 psc,u8 version)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    if(version)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);
      
        GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);
    }
    else        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
   
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
        PWMģʽ1(TIM_OCMode_PWM1)�� �����ϼ���ʱ��һ��TIMx_CNT<TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ
        ��Ч��ƽ�������¼���ʱ��һ��TIMx_CNT>TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ(OC1REF=0)����
        ��Ϊ��Ч��ƽ(OC1REF=1)��
        PWMģʽ2(TIM_OCMode_PWM2)�� �����ϼ���ʱ��һ��TIMx_CNT<TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ
        ��Ч��ƽ�������¼���ʱ��һ��TIMx_CNT>TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��
        ƽ��
    */
    
    TIM_OCInitStructure.TIM_OCMode              = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState         = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState        = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse               = 0;
    TIM_OCInitStructure.TIM_OCPolarity          = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity         = TIM_OCNPolarity_High;
 
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}
/*******************************************************************************
������: Init_Timer2
��������:��ʼ����ʱ��2 ʱ���ж�
�������:NULL
���ز���:NULL
*******************************************************************************/
void Init_Timer2(void)
{
    NVIC_InitTypeDef         NVIC_InitStructure;//�жϽṹ��
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//��ʱ���ṹ��

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//ʱ��ʹ��(�򿪵�Դ)
    if(frequency_pos)   TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1;    // (48MHz)/48 = 1MHz
    else                TIM_TimeBaseStructure.TIM_Prescaler = 8 - 1;
    TIM_TimeBaseStructure.TIM_Period        = 10000-1;// Interrupt per 10mS �´λ����
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//ʱ��ָ�
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
    TIM_TimeBaseInit    (TIM2, &TIM_TimeBaseStructure);//���ݽṹ���ʼ��TIM2
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_ITConfig        (TIM2, TIM_IT_Update, ENABLE);//ʹ��TIM2���ж�Դ
    TIM_Cmd             (TIM2, ENABLE);//ʹ��TIM(�򿪿���)

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;//ʹ��
    NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
������: Set_gKey
��������:���ð���״̬
�������:Ҫ���õİ���״̬
���ز���:NULL
*******************************************************************************/
void Set_gKey(u32 key)
{
    gKey_state = key;
}
/*******************************************************************************
������: Get_gKey
��������:��ȡ����״̬
�������:NULL
���ز���:����״̬
*******************************************************************************/
u32 Get_gKey(void)
{
    return gKey_state;
}
/*******************************************************************************
������: Get_kGap
��������:��ȡ����ʱ����
�������:NULL
���ز���:����ʱ����
*******************************************************************************/
u32 Get_kGap(void)
{
    return gKey_gap;
}
/*******************************************************************************
������: Set_kGp
��������:���ð���ʱ����
�������:����ʱ����
���ز���:NULL
*******************************************************************************/
void Set_kGap(u32 key_gap)
{
    gKey_gap = key_gap;
}
/*******************************************************************************
������: KeyRead
��������:��ȡ����ֵ
�������:NULL
���ز���:NULL
*******************************************************************************/
void Key_Read( void )
{
    static u16 key_cnt = 0;
    static u16 kgap = 0; //key gap ���ΰ����Ŀ�϶
    static u16 press_key = 0;
    u16 read_data = 0;
    
    if(version_number == 0)     read_data = KEY_READ;  //PB7
    else                        read_data = KEY_READ_1;
    press_key = read_data;//����

    if(version_number)
    {
        if(press_key == 0x0002)
        {
            kgap = 0;
            key_cnt++;
        }
    }
    else
    {
        if(press_key == 0x0080 )
        {
            kgap = 0;
            key_cnt++;
        }
    }
    
    if(key_cnt >= SI_DATA) Set_gKey(KEY_CN | KEY_V1);//����Ϊ����
    if(press_key == 0)
    {
        if(key_cnt < SI_DATA && key_cnt != 0)
        {
            Set_gKey(KEY_V1);//���ð���
            if(kgap < 10)       keypress_cnt++;//10*20ms
        }
        kgap++;
        if(kgap >= 10)          keypress_cnt = 0;//�����������
        key_cnt = 0;
    }
    Set_kGap(kgap*20);//���ð���ʱ����
}
/*******************************************************************************
������: TIM2_IRQHandler
��������:��ʱ��2�жϴ�����
�������:NULL
���ز���:NULL
*******************************************************************************/
void TIM2_IRQHandler(void)
{/*10msһ������*/
    static u8 sk = 0; //scan key
    u8 i;
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	   // �жϱ�־��λ
    if(sk++ == 2) 
    {//20msһ��
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
    
    for(i=0; i<6; i++) 
        if(gTimer[i] > 0)       gTimer[i]--;
    Timer_Counter++;
}
/*******************************************************************************
������: Start_Watchdog
��������:��ʼ�����Ź�
�������:ms ���Ź�����
���ز���:����1
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
������: Clear_Watchdog
��������:���ÿ��Ź�����
�������:NULL
���ز���:����1
*******************************************************************************/
u32 Clear_Watchdog(void)
{
    IWDG_ReloadCounter();
    return 1;
}
/*******************************************************************************
������: version_Init
��������:1.3�汾�����޸�
�������:NULL
���ز���:NULL
*******************************************************************************/
void Version_Modify(void)
{
    RCC_Config();
    Delay_Init();
    GPIO_Config_1();
    NVIC_Configuration();
    Adc_Init();

    if(Get_Adc(VIN) > 500) 
    {//��USB
        USB_Port(DISABLE);
        Delay_Ms(200);
        USB_Port(ENABLE);
        USB_Init();
    }

    Disk_BuffInit();            //U�����ݶ�ȡ
    Config_Analysis();          //��������U��
    Init_L3G4200D();            //��ʼ��L3G4200D
    Init_Timer2();
    PWM_Init(2400,0,1);//20k
    Init_Oled();			//��ʼ��OLED
    Clear_Screen();
    Read_MtCnt();//��ȡ���ת��ʱ��
    Init_Gtime();   //��ʼ��Gtime[]
    Set_gKey(NO_KEY);
    Start_Watchdog(3000);
}
/******************************** END OF FILE *********************************/
