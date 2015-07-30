/******************** (C) COPYRIGHT 2009 e-Design Co.,Ltd. *********************
* File Name: Calculate.h  Author: kewei & bure  Hardware: DS0201V1.1 Version: Ver 1.0
*******************************************************************************/
#ifndef __Calculate_H__
#define __Calculate_H__

typedef struct _T_i32str_res I32STR_RES;

struct _T_i32str_res {
    char decPos;
    char len;
    char str[13];
};

void Int32String_sign(I32STR_RES *r, int n, int e);
void Int32String(I32STR_RES *r, int n, int e);
#endif
/********************************* END OF FILE ********************************/
