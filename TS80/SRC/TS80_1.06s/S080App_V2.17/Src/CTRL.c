/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
File Name :      CTRL.c
Version :        TS080 APP 1.02
Description:
Author :         Ning
Data:            2017/11/06
History:
2017.11.01   修改PID温控系数;
2017/11/07   修改电压报警判断;
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "CTRL.h"
#include "Bios.h"
#include "UI.h"
#include "HARDWARE.h"
#include "S100V0_1.h"
#include "Disk.h"
#include "MMA8652FC.h"
#include "I2C.h"
#include "Oled.h"
#include "defaults.h"
#include "own_sprintf.h"

/******************************************************************************/
extern vu16 AWD_entry;
extern u32 Resistance;
extern u8  frequency_pos;
extern u32 slide_data;
extern u8  MarkAd_pos;
extern u8 gAlarm_type;
/******************************************************************************/
DEVICE_INFO_SYS device_info;
static u8 gCtrl_status = START;//当前模式标志位
static u8 gPre_status ;
static u8 gFirst_entry = 0;
static u16 gHt_flag = 0;//温度状态标志位
static u8  Adapt_pos;
vu8 MMA_INT1;
vu32 Timer_Counter;

const DEVICE_INFO_SYS info_def =
{
    "1.06",     //Ver
    STBTEMP_VAL * 10,           //T_Standby;    // 200°C=1800  2520,待机温度
    WKTEMP_VAL * 10,            // T_Work;      // 350°C=3362, 工作温度
    TEMPSTP_VAL * 10,           //T_Step;
    SLPTIME_VAL * 100,          //Wait_Time;    //3*60*100   3  mintute
    RP_VAL,                     //;Rated_Power  //18W
    USMODE_VAL,                 //handers
    TEMPUNIT_VAL,               //摄氏/华氏
    OFFVOLT_VAL * 10 ,          //保护电压
    ZEROCALI_VAL,               //校准AD值
    PORTABLE_VAL                //便携电源
};
struct _pid
{
    s16 settemp;        //定义设定温度
    s16 actualtemp;     //定义实际温度
    s16 err;            //定义温度差值
    s16 err_last;       //定义上一个温度差值
    s32 ht_time;        //定义加热时间
    u16 kp, ki, kd;     //定义比例、积分、微分系数
    s32 integral;       //定义积分值
} pid;

unsigned int Timer_StartValue(void)
{
    // 返回初始值
    return Timer_Counter;
}

unsigned int Timer_ElapseValue(const unsigned int start)
{
    // 根据初始值计算流逝时间。以 ms 为单位
    if (Timer_Counter < start)
    {
        // 计时器已经回滚
        return 0xffffffff - start + Timer_Counter;
    }
    else
    {
        return Timer_Counter - start;
    }
}

unsigned int Timer_ElapseSecond(const unsigned int start)
{
    // 根据初始值计算流逝时间。以 ms 为单位
    if (Timer_Counter < start)
    {
        // 计时器已经回滚
        return (0xffffffff - start + Timer_Counter) / 100;
    }
    else
    {
        return (Timer_Counter - start) / 100;
    }
}
/*******************************************************************************
函数名: newSqrt
函数作用:精确到0.001的开平方
输入参数:double:n
返回参数:输入数开平方
*******************************************************************************/
double newSqrt(double n)
{
    double low, high, mid, tmp;

    // 获取上下界
    if (n > 1)
    {
        low = 1;
        high = n;
    }
    else
    {
        low = n;
        high = 1;
    }
    // 二分法求开方
    while (low <= high)
    {
        mid = (low + high) / 2.000;
        tmp = mid * mid;
        if (tmp - n <= 0.001 && tmp - n >= 0.001 * -1)
        {
            return mid;
        }
        else if (tmp > n)
        {
            high = mid;
        }
        else
        {
            low = mid;
        }
    }
    return -1.000;
}

