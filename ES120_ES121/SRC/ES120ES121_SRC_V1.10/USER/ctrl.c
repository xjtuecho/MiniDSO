/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      ctrl.c
Version :        1.10
Description:
Author :         Ning
Data:            2017/11/22
History:
2017/07/03       去除长按按照上一次转动规则转动的设定;       
2017/09/10       新增连续转动时间上限;
2017/10/17       优化堵转电流判断;
2017/11/01       优化开始转动时升压过程;
2017/11/21       修改充电电压判断语句;
*******************************************************************************/
#include "stdio.h"
#include "app.h"
#include "Ctrl.h"
#include "Adc.h"
#include "Hardware.h"
#include "Disk.h"
#include "UI.h"
#include "L3G4200D.h"
#include "delay.h"
#include "Oled.h"
#include "string.h"
#include "iic.h"
#include "own_sprintf.h"

INFO_SYS info_def = {"1.10",0,0,0};//版本1.7,两度转动,1级扭力//修改版本还需要disk.c中的Def_set[]数组
u8 gMode = IDLE_MOD;//初始状态
u8 gPre_Mode;//前一状态
u32 gMoto_timecnt;//连续转动时间
u8 gMoto_cntflag = 0;//转动标志 0:没转，1:转
u8 version_number = 0;//版本号   0:1.4   1:1.3
u32 gCur_timer;//时间点
u32 gLast_timer;//时间点
u16 The_Current = 0;//2017.8.1当前电流
u32 current_limt = 0;//堵转电流
static u8 turbo = 0;//升压标志
static u8 Low_vol_sign = 0;//低压标志
//static u8 firstkey = 1;//开机第一次按键
extern u8 frequency_pos;//频率标志位
u16 interval;//按键间隔计数
u16 bat_vol;//电池电压
u16 bat_value;//电池电量
    
extern vu32 Timer_Counter;//定时器2中断计数10ms,作为开机后的时间轴
extern u8* SD_IDF;
extern u8* CONSULT;//2017.8.2
extern vu16 AWD_entry;

const u8 HZ16X16[]=	//超时提醒图像  
{   
    0x00,0x00,0x00,0x34,0x52,0x92,0x92,0x92,0x92,0x52,0x3C,0x00,0x00,0x00,0x7E,0x80,
    0x80,0x7E,0x80,0x80,0x7E,0x80,0x00,0x00,0xFE,0x00,0x00,0x0C,0x02,0x02,0xFC,0x00,
    0x00,0x00,0x80,0x40,0x20,0x7E,0x80,0x00,0x34,0x52,0x92,0x92,0x92,0x92,0x52,0x3C,
    0x00,0xC0,0x20,0x18,0x04,0x02,0x04,0x18,0x20,0xC0,0x00,0x00,0xF0,0x08,0x04,0x02,
    0x02,0x02,0x02,0x02,0x04,0x08,0xF0,0x00,0x00,0x00,0x00,0x38,0x44,0x82,0x02,0x02,
    0x02,0x02,0x02,0x02,0xDA,0xDA,0x02,0x02,0x02,0x02,0x02,0x02,0x82,0x44,0x38,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x02,0x02,0x02,0x02,0x3F,0x02,
    0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x10,0x20,0x40,
    0x40,0x40,0x40,0x40,0x20,0x10,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,
    0x04,0x08,0x10,0x20,0x4F,0x4F,0x20,0x10,0x08,0x04,0x02,0x01,0x00,0x00,0x00,0x00,
};

