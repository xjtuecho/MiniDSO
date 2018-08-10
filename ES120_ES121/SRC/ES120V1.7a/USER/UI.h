/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      UI.h
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:   
*******************************************************************************/
#include "stm32f10x.h"


void Clear_Screen(void);
void Display_Str(u8 x,u8 y, char* str);
void Show_BatV(u8 step,u8 Charging);
void Show_Screw(u8 mode);
void Low_Power(void);
void Show_SetLv(u8 gear);
void Clear_Screw(void);
void Show_ScrewMove(u8 status);
void Print_Integer(s32 data,u8 posi);
/******************************** END OF FILE *********************************/
