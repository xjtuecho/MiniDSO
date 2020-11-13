/********************* (C) COPYRIGHT 2018 e-Design Co.,Ltd. ********************
  DS213_APP main.c                                               Author : bure
********************************************************************************
                               �޸ı�ע
  ----------------------------------------------------------------------------
  18.07.25 APP V1.01: ��ʼ�������ˡ����� V- �� ADC������
                      ��ʼ���лָ��� AutoCalib ����
  18.07.27 APP V1.02: ����10V��λ���ӿ����߼�
                      ������������������
  18.07.28 APP V1.03: �޸���ص�ѹ����
                      �����������ļ�ʱ�Զ�У׼�󱣴��ļ�
                      �����ļ�������BAK���ļ�ֻ�ܱ���һ����Ч���غ��ۼӿ��������⣩
                      ���ƴ���ʱ��LED����Ч��
                      �޸�����XP���ƶ���ĩ�ˣ��Ͽ������źţ������޷����
                      singleģʽ����������״̬λFINISH==FULL��
                      ���������ͱ����������
                      ����CH_D�����й���
                      ��������ڵ�ģ�Ⲩ�����
  18.08.08 APP V1.04: ����FAT12�ļ���ʽ���ļ���������
  18.08.10 APP V1.05: ����CAL��ʱ��Shiftʱ�������ۼӣ���û�������Сֵ���ƣ�
                      �޸�T0�α�Խ��ʹ���λ��
                      ����SCAN��ȡ��NONEͬ��ģʽ��SCAN��ͬ����ʽ���ƺ�����������
                      ����DUT��TH��TL
                      <Vt����ģʽ���ȶ�������������
  18.08.14 APP V1.06: �����ļ��ӱ��0��ʼ
                      ����csv�ļ������ʽ
                      ����saveDat��loadDat�Լ�CH3�ĵ�����¼����
                      �ļ�������Զ��ۼӱ��
  18.08.15 APP V1.07: �����ļ������ʾ
                      ɾ��TWTH
  18.08.15 APP V1.08: ���ӿ����Զ�У׼
                      ����Buf�����ʽ
                      ���������ȴ�ʱ��
  18.08.17 APP V1.09: ����T0����λ�ã���ʱ��10us�������ڲ�ֵ�㷨��T0�봥��λ����ƫ�ƣ�δ�����������
 
  18.08.27 APP V1.10: ����Load DAT����
                      ���Ӵ�����ѹ��ֵ��ʾ
                      ���Ӵ���ʱ��˵�ѡ��
  18.08.29 APP V1.11: ����������ʱ������
                      ���ӵ�ص�ѹ��ʾ
 
  18.09.03 APP V1.12: ��������ʱ�䱣�ֲ�������
                      ����K4������������
                      ����δ������ʱ�򣬳���K4+K3�ػ�������Ӳ����֧�֣�
 
  18.09.27 APP V1.13: �޸��͵����������������С��3v�Զ��ػ�
                      0.5us��0.2us��0.1us ʱ���޸�Ϊ500ns��200ns��100ns
                      �������������ʾ����״̬
                      ����Ļͨ����ʾ��1234�ĳ�ABCD
           APP V1.14: �޸��͵����������������С��3.2v�Զ��ػ�

  18.11.09 APP V1.15: ��Ϊ���޿������������ļ�ʱ�ŵ��ÿ���У׼�������濪������
                      �����˱������ʱ��Ļ��ʾ���������ӳ�����Ϣ��ʾͣ��ʱ��
                      ģ��������β���������20KHz��λʱ50�㣬������λʱ100��
 
  18.12.22 APP V1.20: ��� DFU3.73 �� FPGA STMU_213_016.bin �������¸�д
                      �޸��� APP V1.xx ���ڵĸ�������

  19.02.25 APP V1.21: �޸��� NORM ģʽ����ʾ���β��ܱ��ֵ�����
                      �޸��˰��� K4 ���������������Զ��˳� STOP ״̬������

  19.03.08 APP V1.22: �޸��˲���ƫ�����ϴ�Ļ��������Զ�У��������
                      �޸��˸���ͬ��ģʽ�µĲ�����ʾ��ʽ
                      

*******************************************************************************/
#include "Sys.h"
#include "DS213Bios.h"


