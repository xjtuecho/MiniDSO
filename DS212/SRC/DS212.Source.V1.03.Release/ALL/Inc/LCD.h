/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
  Version  : DS212                                                Author : bure
*******************************************************************************/
#ifndef __LCD_H
#define __LCD_H
#define LCD_nRST_LOW()    GPIOD->BRR  = LCD_nRST
#define LCD_nRST_HIGH()   GPIOD->BSRR = LCD_nRST
#define LCD_RS_LOW()      GPIOD->BRR  = LCD_RS
#define LCD_RS_HIGH()     GPIOD->BSRR = LCD_RS
#define LCD_nRD_LOW()     GPIOD->BRR  = LCD_nRD
#define LCD_nRD_HIGH()    GPIOD->BSRR = LCD_nRD
#define LCD_nWR_LOW()     GPIOD->BRR  = LCD_nWR
#define LCD_nWR_HIGH()    GPIOD->BSRR = LCD_nWR
#define LCD_DAT_W(Data)   GPIOE->ODR  = Data;\
                          LCD_nWR_LOW();\
                          LCD_nWR_HIGH()
#define LCD_CMD_W(Cmd)    LCD_RS_LOW();\
                          LCD_DAT_W(Cmd);\
                          LCD_RS_HIGH()

u16  Get_TAB_8x14(u8 Code, u16 Row);
void LCD_Init(void);
void Direction(u8 Direction);
void SetBlock(u16 x1, u16 y1, u16 x2, u16 y2);
void SetPosi(u16 x0, u16 y0);
void SetPixel(u16 Color);
u16  ReadPixel(void);
void ClrScrn(u16 Color);
void Disp_Logo(u16 x0, u16 y0);
void Disp_Str(u16 x, u16 y, u16 Color, u8 *s);
void SendPixels(u16* pBuf, u16 n);
u16  GetTAB_6x8(u8 Code, u16 Row);
void DispStr_6x8(u16 x0, u16 y0, u16 Color, u8 *Str);
void DispChar_6x8(u16 Color, u8 Code);
#endif
/********************************* END OF FILE ********************************/
