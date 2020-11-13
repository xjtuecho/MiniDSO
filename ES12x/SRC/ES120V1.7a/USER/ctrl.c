/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      ctrl.c
Version :        1.7a
Description:
Author :         Ning
Data:            2017/11/22
History:
2017/07/03       ȥ������������һ��ת������ת�����趨;       
2017/09/10       ��������ת��ʱ������;
2017/10/17       �Ż���ת�����ж�;
2017/11/01       �Ż���ʼת��ʱ��ѹ����;
2017/11/21       �޸ĳ���ѹ�ж����;
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

INFO_SYS info_def = {"1.7a",0,0,0};//�汾1.7,����ת��,1��Ť��//�޸İ汾����Ҫdisk.c�е�Def_set[]����
u8 gMode = IDLE_MOD;//��ʼ״̬
u8 gPre_Mode;
u32 gMoto_timecnt;
u8 gMoto_cntflag = 0;//ת����־ 0:ûת��1:ת
u8 version_number = 0;//�汾��   0:1.4   1:1.3
unsigned long gCur_timer,gLast_timer;
u16 The_Current = 0;//2017.8.1��ǰ����
u32 current_limt = 0;//��ת����

extern vu32 Timer_Counter;
extern u8* SD_IDF;
extern u8* CONSULT;//2017.8.2
extern vu16 AWD_entry;
/*******************************************************************************
������: Hardware_version
��������:Ӳ���汾�ж�
�������:��
���ز���:0:1.3�汾     1:1.4�汾
*******************************************************************************/
u8 Hardware_version(void)
{   
    u8 read;
    read = Single_Read(L3G4200_Addr,CTRL_REG1);

    if(read == 0x0f)     return 1;  
    else                 return 0; 
}
/*******************************************************************************
������: Get_Ctrl_Status
��������:��ȡ��ǰ״̬
�������:��
���ز���:��ǰ״̬
*******************************************************************************/
u8 Get_Mode(void)
{
    return gMode;
}
/*******************************************************************************
������: Set_CtrlStatus
��������:���õ�ǰ״̬
�������:status ���õ�״̬
���ز���:��
*******************************************************************************/
void Set_Mode(u8 mode)
{
    gMode = mode;
}
/*******************************************************************************
 ������: Motor_Foreward
 ��������:�����ת
 �������:NULL
 ���ز���:NULL
*******************************************************************************/
void Motor_Foreward(u16 pwm)
{
    TIM_SetCompare1(TIM1,pwm*FULL_SPEED_PWM/MAX_PWM);
    TIM_SetCompare2(TIM1,LOW_SPEED_PWM);
}
/*******************************************************************************
 ������: Motor_Reversal
 ��������:�����ת
 �������:NULL
 ���ز���:NULL
*******************************************************************************/
void Motor_Reversal(u16 pwm)
{
    TIM_SetCompare1(TIM1,LOW_SPEED_PWM);//
    TIM_SetCompare2(TIM1,pwm*FULL_SPEED_PWM/MAX_PWM);//��
}
/*******************************************************************************
 ������: Motor_Standby
 ��������:�������
 �������:NULL
 ���ز���:NULL
*******************************************************************************/
void Motor_Standby(void)
{
    TIM_SetCompare1(TIM1,LOW_SPEED_PWM);
    TIM_SetCompare2(TIM1,LOW_SPEED_PWM);
}

/*******************************************************************************
 ������: Motor_Brake
 ��������:���ɲ��
 �������:NULL
 ���ز���:NULL
*******************************************************************************/
void Motor_Brake(void)
{
    TURBO_OFF();
    TIM_SetCompare1(TIM1,FULL_SPEED_PWM);
    TIM_SetCompare2(TIM1,FULL_SPEED_PWM);
}