void TURBO_OFF(void)
{
	turbo = 0;
	GPIO_ResetBits(GPIOA,GPIO_Pin_10);
}
/*******************************************************************************
函数名: Hardware_version
函数作用:硬件版本判断
输入参数:无
返回参数:0:1.3版本     1:1.4版本
*******************************************************************************/
u8 Hardware_version(void)
{   
    u8 read;
    read = Single_Read(L3G4200_Addr,CTRL_REG1);

    if(read == 0x0f)     return 1;  
    else                 return 0; 
}
/*******************************************************************************
函数名: Get_Ctrl_Status
函数作用:获取当前状态
输入参数:无
返回参数:当前状态
*******************************************************************************/
u8 Get_Mode(void)
{
    return gMode;
}
/*******************************************************************************
函数名: Set_CtrlStatus
函数作用:设置当前状态
输入参数:status 设置的状态
返回参数:无
*******************************************************************************/
void Set_Mode(u8 mode)
{
    gMode = mode;
}
/*******************************************************************************
 函数名: Motor_Foreward
 函数作用:电机正转
 输入参数:NULL
 返回参数:NULL
*******************************************************************************/
void Motor_Foreward(u16 pwm)
{
    if(frequency_pos == 0 && info_def.torque_level == 0)   
        TIM_SetCompare1(TIM1, pwm * FULL_SPEED_PWM / MAX_PWM / 6);//高
    else                
        TIM_SetCompare1(TIM1, pwm * FULL_SPEED_PWM / MAX_PWM);//高
    TIM_SetCompare2(TIM1, LOW_SPEED_PWM);//逆方向不转
}
/*******************************************************************************
 函数名: Motor_Reversal
 函数作用:电机反转
 输入参数:NULL
 返回参数:NULL
*******************************************************************************/
void Motor_Reversal(u16 pwm)
{
    TIM_SetCompare1(TIM1, LOW_SPEED_PWM);//正方向不转
    if(frequency_pos == 0 && info_def.torque_level == 0)   
        TIM_SetCompare2(TIM1, pwm * FULL_SPEED_PWM / MAX_PWM / 6);//高
    else                
        TIM_SetCompare2(TIM1, pwm * FULL_SPEED_PWM / MAX_PWM);//高
}
/*******************************************************************************
 函数名: Motor_Standby
 函数作用:电机待机
 输入参数:NULL
 返回参数:NULL
*******************************************************************************/
void Motor_Standby(void)
{
    TIM_SetCompare1(TIM1, LOW_SPEED_PWM);
    TIM_SetCompare2(TIM1, LOW_SPEED_PWM);
}
/*******************************************************************************
 函数名: Motor_Brake
 函数作用:电机刹车
 输入参数:NULL
 返回参数:NULL
*******************************************************************************/
void Motor_Brake(void)
{
    TURBO_OFF();
    TIM_SetCompare1(TIM1, FULL_SPEED_PWM);
    TIM_SetCompare2(TIM1, FULL_SPEED_PWM);
}
/*******************************************************************************
 函数名: Motor
 函数作用:电机转动
 输入参数:NULL
 返回参数:NULL
*******************************************************************************/
void Motor(u8 dir,u8 pwm)//转动方向
{
    if(dir == 1)    Motor_Foreward(pwm);//1,顺时针
    else            Motor_Reversal(pwm);//0,逆时针
}
/*******************************************************************************
 函数名: TorqueLv_Set
 函数作用:档位设置 Torque Level Set
 输入参数:NULL
 返回参数:NULL
*******************************************************************************/
void TorqueLv_Set(void)
{
    if(gTimer[1] > 0)
    {
        if(Get_gKey() == KEY_V1) 
        { //按键档位增加
            info_def.torque_level++;
            if(info_def.torque_level == MAX_TORQUE) info_def.torque_level = 0;
            Set_gKey(NO_KEY);//清除按键
        }
        Show_SetLv(info_def.torque_level);//现在当前选中档位
    }
 
    if(Get_kGap() < interval)
    {//有按键复位设置模式退出时间计数
        gTimer[1] = 150;
        gTimer[2] = SHOTDOWN_TIME;//关机时间
    }
    if(Get_gKey() == (KEY_CN | KEY_V1))
    {//长按按键退出设置模式
        gTimer[1] = 0;
        gTimer[2] = SHOTDOWN_TIME;
    }

    if(gTimer[1] == 0) 
    {//退出前保存档位  
        gCalib_flag = 1;
        Config_Analysis();//分析配置文件
        gCalib_flag = 0;
        Set_Mode(IDLE_MOD);
        Clear_Screen();
        if(info_def.torque_level != 1 && info_def.torque_level != 0)
        {//根据挡位计算电流
            current_limt = (MAX_ROTATE_I - ((info_def.torque_level - 2)) * GEARS_UNIT_I);
        }
        else        current_limt = 1800;
        Set_CurLimit(current_limt, 0);//adc看门狗界限
    }
}
/*******************************************************************************
 函数名: Motor_Process
 函数作用:电机控制过程
 输入参数:cur_x 转动角度
 返回参数:转动的角度
*******************************************************************************/
float Motor_Process(void)
{
    u8 *ptr;
    static float angle = 0.0;//角度
    static u8 pre_dir = 0;//previous direction , 0 up ,1 dowm
//    char str[8];
//    char skr[8];
//    char skt[8];
//    char ssw[8];
    static u8 run_flag = 0;//转动标志
    static u8 pre_pwm = 100;//previous pwm
    static u16 last_pwm = 0;
    static int Run_Time = 0;//开始转动的时间
    static u8 Restart_Pos = 1;//重新开始转标志位
    static u8 Stop_Pos = 0;//超过转动时间停止
    u16 limt_cnt;
    float cur_scale;
    float temp_angle;
    cur_scale = Get_Angle();   //获取当前转动角度
    static u8 dir_change = 0;//2017.10.31改变转动方向或者开始转动
    
    if(!gKey_Press)
    {//按键弹起
        Set_Mode(IDLE_MOD);
        Clear_Screen();
        angle = 0;
        Run_Time = 0;
        Restart_Pos = 0;
        Stop_Pos = 0;
    } 
    else if(gKey_Press) 
    {//按键按下
        if(Restart_Pos == 0)//开始转动及时和启动慢速处理
        {
            dir_change = 1;
            Restart_Pos = 1;
            Run_Time = Timer_Counter;
        }
        if(Timer_Counter - Run_Time > MAX_RUNTIME)//转动超时
        {   
            Stop_Pos = 1;
            angle = 0;
            turbo = 0;
            run_flag = 0;
            last_pwm = 0;
            Motor_Brake();//电机刹车
            ptr = (u8*)HZ16X16;
            Oled_DrawArea(0, 0, 96, 16, ptr);//超时图片
        }
        gTimer[2] = SHOTDOWN_TIME;//按键后复位关机时间

        if(gPre_Mode == IDLE_MOD) 
        {//空闲状态
            gPre_Mode = CTR_MOD;//进入控制模式
            angle = 0;
            turbo = 0;
            run_flag = 0;
            last_pwm = 0;
            AWD_entry = 0;

            gTimer[0] = 20;  //延时检测电流过限时间 || 开转前不升压时间
            gTimer[3] = 0;   //电机停转时间
            gTimer[4] = 50;  //检测是否固定模式中的长按时间判断
            gTimer[5] = 0;   //显示刷新间隔

            if(Stop_Pos)        //超时提醒
            {
                ptr = (u8*)HZ16X16;
                Oled_DrawArea(0, 0, 96, 16, ptr);//超时图片
            }
            else
            {
                Clear_Screw();
                Show_Screw(1);   //显示固定螺丝
            }
        }
        
        if(info_def.torque_level == 0 || info_def.torque_level == 1)
        {
            if((cur_scale > -0.069 && cur_scale < 0) || (cur_scale < 0.069 && cur_scale > 0))  
                cur_scale = 0; //去波动
        }
        else
        {
            if((cur_scale > -0.169 && cur_scale < 0) || (cur_scale < 0.169 && cur_scale > 0))  
                cur_scale = 0; //去波动
        }
        
        angle += cur_scale;                          //角度计算
        if(angle >= MAX_ANGLE)  angle = MAX_ANGLE;
        if(angle <= -MAX_ANGLE) angle = -MAX_ANGLE;          //控制角度范围  0~90/-90~0

        if(gMoto_cntflag == 0 && run_flag == 1)
        { //在没有转动的时候 记录电机转动累计时间的初始值
            gMoto_cntflag = 1;
            gCur_timer = Timer_StartValue();//获取当前时间
            gLast_timer = gCur_timer;
        }
        
        if(Stop_Pos == 0) 
        {//转动时,小于起始角度
            if(angle <= info_def.start_angle && angle >= -info_def.start_angle)
            { //不转动
                run_flag = 0;
                Motor_Brake();//电机刹车
                Show_Screw(2);
            } 
            else 
            { //转动处理
                if(angle > info_def.start_angle)
                {//正,顺时针
                    if(pre_dir == 0)    dir_change = 1;
                    pre_dir = 1;
                    temp_angle = angle;
                } 
                else if(angle < -info_def.start_angle) 
                {//负,逆时针
                    if(pre_dir == 1)    dir_change = 1;
                    pre_dir = 0;
                    temp_angle = -angle;
                }
                
                if(info_def.torque_level != 0) 
                { //固定速度模式PWM为最大且固定
                    if(dir_change)//刚启动或者更改方向
                    {
                      if(dir_change == 1)//只进入一次 
                      {
                          last_pwm = 0;
                          dir_change++;
                      }
                      Delay_Ms(4);
                      last_pwm++;
                      if(last_pwm == MAX_PWM)
                      {
                          dir_change = 0;
                      }
                      
                      if(pre_dir)//正方向进入
                      {
                          current_limt = MAX_ROTATE_I;//反转默认最大堵转电流
                          Set_CurLimit(current_limt, 0);//adc看门狗界限
                      }
                      else//逆方向进入
                      {
                          if(info_def.torque_level != 1)
                          {
                              current_limt = (MAX_ROTATE_I - ((info_def.torque_level - 2)) * GEARS_UNIT_I);
                          }
                          else
                          {
                              if(temp_angle >= 60)      current_limt = 1800;
                              else                      current_limt = (u32)(temp_angle * 17 + 750);
                          }
                          Set_CurLimit(current_limt, 0);//adc看门狗界限
                      }
                    }
                    else//没改变方向
                    {
                        last_pwm = MAX_PWM;
                    }
                }
                else 
                { //A档速度可变PWM随角度变化而变化
                    if(dir_change)//刚启动或者更改方向
                    {
                        if(dir_change == 1)//只进入一次 
                        {
                            last_pwm = 0;
                            dir_change++;
                        }
                        Delay_Ms(4);
                        last_pwm ++;
                        if(last_pwm == 15 + (int)((temp_angle / 45) * (MAX_PWM - 15)))
                            dir_change = 0;
                        
                        if(pre_dir)     current_limt = MAX_ROTATE_I;//正方向进入反转默认最大堵转电流
                        else            current_limt = MAX_ROTATE_I;//逆方向进入
                        Set_CurLimit(current_limt, 0);//adc看门狗界限
                    }
                    else//没改变方向
                    {
                        if(temp_angle >= 45)    last_pwm = MAX_PWM;//最大
                        else                    last_pwm = 15 + (int)((temp_angle / 45) * (MAX_PWM - 15));
                    }
                }
                if(!run_flag)
                {
                    turbo = 0;
                    gTimer[0] = 20;//不升压时间
                    run_flag = 1;
                }
                pre_pwm = last_pwm;
            }
        }
        
        //堵转的最大电流累计
        The_Current = (60000 * Get_Adc(VCUR) / 4096);//当前电流  PA0
        
        bat_vol = (600 * Get_Adc(VBTY)) / 4096;//电池电压
        bat_vol = (bat_vol * 610) / 600;//电压补偿
            
        if(bat_vol < 330)        Low_vol_sign = 1;
        else                    Low_vol_sign = 0;
        
        if(run_flag == 1 && Stop_Pos == 0) 
        {//转动时
            if(gTimer[0] == 0 && turbo == 0) 
            {
                turbo = 1;
                if(Low_vol_sign == 1)   turbo = 0;
                else                    TURBO_ON();//电池升压
            }
            Motor(pre_dir,pre_pwm);//判断转动方向
            
            bat_vol = (600 * Get_Adc(VBTY)) / 4096;//电池电压
            bat_vol = (bat_vol * 610) / 600;//电压补偿
            
            if(bat_vol < 330)        Low_vol_sign = 1;
            else                    Low_vol_sign = 0;
            
            if(gTimer[5] == 0)// && turbo) 
            {//根据旋转方向显示螺丝上下方向
                Clear_Screw();//清屏
                Show_ScrewMove(pre_dir);//动态显示螺丝图片
//                own_sprintf(str, "%d", The_Current);//显示电流
//                Display_Str(3,0,str);
//                own_sprintf(skr, "%d", current_limt);//显示堵转电流
//                Display_Str(3,5,skr);
//                own_sprintf(skt, "%d", pre_pwm);//转速
//                Display_Str(8,0,skt);
//                own_sprintf(ssw, "%d", dianya);//电机运转时电池电压
//                Display_Str(8,5,ssw);
                gTimer[5] = 5;
            }
        }
        else    return angle;
        
        //堵转的最大电流累计
        The_Current = (60000 * Get_Adc(VCUR) / 4096);//当前电流  PA0
        
        if(info_def.torque_level != 0)
        {
            if(info_def.torque_level == 1)
            {
                if(temp_angle >= 60 )                           limt_cnt = 250;
                if(temp_angle >= 40 && temp_angle < 60)         limt_cnt = 200;
                if(temp_angle >= 20 && temp_angle < 40)         limt_cnt = 150;
                if(temp_angle >= 0 && temp_angle < 20)          limt_cnt = 100;
            }
            else
            {//其他挡位电流计算
                limt_cnt = (500 - (info_def.torque_level - 2) * 100) / 2;
            }
        }
        else    limt_cnt = 500 / 2; //A档电流值
        
        if((gTimer[0] == 0) && (AWD_entry > limt_cnt))
        {//堵转处理,电机刹车,3s内不转动电机，转动电机后200ms不升压。
            GPIO_ResetBits(GPIOB, GPIO_Pin_2);
            Motor_Brake();//电机刹车
            AWD_entry = 0;
            turbo = 0;//2017.12.18堵转后3秒升压
            Set_Mode(STANDBY_MOD);
            gTimer[3] = 300;
            gTimer[0] = 320;
        }
    }
    return angle;
}
/*******************************************************************************
 函数名: Idle_Process
 函数作用:空闲状态处理转换
 输入参数:NULL
 返回参数:NULL
*******************************************************************************/
void Idle_Process(void)
{
    static u8 LowPwer_cnt = 0;
    unsigned long dt = 0;
    
    if(gPre_Mode != IDLE_MOD)    gPre_Mode = IDLE_MOD;
    Motor_Brake();//电机刹车
    if(gMoto_cntflag == 1)
    { //第一次松开按键,计算电机转动时间
        gCur_timer = Timer_StartValue();//获取当前时间
        dt += Elapse_Value(gLast_timer, gCur_timer);//每单位10 ms
        gMoto_timecnt += dt;

        gMoto_cntflag = 0;
        info_def.moto_timecnt = gMoto_timecnt / 100; //秒
        Write_MtFlash();//保存电机转动时间
    }
    
    bat_vol = (600 * Get_Adc(VBTY)) / 4096;//电池电压
    bat_vol = (bat_vol * 610) / 600;//电压补偿
    if(gTimer[5] == 0)
    {//显示电压以及螺丝的待机界面
        if(bat_vol >= 395)
        {
            if(bat_vol >= 405)  bat_value = 10;
            else                bat_value = 9;
        }
        if(bat_vol < 395 && bat_vol > 365)
        {
            bat_value = ((bat_vol - 366) / 4);
            bat_value += 2;
        }
        if(bat_vol <= 365)
        {
            if(bat_vol > 320)   bat_value = 1;
            else                bat_value = 0;
        }
        if(bat_vol >= 415)      bat_value++;
        if(bat_vol < 330)       Low_vol_sign = 1;
        if(bat_vol > 340)       Low_vol_sign = 0;
          
        if(Get_Adc(VIN) < 500) //没连接USB
        { //no usb ,only have bat.
            if(frequency_pos == 1)
            {
                USB_Port(DISABLE);
                frequency_pos = 0;
                RCC_Config();
                Delay_Init();
                Init_Timer2();
                PWM_Init(400,0,0);
                interval = 300;
            }
            if(bat_vol < 320)   bat_vol = 320;
            Show_BatV(bat_value, 0);//显示电量|电池状态
        } 
        else//连接USB
        {
            if(frequency_pos == 0)
            {
                USB_Port(DISABLE);
                frequency_pos = 1;
                RCC_Config();
                Delay_Init();
                Init_Timer2();
                PWM_Init(2400,0,0);
                interval = 50;
                Delay_Ms(200);
                USB_Port(ENABLE);
                USB_Init();               
            }
            if(READ_CHG == 0)   Show_BatV(bat_value, 1);//显示电量|电池状态
            else                Show_BatV(bat_value, 2);//显示电量|电池状态
        }
        Show_Screw(0);//固定位置显示螺丝
        gTimer[5] = 50;
    }

    if(keypress_cnt == 3 && (Get_Mode() != LP_MOD)) 
    { //连击三次按键进入设置模式
        Set_Mode(SET_MOD);
        keypress_cnt = 0;//连续按键次数
        if(gTimer[1] > 150)     gTimer[1] = 150;
        Set_gKey(NO_KEY);
        Clear_Screen();
    }
    
    if(bat_vol < 320)   LowPwer_cnt++;//低电压累计计数,防止短时间堵转拉低电压,
    else                LowPwer_cnt = 0;

    if(LowPwer_cnt > 150 && (Get_Adc(VIN) < 500) && (!gKey_Press)) 
    { //低电压进入LowPower模式
        Set_Mode(LP_MOD);
        gTimer[2] = 500; //没有USB低电压,5秒后关机.
        Motor_Brake();//电机刹车
        Clear_Screen();
    }
}
/*******************************************************************************
 函数名: Shut_Down
 函数作用:关机函数
 输入参数:NULL
 返回参数:NULL
*******************************************************************************/
void Shut_Down(void)
{
    gCalib_flag = 1;
    Config_Analysis();         //保存moto_timecont到config.txt
    gCalib_flag = 0;
    Clear_Screen();
    if(version_number)  POWER_OFF_1();//PD0
    else                POWER_OFF();//拉低关机PB4
}
/*******************************************************************************
 函数名: Stand_By
 函数作用:待机函数
 输入参数:NULL
 返回参数:NULL
*******************************************************************************/
void Stand_By(void)
{
    if(gTimer[3] == 0 || (!gKey_Press))         Set_Mode(CTR_MOD);
}
/*******************************************************************************
 函数名: Mode_Switching
 函数作用:模式控制
 输入参数:NULL
 返回参数:NULL
*******************************************************************************/
void Mode_Switching(void)
{
    switch(Get_Mode()) 
    {//模式转换
    case IDLE_MOD:   //空闲模式(初始状态)
//        if(firstkey == 1)
//        {
//            gTimer[6] = 1000;//开机后的自动关机
//            firstkey++;
//        }
//        if(firstkey == 2)
//        {
//            if(keypress_cnt == 2) 
//            {
//                keypress_cnt = 0;//连续按键次数
//                Set_gKey(NO_KEY);
//                firstkey = 0;
//            }
//            if(gTimer[6] == 0)
//                Set_Mode(SHUT_DOWN);
//            break;
//        }
        
        Adc_Init();  //初始化ADC
        Idle_Process();//空闲状态处理转换
        if(gKey_Press) 
        {
            Adc_Init();    //初始化ADC
            Get_Angle();   //转动经过的角度
            Set_Mode(CTR_MOD);//进入控制模式
        }
        if(gTimer[2] == 0 && Get_Adc(VIN) < 500) 
        {//关机时间为0且没有USB连接时,Shut Down
            Set_Mode(SHUT_DOWN);//进入关机
        }
        break;
    case CTR_MOD:   //控制模式
        Motor_Process();//电机控制过程
        break;
    case STANDBY_MOD:   //待机模式
        Stand_By();
        break;
    case SET_MOD:   //设置模式
        TorqueLv_Set();//挡位设置
        break;
    case LP_MOD:    //低电压模式
        Low_Power(); //显示低电压警告
        if(Get_Adc(VIN) > 500) 
        { //有USB插入则返回控制模式
            Set_Mode(IDLE_MOD);
            Clear_Screen();
        }
        else 
        {//关机时间为0且没有USB连接时,Shut Down
            if(gTimer[2] == 0)  
                Set_Mode(SHUT_DOWN);
        }
        break;
    case SHUT_DOWN:
        Shut_Down();//关机
        break;
    default:
        break;
    }
}
/******************************** END OF FILE *********************************/