/*******************************************************************************
函数名: Get_Ctrl_Status
函数作用:获取当前状态
输入参数:无
返回参数:当前状态
*******************************************************************************/
u8 Get_CtrlStatus(void)
{
    return gCtrl_status;
}
/*******************************************************************************
函数名: Set_CtrlStatus
函数作用:设置当前状态
输入参数:status 设置的状态
返回参数:无
*******************************************************************************/
void Set_CtrlStatus(u8 status)
{
    gPre_status = gCtrl_status;
    gCtrl_status = status;
    gFirst_entry	= 1;
}
/*******************************************************************************
函数名: Get_HtFlag
函数作用:获取当前加热标志,加热标志由加热时间决定
输入参数:NULL
返回参数:当前加热标志
*******************************************************************************/
u16 Get_HtFlag(void)
{
    return gHt_flag;
}

/*******************************************************************************
函数名: System_Init
函数作用: 系统初始化
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void System_Init(void)
{
    memcpy((void *)&device_info, (void *)&info_def, sizeof(device_info));
}
/*******************************************************************************
函数名: Vol_Set
函数作用:电压切换设置
输入参数:pos 1:降为5V   0:升为工作电压  2：升0.2V  3：下0.2V
返回参数:NULL
*******************************************************************************/
void Vol_Set(u8 pos)
{
    if(pos == 1)
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_3);
        GPIO_ResetBits(GPIOA, GPIO_Pin_10);
        GPIO_ResetBits(GPIOA, GPIO_Pin_8);
        Delay_Ms(V_TIMER);
    }
    else if(pos == 0)//9V
    {
        if(Adapt_pos)
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_3);
            GPIO_SetBits(GPIOA, GPIO_Pin_10);
        }
        else
        {
            GPIO_ResetBits(GPIOA, GPIO_Pin_8);
            GPIO_SetBits(GPIOB, GPIO_Pin_3);
            GPIO_SetBits(GPIOA, GPIO_Pin_10);
            Delay_Ms(V_TIMER);
        }
    }
    else if(pos == 2)//+0.2
    {
        /*----------------------升0.2V----------------------*/
        GPIO_ResetBits(GPIOB, GPIO_Pin_3);
        GPIO_SetBits(GPIOA, GPIO_Pin_8);
        GPIO_SetBits(GPIOA, GPIO_Pin_10);
        Delay_Ms(V_TIMER);
        GPIO_SetBits(GPIOB, GPIO_Pin_3);
        Delay_Ms(V_TIMER);
        GPIO_ResetBits(GPIOB, GPIO_Pin_3);
        /*--------------------------------------------------*/

    }
    else if(pos == 3)//-0.2
    {
        /*----------------------降0.2V----------------------*/
        GPIO_ResetBits(GPIOB, GPIO_Pin_3);
        GPIO_SetBits(GPIOA, GPIO_Pin_8);
        GPIO_SetBits(GPIOA, GPIO_Pin_10);
        Delay_Ms(V_TIMER);
        GPIO_ResetBits(GPIOA, GPIO_Pin_8);
        Delay_Ms(V_TIMER);
        GPIO_SetBits(GPIOA, GPIO_Pin_8);
        /*--------------------------------------------------*/
    }
}
/*******************************************************************************
函数名:  Frequency_Set
函数作用:频率设置
输入参数:pos 0:8M  1:48M
返回参数:NULL
*******************************************************************************/
void Frequency_Set (u8 pos)
{

    USB_Port(DISABLE);
    return;	//always 48MHz
    /*   //next version support
        if(pos)    frequency_pos = 0;
        else       frequency_pos = 1;
        RCC_Config();
        Init_Timer2();
        Init_Timer3();
    */
}
/*******************************************************************************
函数名: Get_Resistance
函数作用:内阻获取
输入参数:NULL
返回参数:内阻
*******************************************************************************/
u32 Get_Resistance(void)
{
    u8 i, j;
    static u32 slide_sum = 0;
    
    for(j = 0 ; j < 2; j++)
    {
        slide_sum = 0;
        for(i = 0 ; i < 3; i++)
        {
            GPIO_SetBits(GPIOA, GPIO_Pin_0);
            Delay_Ms(5);
            MarkAd_pos = 0;
            Delay_Ms(5);
            slide_sum = slide_sum + slide_data;
            GPIO_ResetBits(GPIOA, GPIO_Pin_0);
            Delay_Ms(5);
            MarkAd_pos = 1;
            Delay_Ms(5);
            slide_sum = slide_sum - slide_data;
            Clear_Watchdog();

        }
    }
    Resistance = (slide_sum * 533000 / (4096 * 9)) / 22;//旧公式
    //Resistance = (slide_sum * 2200000) / 810 / 4096;
    
    if(Resistance / 100 > 10)
        Get_Resistance();
    
    return Resistance;
}
/*******************************************************************************
函数名: Vol_Get
函数作用:电压获取
输入参数:NULL
返回参数:NULL
*******************************************************************************/
u32 Vol_Get(void)
{
    u32 tmp, i, sum = 0;

    for(i = 0; i < 10; i++)
    {
        tmp = ADC_GetConversionValue(ADC2);
        sum += tmp;
    }
    tmp = sum / 10;
    tmp = ((tmp * 330) / 4096) * 137 / 27; //电压vb = 3.3 * 85 *ad * 3/ 40950 * 14扩大100倍
    //tmp = ((tmp*330/4096)*(2270/27))/10;
    return tmp;
}
/*******************************************************************************
函数名: PID_init
函数作用: PID数据初始化
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void Pid_Init(void)
{
    pid.settemp     = 0;
    pid.actualtemp  = 0;
    pid.err         = 0;
    pid.err_last    = 0;
    pid.integral    = 0;
    pid.ht_time     = 0;
    pid.kp          = 15;
    pid.ki          = 2;
    pid.kd          = 1;
}
/*******************************************************************************
函数名: Pid_Realize
函数作用: PID自整定加热时间
输入参数:temp当前温度
返回参数:返回数据单位/50us
*******************************************************************************/
u16 Pid_Realize(s16 temp)
{
    u8 index = 0, index1 = 2;
    s16 d_err = 0;

    pid.actualtemp   = temp;//当前温度
    pid.err          = pid.settemp - pid.actualtemp;//温差
    //温差大于50大幅加热
    if(pid.err >= 500)  index = 0;//500改成300 2018.3.16
    else
    {
        index = 1;
        pid.integral    += pid.err;//积分项
    }
    //降温去积分
    if(pid.settemp < pid.actualtemp)
    {
        d_err = pid.actualtemp - pid.settemp;//超过的温度
        if(d_err > 30)
        {
            pid.integral = 0; //过冲3℃
            index1 = 0;
            index = 0;
        }
    }
    //温差小于3°精细加热
    if(pid.err <= 30) index1 = 0;
    else index1 = 2;
    /*---------------------------正常加热-------------------大幅加热-------------------------精细加热---------------------*/
    pid.ht_time     = (pid.kp * pid.err) + (pid.ki * index * pid.integral) + (pid.kd * (pid.err - pid.err_last) * index1);
    pid.err_last    = pid.err;

    if(pid.ht_time <= 0)          pid.ht_time = 0;
    else if(pid.ht_time > HEATINGCYCLE * 200) pid.ht_time = HEATINGCYCLE * 200; //30改成10 2018.3.16
    /*------------------------------防止电压掉到过低------------------------------*/
    return pid.ht_time;
}

