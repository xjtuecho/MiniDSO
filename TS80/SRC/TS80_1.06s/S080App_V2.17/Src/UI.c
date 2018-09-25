/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
File Name :      UI.c
Version :        TS080 APP 1.02
Description:
Author :         Ning
Data:            2018/01/04
History:
2018/01/04       温控模式下显示电压大小状态
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "APP_Version.h"
#include "UI.h"
#include "CTRL.h"
#include "WordLib.h"
#include "Bios.h"
#include "OLed.h"
#include "HARDWARE.h"
#include "disk.h"
#include "MMA8652FC.h"
#include "own_sprintf.h"
#include "I2C.h"

extern u32 Resistance;
extern u8  MarkAd_pos;
extern u32 slide_data;
char *gSys_settings[] = {"WkTemp ", "StbTemp", "SlpTime", 
                         "Power  ", "TempStp", "OffVolt", 
                         "Temp   ", "Hand   ", "Source", "\0", "\0"};

/******************************************************************************/
static u8 gTemp_array[16 * 16 + 16];
static u8 gUp_flag = 0, gDown_flag = 0, gLevel_flag = 0, gTempset_showctrl = 0;
static u16 gTemp_array_u16[208];
static s32 gSet_table[10][3] = 
{
    {4000, 1000, 100}, //工作温度
    {4000, 1000, 100}, //休眠温度
    {99900, 6000, 3000}, //休眠时间
    {24, 16, 1}, //工作功率
    {250, 10, 10},    //步长
    {130, 100, 1},     //高低电压
    {1, 0, 1},
    {1, 0, 1},
    {1, 0, 1},
    {7520, 2120, 100}
}; /*温度选择*/
/*******************************************************************************
函数名: Temp_conversion
函数作用:摄氏度华氏度相互转换
输入参数: flag 0,摄氏度转换华氏度
          flag 1,华氏度转换摄氏度
          tmp 要转换的温度
返回参数:转换后的值
*******************************************************************************/
s16 Temp_conversion(u8 flag , s16 tmp)
{
    if(flag == 0)       return (320 + tmp * 9 / 5); //C -> F
    else                return (5 * tmp - 1600) / 9; //F -> C
}
/*******************************************************************************
函数名: APP_Init
函数作用:根据电压初始化开始状态
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void APP_Init(void)
{
    int rev;

    HEATING_TIMER = 0;
    UI_TIMER = 0;

    rev = Read_Vb();//开机电压判断
    if(rev == 1)        Set_CtrlStatus(ALARM);
    Set_CtrlStatus(IDLE);
    G6_TIMER = device_info.wait_time;
}
/*******************************************************************************
函数名: Show_Volt
函数作用:显示电压
输入参数:u8 style：样式    0：小    1：大
返回参数:NULL
*******************************************************************************/
void Show_Volt(u8 style)
{
    u8 buf[20];

    if(style)
    {
        own_sprintf((char *)buf, "%f:", Vol_Get());
        Display_Str6(66, (char *)buf);
    }
    else
    {
        own_sprintf((char *)buf, "%fV", Vol_Get());
        Display_Str8(0, (char *)buf, 0);
    }
}
/*******************************************************************************
函数名: Display_Temp
函数作用:显示温度
输入参数:x:显示位置 Temp:温度
返回参数:NULL
*******************************************************************************/
void Display_Temp(const u8 x, const s16 temp)
{
    char str[8];
    u16 i, len;
    //memset(str,0x0,6);
    own_sprintf(str, "%d", temp);
    len = strlen(str);
    str[len] =   (device_info.temp_flag == TEMP_UNIT_C) ? 'C' : 'E';
    for(i = len + 1; i < 4; i++)        str[i] = ' ';
    str[i] = '\0';
    Display_Str(x, str);
    return;
}
/*******************************************************************************
函数名: Reverse_Bin8
函数作用:按位逆向8位二进制  10101010 变为01010101
输入参数:逆向的数据
返回参数:逆向后的数据
*******************************************************************************/
static u8 Reverse_Bin8(u8 data)
{
    u8 i, cache, result = 0;

    for (i = 0; i < 8; i++)
    {
        cache = data & (1 << i);
        if (i < 4)      cache <<= 7 - 2 * i;
        else            cache >>= 2 * i - 7;
        result += cache;
    }
    return result;
}
/*******************************************************************************
函数名: Show_ReverseChar
函数作用:竖向动态显示字符
输入参数: ptr:字节库   num:个数
              width:宽度   direction :方向 (0 up, 1 down)
返回参数:NULL
*******************************************************************************/
static void Show_ReverseChar(u8 *ptr, u8 num, u8 width, u8 direction)
{
    static u32 j = 0, m = 0, po_j[3] = {0, 0, 0}, po_m[3] = {0, 0, 16};
    u32 i, k;

    if(direction == 0)   //up
    {
        if(gUp_flag == 0)   //前一状态不是加热
        {
            j = 0;
            m = 0;
            gUp_flag     = 1;
            gDown_flag   = 0;
            gLevel_flag  = 0;
        }
        else
        {
            j = po_j[0];
            m = po_m[0];
        }
    }
    else if(direction == 1)
    {
        if(gDown_flag == 0)   //前一状态不是降温
        {
            j = 0;
            m = 0;
            gUp_flag     = 0;
            gDown_flag   = 1;
            gLevel_flag  = 0;
        }
        else
        {
            j = po_j[1];
            m = po_m[1];
        }
    }
    else
    {
        j = po_j[2];
        m = po_m[2];
    }
    for(i = 0; i < width * 2 * num; i++)   gTemp_array[i] = Reverse_Bin8(*(ptr + i)); //逆向8位

    for(k = 0; k < width * 2 * num; k += width * 2)
        for(i = 0; i < width ; i++)
        {
            gTemp_array_u16[i + k] = ((gTemp_array[i + k] & 0x00FF) << 8) | gTemp_array[i + k + width] ;//上半部下半部与成u16 便于移位
            if(direction == 1)
            {
                if(j == 0)      gTemp_array_u16[i + k] <<= m;//下面空，上面显示
                else            gTemp_array_u16[i + k] >>= j;//上面空，下面显示
            }
            else     //上
            {
                if(m == 0)      gTemp_array_u16[i + k] <<= j;//下面空，上面显示
                else            gTemp_array_u16[i + k] >>= m;//上面空，下面显示
            }
            gTemp_array[i + k] = (gTemp_array_u16[i + k] & 0xFF00) >> 8;
            gTemp_array[i + k + width] = gTemp_array_u16[i + k] & 0x00FF;
        }

    for(i = 0; i < width * 2 * num; i++)        
        gTemp_array[i] = Reverse_Bin8(gTemp_array[i]); //移位后再逆向

    if(m == 0 && j == 16)   //全显示，换显示'头数'
    {
        j = 0;
        m = 16;
    }
    if(m == 0)  j++;
    else        m--;

    if(direction == 0)   //up
    {
        po_m[0] = m;
        po_j[0] = j;
    }
    else if(direction == 1)
    {
        po_j[1] = j;
        po_m[1] = m;
    }
    else
    {
        po_j[2] = j;
        po_m[2] = m;
    }
}
/*******************************************************************************
函数名: Show_Notice
函数作用:显示待机提示信息
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Show_Notice(void)
{
    static u8 *ptr0;
    static u8 i = 0;

    if(i == 0)   //第1  步
    {
        ptr0 = Oled_DrawArea(0, 0, 96, 16, (u8 *)Maplib);
    }
    else if(i == 1)    //第2  步
    {
        ptr0 = Oled_DrawArea(0, 0, 96, 16, ptr0);
    }
    else if(i == 2)    //第3  步
    {
        Oled_DrawArea(0, 0, 96, 16, (u8 *)Maplib);
    }
    else if(i == 3)
    {
        Oled_DrawArea(0, 0, 96, 16, ptr0);
    }
    
    i++;
    if(i == 4)  i = 0;
}
/*******************************************************************************
函数名: Show_Ver
函数作用:显示版本
输入参数:ver 版本号flag (0 :滚动显示 )(1不滚动)
返回参数:NULL
*******************************************************************************/
void Show_Ver(u8 ver[], u8 flag)
{
    u8 *ptr;
    int k, i;
    u8 temp0, temp1, temp2;

    if(ver[2] >= 0x30 && ver[2] < 0x3a)  temp1 = ver[2] - 0x30;
    if(ver[3] >= 0x30 && ver[3] < 0x3a)  temp2 = ver[3] - 0x30;
    if(ver[0] >= 0x30 && ver[0] < 0x3a)  temp0 = ver[0] - 0x30;

    for(i = 0; i < 24; i++)
    {
        Ver_s[4 * 24 + i] = Number12[temp0 * 24 + i];
        Ver_s[6 * 24 + i] = Number12[temp1 * 24 + i];
        Ver_s[7 * 24 + i] = Number12[temp2 * 24 + i];
    }

    for(k = 0; k < 16; k++)
    {
        if(flag == 0)
        {
            Show_ReverseChar((u8 *)Ver_s, 8, 12, 2);
            ptr = (u8 *)gTemp_array;
        }
        else
        {
            ptr = (u8 *)Ver_s;
        }
        for(i = 0; i < 8; i++)
        {
            ptr = Oled_DrawArea(i * 12, 0, 12, 16, ptr);
        }
    }
    if(flag == 0)
    {
        Delay_Ms(1000);
        Clear_Screen();
        Clear_Watchdog();
    }
}
/*******************************************************************************
函数名: Show_TempDown
函数作用:显示温度下降: 实际温度>>>目标温度
输入参数:temp 实际温度,dst_temp 目标温度
返回参数:NULL
*******************************************************************************/
void Show_TempDown(s16 temp, s16 dst_temp)
{
    static u8 guide_ui = 0;
    char str[8];
    u16 i, len;

    //memset(str,0,6);
    own_sprintf(str, "%d", temp);
    len = strlen(str);
    str[len] = (device_info.temp_flag == TEMP_UNIT_C) ? 'C' : 'E';
    for(i = len + 1; i < 3; i++)
        str[i] = ' ';
    str[i] = '\0';
    Display_Str10(0, str);

    Oled_DrawArea(41, 0, 16, 16, (u8 *)(Guide + (guide_ui++) * 32));

    if(guide_ui == 3)   guide_ui = 0;

    //memset(str,0x0,6);
    own_sprintf(str, "%d", dst_temp);
    len = strlen(str);
    str[len] = (device_info.temp_flag == TEMP_UNIT_C) ? 'C' : 'E';
    for(i = len + 1; i < 4; i++)
        str[i] = ' ';
    str[i] = '\0';

    Display_Str10(56, str);
}
/*******************************************************************************
函数名: Show_Warning
函数作用:显示警告界面
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Show_Warning(void)
{
    u8 i;
    u8 *ptr;

    switch(Get_AlarmType())
    {
    case HIGH_TEMP:
        ptr = (u8 *)Warning;
        break;
    case SEN_ERR:
        ptr = (u8 *)SenErr;
        break;
    case HIGH_VOLTAGE:
        ptr = (u8 *)HighVt;
        break;
    case LOW_VOLTAGE:
        ptr = (u8 *)LowVot;
        break;
    }
    
    Oled_DrawArea(0, 0, 16, 16, (u8 *)Warning + 20 * 7);
    for(i = 2; i < 9; i++)  Oled_DrawArea(10 * i, 0, 10, 16, (u8 *)ptr +  (i - 2) * 20);
    Delay_Ms(500);
    Clean_Char(16, 80);
}
/*******************************************************************************
函数名: Show_OrderChar
函数作用:横向动态显示字符
输入参数: ptr:字节库num:个数width:宽度
返回参数:NULL
*******************************************************************************/
static void Show_OrderChar(u8 *ptr, u8 num, u8 width)
{
    static u8 i = 1, j = 0, k = 0, m = 10;
    //i是源数组开始位
    //j是目标数组开始位
    //m是整个的长度

    if(gLevel_flag == 0)   //前一状态不是恒温
    {
        i = 1;
        j = 0;
        m = 8;
        gUp_flag     = 0;
        gDown_flag   = 0;
        gLevel_flag  = 1;
    }

    if(i == 0)
    {
        for(k = 0; k <= m ; k++ )
        {
            gTemp_array[k] = *(ptr + 52 + j + k - 1);
            gTemp_array[k + width] = *(ptr + 52 + width + k + j - 1);
        }
        m++;
        j--;
        if(m == 11)
        {
            m--;
            j = 0;
            i = 1;
            return ;
        }
    }

    if(j == 0)
    {
        for(k = 1; k < m ; k++ )
        {
            gTemp_array[k + i] = *(ptr + 52 + j + k - 1);
            gTemp_array[k + i + width] = *(ptr + 52 + width + k + j - 1);
        }
        m--;
        i++;
        if(m == 0)
        {
            m++;
            j = 9;
            i = 0;
        }
    }
}
/*******************************************************************************
函数名: Show_HeatingIcon
函数作用:动态选择加热等状态标识
输入参数: ht_flag 恒温表示  active 移动标识
返回参数:NULL
*******************************************************************************/
void Show_HeatingIcon(u32 ht_flag, u16 active)
{
    u8 *ptr;
    memset(gTemp_array, 0, 20);
    if(ht_flag == 0)         Show_ReverseChar((u8 *)TempIcon, 1, 10, 0); //(5,ptr,16);//加热//
    else if(ht_flag == 1)    Show_ReverseChar((u8 *)TempIcon + 32, 1, 10, 1); //(5,ptr+32,16);//降温//
    else if(ht_flag == 2)    Show_OrderChar((u8 *)TempIcon, 1, 10);      //(5,ptr+64,16);//恒温//

    ptr = (u8 *)gTemp_array;
    Oled_DrawArea(86, 6, 10, 10, (u8 *)ptr);
    if(Get_CtrlStatus() == WAIT) Oled_DrawArea(80, 6, 6, 10, (u8 *)IhIcon + 12);
    else
    {
        if(active == 0) Oled_DrawArea(80, 6, 6, 10, (u8 *)TempIcon + 20);
        else  Oled_DrawArea(80, 6, 6, 10, (u8 *)IhIcon);
    }
    Show_Volt(1);//显示电压
}
/*******************************************************************************
函数名: Display_Str
函数作用:显示16*16字符串
输入参数:x: 位置 str :显示字符串
返回参数:NULL
*******************************************************************************/
void Display_Str(u8 x, const char *str)
{
    u8 *ptr;
    u8  temp, w;
    u16 x1;

    x1 = x * 16;
    while(*str != 0)
    {
        temp = *str++;
        if(temp == ' ') temp = 10;
        else if(temp == '-') temp = 12;
        else
        {
            if((temp >= 0x30) && (temp < 0x3a))
            {
                temp = temp - 0x30; /* 0 --9 */
                w = 14;
            }
            else
            {
                if((temp >= 0x41) && (temp <= 0x46))
                {
                    temp = temp - 0x41 + 11;/*ABD*/
                    w = 16;
                }
                else return ;
            }
        }
        ptr = (u8 *)wordlib;
        ptr += temp * 32 + ((16 - w) / 2);
        Oled_DrawArea_2(x1 , 0, w, 16, (u8 *)ptr, 16);
        x1 += w;
    }
}
/*******************************************************************************
函数名: Display_Str
函数作用:显示8*16字符串
输入参数:x: 位置 str :显示字符串  mode: 1：小数点字库
返回参数:NULL
*******************************************************************************/
void Display_Str8(u8 x, const char *str , const u8 mode)
{
    u8 *ptr;
    u8 temp;

    if( mode == 0 )
    {
        while(*str != 0)
        {
            temp = *str++ - ' ';//得到偏移后的值

            ptr = (u8 *)ASCII8X16;
            ptr += temp * 16;
            Oled_DrawArea(x * 8, 0, 8, 16, (u8 *)ptr);
            x++;
        }
    }
    else
    {
        while(*str != 0)
        {
            temp = *str++ - '0';//得到偏移后的值

            ptr = (u8 *)Decimal;
            ptr += temp * 16;
            Oled_DrawArea(x * 8, 0, 8, 16, (u8 *)ptr);
            x++;
        }
    }
}
/*******************************************************************************
函数名: Triangle_Str
函数作用:显示宽度为16的箭头
输入参数:x: 位置 pos :箭头标志位
返回参数:NULL
*******************************************************************************/
void Triangle_Str(u8 x, u8 pos)
{
    u8 *ptr;
    ptr = (u8 *)Triangle + 16 * 2 * pos ;
    Oled_DrawArea(x, 0, 16, 16, (u8 *)ptr);
}
/*****************************************************************
函数名: Show_Triangle
函数作用:显示设置模式中按键方向的三角方向的各种形态
输入参数: empty_trgl: 0,1,2  不加粗,左加粗,右加粗
          fill_trgl:  0,1,2  不实体,左实体,右实体
返回参数:NULL
*******************************************************************/
void Show_Triangle(u8 empty_trgl, u8 fill_trgl)
{
    int j;
    u8 *ptr;

    ptr = (u8 *)Triangle;

    if((empty_trgl == 0) && (fill_trgl == 0))
    {
        for(j = 0; j < 2; j++)
        {
            if(j == 0)           ptr = Oled_DrawArea(0, 0, 16, 16, (u8 *)ptr);
            else if(j == 1)      ptr = Oled_DrawArea(5 * 16, 0, 16, 16, (u8 *)ptr);
        }
    }
    else if((empty_trgl != 0) && (fill_trgl == 0))
    {
        if(empty_trgl == 1)
        {
            ptr += 32;
            Oled_DrawArea(5 * 16, 0, 16, 16, (u8 *)ptr);
            ptr += 32;
            Oled_DrawArea(0, 0, 16, 16, (u8 *)ptr);
        }
        else if (empty_trgl == 2)
        {
            Oled_DrawArea(0, 0, 16, 16, (u8 *)ptr);
            ptr += 32 * 3;
            Oled_DrawArea(5 * 16, 0, 16, 16, (u8 *)ptr);
        }
    }
    else if((empty_trgl == 0) && (fill_trgl != 0))
    {
        if(fill_trgl == 1)
        {
            ptr += 32;
            Oled_DrawArea(5 * 16, 0, 16, 16, (u8 *)ptr);
            ptr += 32 * 3;
            Oled_DrawArea(0, 0, 16, 16, (u8 *)ptr);
        }
        else if (fill_trgl == 2)
        {
            Oled_DrawArea(0, 0, 16, 16, (u8 *)ptr);
            ptr += 32 * 5;
            Oled_DrawArea(5 * 16, 0, 16, 16, (u8 *)ptr);
        }
    }
}
/*******************************************************************************
函数名: Display_Str6
函数作用:显示宽度为6的字符串
输入参数:x: 位置 str :显示字符串
返回参数:NULL
*******************************************************************************/
void Display_Str6(u8 x, const char *str)
{
    u8 *ptr;
    u8  temp;

    //    if((x < 1)||(x > 96))  x = 0;
    //    else x--;

    while(*str != 0)
    {
        temp = *str++ - '.';//得到偏移后的值

        ptr = (u8 *)ASCII6X5;
        ptr += temp * 6;
        Oled_DrawArea(x, 0, 6, 5, (u8 *)ptr);
        x = x + 6;
    }
}
/*******************************************************************************
函数名: Display_Str10
函数作用:显示宽度为10的字符串
输入参数:x: 位置 str :显示字符串
返回参数:NULL
*******************************************************************************/
void Display_Str10(u8 x, const char *str)
{
    u8 *ptr;
    u8 temp;
    u8 position = 0;

    while(*str != 0)
    {
        temp = *str++;
        if(temp == 'C') temp = 11;
        else if(temp == 'E') temp = 12;
        else
        {
            if((temp >= 0x30) && (temp < 0x3a)) temp = temp - 0x30; /* 0 --9 */
            else
            {
                temp = 10;
            }
        }
        ptr = (u8 *)Number10;
        ptr += temp * 20;
        Oled_DrawArea(x + position * 10, 0, 10, 16, (u8 *)ptr);
        position++;
    }
}
/*******************************************************************************
函数名: Setup_DispItem
函数作用:设置参数显示,显示闪烁图像
输入参数: item:设置选项名 direction:箭头方向 disp_flag:闪烁开关
返回参数:NULL
*******************************************************************************/
static void Setup_DispItem(const u8 item,  u8 *direction , const u8 disp_flag)
{
    u8 buf[20] = {0};//显示的字符串
    u8 buf_TOV[2] = {0};
    u8 wdj[5];
    s16 temp_val;
    s16 temp_sensor;
    u16 digit;//位数
    Display_Str8(0, gSys_settings[item], 0); //选项名
    
    switch(item)
    {
    case WKT://工作温度
        own_sprintf((char *)buf, "%d", device_info.t_work / 10);
        digit = strlen((char *)buf);//(device_info.t_work/10);//求位数
        break;
    case SDT://休眠温度
        own_sprintf((char *)buf, "%d", device_info.t_standby / 10);
        digit = strlen((char *)buf);//Calculated_Digit(device_info.t_standby/10);
        break;
    case WTT://休眠时间
        own_sprintf((char *)buf, "%d", device_info.wait_time / 100);
        digit = strlen((char *)buf);//Calculated_Digit(device_info.wait_time/100);
        break;
    case IDT://额定功率
        own_sprintf((char *)buf, "%d", device_info.rated_power);
        digit = strlen((char *)buf);//Calculated_Digit(device_info.rated_power);
        break;
    case STP://步长
        own_sprintf((char *)buf, "%d", device_info.t_step / 10);
        digit = strlen((char *)buf);//Calculated_Digit(device_info.t_step/10);
        break;
    case TOV://关闭电压
        own_sprintf((char *)buf, "%d", device_info.turn_offv / 10);
        own_sprintf((char *)buf_TOV, "%d", device_info.turn_offv % 10);
        digit = strlen((char *)buf) + 1; //Calculated_Digit(device_info.turn_offv);
        break;
    case DGC://温度模式
        if(device_info.temp_flag == 0)  strcpy((char *)buf, " {"); //"{" 摄氏
        else                            strcpy((char *)buf, " }"); //"}"   //华氏
        digit = 2;
        break;
    case HD://左右手模式
        if(device_info.handers == 0)    strcpy((char *)buf, " RH");
        else                            strcpy((char *)buf, " LH");
        digit = 3;
        break;
    case PT://便携电源使用模式
        if(device_info.portable_flag == 0)      strcpy((char *)buf, "OFF");
        else                                    strcpy((char *)buf, " ON");
        digit = 3;
        break;
    case EXW://恢复出厂
        own_sprintf((char *)buf, "Restore DFLT"); //" Initialize ");
        break;
    case WDJ://电压温度显示
        break;
    }

    if(disp_flag == 1)//是否打开闪烁开关
    {
        if(UI_TIMER < HIGHLIGHT_FREQUENCY) //显示
        {
            //--------------------箭头--------------------
            if(*direction == 1)//左
            {
                Display_Str8(SET_INFO_X, "<", 0); //<
                Display_Str8(SET_PROMPT_X + digit, " ", 0);
            }
            else if(*direction == 2)//右
            {
                Display_Str8(SET_PROMPT_X - 1, " ", 0);
                Display_Str8(SET_PROMPT_X + digit, ">", 0); //>
            }
            else//无操作
            {
                Display_Str8(SET_INFO_X, "<", 0); //<
                Display_Str8(SET_PROMPT_X + digit, ">", 0); //>
            }
            //--------------------数字--------------------
            if(item == TOV)
            {
                Display_Str8(SET_PROMPT_X, (char *)buf, 0);
                Display_Str8(SET_PROMPT_X + digit - 1, (char *)buf_TOV, 1);
            }
            else        Display_Str8(SET_PROMPT_X, (char *)buf, 0); //数值

            if(UI_TIMER == 0)//一次高亮周期结束重新计数
            {
                UI_TIMER = (HIGHLIGHT_FREQUENCY * 2) - HIGHLTGHT_REDUCE;
                if(direction)   *direction = 0;
            }
        }
        else//擦出
        {
            Display_Str8(SET_INFO_X, " ", 0);
            Display_Str8(SET_PROMPT_X + digit, "   ", 0);
        }
    }
    else//没打开闪烁开关
    {
        if(item == WDJ)//温度计和电压显示
        {
            if(UI_TIMER == 0)
            {
                temp_sensor = Get_SensorTmp(0);//获取冷端温度
                temp_val = Get_Temp();//获取当前温度
                if(device_info.temp_flag == 1)   //获取温度显示方式
                {

                    if(_abs(temp_val, temp_sensor) < 50)
                    {
                        temp_val = Temp_conversion(0, temp_val);
                        temp_sensor = Temp_conversion(0, temp_sensor);
                        own_sprintf((char *)wdj, "%d} ", temp_val / 10);
                    }
                    else        own_sprintf((char *)wdj, "--} ");
                }
                else
                {
                    if(_abs(temp_val, temp_sensor) < 50)
                        own_sprintf((char *)wdj, "%d{ ", temp_val / 10);
                    else
                        own_sprintf((char *)wdj, "--{ ");
                }
                Display_Str8(SET_INFO_X, (char *)wdj, 0); //显示温度
                Show_Volt(0);//显示电压
                UI_TIMER = 20;
            }
        }
        else if(item == EXW)//恢复出厂设备
        {
            Display_Str8(0, (char *)buf, 0);
        }
        else
        {
            Display_Str8(SET_INFO_X, " ", 0);
            if(item == TOV)
            {
                Display_Str8(SET_PROMPT_X, (char *)buf, 0);
                Display_Str8(SET_PROMPT_X + digit - 1, (char *)buf_TOV, 1);
            }
            else    Display_Str8(SET_PROMPT_X, (char *)buf, 0); //数值
            Display_Str8(SET_PROMPT_X + digit, "   ", 0);
        }
    }
}
/*******************************************************************************
函数名: Setup_ItemProc
函数作用:设置选中项具体信息
输入参数:item:设置选项名
返回参数:NULL
*******************************************************************************/
static void Setup_ItemProc(const u8 item)
{
    u32 key;
    u8 direction = 0;

    int *temporary_set, last; //参数的地址
    s32 max_value, min_value; //最大最小值
    u16 step;//步长
    UI_TIMER = HIGHLIGHT_FREQUENCY * 2 - HIGHLTGHT_REDUCE; //高亮计算
    EFFECTIVE_KEY_TIMER = EFFECTIVE_KEY / 2; //无操作返回时间

    switch(item)//在不同的选项下
    {
    case WKT:
        temporary_set = (int *)&device_info.t_work;
        gSet_table[item][2] = device_info.t_step;// 设置步长
        break;
    case SDT:
        temporary_set = (int *)&device_info.t_standby;
        gSet_table[item][2] = device_info.t_step;// 设置步长
        break;
    case WTT:
        temporary_set = (int *)&device_info.wait_time;
        break;
    case IDT:
        temporary_set = (int *)&device_info.rated_power;
        break;
    case STP:
        temporary_set = (int *)&device_info.t_step;
        break;
    case TOV:
        temporary_set = (int *)&device_info.turn_offv;
        break;
    case DGC://温度单位选择
        temporary_set = (int *)&device_info.temp_flag;
        break;
    case HD:
        temporary_set = (int *)&device_info.handers;
        break;
    case PT:
        temporary_set = (int *)&device_info.portable_flag;
        break;  
    case WDJ:
        Clear_Screen();
        Clear_Watchdog();
        Zero_Calibration();//校准零点
        if(Get_CalFlag() == 1)
        {
            device_info.zero_ad = gZerop_ad;
            Disk_BuffInit();//磁盘数据初始化
            Config_Save();//保存设置到config.txt 
        }
        if(gCalib_flag != 0)
        {
            //显示校准完成
            //Show_Cal(gCalib_flag);
            if(gCalib_flag == 1)        Display_Str8(0, "Completed.", 0);
            else                        Display_Str8(0, "Retry later.", 0);
            Clear_Watchdog();
            Delay_Ms(1000);
            Clear_Watchdog();
            Delay_Ms(1000);
            gCalib_flag = 0;
            Clear_Screen();
        }
        return;
    case EXW:
        Restore_Setting();//恢复出厂设备
        return;
    }
    UI_TIMER = HIGHLIGHT_FREQUENCY * 2 - HIGHLTGHT_REDUCE; //高亮计算
    EFFECTIVE_KEY_TIMER = EFFECTIVE_KEY / 2; //无操作返回时间
    Delay_Ms(300);
    /*----------------范围和增加单位赋值-------------------*/
    max_value = gSet_table[item][0];
    min_value = gSet_table[item][1];
    step = gSet_table[item][2];

    if(device_info.temp_flag == 1 && (item == WKT || item == SDT)) //F°模式最大值最小值转换
    {
        max_value = gSet_table[9][0];
        min_value = gSet_table[9][1];
        step = gSet_table[9][2];
    }

    while(1)
    {
        Setup_DispItem(item, &direction, 1);//显示闪烁图像
        /*------------------进入选项后的操作-------------------*/
        key = Get_gKey();
        if(key)
        {
            last = *temporary_set;
            /*--------------------减--------------------*/
            if((key == (KEY_CN | KEY_V1)) || (key == KEY_V1))
            {
                //last = *temporary_set;
                if(*temporary_set > min_value)  //大于最小值(可以减少)
                {
                    *temporary_set -= step;
                    if(*temporary_set < min_value)      
                        *temporary_set = min_value;//小于最小值(不能减少)
                }
                if(last != *temporary_set)
                {
                    if(item == DGC )
                    {
                        if(*temporary_set == 0)
                        {
                            device_info.t_work = Temp_conversion(1, device_info.t_work);
                            device_info.t_standby = Temp_conversion(1, device_info.t_standby);

                            if(device_info.t_work < 1000)       device_info.t_work = 1000;//越界判断
                            if(device_info.t_standby < 1000)    device_info.t_standby = 1000;
                        }
                    }
                    else if(item == HD)         Init_Oled();
                }
                direction = 1;
            }
            /*--------------------加--------------------*/
            else if((key == KEY_V2) || (key == (KEY_CN | KEY_V2)))
            {
                //last = *temporary_set;
                if((*temporary_set) < max_value)
                {
                    *temporary_set += step;
                    if((*temporary_set) > max_value) *temporary_set = max_value;
                }
                if(last != *temporary_set)
                {
                    if(item == DGC )
                    {
                        if(*temporary_set == 1)
                        {
                            device_info.t_work = Temp_conversion(0, device_info.t_work);
                            device_info.t_standby = Temp_conversion(0, device_info.t_standby);
                        }
                    }
                    else if(item == HD)
                        Init_Oled();
                }
                direction = 2;
            }
            Delay_Ms(50);
            EFFECTIVE_KEY_TIMER = EFFECTIVE_KEY / 2; //退出倒计时重新计数
            UI_TIMER = HIGHLIGHT_FREQUENCY - HIGHLTGHT_REDUCE; //闪烁显示重新计数
            Setup_DispItem(item, &direction, 	1);//显示闪烁图像
        }
        
        Clear_Watchdog();
        if(EFFECTIVE_KEY_TIMER == 0)
        {
            EFFECTIVE_KEY_TIMER = EFFECTIVE_KEY;
            return ;
        }
    }
}
/*******************************************************************************
函数名: Setup_Proc  
函数作用:设置参数过程  
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Setup_Proc(void)
{
    u32 kaka = 0;
    u8 item = WKT;
    u32 key;
    Clear_Screen();
    Setup_DispItem(item, NULL, 0);//设置参数显示,显示闪烁图像
    EFFECTIVE_KEY_TIMER = 1000;//退出设置模式时间初始化
    while(1)
    {
        key = Get_gKey();
        switch(key)//设置项跳转
        {
        case KEY_V1:
            if(item)
                item --;
            else
                item = END_OF_SETUP_ITEM - 1;
            kaka = 0;
            break;
        case KEY_V2:
            if(item < (END_OF_SETUP_ITEM - 1))
                item ++;
            else
                item = 0;
            kaka = 0;
            break;
        case KEY_CN | KEY_V1:
            Clear_LongKey();
            Setup_ItemProc(item);
            Clear_Screen();
            Setup_DispItem(item, NULL, 0);
            kaka = 0;
            break;
        case KEY_CN | KEY_V2:
            Clear_LongKey();
            EFFECTIVE_KEY_TIMER = 0;//清除计数
            return;
        }
        if(key)
        {
            Clear_Screen();
            EFFECTIVE_KEY_TIMER = EFFECTIVE_KEY;//退出设置模式时间初始化
            Setup_DispItem(item, NULL, 0);
        }
        Clear_Watchdog();
        Delay_Ms(10);
        if(UI_TIMER == 0)       Setup_DispItem(item, NULL, 0);

        if(DISP_TIMER == 0 && kaka < 40)
        {
            Vol_Set(0);
            Set_HeatingTime(70);
            DISP_TIMER = 50;
            kaka++;
        }
        if(kaka == 40)  return;
    }
}
/*******************************************************************************
函数名: Shift_Char
函数作用:字符从右往左移动到指定位置
输入参数:ptr 移动的字符 pos 移动到的指定位置
返回参数:NULL
*******************************************************************************/
void Shift_Char(u8 *ptr, u8 pos)
{
    int k;
    u8 *ptr0;

    k = 80;
    pos = pos * 16;//位置*  字符=   字符所在位置
    while(k >= pos)
    {
        ptr0 = (u8 *)ptr;
        Clean_Char(k + 16, 16);
        ptr0 = Oled_DrawArea(k, 0, 16, 16, (u8 *)ptr0);
        k -= 16;
        Delay_Ms(25);
    }
}
/*******************************************************************************
Show_TempReverse 竖向动态显示温度字符
word_num:     个数
word_width:   宽度
direction :   方向 (0 up, 1 down)
*******************************************************************************/
u8 Show_TempReverse(u8 num, u8 width, u8 direction)
{
    static int i, j = 0, m = 16;
    u8 g , s , b;
    u8 *ptr;
    s16 num_temp;

    num_temp = device_info.t_work;
    num_temp = num_temp / 10;
    b = num_temp / 100;
    s = (num_temp - b * 100) / 10;
    g = (num_temp - b * 100 - s * 10);

    if(gTempset_showctrl == 1)
    {
        j = 1;
        m = 0;
        gTempset_showctrl = 0;
    }
    else if(gTempset_showctrl == 2)
    {
        j = 0;
        m = 16;
        gTempset_showctrl = 0;
    }
    for(i = 0; i < width * 2; i++)    //读字库点阵
    {
        gTemp_array[0 * 32 + i] = *(wordlib + b * 32 + i);
        gTemp_array[1 * 32 + i] = *(wordlib + s * 32 + i);
        gTemp_array[2 * 32 + i] = *(wordlib + g * 32 + i);
        if(device_info.temp_flag == TEMP_UNIT_F)
        {
            gTemp_array[3 * 32 + i] = *(wordlib + 15 * 32 + i);
        }
        else
        {
            gTemp_array[3 * 32 + i] = *(wordlib + 13 * 32 + i);
        }
    }
    ptr = (u8 *)gTemp_array;
    for(i = 1; i <= 4; i++)     ptr = Oled_DrawArea(16 * i, 0, 16, 16, (u8 *)ptr);
    if((m == 0 && j == 0) || (m == 0 && j == 16))       return 0 ;
    if(m == 0)  j++;
    else        m--;
    return 1;
}
/*******************************************************************************
函数名: Show_Set
函数作用:显示设置界面,从右到左移动过来
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Show_Set(void)
{
    u8  m , k;
    s16 num_temp;
    num_temp = device_info.t_work;

    Shift_Char((u8 *)Triangle + 0 * 32, 0); //<
    m = num_temp / 1000; //百
    Shift_Char((u8 *)wordlib + m * 32, 1);
    k = (num_temp - m * 1000) / 100; //十
    Shift_Char((u8 *)wordlib + k * 32, 2);
    m = (num_temp - m * 1000 - k * 100) / 10; //个
    Shift_Char((u8 *)wordlib + m * 32, 3);

    if(device_info.temp_flag == TEMP_UNIT_F)    Shift_Char((u8 *)wordlib + 15 * 32, 4); //F
    else                                        Shift_Char((u8 *)wordlib + 13 * 32, 4); //C

    Shift_Char((u8 *)Triangle + 1 * 32, 5);
}
/*******************************************************************************
函数名: Roll_Num
函数作用:设置中，需要滚动的温度值的位数
输入参数:step 步长,Flag (0+ or 1-)
返回参数:需要滚动的个数
*******************************************************************************/
u8 Roll_Num(u16 step, u8 flag)
{
    u16 b1, b2, g1, g2;
    s16 num_temp;

    num_temp = device_info.t_work;
    b2 = (num_temp) / 1000;
    g2 = (num_temp) / 100;

    if(flag == 0)
    {
        b1 = (num_temp + step) / 1000;
        g1 = (num_temp + step) / 100;
        if(b1 != b2)            return 3;
        else if(g1 != g2)       return 2;
        else                    return 1;
    }
    else
    {
        b1 = (num_temp - step) / 1000;
        g1 = (num_temp - step) / 100;

        if(b1 != b2)            return 3;
        else if(g1 != g2)       return 2;
        else                    return 1;
    }
}
/*******************************************************************************
函数名: Temp_SetProc
函数作用:临时设置温度
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void TempSet_Proc(void)
{
    u32 key;
    u8 theRoll_num = 3;
    //static u8 add_step = 0;//step=1时长按递加
    int *temporary_set;
    s32 max_value, min_value;
    u16 step = device_info.t_step;
    //    Set_LongKeyFlag(1);
    temporary_set = (int *)&device_info.t_work;

    if(device_info.temp_flag == TEMP_UNIT_C)
    {
        max_value = gSet_table[WKT][0];
        min_value = gSet_table[WKT][1];
    }
    else
    {
        max_value = gSet_table[9][0];
        min_value = gSet_table[9][1];
    }
    step = device_info.t_step;

    Clear_Screen();
    Show_Set();
    EFFECTIVE_KEY_TIMER = 1000;
    while(EFFECTIVE_KEY_TIMER > 0)
    {
        key = Get_gKey();
        if(key == KEY_V1 || key == (KEY_CN | KEY_V1))//减
        {
            if(*temporary_set > min_value)  //大于最小值(可以减少)
            {
                gTempset_showctrl = 1;
                theRoll_num = Roll_Num(step, 1); //计算要滚动的位数
                *temporary_set -= step;
                if(*temporary_set < min_value) *temporary_set = min_value;//小于最小值(不能减少)
                Show_TempReverse(theRoll_num, 16, 0);
                gTempset_showctrl = 2;
                Show_Triangle(1, 0);
                if(*temporary_set == min_value)  Show_Triangle(0, 1);
            }
            EFFECTIVE_KEY_TIMER = EFFECTIVE_KEY / 2; //退出倒计时重新计数
        }
        else if(key == KEY_V2 || key == (KEY_CN | KEY_V2))//加
        {
            if((*temporary_set) < max_value)
            {
                gTempset_showctrl = 1;
                theRoll_num = Roll_Num(step, 0);
                *temporary_set += step;
                if((*temporary_set) > max_value) *temporary_set = max_value;
                Show_TempReverse(theRoll_num, 16, 0);
                gTempset_showctrl = 2;
                Show_Triangle(2, 0);
                if(*temporary_set == max_value)         Show_Triangle(0, 2);
            }
            Delay_Ms(50);
            EFFECTIVE_KEY_TIMER = EFFECTIVE_KEY / 2;
        }
        Clear_Watchdog();
    }
}
/*******************************************************************************
函数名: Show_NoElectriciron
函数作用:提示无电烙铁
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Show_NoElectriciron(void)
{
    u8 i;
    Oled_DrawArea(0, 0, 16, 16, (u8 *)Warning + 20 * 7);
    for(i = 2; i < 9; i++)      Oled_DrawArea(10 * i, 0, 10, 16, (u8 *)SenErr + (i - 2) * 20);
}
/******************************** END OF FILE *********************************/
