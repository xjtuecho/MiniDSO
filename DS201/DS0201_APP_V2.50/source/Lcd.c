/******************** (C) COPYRIGHT 2009 e-Design Co.,Ltd. *********************
* File Name: lcd.c      
     Author: bure      
   Hardware: DS0201V1.1~1.6
    Version: V1.0
*******************************************************************************/
#include "Function.h"
#include "lcd.h"
#include "Interrupt.h"
#include "HW_V1_Config.h"
#include "ASM_Funtion.h"

/*******************************************************************************
 LCD_SET_WINDOW: 设置LCD模块上指定的窗口位置。 输入: x0、x1、y0、y1
*******************************************************************************/
void LCD_SET_WINDOW(short x1, short x2,short y1, short y2) 
{
  __LCD_WR_REG(0x0050, y1);
  __LCD_WR_REG(0x0051, y2);
  __LCD_WR_REG(0x0052, x1);
  __LCD_WR_REG(0x0053, x2);
  
  __LCD_WR_REG(0x0020, y1);
  __LCD_WR_REG(0x0021, x1);
  
  LDC_DATA_OUT=0x0022;      //Reg. Addr.
  LCD_RS_LOW();             //RS=0,Piont to Index Reg.
  LCD_nWR_ACT();            //WR Cycle from 1 -> 0 -> 1
  LCD_nWR_ACT();            //WR Cycle from 1 -> 0 -> 1
  LCD_RS_HIGH();            //RS=1,Piont to object Reg.
}
/*******************************************************************************
 Clear_Screen: 清LCD显示器屏幕。 
*******************************************************************************/
void Clear_Screen(unsigned short Color)						
{ 
  unsigned int i; 
  __Point_SCR(0, 0);    //X_pos=0，Y_pos=0
  for(i=0;i<240*320;++i) __Set_Pixel(Color);
}
/*******************************************************************************
 Display_Frame: 画参数栏框架。 
*******************************************************************************/
void Display_Frame(void)						
{ 
  unsigned short i,j;
  for(i=0; i<320; ++i) {         //画上背景框
    __Point_SCR(i,0); 
    for(j=0; j<MIN_Y-1; ++j) __Set_Pixel(LN2_COLOR);
  }
  for(i=0; i<320; ++i) {         //画下背景框
    __Point_SCR(i,MAX_Y+3); 
    for(j=MAX_Y+3; j<240; ++j) __Set_Pixel(LN2_COLOR); 
  }
}
/*******************************************************************************
 Display_Grid: 画屏幕背景及坐标网格。 
*******************************************************************************/
void Display_Grid(void)						
{ 
  unsigned short i,j;
  for(j=MIN_Y; j<=MAX_Y; j+=25){  
    for(i=MIN_X; i<MAX_X+1; i+=5){
      __Point_SCR(i,j); 
      __Set_Pixel(GRD_COLOR);        //画水平格线
    }
  }  
  for(i=MIN_X;i<=MAX_X; i+=25){
    for(j=MIN_Y; j<=MAX_Y; j+=5){
      __Point_SCR(i,j);
      __Set_Pixel(GRD_COLOR);        //画垂直格线
    }
  }
}
/*******************************************************************************
 Draw_SEG: 叠加一个波形片段
*******************************************************************************/
void Draw_CH1_SEG(unsigned short x, unsigned short y1, unsigned short y2)						
{ 
  unsigned short j, Tmp;
  Tmp=y2;
  if(y1>y2) {
    y2=y1;
    y1=Tmp;
  }
  if(y2>=MAX_Y) y2=MAX_Y-1;
  if(y1<=MIN_Y) y1=MIN_Y+1;
  /*  switch ((y2-y1)) {                                //(荧光色定义）
    case 0:  Tmp =0xE780; break;  // 1110 0111 1000 0000
    case 1:  Tmp =0xC680; break;  // 1100 0110 1000 0000
    case 2:  Tmp =0xA580; break;  // 1010 0101 1000 0000
    case 3:  Tmp =0x8480; break;  // 1000 0100 1000 0000
    case 4:  Tmp =0x6380; break;  // 0110 0011 1000 0000
    case 5:  Tmp =0x4280; break;  // 0100 0010 1000 0000
    default: Tmp =0x2180; break;  // 0010 0001 1000 0000
  }
  */
  Tmp =0xE780;

  for(j=y1; j<=y2; ++j) __Add_Color(x+MIN_X, j, Tmp);
}
void Draw_CH3_SEG(unsigned short x, unsigned short y1, unsigned short y2)						
{ 
  unsigned short j, Tmp;
  Tmp=y2;
  if(y1>y2) {
    y2=y1;
    y1=Tmp;
  }
  if(y2>=MAX_Y) y2=MAX_Y-1;
  if(y1<=MIN_Y) y1=MIN_Y+1;
  /*                       
  switch ((y2-y1)) {                                    //(荧光色定义）
    case 0:  Tmp =0xF01C; break;  // 1111 0000 0001 1100
    case 1:  Tmp =0xD018; break;  // 1101 0000 0001 1000
    case 2:  Tmp =0xB014; break;  // 1011 0000 0001 0100
    case 3:  Tmp =0x9010; break;  // 1001 0000 0001 0000
    case 4:  Tmp =0x700C; break;  // 0111 0000 0000 1100
    case 5:  Tmp =0x5008; break;  // 0101 0000 0000 1000
    default: Tmp =0x3004; break;  // 0011 0000 0000 0100
  }
  */
  Tmp =0xF01C;
  for(j=y1; j<=y2; ++j) __Add_Color(x+MIN_X, j, Tmp);
}

