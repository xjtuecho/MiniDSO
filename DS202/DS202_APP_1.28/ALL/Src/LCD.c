/********************* (C) COPYRIGHT 2014 e-Design Co.,Ltd. ********************
 LCD.c  
 Version  : DS202 Ver 1.0                                          Author : bure
*******************************************************************************/
#include "Version.h"
#include "Disk.h"
#include "LCD.h"
#include "Bios.h"
#include "Draw.h"

#ifdef LCD_ILI_9341
  #include "LCD_9341_Drv.c"
#endif

#ifdef LCD_ST_7781
  #include "LCD_7781_Drv.c"
#endif

#ifdef OEM_AES
  #include "AES_LOGO.h"
  #define   BG         WHT
#else
  #define   BG         BLK
#endif


/*******************************************************************************
 Clear_Screen: ��LCD��ʾ����Ļ��
*******************************************************************************/
void ClrScrn(u16 Color)
{ u32 i;
  SetPosi(0, 0);
  for(i=0; i<240*LCD_ROW; i++) SetPixel(Color);
}
/*******************************************************************************
 Display_Str: ��ָ��λ����ʾ�ַ���   ����: X��Y���꣬��ɫֵ���ַ���
*******************************************************************************/
u16 Get_TAB_8x14(u8 Code, u16 Row)
{
  if((Code < 0x21)||(Code > 0x7E))  return 0;
  else if(Row < 8) return CHAR8x14[((Code-0X21)*8)+Row];
  else             return 0;
}

u16 Get_TAB_8x9(u8 Code, u16 Row)
{
  if((Code < 0x22)||(Code > 0x7E))  return 0;
  else if(Row < 8) return CHAR8x9[((Code-0X22)*8)+Row];
  else             return 0;
}

void Disp_Str(u16 x, u16 y, u16 Color, u8 *s)
{
  u16 i, j, b;
  SetBlock(x, y, LCD_ROW-1, y+13);
  for (i=0; i<14; i++) SetPixel(BG);    //�ַ���ǰ����һ�հ���
  while(*s != 0) {
    for(i=0; i<8; i++){
      b = Get_TAB_8x14(*s, i);
      for(j=0 ;j<14; ++j){
        if(b & 4) SetPixel(Color);
        else      SetPixel(BG);
        b >>= 1;
      }
    }
    s++;                                //�ַ���ָ��+1
  }
  SetBlock(0, 0, LCD_ROW-1, LCD_COL-1); //�ָ�ȫ�ߴ細��
}
/*******************************************************************************
 Display_6x8: ��ָ��λ����ʾ�ַ���   ����: X��Y���꣬��ɫֵ���ַ���
*******************************************************************************/
u16 GetTAB_6x8(u8 Code, u16 Row)
{
  if((Code < 0x21)||(Code > 0x7E))  return 0;
  else if(Row < 6) return CHAR6x8[((Code-0X21)*6)+Row];
  else             return 0;
}
void DispStr_6x8(u16 x0, u16 y0, u16 Color, u8 *Str)
{
  Sx = x0; Sy = y0;
  __SetPosi(Sx, Sy);
                                   // ÿһ���ַ���ǰ����һ�հ���
  while(*Str != 0){
    DispChar_6x8(Color, *Str++);
  }
}
/*******************************************************************************
 DispChar_6x8: ����Ļ��ʾ�ַ�/ͼ��       Mode = bit0: Normal/Inv, bit1: Char/Map
*******************************************************************************/
void DispChar_6x8(u16 Color, u8 Code)
{
  u16 i, j, k;
  
  for(i=0; i<6; ++i){
    k = GetTAB_6x8(Code, i);
      SetPosi(Sx, Sy);  
      SetPixel(BG);
      SetPosi(Sx, Sy+1);
      for(j=0; j<10; ++j){
        if(k & 1)  SetPixel(Color);
        else       SetPixel(BG);
        k >>= 1;
      }
    Sx++;
  } 
}
/******************************** END OF FILE *********************************/