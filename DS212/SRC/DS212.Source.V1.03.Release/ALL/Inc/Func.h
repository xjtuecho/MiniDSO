/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
 File Name : Function.h
 Version   : DS212                                        Author : bure & Kewei
*******************************************************************************/
#ifndef __Function_H__
#define __Function_H__

#define SIGN         0
#define UNSIGN       1
#define STD          2
 
extern u32 TestCnt;

void Rev32(u32* pBuf);
void Rev16(u16* pBuf);
u32  Power(u8 x, u8 y);
u32  Exp(u8 x);
u8*  SeekStr(u8* ptr, u8 Idx);
void Delayms(u16 mS);
void Int2Str(u8 *p, s32 n, uc8 *pUnit, u8 e, u8 Mode);
void Value2Str(u8 *p, s32 n, uc8 *pUnit, s8 e, u8 Mode);
u8   Str2Byte(u8 x,u8 y);
void u16ToDec4Str(u8 *p, u16 n);
void u16ToDec5Str(u8 *p, u16 n);
void s16ToDec5Str(u8 *p, s16 n);
void u8ToDec3Str(u8 *p, u8 n);
void u8ToDec2Str(u8 *p, u8 n);
void Char2Hex(u8 *p, u8 n);
void Half2Hex(u8 *p, u16 n);
void Word2Hex(u8 *p, u32 n);
u16  Sqrt32(u32 n);
void s8ToPercen(u8 *p, s8 n);

#endif
/********************************* END OF FILE ********************************/
