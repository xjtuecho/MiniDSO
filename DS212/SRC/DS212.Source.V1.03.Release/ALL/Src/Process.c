/******************** (C) COPYRIGHT 2017 e-Design Co.,Ltd. *********************
 File Name : Process.c
 Version   : DS212                                                Author : bure
*******************************************************************************/
#include "Version.h"
#include "Process.h"
#include "Bios.h"
#include "Func.h"
#include "Draw.h"
#include "Menu.h"
#include "Drive.h"
#include "File.h"
#include "Math.h"

void __Mem32Fill(u32* pMem, u32 Data, u32 n);
void WaveLtdOut(u8* Buf, s16 Value);
void Analys(void);
void Align_Set(void);

u16  T0_PerCnt = PRE_SMPL;

//垂直通道系数表
//=======+======+======+======+======+======+======+======+======+======+======+
//       |          LV 量程挡位             |          HV 量程挡位             |
//-------+------+------+------+------+------+------+------+------+------+------+
// 量程  |   0  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |
//-------+------+------+------+------+------+------+------+------+------+------+
// 增益  |  x20 |  x10 |  x5  |  x2  |  x1  |  x20 |  x10 |  x4  | x2.5 |  x1  | 
//-------+------+------+------+------+------+------+------+------+------+------+
//uc8 GK[]={    2,     4,     2,    4,    8,     20,     2,     4,    10,    20,}; 
uc8 GK[]={    2,    4,     10,    20,    4,     10,     20,     4,    10,    20,}; 
//-------+------+------+------+------+------+------+------+------+------+------+

// 水平通道系数表
//========+======+======+======+======+======+======+======+======+======+======+          
//uc8 TB[][5] = // 时基档字符串 /Div                                            |
//--------+------+------+------+------+------+------+------+------+------+------+
//        { "1uS", "2uS", "5uS","10uS","20uS","50uS",".1mS",".2mS",".5mS","1mS", 
//          "2mS", "5mS","10mS","20mS","50mS","0.1S","0.2S","0.5S","1.0S","2.0S"};
//--------+------+------+------+------+------+------+------+------+------+------+
// 标准值    25M  12.5M     5M   2.5M  1.25M    .5M   .25M  .125M    50K    25K |
//         12.5K     5K   2.5K  1.25K    .5K   .25K  .125K   50Hz   25Hz   12.5 |
//--------+------+------+------+------+------+------+------+------+------+------+
// 实际值  10.3M  10.3M  5.14M  2.57M  1.252M   .5M   .25M  .125M    50K    25K |
//         12.5K     5K   2.5K  1.25K    .5K   .25K  .125K   50Hz   25Hz   12.5 |
//--------+------+------+------+------+------+------+------+------+------+------+

uc16 PSC[] ={  2 ,   2 ,    2 ,    2 ,    4 ,   32 ,   32 ,   32 ,    32 ,   32 ,
              32 ,  64 ,  128 ,  128 ,   256,   512,   512,   512,   1024,  1024};
//----------+------+------+------+------+------+------+------+------+------+----+
uc16 ARR[] ={ 29 ,  29 ,   29 ,    29,   29 ,    9 ,   18 ,   36 ,    90 ,    180,
              360,  450,   450,   900,  1125,  1125,  2250,  5625,   5625,  11250};
//--------+------+------+------+------+------+------+------+------+------+------+
uc16 KS[]={  205,   410,   512,  1024,  1024,  1024,  1024,  1024,  1024,  1024, 
//uc16 KS[]={  256,   512,   512,  1024,  1024,  1024,  1024,  1024,  1024,  1024, 
//uc16 KS[]={ 128,  256,  512,  1024,  1024,  1024,  1024,  1024,  1024,  1024, 
/* 插值 */  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,  1024,};
//========+======+======+======+======+======+======+======+======+======+======+

u8   Norm_Clr = 0, NORM_Kflag, SNGL_Kflag = 1;
u16  Smpl[0x4000];
u8*  V_Buf = VRAM_PTR; // 显示缓冲区 VRAM = CCM = 0x10000000~0x10001FFF 8KB
s32  PmaxA, PmaxB, PminA, PminB, PavgA, PavgB,PrmsA, PrmsB,PvppA,PvppB,PmidA, PmidB;
u32  PssqA, PssqB;
u32  HighA, HighB, LowA,  LowB,  EdgeA, EdgeB;


 // 垂直通道(零点/增益)校正系数表  LV = 低压挡,  HV = 高压挡   
 //=============+=======+=======+=======+=======+=======+=======+=======+=======+
 //  量程挡位   |   LV  |   HV  |   LV  |   HV  |   LV  |   HV  |   LV  |   HV  |
 //-------------+-------+-------+-------+-------+-------+-------+-------+-------+
 //             | CH_A 零点偏移 | CH_B 零点偏移 | CH_A 增益系数 | CH_B 增益系数 |
 //-------------+-------+-------+-------+-------+-------+-------+-------+-------+