/*******************************************************************************
 Erase_SEG: 擦除一个波形片段
*******************************************************************************/
void Erase_SEG(unsigned short i, unsigned short y1, unsigned short y2, unsigned short Color)						
{ 
  unsigned short j, Tmp;  
  Tmp=y2;
  if(y1>y2) { 
    y2=y1;
    y1=Tmp;
  }
  if(y2>=MAX_Y) y2=MAX_Y-1;
  if(y1<=MIN_Y) y1=MIN_Y+1;
  for(j=y1; j<=y2; ++j) __Erase_Color(i+MIN_X, j, Color);  //擦除旧波形曲线
}
/*******************************************************************************
 Erase_Wave: 擦除旧波形曲线 
*******************************************************************************/
void Erase_Wave(void)
{ 
  unsigned short i;
  unsigned char y1, y2, y3, y4;

  y1=View_Buffer[0];
  y3= Ref_Buffer[0]; 

  for(i=0; i<X_SIZE; ++i){
    y2=View_Buffer[i];
    y4= Ref_Buffer[i];
    Erase_SEG(i, y1, y2, CH1_COLOR);    
    Erase_SEG(i, y3, y4, CH3_COLOR);    
    y1 = y2;
    y3 = y4;
  }
}
/*******************************************************************************
 Draw_Vn/Vi/Ti: 叠加或擦除：基准电平、触发电平基线 与 垂直、水平测量游标线
*******************************************************************************/
void Draw_Vn(unsigned short Vn, char Mode, unsigned short Color)
{ 
  unsigned short i;
  for(i=MIN_X+2; i<MAX_X; i+=2){ 
    if(Mode==ADD) __Add_Color(i, Vn, Color);
    else        __Erase_Color(i, Vn, Color);
  }
}
void Draw_Vi(unsigned short Vi, char Mode, unsigned short Color)
{ 
  unsigned short i;
  for(i=MIN_X+5; i<MAX_X; i+=5){ 
    if(Mode==ADD) __Add_Color(i, Vi, Color);
    else        __Erase_Color(i, Vi, Color);
  }
}
void Draw_Ti(unsigned short Ti, char Mode, unsigned short Color)
{ 
  unsigned short j;
  for(j=MIN_Y+3; j<MAX_Y; j+=3){
    if(Mode==ADD) __Add_Color(Ti, j, Color);
    else        __Erase_Color(Ti, j, Color);
  }
}
/*******************************************************************************
 Draw_Dot_Vn/Ti:  叠加或擦除：基准、触发电平基线端点 与 垂直、水平游标线端点
*******************************************************************************/
void Draw_Dot_Vn(unsigned short Vn, char Mode, unsigned short Color)
{ 
  if(Mode==ADD) {

    __Add_Color(MIN_X-1, Vn-2, Color);
    __Add_Color(MIN_X-1, Vn-1, Color);
    __Add_Color(MIN_X-1, Vn,   Color);
    __Add_Color(MIN_X-1, Vn+1, Color);
    __Add_Color(MIN_X-1, Vn+2, Color);

    __Add_Color(MIN_X,   Vn-1, Color);
    __Add_Color(MIN_X,   Vn,   Color);
    __Add_Color(MIN_X,   Vn+1, Color);

    __Add_Color(MIN_X+1, Vn,   Color);

    __Add_Color(MAX_X+1, Vn-2, Color);
    __Add_Color(MAX_X+1, Vn-1, Color);
    __Add_Color(MAX_X+1, Vn,   Color);
    __Add_Color(MAX_X+1, Vn+1, Color);
    __Add_Color(MAX_X+1, Vn+2, Color);

    __Add_Color(MAX_X,   Vn-1, Color);
    __Add_Color(MAX_X,   Vn,   Color);
    __Add_Color(MAX_X,   Vn+1, Color);

    __Add_Color(MAX_X-1, Vn,   Color);

  } else {

    __Erase_Color(MIN_X-1, Vn-2, Color);
    __Erase_Color(MIN_X-1, Vn-1, Color);
    __Erase_Color(MIN_X-1, Vn,   Color);
    __Erase_Color(MIN_X-1, Vn+1, Color);
    __Erase_Color(MIN_X-1, Vn+2, Color);

    __Erase_Color(MIN_X,   Vn-1, Color);
    __Erase_Color(MIN_X,   Vn,   Color);
    __Erase_Color(MIN_X,   Vn+1, Color);

    __Erase_Color(MIN_X+1, Vn,   Color);

    __Erase_Color(MAX_X+1, Vn-2, Color);
    __Erase_Color(MAX_X+1, Vn-1, Color);
    __Erase_Color(MAX_X+1, Vn,   Color);
    __Erase_Color(MAX_X+1, Vn+1, Color);
    __Erase_Color(MAX_X+1, Vn+2, Color);

    __Erase_Color(MAX_X,   Vn-1, Color);
    __Erase_Color(MAX_X,   Vn,   Color);
    __Erase_Color(MAX_X,   Vn+1, Color);

    __Erase_Color(MAX_X-1, Vn,   Color);

    __Erase_Color(MIN_X-1, Vn-2, Color);
    __Erase_Color(MIN_X-1, Vn-1, Color);
    __Erase_Color(MIN_X-1, Vn,   Color);
    __Erase_Color(MIN_X-1, Vn+1, Color);
    __Erase_Color(MIN_X-1, Vn+2, Color);
  }
}
void Draw_Dot_Ti(unsigned short Ti, char Mode, unsigned short Color)
{ 
  if(Mode==ADD) {

    __Add_Color(Ti-2, MIN_Y-1, Color);
    __Add_Color(Ti-1, MIN_Y-1, Color);
    __Add_Color(Ti,   MIN_Y-1, Color);
    __Add_Color(Ti+1, MIN_Y-1, Color);
    __Add_Color(Ti+2, MIN_Y-1, Color);

    __Add_Color(Ti-1, MIN_Y,   Color);
    __Add_Color(Ti,   MIN_Y,   Color);
    __Add_Color(Ti+1, MIN_Y,   Color);

    __Add_Color(Ti,   MIN_Y+1, Color);

    __Add_Color(Ti-2, MAX_Y+1, Color);
    __Add_Color(Ti-1, MAX_Y+1, Color);
    __Add_Color(Ti,   MAX_Y+1, Color);
    __Add_Color(Ti+1, MAX_Y+1, Color);
    __Add_Color(Ti+2, MAX_Y+1, Color);

    __Add_Color(Ti-1, MAX_Y,   Color);
    __Add_Color(Ti,   MAX_Y,   Color);
    __Add_Color(Ti+1, MAX_Y,   Color);

    __Add_Color(Ti,   MAX_Y-1, Color);

  } else {

    __Erase_Color(Ti-2, MIN_Y-1, Color);
    __Erase_Color(Ti-1, MIN_Y-1, Color);
    __Erase_Color(Ti,   MIN_Y-1, Color);
    __Erase_Color(Ti+1, MIN_Y-1, Color);
    __Erase_Color(Ti+2, MIN_Y-1, Color);

    __Erase_Color(Ti-1, MIN_Y,   Color);
    __Erase_Color(Ti,   MIN_Y,   Color);
    __Erase_Color(Ti+1, MIN_Y,   Color);

    __Erase_Color(Ti,   MIN_Y+1, Color);

    __Erase_Color(Ti-2, MAX_Y+1, Color);
    __Erase_Color(Ti-1, MAX_Y+1, Color);
    __Erase_Color(Ti,   MAX_Y+1, Color);
    __Erase_Color(Ti+1, MAX_Y+1, Color);
    __Erase_Color(Ti+2, MAX_Y+1, Color);

    __Erase_Color(Ti-1, MAX_Y,   Color);
    __Erase_Color(Ti,   MAX_Y,   Color);
    __Erase_Color(Ti+1, MAX_Y,   Color);

    __Erase_Color(Ti,   MAX_Y-1, Color);

    __Erase_Color(Ti-2, MIN_Y-1, Color);
    __Erase_Color(Ti-1, MIN_Y-1, Color);
    __Erase_Color(Ti,   MIN_Y-1, Color);
    __Erase_Color(Ti+1, MIN_Y-1, Color);
    __Erase_Color(Ti+2, MIN_Y-1, Color);
  }
}
/*******************************************************************************
 Erase_View_Area: 擦除显示域位置指示 
*******************************************************************************/
void Erase_View_Area(void)
{  
  unsigned short i;
  for(i=MIN_X+2; i<MAX_X-1; ++i){  
    __Erase_Color(i, MIN_Y+3, LN2_COLOR);
    __Erase_Color(i, MIN_Y+4, LN2_COLOR);
    __Erase_Color(i, MIN_Y+2, CH2_COLOR);
    __Erase_Color(i, MIN_Y+3, CH2_COLOR);
    __Erase_Color(i, MIN_Y+4, CH2_COLOR);
    __Erase_Color(i, MIN_Y+5, CH2_COLOR);
  }
}
/*******************************************************************************
 Draw_View_Area: 画显示域位置指示 
*******************************************************************************/
void Draw_View_Area(void)
{  
  unsigned short i, j;
  for(i=MIN_X+2; i<MAX_X-1; ++i){  
    __Add_Color(i, MIN_Y+3, LN2_COLOR);;
    __Add_Color(i, MIN_Y+4, LN2_COLOR);
  }
  j=MIN_X+((((Item_Index[X_POSITION]+150-4096))*X_SIZE)>>12);

  for(i=MIN_X+2; i<MAX_X-1; ++i){  
    if((i>=j)&&(i<=j+22)){
      __Add_Color(i, MIN_Y+2, CH2_COLOR);
      __Add_Color(i, MIN_Y+3, CH2_COLOR);
      __Add_Color(i, MIN_Y+4, CH2_COLOR);
      __Add_Color(i, MIN_Y+5, CH2_COLOR);
    } else {
      __Erase_Color(i, MIN_Y+2, CH2_COLOR);
      __Erase_Color(i, MIN_Y+3, CH2_COLOR);
      __Erase_Color(i, MIN_Y+4, CH2_COLOR);
      __Erase_Color(i, MIN_Y+5, CH2_COLOR);
    }
  }
}
/*******************************************************************************
 Display_Str: 在指定位置显示字符串   输入: X、Y坐标，颜色值，显示模式，字符串 
*******************************************************************************/
void Display_Str(short x0, short y0, short Color, char Mode, unsigned const char *s)
{ 
  short i, j, k, b; 
  LCD_SET_WINDOW(x0,LCD_X2,y0,y0+13);
  for (j=0; j<14;++j){ 
    if(Mode==0) __Set_Pixel(BLACK);   //Normal replace Display
    if(Mode==1) __Set_Pixel(Color);   //Inverse replace Display
  }
  while (*s!=0) {
    b=0x0000;
    if(*s==0x21) k=4;
    else k=8;
    for(i=0;i<k;++i){
      if((*s>0x21)) b=__Get_TAB_8x14(*s, i);
      for(j=0;j<14;++j){
        if(b&4) {
          if(Mode==0) __Set_Pixel(Color);
          if(Mode==1) __Set_Pixel(BLACK);
        } else {
          if(Mode==0) __Set_Pixel(BLACK);
          if(Mode==1) __Set_Pixel(Color);
        }
        b>>=1;
      }
    }
    ++s;                          //字符串指针+1
  }
  LCD_SET_WINDOW(LCD_X1,LCD_X2,LCD_Y1,LCD_Y2);  //恢复全尺寸窗口
}
/******************************** END OF FILE *********************************/
