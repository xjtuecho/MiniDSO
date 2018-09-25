/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
File Name :      HARDWARE.c
Version :        TS080 APP 1.02
Description:
Author :         Ning
Data:            2017/11/06
History:
2017/10/16   修改电压限制数据；
2017/10/27   修改温度计算公式；
2017/11/06   修改警号判断语句
2017/11/07   修改电压报警判断
2017/12/01   修改加热时温度计算
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "APP_Version.h"
#include "HARDWARE.h"
#include "CTRL.h"
#include "bios.h"
#include "HARDWARE.h"
#include "UI.h"
/******************************************************************************/

/*----------------开机上限制----下限-----加热上线---下限------------------*/
//const u16 gRate[] = {60,        40,       100,       80};//2017.10.16修改电压限制
static vu32 gKey_in;
s32  gZerop_ad = 102;
u32  gTurn_offv = 130;//2018.3.26 10V修改为13V关机电压
u32  gTurn_mixv = 60;
u8   gCalib_flag = 0;
//vu16 gMeas_cnt= 0;/* Measure*/

vu8  gLongkey_flag = 0;
u8   gAlarm_type = 1;
//u8   keypress_cnt = 0;
u8   MarkAd_pos = 0;//2017.12.1
u32  slide_data = 0;//2017.12.5平均ad返回值
//u8   Pid_num = 5;//pid调节数值
/*******************************************************************************
函数名: Get_CalFlag
函数作用:读取校准状态
输入参数:NULL
返回参数:校准状态标志
*******************************************************************************/
u32 Get_CalFlag(void)
{
    return gCalib_flag;
}
/*******************************************************************************
函数名: Get_gKey
函数作用:获取按键状态
输入参数:NULL
返回参数:按键状态
*******************************************************************************/
u32 Get_gKey(void)
{
    u32 key;
    key = gKey_in;
    gKey_in = NO_KEY;
    return key;
}
/*******************************************************************************
函数名: Set_gKey
函数作用:设置按键状态
输入参数:要设置的按键状态
返回参数:NULL
*******************************************************************************/
static void Set_gKey(u32 key)
{
    gKey_in = key;
}
/*******************************************************************************
函数名: Set_LongKeyFlag
函数作用:设置长按键标志
输入参数:0 :不可以长按键 1: 可以长按
返回参数:NULL
*******************************************************************************/
void Set_LongKeyFlag(u32 flag)
{
    gLongkey_flag = (flag > 0);
    return;
}