/*******************************************************************************
函数名: Heating_Time
函数作用: 计算加热标志，返回加热时间
输入参数:temp当前温度，wk_temp 工作温度
返回参数:返回数据单位/50us
*******************************************************************************/
u32 Heating_Time(s16 temp, s16 wk_temp)
{
    u32 heat_timecnt;

    pid.settemp = wk_temp;
    if(wk_temp > temp)
    {
        if(wk_temp - temp >= 18)gHt_flag = HEATING;//0;//加热
        else gHt_flag = KEEPUP;////2;//恒温
    }
    else
    {
        if(temp - wk_temp <= 18)gHt_flag = KEEPUP;    ////2     恒温
        else gHt_flag = COOLING;////1;//降温
    }

    heat_timecnt = Pid_Realize(temp);//Sub_data * 1000;

    return heat_timecnt;
}
/*******************************************************************************
函数名: Volt_Setting
函数作用: 根据判断计算出工作电压并设置
输入参数: NULL
返回参数: NULL
*******************************************************************************/
void Vol_Calculation(u8 pos)
{
    double Rated_Vol = 0;
    u16 i;

    Vol_Set(0);
    Delay_Ms(200);
    if(pos)//根据内阻和功率计算电压
    {
        Rated_Vol = newSqrt(device_info.rated_power * Resistance) * 10;
        for(i = 0; i < 4; i++)
        {
            Vol_Set(2);
            Delay_Ms(V_TIMER);
        }
        if(Vol_Get() > 930)//判断适配器类型
        {
            Adapt_pos = QC3_0;//QC3.0
        }
        else
        {
            Adapt_pos = QC2_0;//QC2.0
        }
    }
    if(Adapt_pos == QC3_0)
    {
        if(Rated_Vol > Vol_Get() + 20)//升压
        {
            i = (u16)(Rated_Vol - Vol_Get() - 20) / 20;
            if(i > 15)
            {
                i = 15;
            }
            while(i)
            {
                Vol_Set(2);
                Delay_Ms(V_TIMER);
                i--;
            }
        }
        else if(Rated_Vol < Vol_Get())//降压
        {
            i = (u16)(Vol_Get() + 20 - Rated_Vol) / 20;
            if(i > 15)
            {
                i = 15;
            }
            while(i)
            {
                Vol_Set(3);
                Delay_Ms(V_TIMER);
                i--;
            }
        }
    }
    else
    {
        Delay_Ms(V_TIMER);
        Vol_Set(1);
        Vol_Set(0);
    }
}
/*******************************************************************************
函数名: Status_Tran
函数作用: 根据按键、温度判断等控制状态转换
输入参数: NULL
返回参数: NULL
*******************************************************************************/
void Status_Tran(void)//状态转换
{
    u32 key;
    static s16 cur_temp_c, dst_temp_c;
    static s16 temp_val;
    u8 cur_ht_flag = 0;
    static u8  last_ht_flag = 0;
    static u8 disp_switch = 0;
    static u16 bk = 0x33, dl_cnt = 0;
    static u8 over_off_pos = 0;
    static u8 alarm = 0;
    static u32 count = 0;
    static u32 kaka = 0;
    s16 heat_timecnt = 0;
    char str[16];
    u16 len;
    s32 ad_value = 0, i, h_cnt = 0;
    switch (Get_CtrlStatus()) //获取当前状态
    {
    case IDLE://待机状态
        if(gPre_status == START && gFirst_entry)
        {
            // 首次进入idle状态
            Clear_Screen();
            Display_BG();
            //Show_Ver(device_info.ver, 0);
            Display_Str8(0, "VER : 1.06S", 0);
            Delay_Ms(1000);
            Clear_Screen();
            gFirst_entry = 0;
            G6_TIMER = device_info.wait_time;
            DISP_TIMER = 50;
        }
        else if(UI_TIMER == 0 && G6_TIMER != 0)
        {
            Show_Notice();//显示待机图像
            UI_TIMER = 100;
        }
        
        if(device_info.portable_flag)
        {
            if(DISP_TIMER == 0 && count < 1200)
            {   
                {
                    Vol_Set(0);
                    Set_HeatingTime(50);
//                    Clear_Screen();
//                    Display_Str8(0, "No ew head!", 0);
//                    Clear_Screen();
                    DISP_TIMER = 50;//0.5s
                    count++;
                }
            }
        }
        else
        {
            if(DISP_TIMER == 0 && kaka < 60)
            {
                Vol_Set(0);
                Set_HeatingTime(50);
                DISP_TIMER = 50;
                kaka++;
            }
        }
        
        if(gFirst_entry)
        {
            // 从其它状态转换dle状态
            gFirst_entry = 0;
            G6_TIMER = device_info.wait_time;
            bk = 0x33;
            count = 0, kaka = 0;
        }
        
        Get_Resistance();//获取电阻值
        
        key = Get_gKey();
        if(key && G6_TIMER) // if G6_TIMER==0(屏幕保护状态)第一个按键不起效
        {
            switch(key)
            {
            case KEY_V1://单按A键，进入控温状态
                Get_Resistance();
                if(Resistance / 100 <= 1 || Resistance / 100 > 10)
                {
                    Clear_Screen();
                    //Display_Str8(0, "No ew head!", 0);
                    Show_NoElectriciron();
                    Delay_Ms(1000);
                    Clear_Screen();
                    break;
                }
                else
                {
                    for(i = 0; i < 5; i++)
                    {
                        ad_value = Get_AvgAd();//热端
                        if(ad_value > 4000)   h_cnt++;//错误报警
                        if(h_cnt >= 5)              
                        {
                            gAlarm_type = SEN_ERR;
                            Set_CtrlStatus(ALARM);
                            break;
                        }
                    }                    
                }
                Vol_Calculation(1);
                Frequency_Set(1);//降到8MHz
                Set_CtrlStatus(TEMP_CTR);//设置当前状态为控温状态
                break;
            case KEY_V2://单按B键
                Vol_Calculation(1);
                Set_CtrlStatus(TEMP_SET);//进入设置模式
                break;
            case KEY_CN|KEY_V1://长按A键
                Clear_LongKey()	;
                Clear_Screen();
                own_sprintf(str, "Tip R:%d", Resistance ? Resistance / 100 : 0);
                Display_Str8(0, str, 0);
                len = strlen(str);
                own_sprintf(str, "%d", Resistance ? (Resistance % 100) ? (Resistance % 100) / 10 : 0 : 0);
                Display_Str8(len, str, 1);
                own_sprintf(str, "%d", Resistance ? Resistance % 10 : 0);
                Display_Str8(len + 1, str, 0);
                Clear_Watchdog();
                Delay_Ms(1000);
                Clear_Watchdog();
                break;
            }
        }
        if(G6_TIMER == 0 && bk)
        {
            //屏保
            dl_cnt++;
            if(dl_cnt == 50)
            {
                Scr_Protect(bk--);//屏幕保护  降低屏幕对比度
                dl_cnt = 0;
            }
            if(bk == 0)         Oled_DisplayOff();//关闭OLED显示
        }
        if(Get_MmaActive() || key)//获取加速度传感器静动状态
        {
            G6_TIMER = device_info.wait_time;
            if(bk == 0)
            {
                bk = 0x33;
                Scr_Protect(bk);
                Oled_DisplayOn();//打开OLED显示
            }
        }
        break;
    case TEMP_CTR://温控状态，烙铁工作的主要状态
        if(gFirst_entry)
        {
            gFirst_entry = 0;
            Clear_Screen();
            TEMPSHOW_TIMER = 0;
            G6_TIMER = device_info.wait_time;
        }
        if(Read_Vb() != 0)  //电压异常
        {
            over_off_pos++;
            if(over_off_pos == 200)
            {
                over_off_pos = 0;
                Set_CtrlStatus(ALARM);
            }
        }
        key = Get_gKey();
        switch(key)
        {
            case KEY_CN|KEY_V1://长按A键进入温度设置
                Clear_LongKey();
                Set_HeatingTime(0);//加热时间设置为0，停止加热
                Clear_Screen();
                Set_CtrlStatus(TEMP_SET);//进入设置模式
                HEATING_TIMER       = 0;//内部加热
                break;
            case KEY_CN|KEY_V2://长按B键返回待机
                Clear_LongKey();
                Set_HeatingTime(0);
                Set_CtrlStatus(IDLE);//返回待机状态
                Vol_Set(1);
                break;
        }
        
        if(device_info.temp_flag == TEMP_UNIT_F)        dst_temp_c = Temp_conversion(1, device_info.t_work); //F&C判断
        else                                            dst_temp_c = device_info.t_work;
        
        if(HEATING_TIMER == 0)  //计时结束
        {
            cur_temp_c = Get_Temp();//实际温度(停止加热25ms)
            heat_timecnt  = Heating_Time(cur_temp_c, dst_temp_c); //计算加热时间
            Set_HeatingTime(heat_timecnt);
            HEATING_TIMER = HEATINGCYCLE;//10ms
        }
        if(_abs(cur_temp_c, dst_temp_c) < HYSTERETIC_VAL)   cur_temp_c = dst_temp_c;
        cur_ht_flag = Get_HtFlag();
        if(cur_ht_flag != COOLING || last_ht_flag != COOLING)
        {
            DISP_TIMER = 100;
        }
        last_ht_flag = cur_ht_flag;
        if(DISP_TIMER == 0 && UI_TIMER == 0)
        {
            //进入大幅度冷却降温界面
            if(disp_switch == 0)
            {
                disp_switch = 1;
                Clear_Screen();
            }
            if(device_info.temp_flag == TEMP_UNIT_F)
            {
                Show_TempDown(Temp_conversion(0, cur_temp_c) / 10, Temp_conversion(0, dst_temp_c) / 10);
            }
            else
            {
                Show_TempDown(cur_temp_c / 10, dst_temp_c / 10);
            }

            UI_TIMER = 50;
        }

        if(TEMPSHOW_TIMER == 0  && DISP_TIMER) // && (!td_flag))
        {
            //20ms
            if(disp_switch)
            {
                disp_switch = 0;
                Clear_Screen();
            }
            temp_val = cur_temp_c;
            if(device_info.temp_flag == TEMP_UNIT_F)    temp_val = Temp_conversion(0, temp_val);
            Display_Temp(0, temp_val / 10);
            Show_HeatingIcon(cur_ht_flag, Get_MmaActive());//0升温1降温2恒温
            TEMPSHOW_TIMER = 20;//200ms
        }
        if(Get_MmaActive() || key)//获取加速度传感器静动状态
        {
            G6_TIMER = device_info.wait_time;
        }
        if(G6_TIMER == 0)       //进入休眠
        {
            Set_HeatingTime(0);
            Set_CtrlStatus(WAIT);
        }
        else
        {
            over_off_pos = 0;
        }
        if(Get_AlarmType() > NORMAL_TEMP)  //警告
        {
            Set_CtrlStatus(ALARM);
        }
        break;
    case WAIT://休眠状态
        if( gFirst_entry)
        {
            // 从其它状态转换 到wait状态
            gFirst_entry = 0;
            TEMPSHOW_TIMER = 0;
            G6_TIMER = device_info.wait_time;
        }
        if(device_info.temp_flag == TEMP_UNIT_F)    	dst_temp_c = Temp_conversion(1, device_info.t_standby);
        else                        			dst_temp_c = device_info.t_standby;
        if(device_info.t_standby > device_info.t_work)
        {
            //休眠温度比工作温度高
            if(device_info.temp_flag == TEMP_UNIT_F)    dst_temp_c = Temp_conversion(1, device_info.t_work);
            else                        	        dst_temp_c = device_info.t_work;//不再升温保持低的一项温度
        }
        if(HEATING_TIMER == 0)
        {
            cur_temp_c = Get_Temp();
            heat_timecnt = Heating_Time(cur_temp_c, dst_temp_c);  //计算加热时间
            Set_HeatingTime(heat_timecnt);
            HEATING_TIMER = 30;
        }
        if(_abs(cur_temp_c, dst_temp_c) < HYSTERETIC_VAL)       cur_temp_c = dst_temp_c;

        cur_ht_flag = Get_HtFlag();
        if(cur_ht_flag != COOLING || last_ht_flag != COOLING)
        {
            DISP_TIMER = 100;
        }
        last_ht_flag = cur_ht_flag;
        if(DISP_TIMER == 0 && UI_TIMER == 0)
        {
            //进入大幅度冷却降温界面
            if(disp_switch == 0)
            {
                disp_switch = 1;
            }
            if(device_info.temp_flag == TEMP_UNIT_F)
            {
                Show_TempDown(Temp_conversion(0, cur_temp_c) / 10, Temp_conversion(0, dst_temp_c) / 10);
            }
            else
            {
                Show_TempDown(cur_temp_c / 10, dst_temp_c / 10);
            }
            UI_TIMER = 50;
        }

        temp_val = cur_temp_c;
        if(TEMPSHOW_TIMER == 0  && DISP_TIMER)
        {
            if(disp_switch)
            {
                disp_switch = 0;
                Clear_Screen();
            }
            if(device_info.temp_flag == TEMP_UNIT_F)
            {
                temp_val = Temp_conversion(0, temp_val);
            }
            Display_Temp(0, temp_val / 10);
            Show_HeatingIcon(cur_ht_flag, Get_MmaActive());//0升温1降温2恒温
            TEMPSHOW_TIMER = 20;//200ms
        }

        if(Get_AlarmType() > NORMAL_TEMP)  //警告
        {
            Set_CtrlStatus(ALARM);
            break;
        }
        if(Get_MmaActive() || Get_gKey())
        {
            //加速度感应器感应
            Set_CtrlStatus(TEMP_CTR);
            break;
        }
        if(G6_TIMER == 0)   //进入待机
        {
            Set_HeatingTime(0);
            Set_CtrlStatus(IDLE);
        }
        break;
    case TEMP_SET://温度设置状态
        if(gPre_status == IDLE)//从待机模式进来
        {
            Setup_Proc();//改变参数并保存
            ADC_ITConfig(ADC2, ADC_IT_AWD, DISABLE);
            Disk_BuffInit();//磁盘数据初始化
            Config_Save();//保存设置到config.txt
            ADC_ITConfig(ADC2, ADC_IT_AWD, ENABLE);
            Vol_Set(1);//降回5V
            Set_CtrlStatus(IDLE);//返回待机状态
        }
        else if(gPre_status == TEMP_CTR)//从温控模式进来
        {
            TempSet_Proc();//只改变 临时温度
            Set_CtrlStatus(TEMP_CTR);//返回温控状态
        }
        break;
    case ALARM://警告模式
        if(gFirst_entry)
        {
            // 从其它状态转换dle状态
            gFirst_entry = 0;
            Clear_Screen();
            UI_TIMER = 50;
        }
        if(Get_HeatingTime != 0)
        {
            Set_HeatingTime(0);//马上停止加热
            HEAT_OFF();
        }
        if(UI_TIMER == 0)
        {
            Show_Warning();//显示错误图片
            UI_TIMER = 50;
            alarm++;
            if(alarm == 3)
            {
                alarm = 0;
                Set_CtrlStatus(IDLE);
            }
        }
        switch(Get_AlarmType())
        {
        case HIGH_TEMP:
            Get_Temp();
            break;
        case SEN_ERR:
            Get_Temp();
            break;
        case HIGH_VOLTAGE:
            if(Read_Vb() == 0)
            {
                Set_CtrlStatus(TEMP_CTR);
            }
            break;
        case LOW_VOLTAGE:
            if(Vol_Get() > 650)
            {
                Set_CtrlStatus(TEMP_CTR);
            }
            break;
        case NORMAL_TEMP:
            Set_CtrlStatus(TEMP_CTR);
            break;
        }
        break;
    default:
        break;
    }
}
/******************************** END OF FILE *********************************/
