/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
File Name :      Disk.c
Version :        TS080 APP 1.02
Description:
Author :         Ning
Data:            2017/11/06
History:
*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include "APP_Version.h"
#include "Disk.h"
#include "Bios.h"
#include "Flash.h"
#include "Oled.h"
#include "UI.h"
#include "CTRL.h"
#include "HARDWARE.h"
#include "own_sprintf.h"
#include "defaults.h"

#define Delay_mS    Delay_Ms
void Disk_SecWrite(u8 *pBuffer, u32 DiskAddr);
void Disk_SecRead (u8 *pBuffer, u32 DiskAddr);
void Soft_Delay(void);

uc8  BOOT_SEC[512] = {0xEB, 0x3C, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53,
                      0x35, 0x2E, 0x30, 0x00, 0x02, 0x01, 0x08, 0x00,
                      0x02, 0x00, 0x02, 0x50, 0x00, 0xF8, 0x0c, 0x00,
                      0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29, 0xA2,
                      0x98, 0xE4, 0x6C, 0x4E, 0x4F, 0x20, 0x4E, 0x41,
                      0x4D, 0x45, 0x20, 0x20, 0x20, 0x20, 0x46, 0x41,
                      0x54, 0x31, 0x32, 0x20, 0x20, 0x20, 0x33, 0xC9,
                      0x8E, 0xD1, 0xBC, 0xF0, 0x7B, 0x8E, 0xD9, 0xB8,
                      0x00, 0x20, 0x8E, 0xC0, 0xFC, 0xBD, 0x00, 0x7C,
                      0x38, 0x4E, 0x24, 0x7D, 0x24, 0x8B, 0xC1, 0x99,
                      0xE8, 0x3C, 0x01, 0x72, 0x1C, 0x83, 0xEB, 0x3A,
                      0x66, 0xA1, 0x1C, 0x7C, 0x26, 0x66, 0x3B, 0x07,
                      0x26, 0x8A, 0x57, 0xFC, 0x75, 0x06, 0x80, 0xCA,
                      0x02, 0x88, 0x56, 0x02, 0x80, 0xC3, 0x10, 0x73,
                      0xEB, 0x33, 0xC9, 0x8A, 0x46, 0x10, 0x98, 0xF7,
                      0x66, 0x16, 0x03, 0x46, 0x1C, 0x13, 0x56, 0x1E,
                      0x03, 0x46, 0x0E, 0x13, 0xD1, 0x8B, 0x76, 0x11,
                      0x60, 0x89, 0x46, 0xFC, 0x89, 0x56, 0xFE, 0xB8,
                      0x20, 0x00, 0xF7, 0xE6, 0x8B, 0x5E, 0x0B, 0x03,
                      0xC3, 0x48, 0xF7, 0xF3, 0x01, 0x46, 0xFC, 0x11,
                      0x4E, 0xFE, 0x61, 0xBF, 0x00, 0x00, 0xE8, 0xE6,
                      0x00, 0x72, 0x39, 0x26, 0x38, 0x2D, 0x74, 0x17,
                      0x60, 0xB1, 0x0B, 0xBE, 0xA1, 0x7D, 0xF3, 0xA6,
                      0x61, 0x74, 0x32, 0x4E, 0x74, 0x09, 0x83, 0xC7,
                      0x20, 0x3B, 0xFB, 0x72, 0xE6, 0xEB, 0xDC, 0xA0,
                      0xFB, 0x7D, 0xB4, 0x7D, 0x8B, 0xF0, 0xAC, 0x98,
                      0x40, 0x74, 0x0C, 0x48, 0x74, 0x13, 0xB4, 0x0E,
                      0xBB, 0x07, 0x00, 0xCD, 0x10, 0xEB, 0xEF, 0xA0,
                      0xFD, 0x7D, 0xEB, 0xE6, 0xA0, 0xFC, 0x7D, 0xEB,
                      0xE1, 0xCD, 0x16, 0xCD, 0x19, 0x26, 0x8B, 0x55,
                      0x1A, 0x52, 0xB0, 0x01, 0xBB, 0x00, 0x00, 0xE8,
                      0x3B, 0x00, 0x72, 0xE8, 0x5B, 0x8A, 0x56, 0x24,
                      0xBE, 0x0B, 0x7C, 0x8B, 0xFC, 0xC7, 0x46, 0xF0,
                      0x3D, 0x7D, 0xC7, 0x46, 0xF4, 0x29, 0x7D, 0x8C,
                      0xD9, 0x89, 0x4E, 0xF2, 0x89, 0x4E, 0xF6, 0xC6,
                      0x06, 0x96, 0x7D, 0xCB, 0xEA, 0x03, 0x00, 0x00,
                      0x20, 0x0F, 0xB6, 0xC8, 0x66, 0x8B, 0x46, 0xF8,
                      0x66, 0x03, 0x46, 0x1C, 0x66, 0x8B, 0xD0, 0x66,
                      0xC1, 0xEA, 0x10, 0xEB, 0x5E, 0x0F, 0xB6, 0xC8,
                      0x4A, 0x4A, 0x8A, 0x46, 0x0D, 0x32, 0xE4, 0xF7,
                      0xE2, 0x03, 0x46, 0xFC, 0x13, 0x56, 0xFE, 0xEB,
                      0x4A, 0x52, 0x50, 0x06, 0x53, 0x6A, 0x01, 0x6A,
                      0x10, 0x91, 0x8B, 0x46, 0x18, 0x96, 0x92, 0x33,
                      0xD2, 0xF7, 0xF6, 0x91, 0xF7, 0xF6, 0x42, 0x87,
                      0xCA, 0xF7, 0x76, 0x1A, 0x8A, 0xF2, 0x8A, 0xE8,
                      0xC0, 0xCC, 0x02, 0x0A, 0xCC, 0xB8, 0x01, 0x02,
                      0x80, 0x7E, 0x02, 0x0E, 0x75, 0x04, 0xB4, 0x42,
                      0x8B, 0xF4, 0x8A, 0x56, 0x24, 0xCD, 0x13, 0x61,
                      0x61, 0x72, 0x0B, 0x40, 0x75, 0x01, 0x42, 0x03,
                      0x5E, 0x0B, 0x49, 0x75, 0x06, 0xF8, 0xC3, 0x41,
                      0xBB, 0x00, 0x00, 0x60, 0x66, 0x6A, 0x00, 0xEB,
                      0xB0, 0x4E, 0x54, 0x4C, 0x44, 0x52, 0x20, 0x20,
                      0x20, 0x20, 0x20, 0x20, 0x0D, 0x0A, 0x52, 0x65,
                      0x6D, 0x6F, 0x76, 0x65, 0x20, 0x64, 0x69, 0x73,
                      0x6B, 0x73, 0x20, 0x6F, 0x72, 0x20, 0x6F, 0x74,
                      0x68, 0x65, 0x72, 0x20, 0x6D, 0x65, 0x64, 0x69,
                      0x61, 0x2E, 0xFF, 0x0D, 0x0A, 0x44, 0x69, 0x73,
                      0x6B, 0x20, 0x65, 0x72, 0x72, 0x6F, 0x72, 0xFF,
                      0x0D, 0x0A, 0x50, 0x72, 0x65, 0x73, 0x73, 0x20,
                      0x61, 0x6E, 0x79, 0x20, 0x6B, 0x65, 0x79, 0x20,
                      0x74, 0x6F, 0x20, 0x72, 0x65, 0x73, 0x74, 0x61,
                      0x72, 0x74, 0x0D, 0x0A, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0xAC, 0xCB, 0xD8, 0x55, 0xAA
                     };

