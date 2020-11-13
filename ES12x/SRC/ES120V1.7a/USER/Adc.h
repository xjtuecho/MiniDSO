/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      adc.h
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
*******************************************************************************/
#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"


#define SI_COE         16//56
#define SI_THRESHOLD   60

#define VIN  0
#define VBTY 1
#define VCUR 2
#define ADC1_DR_Address ((u32)0x4001244C)

u16 Get_AdcValue(u8 i);
void Adc_Init(void);
u16 Get_Adc(u8 channel);
u16 Get_AvgAdc(u8 channel);
void Set_CurLimit(u16 high_ma,u16 low_ma);
#endif 
/******************************** END OF FILE *********************************/
