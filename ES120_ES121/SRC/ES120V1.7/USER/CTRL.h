/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      Ctrl.h
Version :
Description:
Author :         Celery
Data:            2016/12/22
History:

*******************************************************************************/
 #include "stm32f10x.h"
#include "Hardware.h"

#define FULL_SPEED_PWM 2400
#define LOW_SPEED_PWM 0
#define MAX_PWM 100
#define MAX_ANGLE       80 //最大角度范围
#define MAX_TORQUE      5  //最大挡位

#define MAX_RUNTIME     6000   //最大持续转动时间  /100   s

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
