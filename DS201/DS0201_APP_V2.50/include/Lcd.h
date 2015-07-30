/******************** (C) COPYRIGHT 2009 e-Design Co.,Ltd. *********************
* File Name: lcd.h      Author: bure      Hardware: DS0201V1.1     Version: V1.0
*******************************************************************************/
#ifndef __LCD_H
#define __LCD_H
/*******************************************************************************
Color Schemes：      B     G      R
--------------------------------------------------------------------------------
CH1_CURVE(CYAN)  ： gggx xggg 1xx0 00xx CH1_FLAG =0x0080   ggg :Gray scale
CH2_CURVE(YEL)   ： 000x xrrr xxxr rr1x CH2_FLAG =0x0002   rrr :Gray scale
CH3_CURVE(PURPLE)： bbb1 x000 xxxb bbx0 CH3_FLAG =0x1000   bbb :Gray scale
TRG_CURVE(BLUE)  ： 111x x100 xxx1 00x1 TRG_FLAG =0x0001   TRG_COLOR:0xE411
LINE_#1(GREEN)   ： 100x x111 xx11 00xx LN1_FLAG =0x0020   LN1_COLOR:0x8730
LINE_#2(WHITE)   ： 100x 1100 xxx1 00xx LN2_FLAG =0x0800   LN2_COLOR:0x8C10
LINE_#2(WHITE)   ： 110x 1110 xxx1 10xx LN2_FLAG =0x0800   LN2_COLOR:0xCE18
GRID(DEEP_GRAY)  ： 010x x010 x1x0 10xx GRD_FLAG =0x0040   GRD_COLOR:0x4248
BACKGROUND(BLACK)： 0000 0000 0000 0000 BLK_FLAG =0x0000   BLK_COLOR:0x0000
*******************************************************************************/
#define C_GROUP     0x1082  // 0001 0000 1000 0010
#define T_GROUP     0x0821  // 0000 1000 0010 0001
#define F_SELEC     0x18E3  // 0001 1000 1110 0011
#define LN1_FLAG    0x0020
#define LN2_FLAG    0x0800
#define GRD_FLAG    0x0040
#define LN1_COLOR   0x8730
#define LN2_COLOR   0xCE18
#define GRD_COLOR   0x4248
#define TRG_COLOR   0xE411
#define CH1_COLOR   0xE780  // 1110 0111 1000 0000
#define CH2_COLOR   0x071E  // 0000 0111 0001 1110
#define CH3_COLOR   0xF01C  // 1111 0000 0001 1100

//============================== 显示颜色定义 ==================================
#define YEL                     0x07FF	//黄色：B = 0000, G = 07E0, R = 001F
#define WHITE                   0xFFFF	//白色：B = F800, G = 07E0, R = 001F
#define RED                     0x001F	//红色：B = 0000, G = 0000, R = 001F
#define GRN                     0x07E0	//绿色：B = 0000, G = 07E0, R = 0000
#define BLACK                   0x0000	//黑色：B = 0000, G = 0000, R = 0000
#define BACKGROUND              0x0000	//黑色：B = 0000, G = 0000, R = 0000

//================================= 常数定义 ===================================

#define PRN		0
#define INV		1
#define ADD		0
#define ERASE		1

extern unsigned const int Logo_Dot[512];

//========================== LCD模块显示相关函数声明 ===========================
void LCD_SET_WINDOW(short x1, short x2,short y1, short y2);
void Clear_Screen(unsigned short Color);
void Display_Frame(void);
void Display_Grid(void);

void Display_Str(short x0, short y0, short Color, char Mode, unsigned const char *s);
void Display_Logo(short x0, short y0);
void Erase_Wave(void);
void Erase_SEG(unsigned short i, unsigned short y1, unsigned short y2, unsigned short Color);
void Draw_View_Area(void);
void Erase_View_Area(void);

void Draw_Vn(unsigned short Vn, char Mode, unsigned short Color);
void Draw_Vi(unsigned short Vi, char Mode, unsigned short Color);
void Draw_Ti(unsigned short Ti, char Mode, unsigned short Color);
void Draw_Dot_Ti(unsigned short Ti, char Mode, unsigned short Color);
void Draw_Dot_Vn(unsigned short Vn, char Mode, unsigned short Color);

void Draw_CH1_SEG(unsigned short x, unsigned short y1, unsigned short y2);
void Draw_CH3_SEG(unsigned short x, unsigned short y1, unsigned short y2);

#endif

/********************************* END OF FILE ********************************/