void Clear_LongKey(void)
{
    gLongkey_flag = 0;
    Set_gKey(NO_KEY);
    Delay_Ms(50);
    gLongkey_flag = 1;
    return;
}
/*******************************************************************************
函数名: Get_AlarmType
函数作用:获取报警类型
输入参数:NULL
返回参数: 警告类型
          0:正常
          1:sen - err
          2:超温
          3:超压
*******************************************************************************/
u8 Get_AlarmType(void)
{
    return gAlarm_type;
}
/*******************************************************************************
函数名: Read_Vb
函数作用:读取电源电压值
输入参数:标志
返回参数:0:正常    1:  电压高了   2:电压低了
*******************************************************************************/
int Read_Vb(void)
{
    u32 tmp = 0;
    tmp = Vol_Get();

    if(tmp > gTurn_offv * 10)
    {
        gAlarm_type = HIGH_VOLTAGE;
        return 1;
    }
    if(tmp < gTurn_mixv * 10)
    {
        gAlarm_type = LOW_VOLTAGE;
        return 2;
    }
    return 0;
}
/*******************************************************************************
函数名: Key_Read
函数作用:读取按键
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Key_Read(void)
{
    static u16 press_buttonA = 0, press_buttonB = 0;
    static u16 buttonA_cnt = 0, buttonB_cnt = 0;
    static u16 kgap = 0; //key gap 两次按键的空隙
    static u8 longkey_flag_a = 1;
    static u8 longkey_flag_b = 1;
    static u16 longkey_a = 0, longkey_b = 0; //长按标志，产生长按键后未释放按键前不计算按键时长
    u16 read_data = 0;

    read_data = KEYA_READ;
    press_buttonA = read_data;//长按

    read_data = KEYB_READ;
    press_buttonB = read_data;//长按
    longkey_flag_a &= gLongkey_flag;
    longkey_flag_b &= gLongkey_flag;

    if(press_buttonA == KEY1_PIN )
    {
        kgap = 0;
        buttonA_cnt++;
    }
    if(press_buttonB == KEY2_PIN )
    {
        kgap = 0;
        buttonB_cnt++;
    }

    //    if(buttonA_cnt >= SI_DATA && buttonB_cnt >= SI_DATA && gLongkey_flag) Set_gKey(KEY_CN | KEY_V3);
    //    else {
    if(buttonA_cnt >= SI_DATA && gLongkey_flag && longkey_flag_a && buttonB_cnt == 0)
    {
        if(device_info.handers)         Set_gKey(KEY_CN | KEY_V1);
        else                            Set_gKey(KEY_CN | KEY_V2);
        //buttonA_cnt = 0;
        longkey_a	= 1;
    }
    if(buttonB_cnt >= SI_DATA && gLongkey_flag && longkey_flag_b && buttonA_cnt == 0)
    {
        if(device_info.handers)         Set_gKey(KEY_CN | KEY_V2);
        else                            Set_gKey(KEY_CN | KEY_V1);
        //buttonB_cnt = 0;
        longkey_b	= 1;
    }
    //    }

    if(press_buttonA == 0)
    {
        if(!longkey_a)
        {
            if(buttonA_cnt < SI_DATA && buttonA_cnt != 0)
            {
                if(device_info.handers)         Set_gKey(KEY_V1);
                else                            Set_gKey(KEY_V2);
            }
            kgap++;
        }
        else    longkey_a = 0;

        if(buttonA_cnt == 0)    longkey_flag_a = 1;
        buttonA_cnt = 0;
    }

    if(press_buttonB == 0)
    {
        if(!longkey_b)
        {
            if(buttonB_cnt < SI_DATA && buttonB_cnt != 0)
            {
                if(device_info.handers)     Set_gKey(KEY_V2);
                else                        Set_gKey(KEY_V1);
            }
            kgap++;
        }
        else    longkey_b = 0;
        if(buttonB_cnt == 0)    longkey_flag_b = 1;
        buttonB_cnt = 0;
    }
}
/*******************************************************************************
函数名: Get_SlAvg
函数作用:滑动平均值
输入参数:avg_data 平均的AD值
返回参数:滑动平均值
*******************************************************************************/
u32 Get_SlAvg(u32 avg_data)
{
    static u32 sum_avg = 0;
    static u8 init_flag = 0;
    u16 si_avg = sum_avg / SI_COE, abs;

    if(init_flag == 0)   /*第一次上电*/
    {
        sum_avg = SI_COE * avg_data;
        init_flag = 1;
        return sum_avg / SI_COE;
    }
    if (avg_data > si_avg)      abs = avg_data - si_avg;
    else                        abs = si_avg - avg_data;

    if(abs > SI_THRESHOLD)      sum_avg =   SI_COE * avg_data;//一次AD跳动超过60视为无效
    else                        sum_avg +=  avg_data  - sum_avg / SI_COE;

    return sum_avg / SI_COE;
}
/*******************************************************************************
函数名: Get_AvgAd
函数作用:获取并计算热端AD平均值
输入参数:NULL
返回参数:AD平均值
*******************************************************************************/
u32 Get_AvgAd(void)
{
    Set_HeatingTime(0);
    HEAT_OFF();
    /*--2017-12-1--停止加热5ms后开始获取ad值1ms后停止获取--*/
    Delay_HalfMs(10);//10
    MarkAd_pos = 1;
    Delay_HalfMs(2);
    MarkAd_pos = 0;

    return slide_data;
}
/*******************************************************************************
函数名: Get_TempSlAvg
函数作用:冷端温度滑动平均值
输入参数:avg_data 冷端温度平均值
返回参数:冷端温度滑动平均值
*******************************************************************************/
int Get_TempSlAvg(int avg_data)
{
    static int sum_avg = 0;
    static u8 init_flag = 0;

    if(init_flag == 0)   /*第一次上电*/
    {
        sum_avg = 8 * avg_data;
        init_flag = 1;
        return sum_avg / 8;
    }
    
    sum_avg += avg_data - sum_avg / 8;

    return sum_avg / 8;
}
/*******************************************************************************
函数名: Get_SensorTmp
函数作用:获取冷端温度
输入参数:pos: 0:温度 1:ad
返回参数:获取冷端温度
*******************************************************************************/
int Get_SensorTmp(u8 pos)
{
    static u32 ad_sum = 0;
    static u32 max = 0, min = 5000;
    u32 ad_value, avg_data, slide_data;
    int sensor_temp = 0;
    u16 meas_cnt;

    meas_cnt = 10;

    while(meas_cnt > 0)
    {
        ad_value = Get_AdcValue(1);
        ad_sum += ad_value;
        if(ad_value > max)   max = ad_value;
        if(ad_value < min)   min = ad_value;

        if(meas_cnt == 1)
        {
            ad_sum    = ad_sum - max - min;
            avg_data  = ad_sum / 8;

            slide_data = Get_TempSlAvg(avg_data);
            if(!pos)    sensor_temp = ((3300 * slide_data / 4096) - 500); //(25 + ((10*(33*gSlide_data)/4096)-75));
            ad_sum = 0;
            min = 5000;
            max = 0;
        }
        meas_cnt--;
    }
    if(pos)
    {
        return slide_data;
    }
    return sensor_temp;
}
/*******************************************************************************
函数名: Zero_Calibration
函数作用:校准零点AD
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Zero_Calibration(void)
{
    u32 zerop;
    int cool_tmp;

    zerop = Get_AvgAd();
    cool_tmp = Get_SensorTmp(0);

    //平均温度大于等于400
    if(zerop >= 400)    gCalib_flag = 2;//校验失败
    else
    {
        //if(_abs(zerop, cool_tmp) <100) {
        if(cool_tmp < 300)  //冷却端温度小于300
        {
            gZerop_ad = zerop;
            gCalib_flag = 1;
        }
        else    gCalib_flag = 2;//校验失败
    }
}
/*******************************************************************************
函数名: Get_Temp
函数作用:根据冷端,热端温度,补偿AD计算温度
输入参数:wk_temp 工作温度
返回参数:实际温度
*******************************************************************************/
s32 Get_Temp(void)
{
    int ad_value, cool_tmp;
    static u16 h_cnt = 0;
    s32 rl_temp = 0;

    ad_value = Get_AvgAd();//热端
    cool_tmp = Get_SensorTmp(0);//冷端

    if(cool_tmp > 300)  cool_tmp = 300;
    
    rl_temp = ((ad_value - gZerop_ad) * 904 / (650 + ((ad_value - gZerop_ad) / 18))) + cool_tmp;
    
    if(ad_value > 3600 && ad_value < 4000)      h_cnt++;//错误报警
    else                                        h_cnt = 0;
    
    if(h_cnt >= 5)              gAlarm_type = HIGH_TEMP;
    else                        gAlarm_type = NORMAL_TEMP;
    if(ad_value > 4000)         gAlarm_type = SEN_ERR;
    
    return rl_temp;
}
/*******************************************************************************
函数名: Start_Watchdog
函数作用:初始化开门狗
输入参数:ms 开门狗计数
返回参数:返回1
*******************************************************************************/
u32 Start_Watchdog(u32 ms)
{
    /* 使能写保护0x5555 */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz (min:0.8ms -- max:3276.8ms */
    IWDG_SetPrescaler(IWDG_Prescaler_32);

    /* Set counter reload value to XXms */
    IWDG_SetReload(ms * 10 / 8);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
    return 1;
}
/*******************************************************************************
函数名: Clear_Watchdog
函数作用:重置开门狗计数
输入参数:NULL
返回参数:返回1
*******************************************************************************/
u32 Clear_Watchdog(void)
{
    IWDG_ReloadCounter();
    return 1;
}
/******************************** END OF FILE *********************************/
