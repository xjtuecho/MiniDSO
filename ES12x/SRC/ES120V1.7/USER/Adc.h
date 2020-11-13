#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

#define SI_COE 16//56
#define SI_THRESHOLD   60

#define VIN 0
#define VBTY 2
#define VCUR 3

void Adc_Init(void);
u16 Get_Adc(u8 channel);
 u16 Get_AvgAdc(u8 channel);
#endif 
/******************************** END OF FILE *********************************/
