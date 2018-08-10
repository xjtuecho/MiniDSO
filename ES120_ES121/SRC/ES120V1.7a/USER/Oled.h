/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      olde.h
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
2017/10/21       ¥˙¬Î”≈ªØ;    
*******************************************************************************/
#ifndef _OLED_SSD1306_H
#define _OLED_SSD1306_H
#include "stm32f10x.h"

#define OLED_RST_PIN            GPIO_Pin_0     //PB0
#define OLED_RST()              GPIO_ResetBits(GPIOB, OLED_RST_PIN)
#define OLED_ACT()              GPIO_SetBits  (GPIOB, OLED_RST_PIN)

void OLED_Set_Pos(u8 x, u8 y);
u8* Oled_DrawArea(u8 x0,u8 y0,u8 wide, u8 high,u8* ptr);
void Init_Oled(void);
void Clear_Screen(void);
#endif
/******************************** END OF FILE *********************************/