/*******************************************************************************
 ������: Motor
 ��������:���ת��
 �������:NULL
 ���ز���:NULL
*******************************************************************************/
void Motor(u8 dir,u8 pwm)//ת������
{
    if(dir == 1)    Motor_Foreward(pwm);//1,˳ʱ��
    else            Motor_Reversal(pwm);//0,��ʱ��
}
/*******************************************************************************
 ������: TorqueLv_Set
 ��������:��λ���� Torque Level Set
 �������:NULL
 ���ز���:NULL
*******************************************************************************/
void TorqueLv_Set(void)
{
    if(gTimer[1] > 0) {
        if(Get_gKey() == KEY_V1) { //������λ����
            info_def.torque_level++;
            if(info_def.torque_level == MAX_TORQUE) info_def.torque_level = 0;
            Set_gKey(NO_KEY);//�������
        }
        Show_SetLv(info_def.torque_level);//���ڵ�ǰѡ�е�λ
    }

    if(Get_kGap() < 50) {//�а�����λ����ģʽ�˳�ʱ�����
        gTimer[1] = 150;
        gTimer[2] = SHOTDOWN_TIME;
    }

    if(Get_gKey() == (KEY_CN | KEY_V1)) {//���������˳�����ģʽ
        gTimer[1] = 0;
        gTimer[2] = SHOTDOWN_TIME;
    }

    if(gTimer[1] == 0) {//�˳�ǰ���浵λ  
        gCalib_flag = 1;
        Config_Analysis();//���������ļ�
        gCalib_flag = 0;
        Set_Mode(IDLE_MOD);
        Clear_Screen();
        current_limt = (MAX_ROTATE_I - ((info_def.torque_level - 1))*GEARS_UNIT_I);//���ݵ�λ�������
        Set_CurLimit(current_limt,0);//adc���Ź�����
    }
}
const u8 HZ16X16[]=	//��ʱ����  
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
/*******************************************************************************
 ������: Motor_Process
 ��������:������ƹ���
 �������:cur_x ת���Ƕ�
 ���ز���:ת���ĽǶ�
*******************************************************************************/
float Motor_Process(void)
{   
    u8 *ptr;
    static float angle = 0.0;//�Ƕ�
    static u8 pre_dir = 0;//previous direction , 0 up ,1 dowm
    static u8 turbo = 0;//��ѹ��־
    static u8 run_flag = 0;//ת����־
    static u8 pre_pwm = 100;//previous pwm
    static u16 last_pwm = 0;
    static int Run_Time = 0;//��ʼת����ʱ��
    static u8 Restart_Pos = 0;//���¿�ʼת��־λ
    static u8 Stop_Pos = 0;//����ת��ʱ��ֹͣ
    u16   limt_cnt;
    float cur_scale;
    float temp_angle;
    cur_scale = Get_Angle();   //��ȡ��ǰת���Ƕ�
    static u8 dir_change = 0;//2017.10.31�ı�ת��������߿�ʼת��
    
    if(!gKey_Press) {//��������
        Set_Mode(IDLE_MOD);
        Clear_Screen();
        angle = 0;
        Run_Time = 0;
        Restart_Pos = 0;
        Stop_Pos =0;
    } else if(gKey_Press) {//��������
        if(Restart_Pos == 0)//��ʼת����ʱ���������ٴ���
        {
            dir_change = 1;
            Restart_Pos =1;
            Run_Time = Timer_Counter;
        }
        if(Timer_Counter - Run_Time > MAX_RUNTIME)//ת����ʱ
        {   
            Stop_Pos = 1;
            angle = 0;
            turbo = 0;
            run_flag = 0;
            last_pwm = 0;
            Motor_Brake();
            ptr = (u8*)HZ16X16;
            Oled_DrawArea(0,0,96,16,ptr);//��ʱͼƬ
        }
        gTimer[2] = SHOTDOWN_TIME;//������λ�ػ�ʱ��

        if(gPre_Mode == IDLE_MOD) {//����״̬
            gPre_Mode = CTR_MOD;//�������ģʽ
            angle = 0;
            turbo = 0;
            run_flag = 0;
            last_pwm = 0;
            AWD_entry = 0;

            gTimer[0] = 20;  //��ʱ����������ʱ�� || ��תǰ����ѹʱ��
            gTimer[3] = 0;   //���ͣתʱ��
            gTimer[4] = 50;  //����Ƿ�̶�ģʽ�еĳ���ʱ���ж�
            gTimer[5] = 0;   //��ʾˢ�¼��
            
            if(Stop_Pos)        //��ʱ����
            {
                ptr = (u8*)HZ16X16;
                Oled_DrawArea(0,0,96,16,ptr);//��ʱͼƬ
            }
            else
            {
                Clear_Screw();
                Show_Screw(1);   //��ʾ�̶���˿
            }
        }
        if((cur_scale > -0.169 && cur_scale < 0) || (cur_scale < 0.169 && cur_scale > 0))  cur_scale = 0; //ȥ����
        angle += cur_scale;                          //�Ƕȼ���
        if(angle >= MAX_ANGLE)  angle = MAX_ANGLE;
        if(angle <= -MAX_ANGLE) angle = -MAX_ANGLE;          //���ƽǶȷ�Χ  0~90/-90~0

        if(gMoto_cntflag == 0 && run_flag == 1) { //��û��ת����ʱ�� ��¼���ת���ۼ�ʱ��ĳ�ʼֵ
            gMoto_cntflag = 1;
            gCur_timer = Timer_StartValue();//���ת���ۼ�ʱ��ĳ�ʼֵ
            gLast_timer = gCur_timer;
        }

        if(Stop_Pos == 0) {//ת��ʱ
            if(angle <= info_def.start_angle && angle >= -info_def.start_angle) { //��ת��
                run_flag = 0;
                Motor_Brake();//ֹͣת��
                Show_Screw(2);
            } else { //ת������
                if(angle > info_def.start_angle) {//��,˳ʱ��
                    if(pre_dir == 0)    dir_change = 1;
                    pre_dir = 1;
                    temp_angle = angle;
                } else  if(angle < -info_def.start_angle) {//��,��ʱ��
                    if(pre_dir == 1)    dir_change = 1;
                    pre_dir = 0;
                    temp_angle = -angle;
                
                }
                if(info_def.torque_level != 0) { //�̶��ٶ�ģʽPWMΪ����ҹ̶�
                    if(dir_change)//���������߸��ķ���
                    {
                      if(dir_change == 1)//ֻ����һ�� 
                      {
                          last_pwm = 0;
                          dir_change++;
                      }
                      Delay_Ms(4);
                      last_pwm ++;
                      if(last_pwm == MAX_PWM)
                      {
                          dir_change = 0;
                      }
                      if(pre_dir)
                      {
                          current_limt = MAX_ROTATE_I;//��תĬ������ת����
                          Set_CurLimit(current_limt,0);//adc���Ź�����
                      }
                      else
                      {
                          current_limt = (MAX_ROTATE_I - ((info_def.torque_level - 1))*GEARS_UNIT_I);
                          Set_CurLimit(current_limt,0);//adc���Ź�����
                      }
                    }
                    else
                    {
                        last_pwm = MAX_PWM;
                    }
                } else { //A���ٶȿɱ�PWM��Ƕȱ仯���仯
                    if(temp_angle >= (info_def.start_angle + 28))  last_pwm = MAX_PWM;//
                    else {
                        last_pwm = 20 + (int)((temp_angle/(info_def.start_angle + 28))*(MAX_PWM - 20));
                    }
                }
                if(!run_flag) {
                    turbo = 0;
                    gTimer[0] = 20;//����ѹʱ��
                    run_flag = 1;
                }
                pre_pwm = last_pwm;
            }
        }
        if(run_flag == 1 && Stop_Pos == 0) {//ת��ʱ
          if(gTimer[0] == 0 && turbo == 0) {
                turbo = 1;
                TURBO_ON();//��ʼת��
                
                
                gTimer[0] = 10;
            }
            Motor(pre_dir,pre_pwm);//�ж�ת������

            if(gTimer[5] == 0) {//������ת������ʾ��˿���·���
                Clear_Screw();
                Show_ScrewMove(pre_dir);
                gTimer[5] = 5 ;
            }
        } else {
            return angle;
        }
        //��ת���������ۼ�
        The_Current = (60000*Get_Adc(VCUR)/4096);//��ǰ����  PA0
        if(The_Current < current_limt)  AWD_entry = 0;
        //����λ�Ķ�ת���������ۼ���    
        if(info_def.torque_level != 0)  limt_cnt = 500 - (info_def.torque_level - 1)*100;//������λ��������
        else  limt_cnt = 500;      

        if((gTimer[0] == 0) && (AWD_entry > limt_cnt)) {//��ת����,���ɲ��,3s�ڲ�ת�������ת�������200ms����ѹ��
            Motor_Brake();
            AWD_entry = 0;
            turbo = 0;//2017.12.18��ת��3����ѹ
            Set_Mode(STANDBY_MOD);
            gTimer[3] = 300;
            gTimer[0] = 320;
        }
    }

    return angle;
}
/*******************************************************************************
 ������: Idle_Process
 ��������:����״̬����ת��
 �������:NULL
 ���ز���:NULL
*******************************************************************************/
void Idle_Process(void)
{
    u16 bat_v,bat_value;
    static u8 LowPwer_cnt = 0;
    unsigned long dt = 0;
    
    if(gPre_Mode != IDLE_MOD)    gPre_Mode = IDLE_MOD;

    Motor_Brake();

    if(gMoto_cntflag == 1) { //��һ���ɿ�����,������ת��ʱ��
        gCur_timer = Timer_StartValue();
        dt += Elapse_Value(gLast_timer,gCur_timer);//10 ms
        gMoto_timecnt += dt;

        gMoto_cntflag = 0;
        info_def.moto_timecnt = gMoto_timecnt/100; //��
        Write_MtFlash();//������ת��ʱ��
    }
    bat_v = (600*Get_Adc(VBTY))/4096;//��ص�ѹ
    if(gTimer[5] == 0) {//��ʾ��ѹ�Լ���˿�Ĵ�������
        bat_value = ((bat_v - 320)*10)/88;
        if((Get_Adc(VBTY)*600/4096)>=415)//2017.1.21�޸ĳ���ѹ�ж�
        {
            bat_value++;
        }
        if(Get_Adc(VIN) < 500) { //no usb ,only have bat.
            if(bat_v < 320) bat_v = 320;
            Show_BatV(bat_value,0);//(V-3.20)/(4.1-3.20) ��10����ѡ��ͼ�Ρ�
        } else {
            if(READ_CHG == 0)   Show_BatV(bat_value,1);
            else                Show_BatV(bat_value,2);
        }
        Show_Screw(0);
        gTimer[5] = 50;
    }

    if(keypress_cnt == 3 && (Get_Mode() != LP_MOD)) { //�������ΰ�����������ģʽ
        Set_Mode(SET_MOD);
        keypress_cnt = 0;//������������
        if(gTimer[1] > 150)  gTimer[1] = 150;
        Set_gKey(NO_KEY);
        Clear_Screen();
    }
    
    if(bat_v < 320) LowPwer_cnt++;//�͵�ѹ�ۼƼ���,��ֹ��ʱ���ת���͵�ѹ,
    else            LowPwer_cnt = 0;

    if(LowPwer_cnt > 150 && (Get_Adc(VIN) < 500)&&(!gKey_Press)) { //�͵�ѹ����LowPowerģʽ
        Set_Mode(LP_MOD);
        gTimer[2] = 500; //û��USB�͵�ѹ,5���ػ�.
        Motor_Brake();
        Clear_Screen();
    }
}
/*******************************************************************************
 ������: Shut_Down
 ��������:�ػ�����
 �������:NULL
 ���ز���:NULL
*******************************************************************************/
void Shut_Down(void)
{
    gCalib_flag = 1;
    Config_Analysis();         //����moto_timecont��config.txt
    gCalib_flag = 0;
    Clear_Screen();
    if(version_number)
    {
        POWER_OFF_1();//PD0
    }
    else
    {
        POWER_OFF();//���͹ػ�PB4
    }
}
/*******************************************************************************
 ������: Stand_By
 ��������:�ػ�����
 �������:NULL
 ���ز���:NULL
*******************************************************************************/
void Stand_By(void)
{
    if(gTimer[3] == 0 ||(!gKey_Press))   Set_Mode(CTR_MOD);
}
/*******************************************************************************
 ������: Mode_Switching
 ��������:ģʽ����
 �������:NULL
 ���ز���:NULL
*******************************************************************************/
void Mode_Switching(void)
{
    switch(Get_Mode()) {//ģʽת��
    case IDLE_MOD:   //����ģʽ(��ʼ״̬)
        Idle_Process();
        if(gKey_Press) {  
            Get_Angle();   //����cultimeʱ��
            Set_Mode(CTR_MOD);
        }
        if(gTimer[2] == 0 && Get_Adc(VIN) < 500) {//�ػ�ʱ��Ϊ0��û��USB����ʱ,Shut Down
            Set_Mode(SHUT_DOWN);
        }
        break;
    case CTR_MOD:   //����ģʽ
        Motor_Process();
        break;
    case STANDBY_MOD:   //����ģʽ
        Stand_By();
        break;
    case SET_MOD:   //����ģʽ
        TorqueLv_Set();//��λ����
        break;
    case LP_MOD:    //�͵�ѹģʽ
        Low_Power(); //��ʾ�͵�ѹ����
        if(Get_Adc(VIN) > 500) { //��USB�����򷵻ؿ���ģʽ
            Set_Mode(IDLE_MOD);
            Clear_Screen();
        } else {//�ػ�ʱ��Ϊ0��û��USB����ʱ,Shut Down
            if(gTimer[2] == 0) {
                Set_Mode(SHUT_DOWN);
            }
        }
        break;
    case SHUT_DOWN:
        Shut_Down();//�ػ�
        break;
    default:
        break;
    }
}
/******************************** END OF FILE *********************************/
