/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
File Name :      own_sprintf.h
Version :        TS200 APP Test 1.0
Description:
Author :         Ning
Data:            2017/11/06
2015/07/07   统一命名；
2017/01/17   增加文件
*******************************************************************************/
#include "APP_Version.h"
u32 _abs(const int a1, const int a2);

char *__own_itoa(int num, char *str);
char *own_sprintf(char *dest, const char *cntrl_string, ...);
