/******************** (C) COPYRIGHT 2018 e-Design Co.,Ltd. *********************
  DS213_APP Calibrat.c                                           Author : bure
*******************************************************************************/
#include "Sys.h"
#include "DS213Bios.h"
#include "Calibrat.h"

/*******************************************************************************
  У����ֱλ����λƫ����б��ƫ��
*******************************************************************************/
void Correction(void)
{
  u16 BakA[10], BakB[10];
  for(u32 i = 0; i < ITEM_END; i++) CtrlUpdate(i);          // ȫ��Ӳ����ʼ������
  *Hw.pOut_A_Coupl = AC, *Hw.pOut_B_Coupl = AC;             // ģ������Ϊ�������
  SetBase(15);                                              // ���ò������� 10uS/Div
  SetTriggTyp(TRIG_ANY, Menu[V_T].Src);                     // �����ⴥ��
  Hw.pDevInit(SO_ANLG);
  *Hw.pFout_mV_DAC = 0;                                     // ����ģ�����ֵ = 0
  Hw.pFout_DMA(DISABLE);
  SetRangeA(9), SetRangeB(9), 
  Delay_mS(1000);                                           // �ȴ���������ȶ�
  u32 AbsA, AbsB;
  for(s32 Range = 9; Range >= 0; Range--){
    SetOffsetA(9, 20), SetOffsetB(9, 20);
    SetRangeA(Range), SetRangeB(Range);
    BakA[Range] = Diff[CH_A][Range]; 
    BakB[Range] = Diff[CH_B][Range];                        // ����ƫ��У��ϵ��
    
//u8 Str[10]; 
//RowPosi(0*8, Range*18+36);
//u8To2DecStr(Str, Range); AddStr(TXT2C, CHAR, Str); AddStr(TXT2C, CHAR, ": ");

    Delay_mS((Range == 4) ? 500 : 200);                     // �ȸߵ�ѹ�л��ȶ�    
    for(u32 n = 20; n >= 10; n--){
      SetOffsetA(Range, n), SetOffsetB(Range, n);           // ���õͶ�ƫ��
      *Hw.pOut_Clr = 1, *Hw.pOut_Clr = 0;                   // ��ʼ����
      Delay_mS(50);                                         // �ȴ�ƫ���˲��ȶ�    
      s32 DiffA = n-FPGA_ByteRd(A_C_CH, VAVG);
      s32 DiffB = n-FPGA_ByteRd(B_D_CH, VAVG);              // �Ͷ�ƫ�����
      
//s8To2DecStr(Str, DiffA); AddStr(TXT2C, CHAR, Str); AddStr(TXT2C, CHAR, " ");
      
      AbsA = abs(DiffA), AbsB = abs(DiffB);
      if(AbsA == 0 && AbsB == 0) break;
      Diff[CH_A][Range] += DiffA; 
      Diff[CH_B][Range] += DiffB;                           // �����Ͷ�ƫ��
    }
    if(AbsA > 2) Diff[CH_A][Range] = BakA[Range];           // У׼ʧ�ָܻ�ԭֵ
    if(AbsB > 2) Diff[CH_B][Range] = BakB[Range];
    AddStr(TXT2C, CHAR, ".");
  }
  SetOffsetA(9, 190), SetOffsetB(9, 190);                   // ���ø߶�ƫ��
  SetRangeA(9), SetRangeB(9), Delay_mS(1000);               // �ȵ�ѹ�л���ѹ�ȶ�
  BakA[0] = Slope[CH_A], BakB[0] = Slope[CH_B];             // ����б��У��ϵ��
  
//RowPosi(3*8, 18);
//u8 Str[10]; 

  for(u32 n = 0; n < 4; n++){
    s32 Asum = 0, Bsum = 0;
    s32 OffsA, OffsB;
    AddStr(TXT2C, CHAR, (u8*)".");
    for(u32 Range = 9; Range >= 6; Range--){
      SetRangeA(Range), SetRangeB(Range);                   // ���õ�ǰ��λ
      SetOffsetA(Range, 190), SetOffsetB(Range, 190);       // ���ø߶�ƫ��
      *Hw.pOut_Clr = 1, *Hw.pOut_Clr = 0;                   // ��ʼ����
      Delay_mS((Range == 9) ? 500 : 100);                   // �ȴ������ȶ�    
      OffsA = FPGA_ByteRd(A_C_CH, VAVG);
      OffsB = FPGA_ByteRd(B_D_CH, VAVG);                    // �����߶�ƫ�����
      Asum += OffsA, Bsum += OffsB;
    }
    Slope[CH_A] = (Slope[CH_A]*190)*4/Asum;                 // ����б�����ϵ��
    Slope[CH_B] = (Slope[CH_B]*190)*4/Bsum;
    SetOffsetA(6, 190), SetOffsetB(6, 190);                 // �����߶�ƫ��
    *Hw.pOut_Clr = 1, *Hw.pOut_Clr = 0;                     // ��ʼ����
    Delay_mS(100);                                          // �ȴ�ƫ���˲��ȶ�    
    OffsA = FPGA_ByteRd(A_C_CH, VAVG)-190;
    OffsB = FPGA_ByteRd(B_D_CH, VAVG)-190;                  // �����߶�ƫ�����
    
//s16To5DecStr(Str, OffsA); AddStr(TXT1C, CHAR, Str); AddStr(TXT1C, CHAR, " ");

    AbsA = abs(OffsA), AbsB = abs(OffsB);
    if(AbsA == 0 && AbsB == 0) break;
  } 
  if(AbsA > 2) Slope[CH_A] = BakA[0];                       // У׼ʧ�ָܻ�ԭֵ
  if(AbsB > 2) Slope[CH_B] = BakB[0];
  for(u32 i = 0; i < ITEM_END; i++) Menu[i].Flg |= UPDT;

//while(nKeySt & K4){};
  
} 

/*********************************  END OF FILE  ******************************/
