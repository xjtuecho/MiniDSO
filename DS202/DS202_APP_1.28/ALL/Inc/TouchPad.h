/********************* (C) COPYRIGHT 2014 e-Design Co.,Ltd. ********************
FileName: TouchPad.h                                               Author : xie
*******************************************************************************/
#ifndef __TOUCH_PAD_H
#define __TOUCH_PAD_H

#include "STM32F30x.h"

#define   TG_HOLD     0x03
#define   TG_CLICK    0x04
#define   TG_DOUBLE   0x05
#define   TG_SLIDE    0x06

extern u16 *TPx, *TPy;
extern u8  *TPs, *TPn, *TGk, *TGn;
extern u16 *TEx, *TEy;

void Gesture_Enable(void);
void CTP_Soft_Reset(void);
void CTP_Hard_Reset(void);
void Read_TP(void);
u16  Touch_Motion(void);
u16  CTP_Upgrade(void);


#endif
/********************************* END OF FILE ********************************/