s16   Kpg[] =    {  2044,   2046, 2044,   2046,  2011,   2046,   1230,  1020,   1120,  1230,   1020,   1120}; 
sc16 Kpgc[] =    {  2005,   2053, 2005,   2053,  1983,   2058,   1254,  1261,   1254,  1261,   1254,   1256};
//=============+=======+=======+=======+=======+=======+=======+=======+=======+
s16 *KpA = &Kpg[0], *KpB = &Kpg[3], *KgA = &Kpg[6], *KgB = &Kpg[9];
s16 Posi_3F1, Posi_3F2, Posi_3F3;
s16 Trigger_k;              //是否触发 大于0触发，小于0不触发
s16 Ch1_Posi, Ch2_Posi, Ch3_Posi, Tri_Posi;
/*******************************************************************************
 Align_Set:  误差校正
*******************************************************************************/
void Align_Set(void)
{ 
  s16 i, TmpA = 0, TmpB = 0, StA = 0, StB = 0;
  
  Analys(); 
  for(i=0; i<200; i++){
    AiPosi(100);          BiPosi(100);       // 设置参考点
    Wait_mS(10);                             // Wait 10mS
    Analys();                    
    TmpA = 2048-PavgA;    TmpB = 2048-PavgB;
    
    if(TmpA != 0) {
      KpA[KindA+(StateA?1:0)] += TmpA; StA = 0;           // CH_A 误差校正
    } else StA++; 
    
    if(TmpB != 0) {
      KpB[KindB+(StateB?1:0)] += TmpB; StB = 0;           // CH_B 误差校正
    } else StB++; 
    
    if((StA > 4)&&(StB > 4)) return;
  } 
}
/*******************************************************************************
 Channel's zero alignment:  通道垂直位移零点对齐
  u32  Bk = TmpB; // 8192~409
  __Ctrl(BOFFSET,(((u8)Val-100)*1024*GK[GainB+5*KindB])/KgB[KindB]+KpB[KindB]);
*******************************************************************************/
void Zero_Align(void)
{
      
  GainA  = GainB  = 8;//x20;  
  KindA  = KindB  = HV;
  StateA = StateB = ACT;
  __Ctrl(AiRANGE, HV+AC+ACT);  // 设置 HV 挡输入量程
  __Ctrl(BiRANGE, HV+AC+ACT);  // 设置 HV 挡输入量程
  AiPosi(100);                 // 设置 HV 挡参考零点  
  BiPosi(100);                 // 设置 HV 挡参考零点  
  Wait_mS(1000);  
  //for(u8 i=0; i<100; i++) Analys(); 
  Align_Set();                      // 延时后校正零点误差 
  
  /**/
  GainA  = GainB  = 5;//0x20; 
  KindA  = KindB  = HV;
  StateA = StateB = GND;
  __Ctrl(AiRANGE, HV+AC+GND);  // 设置 MV 挡输入量程
  __Ctrl(BiRANGE, HV+AC+GND);  // 设置 MV 挡输入量程
  AiPosi(100);                 // 设置 MV 挡参考零点 
  BiPosi(100);                 // 设置 MV 挡参考零点 
  Wait_mS(1000);  
  Align_Set();                      // 延时后校正零点误差
  
  GainA  = GainB  = 2;//0x20;   
  KindA  = KindB  = LV;
  StateA = StateB = GND;
  __Ctrl(AiRANGE, LV+AC+GND);  // 设置 LV 挡输入量程
  __Ctrl(BiRANGE, LV+AC+GND);  // 设置 LV 挡输入量程
  AiPosi(100);                 // 设置 LV 挡参考零点 
  BiPosi(100);   
  Wait_mS(1000); 
  Align_Set();                      // 延时后校正零点误差
  
  Update_Proc_All();
}
/*******************************************************************************
 Channel's error analys
*******************************************************************************/
void Analys(void)
{
  u32  i, SumA = 0, SumB = 0;

  __Ctrl(SMPL_ST, DISABLE); 
  __Ctrl(SMPLTIM, 360-1);           // 144MHz/360 = 400kHz = 2.5uS
  __Ctrl(SMPLNUM, 8192);  // 8192
  __Ctrl(SMPL_ST, ENABLE); 
  while((__Info(CHA_CNT) && __Info(CHB_CNT)) != 0) {};
  for(i=4000; i<8000; i++){
    SumA += Smpl[i];  
    SumB += Smpl[i+8192]; //8192
  }
  PavgA = SumA/4000; // 4000
  PavgB = SumB/4000; // 4000
}
/*******************************************************************************
 Channel's data process
*******************************************************************************/
void Process(void)
{
//------------------ 数据预处理,生成近程指针提高数据调用效率 -----------------//
  u16 SmplCnt = 0;
   
  u8*  BufA = (u8*)(V_Buf+TR1_pBUF);
  u8*  BufB = (u8*)(V_Buf+TR2_pBUF); 
  u8*  BufC = (u8*)(V_Buf+TR3_pBUF);
  u16* Ain  = (u16*)&Smpl[0];
  u16* Bin  = (u16*)&Smpl[8192];                  //8192

  s32  Ak = (KgA[KindA+(StateA?1:0)]*4)/GK[GainA];
  s32  Bk = (KgB[KindB+(StateB?1:0)]*4)/GK[GainB];     // 8192~409
  s16  i = 0, n = 0, k = 0, p = 0;
  
  s16  VtlA = 0, ViA = 0, VthA = 0, VeA = 0, VpA = 0, VoA = 0;            // VtA, 
  s16  VtlB = 0, ViB = 0, VthB = 0, VeB = 0, VpB = 0, VoB = 0;            // VtB, 
  
  u16  DtA   = 0, TslA  =  0, TshA = 0, StA  = 2; 
  u16  DtB   = 0, TslB  =  0, TshB = 0, StB  = 2; 
  u16  FallA = 0, RiseA =  0, UpA  = 0, DnA  = 0;  
  u16  FallB = 0, RiseB =  0, UpB  = 0, DnB  = 0;  
  u32  MaxA  = 0, MinA  = ~0, SumA = 0, SsqA = 0; // 最大/最小值,累加/平方和 = 0
  u32  MaxB  = 0, MinB  = ~0, SumB = 0, SsqB = 0;
  
  Posi_3F1 = PopMenu1_Value[CH3_Posi] - FileBuff[0];
  Posi_3F2 = PopMenu1_Value[CH3_Posi] - FileBuff[1]; 
  Posi_3F3 = PopMenu1_Value[CH3_Posi] - FileBuff[2];  
  
  u16  Dpth  = DEPTH[PopMenu1_Value[WIN_Depth]];//-1;// = 1024, 2048, 4096, 8192
  u16  Tp    = T0_PerCnt, Tm = PopMenu1_Value[TRI_Mode]&1;
  u16  Ks    = KS[PopMenu1_Value[TIM_Base]],  Sm = PopMenu1_Value[TRI_Sync];   
  u16  Ap    = ParamTab[P1x2]/2;
  u16  Bp    = ParamTab[P2x2]/2;
  u16  Cp    = ParamTab[P3x2]/2;
  u8   TrSrc = PopMenu1_Value[TRI_Ch] & 1;
     
//---------------------- 触发条件预处理,生成实际触发阈值 ---------------------//
  if(Status == STOP){   
    VtlA  = (((Tri_Posi+Ch1_Posi-100-0)<<12)/Ak)+2048; 
    VthA  = (((Tri_Posi+Ch1_Posi-100+0)<<12)/Ak)+2048;
    VpA   = (((ParamTab[P1x2]/2       -100)<<12)/Ak)+2048;
    VtlB  = (((Tri_Posi+Ch2_Posi-100-0)<<12)/Bk)+2048; 
    VthB  = (((Tri_Posi+Ch2_Posi-100+0)<<12)/Bk)+2048;
    VpB   = (((ParamTab[P2x2]/2       -100)<<12)/Bk)+2048;
  }
  else{
    VtlA  = (((PopMenu1_Value[TRI_Ch1]+PopMenu1_Value[CH1_Posi]-100-0)<<12)/Ak)+2048; 
    VthA  = (((PopMenu1_Value[TRI_Ch1]+PopMenu1_Value[CH1_Posi]-100+0)<<12)/Ak)+2048;
    VpA   = (((ParamTab[P1x2]/2       -100)<<12)/Ak)+2048;
    VtlB  = (((PopMenu1_Value[TRI_Ch2]+PopMenu1_Value[CH2_Posi]-100-0)<<12)/Bk)+2048;
    VthB  = (((PopMenu1_Value[TRI_Ch2]+PopMenu1_Value[CH2_Posi]-100+0)<<12)/Bk)+2048;
    VpB   = (((ParamTab[P2x2]/2       -100)<<12)/Bk)+2048;
  }
//-------------------------- 采样数据统计与分析 ------------------------------//
  
  if(PopMenu1_Value[TIM_Base]>9) //>1ms
    SmplCnt = Dpth-DEPTH[PopMenu1_Value[WIN_Depth]]/2;
  else  
    SmplCnt = Dpth+1;
  
  if(((PopMenu1_Value[TRI_Sync] == NONE)||(PopMenu1_Value[TRI_Sync] == SCAN))
     &&(PopMenu1_Value[TIM_Base]>11)){
    Dpth = 300;
    PopMenu1_Value[WIN_Posi]= 0;
  }
  
  
  n = DMA_CH_A->CNDTR;  
  
  if(n < SmplCnt){
    
    for(i=0; i<Dpth; i++){
      n = DMA_CH_A->CNDTR;
      if(i >= (Dpth-n)) break;               // 跳空一周期等待A/D转换完成
      ViA = Ain[i]; ViB = Bin[i];        
      
      if(MaxA < ViA) MaxA = ViA;  if(MaxB < ViB) MaxB = ViB;  // 统计最大值
      if(MinA > ViA) MinA = ViA;  if(MinB > ViB) MinB = ViB;  // 统计最小值
      SumA += ViA;                SumB += ViB;                // 统计累加和
      SsqA +=(ViA-VpA)*(ViA-VpA); SsqB +=(ViB-VpB)*(ViB-VpB); // 统计平方和
      
      if(StA == 2) DtA = 0;                  // 放弃CH_A首个跳变沿前的时间累计
      else         DtA++;
      if(StB == 2) DtB = 0;                  // 放弃CH_B首个跳变沿前的时间累计
      else         DtB++;
      if(ViA > VthA){                        // ViA 高于阈值上限
        if(StA == 0){
          TslA += DtA; DtA = 0; RiseA++;     // CH_A低位时间累计，上升沿累计
          if(UpA < Tp) UpA = i;              // 记录预采样后CH_A第一个上升沿位置
        }  
        StA = 1;                             // 当前CH_A状态设为高位
      } else if(ViA < VtlA){                 // ViA 低于阈值下限
        if(StA == 1){
          TshA += DtA; DtA = 0; FallA++;     // CH_A高位时间累计，下降沿累计
          if(DnA < Tp) DnA = i;              // 记录预采样后CH_A第一个下降沿位置
        }
        StA = 0;                             // 当前CH_A状态设为低位
      }
      if(ViB > VthB){                        // ViB 高于阈值上限
        if(StB == 0){
          TslB += DtB; DtB = 0; RiseB++;     // CH_B低位时间累计，上升沿累计
          if(UpB < Tp) UpB = i;              // 记录预采样后CH_B第一个上升沿位置
        }
        StB = 1;                             // 当前CH_B状态设为高位
      } else if(ViB < VtlB){                 // ViB 低于阈值下限
        if(StB == 1){
          TshB += DtB; DtB = 0; FallB++;     // CH_B高位时间累计，下降沿累计
          if(DnB < Tp) DnB = i;              // 记录预采样后CH_B第一个下降沿位置
        }
        StB = 0;                             // 当前CH_B状态设为低位
      }
    }
  }
  
  if(n == 0){                              // 采样全部完成后输出测量值 i == Dpth
    PmaxA = MaxA; PminA = MinA; PavgA = SumA/i; PssqA = SsqA/(i+1); 
    PmaxB = MaxB; PminB = MinB; PavgB = SumB/i; PssqB = SsqB/(i+1); 
    PrmsA = (Sqrt32(PssqA)); PmidA = (PmaxA + PminA)/2; PvppA = PmaxA - PminA;
    PrmsB = (Sqrt32(PssqB)); PmidB = (PmaxB + PminB)/2; PvppB = PmaxB - PminB;
    
    HighA = TshA/FallA; LowA = TslA/RiseA; EdgeA = FallA+ RiseA;
    HighB = TshB/FallB; LowB = TslB/RiseB; EdgeB = FallB+ RiseB;
  }
//--------------------------- 生成波形显示数据 -------------------------------//
  
  if(((Sm == NONE)||(Sm == SCAN))){
    if(PopMenu1_Value[TIM_Base] > 11)
      k = 1;                         // NONE SCAN 模式下强制触发
    else
    {
      if(TrSrc == CH_A) k =((Tm == RISE)? UpA : DnA)-Tp; // k:预采样后触发沿位置 
      else              k =((Tm == RISE)? UpB : DnB)-Tp;
      if((Sm == SCAN)&&(k < 0)){ // AUTO 模式下无触发时
        if(i > Dpth/4) k = 1;    // 采样区1/4满后强制触发
        else           k = 0;    // 采样区1/4满前不显示
      }
    }
  } 
  else   
  {
    if(TrSrc == CH_A) k =((Tm == RISE)? UpA : DnA)-Tp; // k:预采样后触发沿位置 
    else              k =((Tm == RISE)? UpB : DnB)-Tp;
  }
  
  
  if((!n)&&(Sm == SNGL)&&(k > 0)){ // SNGL 模式下有触发时采样区满后自动停止
    Status = STOP; 
    Update_Status();
  }
  
  
  if((Sm == AUTO)&&(k < 0)){ // AUTO 模式下无触发时
    if(i > Dpth/4) k = 1;    // 采样区1/4满后强制触发
    else           k = 0;    // 采样区1/4满前不显示
  }
  
  Trigger_k = k;
  
  if(Trigger_k>0)NORM_Kflag = 1;

  if((SNGL_Kflag)&&(Status == STOP)){
    Ch1_Posi = PopMenu1_Value[CH1_Posi];
    Ch2_Posi = PopMenu1_Value[CH2_Posi];
    Ch3_Posi = PopMenu1_Value[CH3_Posi];
    Tri_Posi = (PopMenu1_Value[TRI_Ch])?PopMenu1_Value[TRI_Ch2]:PopMenu1_Value[TRI_Ch1];
    SNGL_Kflag = 0;
  }
  
  if(k > 0){                                               // 有触发时的显示处理
    k += PopMenu1_Value[WIN_Posi]+((1024-Ks)*Tp+512)/1024; // 波形插值起点
    VeA =(((Ain[k]-2048)*Ak)>>12)+100; 
    VeB =(((Bin[k]-2048)*Bk)>>12)+100;
    
    if(k<8192-300){
      for(n=0; n<300;){
        ViA =(((Ain[k]-2048)*Ak)>>12)+100;
        ViB =(((Bin[k]-2048)*Bk)>>12)+100;
        k++;
        while(p > 0){
          VoA = VeA +((ViA-VeA)*(1024-p))/1024; // 计算 A 通道当前波形点的插值
          VoB = VeB +((ViB-VeB)*(1024-p))/1024; // 计算 B 通道当前波形点的插值
          
          if(Status == STOP){
            WaveLtdOut(BufA+n, VoA+(PopMenu1_Value[CH1_Posi]-Ch1_Posi));
            WaveLtdOut(BufB+n, VoB+(PopMenu1_Value[CH2_Posi]-Ch2_Posi));
            switch(PopMenu1_Value[CH3_Type]){                      
            case aNEG: WaveLtdOut(BufC+n, Ap-VoA+Cp);        break; // - CH_A
            case bNEG: WaveLtdOut(BufC+n, Bp-VoB+Cp);        break; // - CH_B
            case ADD:  WaveLtdOut(BufC+n, VoA-Ch1_Posi+VoB-Ch2_Posi+Cp); break; 
            //case SUB:  WaveLtdOut(BufC+n, Ch1_Posi-VoA-Ch2_Posi+VoB+Cp); break; 
            case SUB:  WaveLtdOut(BufC+n, (VoA-Ch1_Posi)-(VoB-Ch2_Posi)+Cp); break;
            case MR1:  WaveLtdOut(BufC+n, FileBuff[DAT_Var+n]     + Posi_3F1); break; 
            case MR2:  WaveLtdOut(BufC+n, FileBuff[DAT_Var+300+n] + Posi_3F2); break; 
            case MR3:  WaveLtdOut(BufC+n, FileBuff[DAT_Var+600+n] + Posi_3F3); break; 
            }
          }
          else{
            WaveLtdOut(BufA+n, VoA);
            WaveLtdOut(BufB+n, VoB);
            switch(PopMenu1_Value[CH3_Type]){                       
            case aNEG: WaveLtdOut(BufC+n, Ap-VoA+Cp);        break; // - CH_A
            case bNEG: WaveLtdOut(BufC+n, Bp-VoB+Cp);        break; // - CH_B
            case ADD:  WaveLtdOut(BufC+n, VoA-Ap+VoB-Bp+Cp); break; // CH_A+CH_B
            //case SUB:  WaveLtdOut(BufC+n, Ap-VoA-Bp+VoB+Cp); break; // CH_A-CH_B
            case SUB:  WaveLtdOut(BufC+n, (VoA-Ap)-(VoB-Bp)+Cp); break; // CH_A-CH_B
            case MR1:  WaveLtdOut(BufC+n, FileBuff[DAT_Var+n]     + Posi_3F1); break;
            case MR2:  WaveLtdOut(BufC+n, FileBuff[DAT_Var+300+n] + Posi_3F2); break; 
            case MR3:  WaveLtdOut(BufC+n, FileBuff[DAT_Var+600+n] + Posi_3F3); break; 
            }
          } 
          
          if(n++ >= 300) break;
          p -= Ks;
        }
        p += 1024;
        VeA = ViA; VeB = ViB;                                // 暂存当前波形点的值
      }
    }
    Norm_Clr = 0;                                          //NORM清屏
  } 
  else if((k < 0) && ((Sm != NORM)||(Norm_Clr))){          //NORM不更新屏幕   
    for(n=0; n<300;){
      while(p > 0){
        WaveLtdOut(BufA+n, ~0);                            // 轨迹 A 消隐
        WaveLtdOut(BufB+n, ~0);                            // 轨迹 B 消隐
        switch(PopMenu1_Value[CH3_Type]){                  // 轨迹 C 信号来源  
          case MR1:  WaveLtdOut(BufC+n, FileBuff[DAT_Var+n]     + Posi_3F1);  break; 
          case MR2:  WaveLtdOut(BufC+n, FileBuff[DAT_Var+300+n] + Posi_3F2);  break; 
          case MR3:  WaveLtdOut(BufC+n, FileBuff[DAT_Var+600+n] + Posi_3F3);  break; 
          default:   WaveLtdOut(BufC+n, ~0);               // 轨迹 C 消隐 
        }
        if(n++ >= 300) break;
        p -= Ks;
      }
      p += 1024;
    }
  }
  if((i == Dpth)&&(Status != STOP)) ADC_Start();       // 重新开始 ADC 扫描采样
}
/*******************************************************************************
 WaveLimitd: 波形限幅输出
*******************************************************************************/
void WaveLtdOut(u8* Buf, s16 Value)
{
  asm(" MOVS    R2, #1     ");
  asm(" MOVS    R3, #199   ");
  asm(" CMP     R1, R2");
  asm(" ITT     MI         ");
  asm(" STRHMI  R2, [R0]   ");
  asm(" BXMI    LR         ");
  asm(" CMP     R1,  R3    ");
  asm(" ITE     LS         ");
  asm(" STRHLS  R1, [R0]   ");
  asm(" STRHHI  R3, [R0]   ");
}