HwDrvDef  Hw;
u8  APP_VERSION[12] = { "Ver 1.22" };
u8  PwrDownSt = 0;

/*******************************************************************************

*******************************************************************************/
void main(void)
{
  pFunc1 __BiosInit = (pFunc1)0x08000101; // Point to DFU BiosInit
  __BiosInit((u32)&Hw);                   // ����Ӳ���豸��������
  
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);
  while(SysTick_Config(SystemCoreClock/1000)){};

/****** �� APP ���� DEBUG ��������ʱ���ã�ͨ�� DFU ����ʱ�ò��ֲ��������� ****
  
  Hw.pDevInit(PWRCTRL);
  Hw.pDevInit(KEYnDEV);
  Hw.pDevInit(DISPDEV);
  Hw.pDevInit(BUZZDEV);
  Hw.pClrScrn(BLK);
  u16 x0 = Hw.LogoX0, y0 = Hw.LogoY0;
  Hw.pDispLogo(x0, y0);
  *Hw.pPwm_Bkl = 50;
  *Hw.pBuz_Vol = 50;
//  Beep_mS(500);
  Hw.pDevInit(FPGADEV);
  Hw.pDevInit(FPGACFG);
  */
/******************************************************************************/
  
  DispStr(12*8, 90, WHT, CHAR, "Oscilloscope APP");
  DispStr(29*8, 90, WHT, CHAR,  APP_VERSION );
  
  Hw.pDevInit(USBDISK);
  Hw.pDevInit(USB_DEV);
  USB_Init();
  AppDiskInit();
  InitFatParam();

  *Hw.pOut_nPD = 1;                                // ���� V- �� ADC
  Hw.pDevInit(SGNL_IN);                            // �ź�����ͨ����ʼ��
  Hw.pDevInit(SGNLOUT);                            // �ź����ͨ����ʼ��
  Hw.pDevInit(SO_ANLG);                            // �ź����Ϊģ���ź�
  Hw.pDevInit(SO_DGTL);                            // �ź����Ϊ�����ź�
  FPGA_CtrlRW(A_C_CH, CHIP_RST);                   // FPGA #0 ģ�鸴λ
  FPGA_CtrlRW(B_D_CH, CHIP_RST);                   // FPGA #1 ģ�鸴λ
  SmplStart();                                     // FPGA ��ʼ��һ�ֲ���
  TrackClr();                                      // �����ʾ���ι켣
  
  uc8 STR1[] = "Parameter record not found";
  uc8 STR2[] = "Reload parameter from disk";
  uc8* STR   = (LoadParam()) ? STR1 : STR2;        // ��ȡԤ�迪������
  DispStr(12*8, 30, GRN, CHAR, (u8*)STR);          // ��ʾ��ȡ�����Ϣ

