/********************* (C) COPYRIGHT 2016 e-Design Co.,Ltd. ********************
File Name :      UI.c
Version :
Description:
Author :         Celery
Data:            2016/12/22
History:

*******************************************************************************/
#include "Oled.h"
#include "Oledfont.h"
#include "UI.h"
#include "string.h"
#include "ctrl.h"
#include "own_sprintf.h"
#include "stdio.h"//2017.8.2��ʾ��������
#include "adc.h"

u8 gScrew_position = 17;/*��˿ͼ����ʾλ��*/
extern u16 The_Current;//2017.8.1��ǰ����

/*******************************************************************************
������: Clear_Screen
��������:����
�������:NULL
���ز���:NULL
*******************************************************************************/
void Clear_Screen(void)
{
    u8 data[128];
    u8 i;
    
    memset(&data[0],0,128);
    for(i = 0; i < 2; i++) {
        Oled_DrawArea(0,i * 8,128,8,data);
    }
}

/*******************************************************************************
������: Display_Str
��������:��ʾ16*16�ַ���
�������:x: λ�� str :��ʾ�ַ���
���ز���:NULL
*******************************************************************************/
void Display_Str(u8 x,u8 y, char* str)
{
    u8* ptr;
    u8  temp;

    if((x < 1)||(x > 16))  x = 0;
    else x--;
    while(*str != 0) {
        temp = *str++ - ' ';//�õ�ƫ�ƺ��ֵ

        ptr = (u8*)ASCII6x8;
        ptr += temp*6;
        Oled_DrawArea(x * 8,y,6,8,(u8*)ptr);
        x++;
    }
}

