/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
 File Name : file.h  
 Version   : DS212                                                Author : bure
*******************************************************************************/
#ifndef __FILE_H
#define __FILE_H

#include "STM32F30x.h"

extern vu8  DFS_FUNC;
extern u8   F_Sector[];
extern u8   FileBuff[];
extern u8   About_Flag;
extern u8   Rest_Flag ;
extern u8   Cal_Flag;
extern u16  BmpNum[8];
extern uc8  DAT_Var;

u8 Save_Bmp(s16 FileNum);
u8 Open_Bmp(u8 FileNum);
u8 Color_Num(u16 Color);
u8 Save_Dat(s16 FileNum);
u8 SD_Save_Dat(s16 FileNo);
u8 Load_Dat(s16 FileNum);
u8 Save_Buf(s16 FileNum);
u8 Load_Buf(s16 FileNum);
u8 Save_Csv(s16 FileNum);
u8 Save_Svg(s16 FileNum);
u8 Save_One_Param(u8 Page, u8 Num);
u8 Save_Kpg(void);
u8 Save_Param(void);
u8 Load_Param(void);
u8 Save_Parameter(void);
s16 Load_File_Num(u8 Tpye);

void Make_Filename(s16 FileNum,u8* FileName);
void reset_parameter(void);
void Restore_CalPar(void);
void Restore_Scene(void); 
void SVC_IRST(void);
void PrintClk(u16 x0, u16 y0, u8 Phase);
void make_Vertical(u8 CHn, u8 TRACK, u8* buf, u8* len);
void Read_CalFlag(void);
void Restore_OrigVal(void);
void Read_Parameter(void);
void Read_Kpg(void);
void WriteStr(u8* Str, u16* pClu);
void WriteVar(u16 Var, u16* pClu);
void WriteVar_u64(uint64_t Var, u16* pClu);
void File_Num(void);

#endif
/********************************* END OF FILE ********************************/
