/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      own_sprintf.c
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
*******************************************************************************/
#include <stdarg.h>
#include "own_sprintf.h"
/*******************************************************************************
������: __own_itoa
��������:����
�������:num:        *str:
���ز���:
*******************************************************************************/
char * __own_itoa(int num,char *str)
{
  
    int i=0,digits=0,n;
    if (num < 0) {
        str[i++] = '-';
        num = -num;
    }
    n = num;
    n /= 10;
    digits = 1;
    while(n) {
        n /= 10;
        digits*= 10;
    }

    while(digits) {
        str[i++] = (num/digits)+0x30;
        num %= digits;
        digits /= 10;

    }

    str[i] = '\0';
    return str;
}
/*******************************************************************************
������: own_sprintf
��������:����
�������:
���ز���:
*******************************************************************************/
char * own_sprintf(char* dest,const char*cntrl_string, ...)
{
    int pos=0,cnt_printed_chars=0,i;
    unsigned char* chptr;
    char str[8];
    int flp;
    va_list ap;
    if (!dest ) return 0;
    va_start(ap,cntrl_string);//ʹapָ���һ����ѡ����

    while (cntrl_string[pos]) {
        if (cntrl_string[pos] == '%') {
            pos++;
            switch (cntrl_string[pos]) {
            case 'c':
                dest[cnt_printed_chars++] = va_arg (ap, unsigned char);//va_arg���ز����б��еĵ�ǰ������ʹapָ������б��е���һ������
                break;
            case 's':
                chptr = va_arg (ap, unsigned char*);
                i = 0;

                while (chptr [i]) {
                    dest[cnt_printed_chars++] = chptr [i++];
                }
                break;
            case 'd':
                __own_itoa (va_arg (ap, int), str);
                i = 0;
                while (str [i]) {
                    dest[cnt_printed_chars++] = str [i++];
                }
                break;
            case 'f':
                flp = va_arg (ap, int);
                if(flp <= 9 && flp > 0){//С��0.1����0.01
                    
                }else if(flp > 9 && flp <= 99){//����0.1 С��1

                }else if(flp > 99 && flp <= 999){//����1 С��10
                    dest[3] = (flp % 10) + '0';
                    flp /= 10;
                    dest[2] = (flp % 10) + '0';
                    dest[1] = '.';
                    flp /= 10;
                    dest[0] = (flp % 10) + '0';
                    cnt_printed_chars = 4;
                }else if(flp > 999 && flp <= 9999){//����10 С��100
                    dest[4] = (flp % 10) + '0';
                    flp /= 10;
                    dest[3] = (flp % 10) + '0';
                    dest[2] = '.';
                    flp /= 10;
                    dest[1] = (flp % 10) + '0';
                    flp /= 10;
                    dest[0] = (flp % 10) + '0';
                    cnt_printed_chars = 5;
                }
                break;
            default:
                dest[cnt_printed_chars++] = cntrl_string[pos];
                break;

            }
        } else {
            dest[cnt_printed_chars++] = cntrl_string[pos];
        }
        pos++;
    }
    va_end(ap); //va_end��apָ����ΪNULL
    dest[cnt_printed_chars] = '\0';
    return dest;
}
/******************************** END OF FILE *********************************/