static u8 gDisk_buff[0x2600];
static u32 gDisk_var[(512 + 32 + 28) / 4];   // 磁盘工作数据区
static u32 *gV32  = (u32 *) &gDisk_var[512 / 4];
static u8 *gVar  = (u8 *) &gDisk_var[512 / 4 + 8];

static u8 *gBuff = (u8 *) &gDisk_var[0];
const u8 gFat_data[] = {0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //{0xF8,0XFF,0XFF,0xff,0X0f};//

const u8 *gKey_words[] = { STR(STBTEMP_STR), STR(WKTEMP_STR),
                           STR(SLPTIME_STR), STR(RP_STR),
                           STR(TEMPSTP_STR), STR(OFFVOLT_STR),
                           STR(TEMPUNIT_STR), STR(USMODE_STR),
                           STR(ZEROCALI_STR),STR(PORTABLE_STR)
                         };

const u8 *gDef_set[]   = {  CONS(STBTEMP_STR,  STBTEMP_VAL)  , CONS(WKTEMP_STR, WKTEMP_VAL) ,
                            CONS(SLPTIME_STR,   SLPTIME_VAL)   , CONS(RP_STR, RP_VAL) ,
                            CONS(TEMPSTP_STR,  TEMPSTP_VAL)   , CONS(OFFVOLT_STR, OFFVOLT_VAL) ,
                            CONS(TEMPUNIT_STR, TEMPUNIT_VAL) , CONS(USMODE_STR, USMODE_VAL) ,
                            CONS(ZEROCALI_STR, ZEROCALI_VAL) , CONS(PORTABLE_STR, PORTABLE_VAL)
                         };

const char *gSet_range[] = {"   #(100~400)\r\n", "    #(100~400)\r\n", "   #(60~999)\r\n",
                            "    #(16~24)\r\n", "    #(1~25)\r\n", "  #(100~120)\r\n",
                            "        #(0:C,1:F)\r\n", "        #(0:Left,1:Right)\r\n",
                            "    #Do not modify!\r\n",  "    #(0:not por,1:por)\r\n"
                           };

static u8 gFile_con[512];
u8 gRewriteflag[16];
/*******************************************************************************
函数名: Soft_Delay
函数作用:延时
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Soft_Delay()
{
    int i, j;
    for(i = 0 ; i < 1000 ; i++)
        for(j = 0; j < 100; j++);
}
/*******************************************************************************
函数名: Disk_BuffInit
函数作用:磁盘数据初始化
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Disk_BuffInit(void)
{
    memcpy(gDisk_buff, (u8 *)APP_BASE, 0x2600);
    memset(gRewriteflag, 0, 16);
}
/*******************************************************************************
函数名: Upper
函数作用:小写转为大写
输入参数:str需要转换的字符串，len 字符串长度
返回参数:NULL
*******************************************************************************/
void Upper(u8 *str, u16 len)
{
    u16 i;
    for(i = 0; i < len; i++)
        if(str[i] >= 'a' && str[i] <= 'z')
            str[i] -= 32;
}
/*******************************************************************************
函数名: SearchFile
函数作用:查找文件
输入参数:pfilename 文件名，pfilelen文件长度，root_addr 查找开始地址
返回参数:文件所在地址
*******************************************************************************/
u8 *SearchFile(u8 *pfilename, u16 *pfilelen, u16 *root_addr)
{
    u16 n, sector;
    u8  str_name[11];
    u8 *pdiraddr;

    pdiraddr = ROOT_SECTOR;

    for(n = 0; n < 16; n++)
    {
        memcpy(str_name, pdiraddr, 11);
        Upper(str_name, 11);
        if(memcmp(str_name, pfilename, 11) == 0)
        {
            memcpy((u8 *)pfilelen, pdiraddr + 0x1C, 2);
            memcpy((u8 *)&sector, pdiraddr + 0x1A, 2);
            return (u8 *)FILE_SECTOR + (sector - 2) * 512;
        }

        pdiraddr += 32;
        root_addr++;
    }
    return NULL;
}

static u8 _isSpaceLFCR(const u8 ch)
{
    return (ch == ' ' || ch == '\r' || ch == '\n' );
}

static u8 _isdigit(const u8 ch)
{
    return (ch >= '0' && ch <= '9' );
}

int _atoi(const char *nptr )
{
    int c;              /* current char */
    int total;         /* current total */
    int sign;           /* if '-', then negative, otherwise positive */

    /* skip whitespace */
    while ( _isSpaceLFCR((int)(unsigned char)*nptr) )
        ++nptr;

    c = (int)(unsigned char) * nptr++;
    sign = c;           /* save sign indication */
    if(c == '-' || c == '+')
        c = (int)(unsigned char) * nptr++;  /* skip sign */

    total = 0;

    while(_isdigit(c))
    {
        total = 10 * total + (c - '0');     /* accumulate digit */
        c = (int)(unsigned char) * nptr++;  /* get next char */
    }
    if(sign == '-')     return -total;
    else                return total;   /* return result, negated if necessary */
}

static void _memmove(u8 *dest, const u8 *src, const u16 num)
{
    for(u16 i = 0; i < num; i++)
        *dest++ = *src++;
    return;
}

//去除字符串左右两边的空格、回车和换行
static u8 *StrStrip(u8 *str)
{
    u16 size;
    u8 *p1, *p2;

    size = strlen((char *)str);
    if (!size)
        return str;

    p2 = str + size - 1;

    while((p2 >= str) &&  _isSpaceLFCR(*p2))
        p2 --;
    *(p2 + 1) = '\0';
    p1 = str;
    while(*p1 && _isSpaceLFCR(*p1))
        p1++;
    if(str != p1)
        _memmove(str, p1, p2 - p1 + 2);
    return str;
}

//不区分大小写比较
static int StriCmp(const u8 *s1, const u8 *s2, u16 len)
{
    int ch1, ch2;
    do
    {
        ch1 = *(s1++);
        if((ch1 >= 'A') && (ch1 <= 'Z'))
            ch1 += 0x20;

        ch2 = *(s2++);
        if((ch2 >= 'A') && (ch2 <= 'Z'))
            ch2 += 0x20;
        len --;
    }
    while( len > 0 && (ch1 == ch2) );
    return((ch1 - ch2) && !(len == 0));
}

/*******************************************************************************
函数名: isValidConfg
函数作用:检查格式化的设置文本是否有效数据，并把
                       数据保存到传入的结构中
输入参数:格式化的设置文本，获取设置数据的结构指针
返回参数:1: 有效数据, 0:无效数据
*******************************************************************************/
static u8 isValidConfg(const u8 ini[][ROW_CONT], DEVICE_INFO_SYS *config)
{
    char *valueStr;
    u16 value, T_Unit;

    //摄氏/华氏设置
    if(valueStr = strchr((char *)ini[6], '='))
    {
        value = _atoi((char *)valueStr + 1);
        if(value != TEMP_UNIT_C && value != TEMP_UNIT_F)        return 0;
        T_Unit = value;
    }
    else        return 0;
    config->temp_flag = value;

    //工作温度//休眠温度
    for(u16 i = 0; i < 2; i++)
    {
        if(valueStr = strchr((char *)ini[i], '=')) //
        {
            value = _atoi((char *)valueStr + 1);
        }
        else    return 0;
        
        if(T_Unit == TEMP_UNIT_C)
        {
            if(value < 100 || value > 400)      return 0;
        }
        else //TEMP_UNIT_F
        {
            if(value < 212 || value > 752)      return 0;
        }
        
        if(i == 0)      config->t_standby = value * 10;
        else            config->t_work = value * 10;
    }
    
    //休眠时间
    if(valueStr = strchr((char *)ini[2], '='))
    {
        value = _atoi((char *)valueStr + 1);
    }
    else        return 0;
    
    if(value < 60 || value > 999)       return 0;
    config->wait_time = value * 100;

    //功率设置
    if(valueStr = strchr((char *)ini[3], '='))
    {
        value = _atoi((char *)valueStr + 1);
    }
    else        return 0;
    if(value < 16 || value > POWER_LIMIT)       return 0;
    config->rated_power = value;

    //温度调整步长
    if(valueStr = strchr((char *)ini[4], '='))
    {
        value = _atoi((char *)valueStr + 1);
    }
    else        return 0;
    if(value < 1 || value > 25)         return 0;
    config->t_step = value * 10;

    //保护电压
    if(valueStr = strchr((char *)ini[5], '='))
    {
        value = _atoi((char *)valueStr + 1);
    }
    else        return 0;
    if(value < 100 || value > 150)      return 0;
    config->turn_offv = value; //*10;

    //左右手选择
    if(valueStr = strchr((char *)ini[7], '='))
    {
        value = _atoi((char *)valueStr + 1);
    }
    else        return 0;
    if(value != 0 && value != 1)        return 0;
    config->handers = value;

    //校准值
    if(valueStr = strchr((char *)ini[8], '='))
    {
        value = _atoi((char *)valueStr + 1);
    }
    else        return 0;
    config->zero_ad = value;
    
    //便携移动电源
    if(valueStr = strchr((char *)ini[9], '='))
    {
        value = _atoi((char *)valueStr + 1);
    }
    else        return 0;
    if(value != 0 && value != 1)        return 0;
    config->portable_flag = value;
    
    return 1;
}
/*******************************************************************************
函数名: Config_Analysis
函数作用:检查config.txt的文本是否有效数据 ( 无效数据会恢复成
                       出厂设置，并把格式化后的数据保存到传入的指针
                       并保存设置到结构device_info中
输入参数:config.txt 的文本字串，存储格式化数据的指针
返回参数: 0:无效数据 1:有效数据
*******************************************************************************/
static u8 Config_Analysis(const u8 *p_file , u8 *dest)
{

    u8 is_illegality = 0;
    u16 i, k, pos = 0;
    u8 tmpLine[ROW_CONT + 1];
    u8 t_p[CONFIG_CONT][ROW_CONT];
    DEVICE_INFO_SYS TmpConfig;
    memset(t_p, 0x00, CONFIG_CONT * (ROW_CONT));
    memcpy((u8 *)gFile_con, p_file, 512);

    while(pos < 512)
    {
        i = 0;
        memset(tmpLine, 0x0, ROW_CONT + 1);
        while(i < ROW_CONT && pos < 512)
        {
            tmpLine[i] = gFile_con[pos++];
            if(tmpLine[i] == '\n' || tmpLine[i] == '\r')
            {
                tmpLine[i] = '\0';
                StrStrip(tmpLine);
                if(*tmpLine)
                {
                    for(k = 0; k < CONFIG_CONT; k++)
                    {
                        //分析CONFIG_CONT   行
                        if(StriCmp(tmpLine, gKey_words[k], strlen((char *)gKey_words[k])) == 0)
                        {//找到关键字
                            strcpy((char *)t_p[k], (char *)tmpLine);
                            break;
                        }
                    }
                }
                break;
            }
            if(is_illegality)   break;
            i++;
        }
    }
    if(!is_illegality)
    {
        for(i = 0; i < CONFIG_CONT; i++)
        {
            if(t_p[i][0] == NULL)
            {
                is_illegality = 1;
                break;
            }
        }

        if(!is_illegality)
        {
            is_illegality = !(isValidConfg(t_p, &TmpConfig));
            if(!is_illegality)
            {
                memcpy((char *)&device_info + 16 , (char *)&TmpConfig + 16, sizeof(DEVICE_INFO_SYS) - 16);
                own_sprintf((char *)t_p[0], "StbTemp=%d", device_info.t_standby / 10);
                own_sprintf((char *)t_p[1], "WkTemp=%d", device_info.t_work / 10);
                own_sprintf((char *)t_p[2], "SlpTime=%d", device_info.wait_time / 100);
                own_sprintf((char *)t_p[3], "Rated_P=%d", device_info.rated_power);
                own_sprintf((char *)t_p[4], "TempStp=%d", device_info.t_step / 10);
                own_sprintf((char *)t_p[5], "OffVolt=%d", device_info.turn_offv); ///10);
                own_sprintf((char *)t_p[6], "Temp=%d", device_info.temp_flag);
                own_sprintf((char *)t_p[7], "Hand=%d", device_info.handers);
                own_sprintf((char *)t_p[8], "ZeroP_Ad=%d", device_info.zero_ad);
                own_sprintf((char *)t_p[9], "Portable=%d", device_info.portable_flag);
                gZerop_ad = device_info.zero_ad;
            }
        }
    }

    memset(dest, 0x00, FILE_CONT);
    if(is_illegality)
    {
        for(k = 0; k < CONFIG_CONT; k++)
        {
            strcat((char *)dest, (char *)gDef_set[k]);
            strcat((char *)dest, (char *)gSet_range[k]);
            //gZerop_ad = info_def.zero_ad;
        }
    }
    else
    {
        for(k = 0; k < CONFIG_CONT; k++)
        {
            strcat((char *)dest, (char *)t_p[k]);
            strcat((char *)dest, (char *)gSet_range[k]);
        }
    }

    return is_illegality;
}
/*******************************************************************************
函数名: Config_Save
函数作用:保存设置到config.txt
输入参数:NULL
返回参数:NULL
*******************************************************************************/
u8 Config_Save(void)
{

    u8 str[FILE_CONT];
    u8 tmpLine[ROW_CONT];

    u32 m;
    //u8 is_illegality;
    u16 file_len;
    u8 *p_file;
    u16 root_addr;
    root_addr = 0;

    memset(str, 0x0, FILE_CONT);
    own_sprintf((char *)tmpLine, "StbTemp=%d", device_info.t_standby / 10);
    strcat((char *)str, (char *)tmpLine);
    strcat((char *)str, (char *)gSet_range[0]);

    own_sprintf((char *)tmpLine, "WkTemp=%d", device_info.t_work / 10);
    strcat((char *)str, (char *)tmpLine);
    strcat((char *)str, (char *)gSet_range[1]);

    own_sprintf((char *)tmpLine, "SlpTime=%d", device_info.wait_time / 100);
    strcat((char *)str, (char *)tmpLine);
    strcat((char *)str, (char *)gSet_range[2]);

    own_sprintf((char *)tmpLine, "Rated_P=%d", device_info.rated_power);
    strcat((char *)str, (char *)tmpLine);
    strcat((char *)str, (char *)gSet_range[3]);

    own_sprintf((char *)tmpLine, "TempStp=%d", device_info.t_step / 10);
    strcat((char *)str, (char *)tmpLine);
    strcat((char *)str, (char *)gSet_range[4]);

    own_sprintf((char *)tmpLine, "OffVolt=%d", device_info.turn_offv);
    strcat((char *)str, (char *)tmpLine);
    strcat((char *)str, (char *)gSet_range[5]);

    own_sprintf((char *)tmpLine, "Temp=%d", device_info.temp_flag);
    strcat((char *)str, (char *)tmpLine);
    strcat((char *)str, (char *)gSet_range[6]);

    own_sprintf((char *)tmpLine, "Hand=%d", device_info.handers);
    strcat((char *)str, (char *)tmpLine);
    strcat((char *)str, (char *)gSet_range[7]);

    own_sprintf((char *)tmpLine, "ZeroP_Ad=%d", device_info.zero_ad);
    strcat((char *)str, (char *)tmpLine);
    strcat((char *)str, (char *)gSet_range[8]);
    
    own_sprintf((char *)tmpLine, "Portable=%d", device_info.portable_flag);
    strcat((char *)str, (char *)tmpLine);
    strcat((char *)str, (char *)gSet_range[9]);

    if(p_file = SearchFile("CONFIG  TXT", &file_len, &root_addr))
    {
        m = strlen((char *)str);
        gDisk_buff[0x400 + root_addr * 32 + 0x1C] = m % 256;
        gDisk_buff[0x400 + root_addr * 32 + 0x1D] = m / 256;

        gRewriteflag[(p_file - ROOT_SECTOR + 0x200) / 0x400] = 1;
        memcpy(p_file, str, strlen((char *)str));
        ReWriteFlsash();
    }
    else
    {
        if(p_file = SearchFile("LOGOIN  BMP", &file_len, &root_addr))
        {
            memcpy(str, p_file, 254);
            memset(gDisk_buff, 0x00, 0x2600);
            memcpy(ROOT_SECTOR + 32, "LOGOIN  BMP", 0xC);
            memcpy(FILE_SECTOR + 512, str, 254);
            gDisk_buff[0x40B + 32] = 0x0; //属性
            *(u32 *)(VOLUME_BASE + 32) = VOLUME;
            gDisk_buff[0x41A + 32] = 0x03;//簇号
            gDisk_buff[0x41C + 32] = 254;//文件大小
        }
        else    memset(gDisk_buff, 0x00, 0x2600);

        memcpy(ROOT_SECTOR, "CONFIG  TXT", 0xC);
        memcpy(FAT1_SECTOR, gFat_data, 6);
        memcpy(FAT2_SECTOR, gFat_data, 6);

        m = strlen((char *)str);
        memcpy(FILE_SECTOR, str, m);
        gDisk_buff[0x40B] = 0x0; //属性
        *(u32 *)VOLUME_BASE = VOLUME;
        gDisk_buff[0x41A] = 0x02;//簇号
        gDisk_buff[0x41C] = m % 256; //文件大小
        gDisk_buff[0x41D] = m / 256;
        ReWrite_All();
    }

    gVar[F_TYPE]   = HEX;
    gVar[F_FLAG]   = RDY;
    gVar[SEG_ST]   = 0;
    gV32[OFFSET]   = 0;
    gV32[COUNT]    = 0;
    gV32[WR_CNT]   = 0;
    gV32[RD_CNT]   = 0;
    return 0;
}
/*******************************************************************************
函数名: Config_Read
函数作用:配置文件分析
输入参数:NULL
返回参数:NULL
*******************************************************************************/
u8 Config_Read(void)
{
    u32 k, m;
    u8 str[FILE_CONT];
    u8 is_illegality;
    u16 file_len;
    u8 *p_file;
    u16 root_addr;
    root_addr = 0;

    if(p_file = SearchFile("CONFIG  TXT", &file_len, &root_addr))
    {
        is_illegality = Config_Analysis(p_file, str);
        if(is_illegality)
        {
            m = strlen((char *)str);
            gDisk_buff[0x400 + root_addr * 32 + 0x1C] = m % 256;
            gDisk_buff[0x400 + root_addr * 32 + 0x1D] = m / 256;

            gRewriteflag[(p_file - ROOT_SECTOR + 0x200) / 0x400] = 1;
            memcpy(p_file, str, strlen((char *)str));
            ReWriteFlsash();
        }
    }
    else
    {
        if(p_file = SearchFile("LOGOIN  BMP", &file_len, &root_addr))
        {
            memcpy(str, p_file, 254);
            memset(gDisk_buff, 0x00, 0x2600);
            memcpy(ROOT_SECTOR + 32, "LOGOIN  BMP", 0xC);
            memcpy(FILE_SECTOR + 512, str, 254);
            gDisk_buff[0x40B + 32] = 0x0; //属性
            *(u32 *)(VOLUME_BASE + 32) = VOLUME;
            gDisk_buff[0x41A + 32] = 0x03;//簇号
            gDisk_buff[0x41C + 32] = 254;//文件大小
        }
        else    memset(gDisk_buff, 0x00, 0x2600);
        
        memcpy(ROOT_SECTOR, "CONFIG  TXT", 0xC);
        memcpy(FAT1_SECTOR, gFat_data, 6);
        memcpy(FAT2_SECTOR, gFat_data, 6);

        m = 0;
        for(k = 0; k < CONFIG_CONT; k++)
        {
            memcpy(FILE_SECTOR + m, gDef_set[k], strlen((char *)gDef_set[k]));
            m += strlen((char *)gDef_set[k]);
            memcpy(FILE_SECTOR + m, gSet_range[k], strlen((char *)gSet_range[k]));
            m += strlen((char *)gSet_range[k]);
        }

        gDisk_buff[0x40B] = 0x0; //属性
        *(u32 *)VOLUME_BASE = VOLUME;
        gDisk_buff[0x41A] = 0x02;//簇号
        gDisk_buff[0x41C] = m % 256; //文件大小
        gDisk_buff[0x41D] = m / 256;
        ReWrite_All();
    }

    gVar[F_TYPE]   = HEX;
    gVar[F_FLAG]   = RDY;
    gVar[SEG_ST]   = 0;
    gV32[OFFSET]   = 0;
    gV32[COUNT]    = 0;
    gV32[WR_CNT]   = 0;
    gV32[RD_CNT]   = 0;
    return 0;
}
/*******************************************************************************
函数名: Disk_SecWrite
函数作用:PC 保存文件时写磁盘
输入参数:pbuffer 保存内容 diskaddr 地址
返回参数:NULL
*******************************************************************************/
void Disk_SecWrite(u8 *pbuffer, u32 diskaddr)//PC 保存数据调用
{
    u8 i;
    u8 is_illegality = 0;
    u8 ver[20];
    static u16 Config_flag = 0;
    static u8 txt_flag = 0;
    u8 config_filesize = 0;
    u8 str[FILE_CONT];
    u8 *p_file;
    u32  m = 0;
    
    if(diskaddr == 0x1000)                                 // Write FAT1 sector
    {
        if(memcmp(pbuffer, (u8 *)FAT1_SECTOR, 512))
        {
            memcpy((u8 *)FAT1_SECTOR, pbuffer, 512);
        }
    }
    else if(diskaddr == 0x2800)                            // Write FAT2 sector
    {
        if(memcmp(pbuffer, (u8 *)FAT2_SECTOR, 512))
        {
            memcpy((u8 *)FAT2_SECTOR, pbuffer, 512);
        }
    }
    else if(diskaddr == 0x4000)                            // Write DIR sector
    {
        if(memcmp(pbuffer, (u8 *)ROOT_SECTOR, 512))
        {
            memcpy((u8 *)ROOT_SECTOR, pbuffer, 512);
            gRewriteflag[1] = 1;
            for(i = 0; i < 16; i++)
            {
                memcpy((u8 *)ver, (u8 *)(pbuffer), 12);
                if(memcmp(ver, "CONFIG  TXT", 11) == 0)
                {
                    Config_flag = pbuffer[0x1A];
                    config_filesize = pbuffer[0x1C];
                    txt_flag = 1;
                    break;
                }
                pbuffer += 32;
            }
            if(config_filesize == 0 && txt_flag == 1)
            {
                txt_flag = 0;
                gRewriteflag[1] = 0;
                gRewriteflag[0] = 0;
            }
            else        gRewriteflag[0] = 1;
        }
    }
    else if(diskaddr >= 0x8000 && diskaddr <= 0xA000)                            // Write FILE sector
    {
        if(memcmp(pbuffer, (u8 *)(FILE_SECTOR + (diskaddr - 0x8000)), 512))
        {
            memcpy((u8 *)(FILE_SECTOR + (diskaddr - 0x8000)), pbuffer, 512);
        }
        if((((diskaddr - 0x8000) / 0x200) + 2) == Config_flag) //簇号
        {
            //is_illegality = Setting_Analysis((u8*)(FILE_SECTOR + (diskaddr - 0x8000)),(Config_flag - 2));
            p_file = (u8 *)(FILE_SECTOR + (diskaddr - 0x8000));
            is_illegality = Config_Analysis(p_file, str);
            if(!is_illegality)
            {
                m = strlen((char *)str);
                gDisk_buff[0x400 + (Config_flag - 2) * 32 + 0x1C] = m % 256;
                gDisk_buff[0x400 + (Config_flag - 2) * 32 + 0x1D] = m / 256;
                gRewriteflag[(p_file - ROOT_SECTOR + 0x200) / 0x400] = 1;
                memcpy(p_file, str, strlen((char *)str));
                ReWriteFlsash();
                return ;
            }
            else        return ;
        }
        else
        {
            gRewriteflag[((diskaddr - 0x8000 + 0x200) / 0x400) + 1] = 1;
        }
    }
    ReWriteFlsash();
}
/*******************************************************************************
函数名: Disk_SecRead
函数作用:PC 读取文件时写磁盘
输入参数:pbuffer 读取内容 diskaddr 地址
返回参数:NULL
*******************************************************************************/
void Disk_SecRead(u8 *pbuffer, u32 disk_addr)
{
    Soft_Delay();
    if(disk_addr == 0x0000)                                 // Read BOOT sector
    {
        memcpy(pbuffer, BOOT_SEC, 512);
    }
    else if(disk_addr == 0x1000)                            // Read FAT1 sector
    {
        memcpy(pbuffer, FAT1_SECTOR, 512);
    }
    else if(disk_addr == 0x2800)                            // Read FAT2 sector
    {
        memcpy(pbuffer, FAT2_SECTOR, 512);
    }
    else if(disk_addr == 0x4000)                            // Read DIR sector
    {
        memcpy(pbuffer, (u8 *)(ROOT_SECTOR), 512);
    }
    else if(disk_addr >= 0x8000 && disk_addr <= 0xA000)                            // Read FILE sector
    {
        memcpy(pbuffer, (u8 *)(APP_BASE + 0x600 + (disk_addr - 0x8000)), 512);
    }
    else        memset(pbuffer, 0, 512);
}
/*******************************************************************************
函数名: ReWriteFlsash
函数作用:写Flash
输入参数:NULL
返回参数:是否写入标志
*******************************************************************************/
u8 ReWriteFlsash(void)
{
    u32 i, j;
    u8  result;
    u16 *f_buff;

    FLASH_Unlock();
    for(i = 0; i < 16; i++)
    {
        if(gRewriteflag[i])
        {
            gRewriteflag[i] = 0;
            FLASH_Erase(APP_BASE + i * 0x400);
            f_buff = (u16 *)&gDisk_buff[i * 0x400];
            for(j = 0; j < 0x400; j += 2)
            {
                result = FLASH_Prog((u32)(APP_BASE + i * 0x400 + j), *f_buff++);
                if(result != FLASH_COMPLETE)
                {
                    FLASH_Lock();
                    return ERR;
                }
            }
            break;
        }
    }
    FLASH_Lock();
    
    return RDY;
}
/*******************************************************************************
函数名: ReWrite_All
函数作用:擦除整个flash，重写数据
输入参数:NULL
返回参数:是否写入标志
*******************************************************************************/
u8 ReWrite_All(void)
{
    u16 i;
    u8  result;
    u16 *f_buff = (u16 *)gDisk_buff;

    FLASH_Unlock();
    for(i = 0; i < 9; i++)      FLASH_Erase(APP_BASE + i * 0x400);
    for(i = 0; i < 0X2600; i += 2)
    {
        result = FLASH_Prog((u32)(APP_BASE + i), *f_buff++);
        if(result != FLASH_COMPLETE)  return ERR;
    }
    FLASH_Lock();
    
    return RDY;
}
/*******************************************************************************
函数名: Erase
函数作用:格式化Flash
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Erase(void)
{
    u16 i;
    FLASH_Unlock();
    for(i = 0; i < 9; i++)FLASH_Erase(APP_BASE + i * 0x400);

    FLASH_Lock();
}
/*******************************************************************************
函数名: Read_Memory
函数作用:处理microSD卡的读操作。
输入参数:r_offset 偏移 r_length长度
返回参数:NULL
*******************************************************************************/
void Read_Memory(u32 r_offset, u32 r_length)
{
    static u32 offset, length, block_offset;

    if(gVar[USB_ST] == TXFR_IDLE )
    {
        offset = r_offset * SECTOR_SIZE;
        length = r_length * SECTOR_SIZE;
        gVar[USB_ST] = TXFR_ONGOING;
    }
    if(gVar[USB_ST] == TXFR_ONGOING )
    {
        if (!gV32[RD_CNT])
        {
            Disk_SecRead(gBuff, offset);
            UserToPMABufferCopy(gBuff, ENDP1_TXADDR, BULK_MAX_PACKET_SIZE);
            gV32[RD_CNT] = SECTOR_SIZE - BULK_MAX_PACKET_SIZE;
            block_offset = BULK_MAX_PACKET_SIZE;
        }
        else
        {
            UserToPMABufferCopy(gBuff + block_offset, ENDP1_TXADDR, BULK_MAX_PACKET_SIZE);
            gV32[RD_CNT] -= BULK_MAX_PACKET_SIZE;
            block_offset += BULK_MAX_PACKET_SIZE;
        }
        SetEPTxCount(ENDP1, BULK_MAX_PACKET_SIZE);
        SetEPTxStatus(ENDP1, EP_TX_VALID);
        offset += BULK_MAX_PACKET_SIZE;
        length -= BULK_MAX_PACKET_SIZE;

        CSW.dDataResidue -= BULK_MAX_PACKET_SIZE;
    }
    
    if(length == 0)
    {
        gV32[RD_CNT] = 0;
        block_offset = 0;
        offset = 0;
        Bot_State = BOT_DATA_IN_LAST;
        gVar[USB_ST] = TXFR_IDLE;
    }
}
/*******************************************************************************
函数名: Write_Memory
函数作用:处理microSD卡的写操作。
输入参数:r_offset 偏移 r_length长度
返回参数:NULL
*******************************************************************************/
void Write_Memory (u32 w_offset, u32 w_length)
{
    static u32 offset, length;
    u32 idx, temp = gV32[WR_CNT] + 64;

    if(gVar[USB_ST] == TXFR_IDLE )
    {
        offset = w_offset * SECTOR_SIZE;
        length = w_length * SECTOR_SIZE;
        gVar[USB_ST] = TXFR_ONGOING;
    }
    if(gVar[USB_ST] == TXFR_ONGOING )
    {
        for(idx = 0 ; gV32[WR_CNT] < temp; gV32[WR_CNT]++)
            *(u8 *)(gBuff + gV32[WR_CNT]) = Bulk_Buff[idx++];

        offset += Data_Len;
        length -= Data_Len;

        if(!(length % SECTOR_SIZE))
        {
            gV32[WR_CNT] = 0;
            Disk_SecWrite(gBuff, offset - SECTOR_SIZE);
        }

        CSW.dDataResidue -= Data_Len;
        SetEPRxStatus(ENDP2, EP_RX_VALID); /* enable the next transaction*/
    }
    if((length == 0) || (Bot_State == BOT_CSW_Send))
    {
        gV32[WR_CNT] = 0;
        Set_CSW (CSW_CMD_PASSED, SEND_CSW_ENABLE);
        gVar[USB_ST] = TXFR_IDLE;
    }
}
/*******************************************************************************
函数名: Restore_Setting
函数作用:恢复出厂设备
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Restore_Setting(void)
{
    System_Init();
    Clear_Screen();
    Init_Oled();
    Clear_Screen();
    gCalib_flag = 1;
    Disk_BuffInit();
    Config_Save();//保存设置到config.txt
    gCalib_flag = 0;
    Clear_Watchdog();
    Display_Str8(0, "Completed.", 0);
    Delay_Ms(1000);
    Clear_Watchdog();
    Delay_Ms(1000);
}
/*********************************  END OF FILE  ******************************/
