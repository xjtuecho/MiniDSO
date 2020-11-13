/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      Hardware.h
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
2017/06/20       ����Ӳ���汾�жϹ���;
*******************************************************************************/
#ifndef _HARDWARE_H
#define _HARDWARE_H

#include "STM32F10x.h"

typedef enum WORK_MOD {
    IDLE_MOD = 1,   //����
    CTR_MOD,//����ģʽ
    SET_MOD,    //����ģʽ
    LP_MOD,     //LowPowerģʽ
    STANDBY_MOD,//����
    SHUT_DOWN, //����
} WORK_MOD;

typedef struct {
    u8 ver[16];
    u8 start_angle;//��ʼת���Ƕ�  
    u8 torque_level;//Ť���ȼ�
    u32 moto_timecnt;//���ת��ʱ��
}INFO_SYS;

#define TURBO_ON()  GPIO_SetBits(GPIOA,GPIO_Pin_10);
//#define TURBO_OFF() GPIO_ResetBits(GPIOA,GPIO_Pin_10);

#define POWER_ON()  GPIO_SetBits(GPIOB,GPIO_Pin_4)
#define POWER_OFF() GPIO_ResetBits(GPIOB,GPIO_Pin_4)

#define POWER_ON_1()  GPIO_SetBits(GPIOD,GPIO_Pin_0)
#define POWER_OFF_1() GPIO_ResetBits(GPIOD,GPIO_Pin_0)

#define SHOTDOWN_TIME   1500    //�ػ�ʱ�䵥λ10����

extern volatile u32 gTimer[7];
extern vu8 gKey_Press;
extern u8 keypress_cnt;

void USB_Port(u8 state);
u32 Start_Watchdog(u32 ms);
u32 Clear_Watchdog(void);
void Init_Gtime(void);
void Init_Timer2(void);
void GPIO_Config(void);
void GPIO_Config_1(void);
void Set_gKey(u32 key);
u32 Get_gKey(void);
u32 Get_kGap(void);
void Set_kGap(u32 key_gap);
void Key_Read( void );
void TIM2_IRQHandler(void);
u32 Timer_StartValue(void);
u32 Elapse_Value(u32 start,u32 cur);
void PWM_Init(u16 arr,u16 psc,u8 version);
void Version_Modify(void);
void TURBO_OFF(void);
u8 Is_Turbo_On(void);
#endif
/******************************** END OF FILE *********************************/