/*******************************************************************************
������: Show_SetLv
��������:��ʾ���õ�λ
�������:gear ��λ
���ز���:NULL
*******************************************************************************/
void Show_SetLv(u8 gear)
{
    u8* ptr;
    ptr = (u8*)SET_CNT + 96*2*gear ;

    Oled_DrawArea(0,0,96,16,ptr);//��������Ļ
}
/*******************************************************************************
������: Low_Power
��������:��ʾ�͵�������
�������:NULL
���ز���:NULL
*******************************************************************************/
void Low_Power(void)
{
    u8 *ptr;
    ptr = (u8*)LOW_POWER;

    Oled_DrawArea(0,0,96,16,ptr);
}
/*******************************************************************************
������: Show_BatV
��������:��ʾ����|���״̬
�������:step: ���� Charging:���״̬
���ز���:NULL
*******************************************************************************/
void Show_BatV(u8 step,u8 Charging)
{
    u8 *ptr;
    static u8 min_v = 10;
    //char str[8] = {0};
    
    if(step > 10 && Charging == 0) step = 10;

    if(Charging > 0) { //���ʱ�������������� �����Զ���
      if(step > min_v)
      {
        min_v = step;
      }
    } else {
        if(min_v >= step) min_v = step;
        else step = min_v; //��ֹ������ʾ����
    }

    if(Charging == 0) { //���ǳ��
        ptr = (u8*)BAT_STAT + 20*step;
        Oled_DrawArea(0,0,10,16,ptr);
    } else if(Charging == 1) { //���
        ptr = (u8*)BAT_STAT + 20*(11 + step);
        Oled_DrawArea(0,0,10,16,ptr);
    } else if(Charging == 2) {
        ptr = (u8*)BAT_STAT + 20*21;
        Oled_DrawArea(0,0,10,16,ptr);
    }

//    sprintf((char*)str, "%d", step);  //2017.8.2
//    Display_Str(3, 2, (char*)str);
//    sprintf((char*)str, "%dV", (Get_Adc(VBTY)*600/4096));  //2017.8.2
//    Display_Str(7, 2, (char*)str);
}
/*******************************************************************************
������: Clear_Screw
��������:����
�������:NULL
���ز���:NULL
*******************************************************************************/
void Clear_Screw(void)
{
    u8 data[128],i;

    memset(&data[0],0,128);
    for(i = 0; i < 2; i++) {
        Oled_DrawArea(11,i * 8,81,8,data);//92 -16
    }
}
/*******************************************************************************
������: Show_ScrewMove
��������:��̬��ʾ��˿ͼƬ
�������:status : Screw down status = 0 down��1up
���ز���:NULL
*******************************************************************************/
void Show_ScrewMove(u8 status)//Screw down status = 0 down��1up
{
    u8 *ptr;
    static u8 i = 1,step = 0;    
//    u8 str_test[5] = {0};//2017.8.2
    
    if(gScrew_position != i) i = gScrew_position;
    if(status == 0) { //down
        ptr = (u8*)SD_IDF + 64*step;
        Oled_DrawArea(11 + i,0,32,16,ptr);
        i++;
        if(i >= 50 || i <= 0) i = 0;//50-16
    }
    if(status == 1) { //up
        ptr = (u8*)SD_IDF + 64*step;
        Oled_DrawArea(11 + i,0,32,16,ptr);
        i--;
        if(i <= 0 || i >= 50) i = 49;//50-16
    }
    
    /*����ʾ����2017.8.2*/
//    ptr = (u8*)CONSULT+8;
//    Oled_DrawArea(92,0,4,16,ptr);//����
    /*��ʾ����2017.8.2*/
//    sprintf((char*)str_test, "%d", The_Current);  //2017.8.2
//    Display_Str(5, 5, (char*)str_test);
    
    ptr = (u8*)LEVEL_IDF + 11*2*info_def.torque_level;
    Oled_DrawArea(0,0,11,16,ptr);//��λ
                                               
    gScrew_position = i;
    step++;
    if(step == 4)  step = 0;
    
    /*2017.8.1��ǰ����������*/
    if(step%2)
    {
        if(The_Current>1100)//������ת����
        {
            ptr = (u8*)I_Size + 56;//������С��ʾ��
            Oled_DrawArea(93,0,2,16,ptr);
        }
        else if(The_Current == 0)//0
        {
            ptr = (u8*)I_Size;
            Oled_DrawArea(93,0,2,16,ptr);
        }
        else if(The_Current < 300)//1~300mA
        {
            ptr = (u8*)I_Size + (The_Current/50*4 + 4);
            Oled_DrawArea(93,0,2,16,ptr);
        }
        else
        {
            ptr = (u8*)I_Size + 24 + ((The_Current - 300)/100*4);   
            Oled_DrawArea(93,0,2,16,ptr);
        }
    }
}
/*******************************************************************************
������: Show_Screw
��������:�̶�λ����ʾ��˿
�������:u8 mode  0:����ģʽ   1:����ģʽ�µĹ̶���ʾ
���ز���:NULL
*******************************************************************************/
void Show_Screw(u8 mode)
{
    u8 *ptr;
 
    if(mode)
    {
        ptr = (u8*)LEVEL_IDF + 11*2*info_def.torque_level;
        Oled_DrawArea(0,0,11,16,ptr);//��λ
        ptr = (u8*)CONSULT + 8;
        Oled_DrawArea(92,0,4,16,ptr);//����
    }
    else
    {
        ptr = (u8*)LEVEL_IDF + 11*2*info_def.torque_level;
        Oled_DrawArea(75,0,11,16,ptr);//��λ
        ptr = (u8*)CONSULT;
        Oled_DrawArea(92,0,4,16,ptr);//����
    }
    ptr = (u8*)SD_IDF;
    Oled_DrawArea(27,0,32,16,ptr);//��˿
    gScrew_position = 17;    
}
/*******************************************************************************
������: Print_Integer
��������:�������ݴ�ӡ����
�������:data ����,posi λ��
���ز���:NULL
*******************************************************************************/
void Print_Integer(s32 data,u8 posi)
{
    char str[8];

    own_sprintf(str,"%d      ",data);
    Display_Str(posi,0,str);
}

/******************************** END OF FILE *********************************/