//  if(STR == STR1){                                 // ��Ԥ�迪�����������ļ�
//    RowPosi(12*8, 50);
//    AddStr(TXT2C, CHAR, (u8*)"Correction");
//    Correction();                                  // ���������У��
//  } else Delay_mS(2000);
  
  *Hw.pPwm_Bkl = Pop[SBKL].Val;                    // ���ñ�������
  *Hw.pBuz_Vol = Pop[SVOL].Val;                    // ���÷���������
  
  //****************************** ϵͳ��ѭ������ ******************************
  KeyAct = 0;
  while (1){
    UpdtMainMenu();                                           // ����������˵�
    DispSync();                                               // ��ʾͬ������ 
    if(PopUpdt){
      UpdtPopMenu();                                          // ���µ������˵�
      PopUpdt = 0;
    } 
    //============================== ��������ѭ�� ==============================
    u16 Act = KeyAct & (ENCDBIT | K1_ACTn | K2_ACTn | K3_ACTn);
    KeyAct = 0;
    if(Act){                                                  // �а��������������
      u32 Track = Menu[Item].Src;
      u32 Range = Menu[(Track == CH_A)? RNA : RNB].Val;
      u16 *pKn  = &Menu[Item].Val;
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ K1 Push ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      if(Act & K1_ACTn){ 
        if(~nKeySt & K4){                                     // ��ס K4 ʱ�� K1
          u32 r = Snapshot();                                 // ���浱ǰ��Ļ��ͼ
          RowPosi(Menu[TM2].X0, Menu[TM2].Y0);
          if(r == OK) AddStr(TXT2C, CHAR, " Saveing OK  ");
          else        AddStr(WARNG, CHAR, " Saveing Err ");
          Delay_mS(2000);
          Menu[TM2].Flg |= UPDT;                              // ���ø�����±�־
        } else {                                              // �������� K1
          Menu[RUN].Val = (Menu[RUN].Val)? HOLD : SMPL;       // �л� ��ͣ/���� ״̬
          Menu[RUN].Flg |= UPDT;                              // ���ø�����±�־
        }
      }
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ K2 Push ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      if(Act & K2_ACTn){
        if(~nKeySt & K4){                                     // ��ס K4 ʱ�� K2
          u32 r = SaveParam();                                // ���浱ǰ���ò���
          RowPosi(Menu[TM2].X0, Menu[TM2].Y0);
          if(r == OK) AddStr(TXT2C, CHAR, " File Saved  ");
          else        AddStr(WARNG, CHAR, " Save Error  ");
          Delay_mS(2000);
          Menu[TM2].Flg |= UPDT;                              // �ָ�TM2��ʾ��
        } else {                                              // �������� K2
          PopHide = (PopHide) ? 0 : 1;                        // ��/�ر� ������
          if(!PopHide) UpdtPopMenu();                         // ���µ������˵�
        }
      }
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ K3 Push ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      if(Act & K3_ACTn){
        if(~nKeySt & K4){                                     // ��ס K4 ʱ�� K3
          if(Hw.LicenceOk == 0) *Hw.pOut_PEn = 0;             // ����ǰ���Թػ�
          else {
            RowPosi(12*8, 120);
            AddStr(TXT2C, CHAR, (u8*)"Correction");
            Correction();                                     // �������У��
            RowPosi(23*8, 120);
            AddStr(TXT2C, CHAR, "Completed    ");
            Delay_mS(2000);
          }
        } else {                                              // �������� K3
          if(PopHide){                                        // ���˵��²���
            if(Item == V_T || Item == TRG){
              Menu[Item].Flg |= UPDT, Menu[Item].Src += 1;
              Menu[Item].Src &= 3;                            // ѡ�� CH A~D
              Menu[V_T].Src = Menu[Item].Src;
              Menu[V_T].Flg |= UPDT;                          // ��������ͬ��
              Menu[TRG].Src = Menu[Item].Src; 
              Menu[TRG].Flg |= UPDT;
              Menu[T_0].Src = Menu[Item].Src;
              Menu[T_0].Flg |= UPDT;
            } else if(Item == YNP || Item == TM1 || Item == TM2){
              Menu[Item].Src += 1;
              Menu[Item].Src &= 3;                            // ѡ�� CH A~D
              Menu[Item].Flg |= UPDT;
            } else if(Item == VM1 || Item == VM2 || Item == CAL){
              Menu[Item].Src += 1;
              Menu[Item].Src &= 1;                            // ѡ�� CH A/B
              Menu[Item].Flg |= UPDT;
            } else if(Item >= V_1 && Item <= T_2) Menu[Item].Flg ^= INVR;
          } else PopProcess();                                // �������ڲ���
        }
      }
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Indx Encoder ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      if(Act & ENCD_1n || Act & ENCD_1p){
        u32 Inc   = (~nKeySt & K4) ? 10 : 1;                  // ��ס K4 ʱ���� *10
        if(PopHide){                                          // ���˵��µĲ���
          Menu[Item].Flg |= UPDT;  
          if     (Item == V_T) pKn = &Vt[Track];
          else if(Item == YNP) pKn = &Yn[Track];
          else if(Item == CAL) pKn = &Diff[Track][Range];
          if((Item == VM1 || Item == VM2)&& *pKn == RMS){     // �뿪��Чֵ����ʱ
            Menu[T_1].Flg &= ~INVR, Menu[T_2].Flg &= ~INVR;   // ���� T1,T2 �α�
          }
          if(Item != V_T || Track < CH_C){
            if(Act & ENCD_1n) *pKn =IndxInc(*pKn, Inc, Item); // Indx+
            else              *pKn =IndxDec(*pKn, Inc, Item); // Indx-
          }
          if(Item == OUT){                                    // OUT ����
            Menu[FRQ].Flg |= UPDT;
            if(Menu[FRQ].Val > 10 && *pKn != DGTL) Menu[FRQ].Val = 10; 
          }
          if(Item == FRQ){                                    // FRQ ����
            if(*pKn > 10 && Menu[OUT].Val != DGTL) *pKn = 10; 
          }
          if(Item == SYN && *pKn == SLOW){                    // SYN ����
            if(Menu[TIM].Val > 5 ) Menu[TIM].Val = 5; 
            Menu[TIM].Flg |= UPDT;
          }
          if(Item == TIM && Menu[SYN].Val == SLOW){           // TIM ����
            if(*pKn > 5) *pKn = 5; 
          }
        } else {                                              // �������µĲ���
          PopUpdt = 1;
          if(Act & ENCD_1n) PopValInc(Line, Inc);             // Line+
          else              PopValDec(Line, Inc);             // Line-
        }
      }
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Item+ Encoder ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      if(Act & ENCD_2p){
        Menu[Item].Flg |= FLSH;                               // ǰ������˸��־
        if(PopHide){                                          // ���˵��µĲ���
          Item = (Item >= VM1) ? 0 : Item+1;                  // ��ǰ��+1
        } else {                                              // �������µĲ���
          Line = (Line == SPDT) ? 0 : Line+1;                 // ��ǰ��+1
          PopUpdt = 1;
        }
      }
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Item- Encoder ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      if(Act & ENCD_2n){
        Menu[Item].Flg |= FLSH;                               // ǰ������˸��־
        if(PopHide){                                          // ���˵��µĲ���
          Item = (Item > 0) ? Item-1 : VM1;                   // ��ǰ��-1
        } else {                                              // �������µĲ���
          Line = (Line > 0) ? Line-1 : SPDT;                  // ��ǰ��-1
          PopUpdt = 1;
        }
      }
      //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      Beep_mS(20);
      if(Pop[SPDT].Val != 0) PD_Cnt = Pop[SPDT].Val*60;       // �������߼�ʱ��
    }
    //==========================================================================
    if(*Hw.pSt_Vin == 1){
      PD_Cnt = Pop[SPDT].Val*60;                              // �������߼�ʱ��
      
    }
    if(PD_Cnt == 0 && PwrDownEn && *Hw.pSt_Vin == 0){         // ����ʡ��״̬
      Hw.pPowerDown(ENABLE);                                  // �͹��Ĺ���ģʽ
      *Hw.pPwm_Bkl = 0;                                       // �رձ���
      *Hw.pOut_Clr = 1;                                       // FPGA ʡ��״̬
      PwrDownSt = 1;
    } else if(PwrDownSt == 1){                                // �ָ�����״̬
      Hw.pPowerDown(DISABLE);                                 // ��������ģʽ
      *Hw.pPwm_Bkl = Pop[SBKL].Val;                           // �ָ�����
      *Hw.pOut_Clr = 0;                                       // FPGA ����״̬
      *Hw.pPwm_LED = LED_PWM_MAX;                             // �رպ�����
      PwrDownSt = 0;
      PD_Cnt = Pop[SPDT].Val*60;                              // �������߼�ʱ��
    }
  }
  //****************************************************************************   
}

/******************************** END OF FILE *********************************/
