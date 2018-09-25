/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
File Name :      Oled.h
Version :        TS200 APP Test 1.0
Description:
Author :         Ning
Data:            2017/11/06
History:
*******************************************************************************/
#ifndef _OLED_SSD1306_H
#define _OLED_SSD1306_H

#define DEVICEADDR_OLED  0x3c



void Scr_Protect(u8 Co);
void Oled_DisplayOn(void);
void Oled_DisplayOff(void);
u8 *Oled_DrawArea(u8 x0, u8 y0, u8 wide, u8 high, u8 *ptr);
void Set_ShowPos(u8 x, u8 y);

u8 *Show_posi(u8 posi, u8 *ptr, u8 word_width);
void Clean_Char(int k, u8 wide);
void Write_Command(u8 Data);
void Write_Data(u8 Data);
void GPIO_Init_OLED(void);
void Init_Oled(void);
void Reg_Command(u8 posi, u8 flag);
void Clear_Screen(void);
void Write_InitCommand_data(u32 Com_len, u8 *data);
void  Display_BG(void);
u8 *Oled_DrawArea_2(u8 x0, u8 y0, u8 wide, u8 high, u8 *ptr, const u8 font_width);
#endif
/******************************** END OF FILE *********************************/