/*******************************************************************************
 Set_Base: 水平扫描时基转换设置
*******************************************************************************/
void Set_Base(u8 Base)
{
  TIM_AD->PSC = PSC[Base]-1;
  TIM_AD->ARR = ARR[Base]-1;
  TIM_AD->CCR3= (TIM_AD->ARR+1)/2;
}
/*******************************************************************************
 Auto_Fit: 
*******************************************************************************/

void Auto_Fit(void)
{
  u8 i,j,k=0;
  s16 Tmp,Tmp1,Tmp2;
  
  if(Status != STOP){
    //======电压档自动选择==============
    for(i=0;i<9;i++){  
      //******A通道***********
      Tmp1 = ((((PmaxA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi]) 
           - ((((PminA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi]);
      if((Tmp1 < 40)&&(PopMenu1_Value[CH1_Vol] > 2)){       //40
        PopMenu1_Value[CH1_Vol]--;
        PopMenu1_Value[CH1_Posi]=100;
        Process();
        Update_Proc_All();
        Show_Title(); 
      } 
      else if((Tmp1 > 110)&&(PopMenu1_Value[CH1_Vol] < 9)){ //110
        PopMenu1_Value[CH1_Vol]++;
        PopMenu1_Value[CH1_Posi]=100;
        Process();
        Update_Proc_All();
        Show_Title(); 
      }
      //******B通道***********
      Tmp2 = ((((PmaxB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi]) 
           - ((((PminB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi]);
      if((Tmp2 < 40)&&(PopMenu1_Value[CH2_Vol] > 2)){
        PopMenu1_Value[CH2_Vol]--;
        PopMenu1_Value[CH2_Posi]=80;
        Process();
        Update_Proc_All();
        Show_Title();  
      }
      else if((Tmp2 > 110)&&(PopMenu1_Value[CH2_Vol] < 9)){
        PopMenu1_Value[CH2_Vol]++;
        PopMenu1_Value[CH2_Posi]=80;
        Process();
        Update_Proc_All();
        Show_Title();  
      } 
    }
    
    //============识别触发源======================
    Process();
    __Ctrl(DELAYmS,100);
    Tmp1 = ((((PmaxA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi]) 
      - ((((PminA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi]);
    Tmp2 = ((((PmaxB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi]) 
      - ((((PminB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi]);
    
    if(Tmp1 > 20)
      PopMenu1_Value[TRI_Ch]=0;
    else if(Tmp2 > 20)
      PopMenu1_Value[TRI_Ch]=1;
    else 
      PopMenu1_Value[TRI_Ch]=0;
    
    Process();
    Update_Proc_All();
    Show_Title(); 
    __Ctrl(DELAYmS,100);
    
    //======触发线自动选择==============
    if(!PopMenu1_Value[TRI_Ch]){//******A通道***********
      Tmp = (((((PmaxA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi]) 
             - ((((PminA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi]))/4;
      if((PopMenu1_Value[TRI_Ch1] > (((((PmidA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi])  + Tmp))||
         (PopMenu1_Value[TRI_Ch1] < (((((PmidA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi])  - Tmp))){
           if(fabs(((((PmidA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi])+15)<195)           
             PopMenu1_Value[TRI_Ch1] = ((((PmidA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi])+15 ;
           Process();
           Show_Title(); 
         }
      ParamTab[VTx2]= ParamTab[P1x2] + 2*PopMenu1_Value[TRI_Ch1];
      Update[VTF]|=UPD;
      Update_Label();
    }
    else{                       //******B通道***********
      Tmp = (((((PmaxB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi]) 
          - ((((PminB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi]))/4;
      if((PopMenu1_Value[TRI_Ch2] > (((((PmidB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi])  + Tmp))||
         (PopMenu1_Value[TRI_Ch2] < (((((PmidB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi])  - Tmp))){
           if(fabs(((((PmidB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi])+15)<195)
           PopMenu1_Value[TRI_Ch2] = ((((PmidB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi])+15 ;
           Process();
           Show_Title(); 
         }
      ParamTab[VTx2]= ParamTab[P2x2] + 2*PopMenu1_Value[TRI_Ch2];
      Update[VTF]|=UPD;
      Update_Label();
    }
    
    //===========电平时调整幅度==================
    __Ctrl(DELAYmS,100);
    
    if(((((PmidB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi])>10){
      PopMenu1_Value[TRI_Ch]=1;
      Process();
      Update_Proc_All();
      Show_Title(); 
      __Ctrl(DELAYmS,100);
      for(i=0;i<9;i++){
        Process();
        Tmp2 = (((PmaxB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi];
        if((Tmp2 < 40)&&(PopMenu1_Value[CH2_Vol] > 2)){       //40
          PopMenu1_Value[CH2_Vol]--;
          PopMenu1_Value[CH2_Posi]=80;
          Process();
          Update_Proc_All();
          Show_Title(); 
        } 
        else if((Tmp2 > 100)&&(PopMenu1_Value[CH2_Vol] < 9)){ //110
          PopMenu1_Value[CH2_Vol]++;
          PopMenu1_Value[CH2_Posi]=80;
          Process();
          Update_Proc_All();
          Show_Title(); 
        }
      }
    }
    else if((((((PmaxB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi]) 
         - ((((PminB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi])) <10){
           //===========无信号时电压调2V==================
        PopMenu1_Value[CH2_Vol]=7; //2v
        Process();
        Update_Proc_All();
        Show_Title();
        Show_Title(); 
        __Ctrl(DELAYmS,50);
      }
    
        if(((((PmidA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi])>10){
      PopMenu1_Value[TRI_Ch]=0;
      Process();
      Update_Proc_All();
      Show_Title(); 
      __Ctrl(DELAYmS,100);
      for(i=0;i<9;i++){
        Process();
        Tmp1 = (((PmaxA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi];
        if((Tmp1 < 40)&&(PopMenu1_Value[CH1_Vol] > 2)){       //40
          PopMenu1_Value[CH1_Vol]--;
          PopMenu1_Value[CH1_Posi]=100;
          Process();
          Update_Proc_All();
          Show_Title(); 
        } 
        else if((Tmp1 > 100)&&(PopMenu1_Value[CH1_Vol] < 9)){ //110
          PopMenu1_Value[CH1_Vol]++;
          PopMenu1_Value[CH1_Posi]=100;
          Process();
          Update_Proc_All();
          Show_Title(); 
        }
      }
    }
    else if((((((PmaxA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi]) 
         - ((((PminA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi])) <10){
      //===========无信号时电压调2V==================
        PopMenu1_Value[CH1_Vol]=7; //2v
        Process();
        Update_Proc_All();
        Show_Title(); 
        __Ctrl(DELAYmS,50);
      }
    
    //======时基自动选择==============
        
    if(!PopMenu1_Value[TRI_Ch]){//******A通道***********
      Tmp1 = ((((PmaxA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi]) 
           - ((((PminA-2048)*((KgA[KindA+(StateA?1:0)]*4)/GK[GainA]))>>12)+100-PopMenu1_Value[CH1_Posi]);
      for(j=0;j<11;j++){
        Process();
        if((k==0)&&(EdgeA < 60)&&(PopMenu1_Value[TIM_Base] >5)){
          Set_Base(3);                           // 硬件设置扫描时基档位
          PopMenu1_Value[TIM_Base] = 3;
          Process();
          Show_Title(); 
          k=1;
        }
        if((EdgeA > 180)&&(PopMenu1_Value[TIM_Base] > 2)){
          PopMenu1_Value[TIM_Base]--;
          Set_Base(PopMenu1_Value[TIM_Base]);   // 硬件设置扫描时基档位
          //Process();
          Show_Title(); 
          __Ctrl(DELAYmS,50);
        }
        else if((EdgeA < 80)&&(PopMenu1_Value[TIM_Base] < 11)){
          PopMenu1_Value[TIM_Base]++;
          Set_Base(PopMenu1_Value[TIM_Base]);   // 硬件设置扫描时基档位
          //Process();
          Show_Title(); 
          __Ctrl(DELAYmS,50);
        }        
      }
      if(Tmp1 < 20){
        PopMenu1_Value[TIM_Base]=5; //50us
        Set_Base(PopMenu1_Value[TIM_Base]);   // 硬件设置扫描时基档位
        Show_Title(); 
        __Ctrl(DELAYmS,50);
      }
    }
    else{                       //******B通道***********
      Tmp2 = ((((PmaxB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi]) 
           - ((((PminB-2048)*((KgB[KindB+(StateB?1:0)]*4)/GK[GainB]))>>12)+100-PopMenu1_Value[CH2_Posi]);
      for(j=0;j<11;j++){
        Process();
        if((k==0)&&(EdgeB < 60)&&(PopMenu1_Value[TIM_Base] >5)){
          Set_Base(3);   
          PopMenu1_Value[TIM_Base] = 3;
          Process();
          Show_Title(); 
          k=1;
        }
        if((EdgeB > 180)&&(PopMenu1_Value[TIM_Base] > 2)){
          PopMenu1_Value[TIM_Base]--;
          Set_Base(PopMenu1_Value[TIM_Base]);   // 硬件设置扫描时基档位
          //Process();
          Show_Title(); 
          __Ctrl(DELAYmS,50);
        }
        else if((EdgeB < 80)&&(PopMenu1_Value[TIM_Base] < 11)){
          PopMenu1_Value[TIM_Base]++;
          Set_Base(PopMenu1_Value[TIM_Base]);   // 硬件设置扫描时基档位
          //Process();
          Show_Title(); 
          __Ctrl(DELAYmS,50);
        } 
      }
      if(Tmp2 < 20){
        PopMenu1_Value[TIM_Base]=5; //50us
        Set_Base(PopMenu1_Value[TIM_Base]);   // 硬件设置扫描时基档位
        Show_Title(); 
        __Ctrl(DELAYmS,50);
      }
    } 
  }
}


/******************************** END OF FILE *********************************/
