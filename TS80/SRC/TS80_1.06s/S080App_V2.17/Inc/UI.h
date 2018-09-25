/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
File Name :      UI.h
Version :        TS200 APP Test 1.0
Description:
Author :         Ning
Data:            2017/11/06
History:
*******************************************************************************/
#ifndef _UI_H
#define _UI_H

#include "STM32F10x.h"

#define HIGHLIGHT_FREQUENCY     60
#define HIGHLTGHT_REDUCE        20
#define SET_PROMPT_X            8
#define SET_INFO_X              7
#define EFFECTIVE_KEY           800
#define TEMP_UNIT_C		(0)
#define TEMP_UNIT_F		(1)

void APP_Init(void);
void Show_Volt(u8 style);
void Display_Temp(u8 x, s16 Temp);
void Show_Notice(void);
void Show_Warning(void);
void Show_TempDown(s16 Temp, s16 Dst_Temp);
s16 Temp_conversion(u8 flag , s16 Tmp);
void Show_Ver(u8 ver[], u8 flag);
void Show_OrderChar(u8 *ptr, u8 num, u8 width);
void Show_HeatingIcon(u32 ht_flag, u16 active);
void Display_Str(u8 x, const char *str);
void Display_Str8(u8 x, const char *str , const u8 mode);
void Display_Str6(u8 x, const char *str);
void Display_Str10(u8 x, const char *str);
void Triangle_Str(u8 x, u8 pos);
void Setup_Proc(void);
void TempSet_Proc(void);
void Show_NoElectriciron(void);
#endif
/******************************** END OF FILE *********************************/
