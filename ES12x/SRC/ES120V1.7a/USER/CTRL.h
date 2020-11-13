/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      ctrl.h
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
2017/09/10       ��������ת��ʱ������;
*******************************************************************************/
 #include "stm32f10x.h"
#include "Hardware.h"

#define READ_CHG   (GPIOB->IDR & GPIO_Pin_6)

#define FULL_SPEED_PWM 2400
#define LOW_SPEED_PWM 0
#define MAX_PWM 100
#define MAX_ANGLE       80      //���Ƕȷ�Χ
#define MAX_TORQUE      5       //���λ
#define MAX_ROTATE_I    1200    //����ת����
#define MAX_RUNTIME     12000   //������ת��ʱ��  = /100   s
#define GEARS_UNIT_I    250     //ÿ����ת������

extern INFO_SYS info_def;

u8 Hardware_version(void);
void Motor_Foreward(u16 pwm);
void Motor_Reversal(u16 pwm);
void Motor_Standby(void);
void Motor_Brake(void);
float Motor_Process(void);
void Idle_Process(void);
u8 Get_Mode(void);
void Set_Mode(u8 mode);
void TorqueLv_Set(void);
void Mode_Switching(void);
void Shut_Down(void);
void Stand_By(void);
void Motor(u8 dir,u8 pwm);
/******************************** END OF FILE *********************************/
