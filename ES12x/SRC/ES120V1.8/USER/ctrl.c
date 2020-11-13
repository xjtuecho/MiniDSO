/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      Ctrl.c
Version :
Description:
Author :         Celery
Data:            2016/12/22
History:

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


INFO_SYS info_def = {"1.8",0,0,0};//�汾1.8,����ת��,1��Ť��//�޸İ汾����Ҫdisk.c�е�Def_set[]����
u8 gMode = IDLE_MOD;//��ʼ״̬
u8 gPre_Mode;
u32 gMoto_timecnt;
u8 gMoto_cntflag = 0;//ת����־ 0:ûת��1:ת
u8 version_number = 0;//�汾��   0:1.4   1:1.3
unsigned long gCur_timer,gLast_timer;
u8 Version_jud = 0;
u16 The_Current = 0;//2017.8.1��ǰ����

extern vu32 Timer_Counter;
extern u8* SD_IDF;
extern u8* CONSULT;//2017.8.2
extern u8 HZ16X16[];
/*******************************************************************************
������: Hardware_version
��������:Ӳ���汾�ж�
�������:��
���ز���:0:1.3�汾     1:1.4�汾
*******************************************************************************/
u8 Hardware_version(void)
{   
    u8 data[128];
    
    memset(&data[0],0,128);
    Oled_DrawArea(0,0,128,8,data);
    
    Delay_Ms(5);

    if(I2C_RadeByte() != 0xff)  return 1;
    else                        return 0; 
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
    }
}

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
    static u8 fixed_dir = 0; //�̶�����  0:����ģʽ, 1:�̶�ģʽ
    static u8 fixed_flag = 0; //��־  0:��תΪ�̶�, 1:������תΪ�̶�
    static u8 turbo = 0;//��ѹ��־
    static u8 run_flag = 0;//ת����־
    static u8 pre_pwm = 100;//previous pwm
    static u16 last_pwm = 0;
    static u16 overcurrent_cnt = 0;//�������� over current count
    static int Run_Time = 0;//��ʼת����ʱ��
    static u8 Restart_Pos = 0;//���¿�ʼת��־λ
    static u8 Stop_Pos = 0;//����ת��ʱ��ֹͣ
    u16 current_limt,limt_cnt;
    float cur_scale;
    float temp_angle;
    cur_scale = Get_Angle();   //��ȡ��ǰת���Ƕ�
    
    if(!gKey_Press) {//��������
        Set_Mode(IDLE_MOD);
        Clear_Screen();
        angle = 0;
        Run_Time = 0;
        Restart_Pos = 0;
        Stop_Pos =0;
    } else if(gKey_Press) {// ��������
        if(Restart_Pos == 0)
        {
            Restart_Pos =1;
            Run_Time = Timer_Counter;
        }
        if(Timer_Counter - Run_Time > MAX_RUNTIME)
        {   
            Stop_Pos = 1;
            fixed_dir = 0;
            fixed_flag = 0;
            angle = 0;
            turbo = 0;
            run_flag = 0;
            last_pwm = 0;
            TURBO_OFF();
            ptr = (u8*)HZ16X16;
            Oled_DrawArea(0,0,96,16,ptr);//����
        }
        
        gTimer[2] = SHOTDOWN_TIME;//������λ�ػ�ʱ��

        if(gPre_Mode == IDLE_MOD) {//����״̬
            gPre_Mode = CTR_MOD;//�������ģʽ

            fixed_dir = 0;
            fixed_flag = 0;
            angle = 0;
            turbo = 0;
            run_flag = 0;
            last_pwm = 0;
            overcurrent_cnt = 0;

            gTimer[0] = 20;  //��ʱ����������ʱ�� || ��תǰ����ѹʱ��
            gTimer[3] = 0;   //���ͣתʱ��
            gTimer[4] = 50;  //����Ƿ�̶�ģʽ�еĳ���ʱ���ж�
            gTimer[5] = 0;   //��ʾˢ�¼��
            
            if(Stop_Pos)        //��ʱ����
            {
                ptr = (u8*)HZ16X16;
                Oled_DrawArea(0,0,96,16,ptr);//����
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
        /*2017.8.1ȡ���޽Ƕȱ仯����������һ��ת��������ٶ�*/
//        if(run_flag == 0 && gTimer[4] == 0 && (fixed_flag == 0)) {
//            fixed_dir = 1;//�̶�ģʽ
//            run_flag = 1;//��ʼת��
//            last_pwm = pre_pwm;//���ڼ����ת����
//            gTimer[0] = 20;//����ѹʱ��
//        }

        if(fixed_dir == 0) { //���ٶ�ģʽ
            if(angle <= info_def.start_angle && angle >= -info_def.start_angle) { //��ת��
                run_flag = 0;
                Motor_Brake();
            } else { //ת������
                if(angle > info_def.start_angle) {//��,˳ʱ��
                    pre_dir = 1;
                    temp_angle = angle;
                } else  if(angle < -info_def.start_angle) {//��,��ʱ��
                    pre_dir = 0;
                    temp_angle = -angle;
                }
                if(info_def.torque_level != 0) { //�̶��ٶ�ģʽPWMΪ����ҹ̶�
                    last_pwm = MAX_PWM;
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
                fixed_flag = 1;
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
        //��ת������������
        if(angle >0) current_limt = 1100;//��תΪ���
        else if(info_def.torque_level != 1 && info_def.torque_level != 0)  current_limt = (11 - ((info_def.torque_level - 2))*3)*last_pwm;//
        else if(info_def.torque_level == 1)//��תʱP��Ť����Ƕ����������
        {
            if(temp_angle >= (info_def.start_angle + 60))  current_limt = 1100;//һ��ת������60���������Ť��
            else  if(temp_angle >= 80)
            {
                current_limt = 1100;
            }
            else
            {
                current_limt = temp_angle * 10 + 300;//P���Ƕ�����1���ת��������10mA
            }
        } 
        else current_limt = 1100;     
        //��ת���������ۼ�
        The_Current = (60000*Get_AvgAdc(0)/4096);//��ǰ����
        if(The_Current > current_limt)overcurrent_cnt++;
        else overcurrent_cnt = 0;
        //����λ�Ķ�ת���������ۼ���    
        if(info_def.torque_level == 1)//��P��λ��ʱ��
        {
          if(temp_angle >= (info_def.start_angle + 60))  limt_cnt = 100;//һ��ת������60���������Ť��
          else {
                  if(temp_angle - info_def.start_angle <20)//0~19��
                  {
                      limt_cnt = 40;
                  }
                  else
                  {
                      if(temp_angle - info_def.start_angle <40)//20~39
                      {
                          limt_cnt = 40;
                      }
                      else
                      {
                          if(temp_angle - info_def.start_angle <60)//40~59
                          {
                              limt_cnt = 60;
                          }
                          else//60~80
                          {
                              limt_cnt = 100;
                          }
                      }
                  }
               }
        }
        else//����P����������λ
        {
          if(info_def.torque_level != 0)  limt_cnt = 40 + (info_def.torque_level - 2)*20;//������λ��������
          else  limt_cnt = 100;
        }        

        if((gTimer[0] == 0) && (overcurrent_cnt > limt_cnt)) {//��ת����,���ɲ��,3s�ڲ�ת�������ת�������200ms����ѹ��
            Motor_Brake();
            overcurrent_cnt = 0;
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
#define READ_CHG   (GPIOB->IDR & GPIO_Pin_6)
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
        POWER_OFF_1();
    }
    else
    {
        POWER_OFF();//�ػ�
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
        TorqueLv_Set();//Ť�صȼ�����
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
