;/******************** (C) COPYRIGHT 2014 e-Design Co.,Ltd. ********************
; File Name : Display.s
; Version   : For DS202 Ver 1.0 With STM32F303VC                  Author : bure
;******************************************************************************/
    RSEG VIEW:CODE(3)
    PRESERVE8

    IMPORT    __SetPosi
    IMPORT    __SendPixels

BACKGROUND    = 0x0000         ;// 背景底色
GRID_COLOR    = 0x7BEF         ;// 背景网格线的颜色
WR            = 0x800          ;// Gpio Pin 11
P_HID         = 0x01
L_HID         = 0x02
D_HID         = 0x01
W_HID         = 0x04
                               ;//    0 ~  409: LCD Col Buffer 
P_TAB         =  410           ;//  410 ~  499: ParamTab 
A_BUF         =  500           ;//  500 ~  799: Wave Track#1 Buffer
B_BUF         =  800           ;//  800 ~ 1099: Wave Track#2_Buffer 
C_BUF         = 1100           ;// 1100 ~ 1399: Wave Track#3_Buffer 
P_BUF         = 1400           ;// 1400 ~ 8191: Pop Buffer


CH_A          = P_TAB+2*0      ;// Wave Track#1 Flag
CH_B          = P_TAB+2*1      ;// Wave Track#2 Flag
CH_C          = P_TAB+2*2      ;// Wave Track#3 Flag
CCHA          = P_TAB+2*18     ;// Wave Track#1 Color
CCHB          = P_TAB+2*19     ;// Wave Track#2 Color
CCHC          = P_TAB+2*20     ;// Wave Track#3 Color
M_X0          = P_TAB+2*30     ;// 波形显示窗的起始位置 X0
M_Y0          = P_TAB+2*31     ;// 波形显示窗的起始位置 Y0
M_WX          = P_TAB+2*32     ;// 波形显示窗的水平宽度 WX
POPF          = P_TAB+2*33     ;// Pop Flag
PXx1          = P_TAB+2*34     ;// Pop X Position
PWx1          = P_TAB+2*35     ;// Pop Width
PYx2          = P_TAB+2*36     ;// Pop Y Position *2
PHx2          = P_TAB+2*37     ;// Pop Hight *2

    EXPORT  Align00
    EXPORT  Align01
    EXPORT  Align02
    EXPORT  Align03
    EXPORT  Align04
    EXPORT  Align05
    EXPORT  Align06
    EXPORT  Align07
    EXPORT  Align08
    EXPORT  Align09
    EXPORT  Align10
    EXPORT  Align11

;//=============================================================================
;//                  View 窗口波形显示相关汇编语言子程序
;//=============================================================================
;// void __DrawWindow(u32 VRAM_Addr)
;//=============================================================================
    EXPORT  __DrawWindow
    NOP.W
    NOP.W
__DrawWindow
    PUSH    {R4-R12,LR}
    NOP.W                      ;// R0: VRAM 起始指针
    LDRH    R1,  [R0, #M_WX]   ;// 弹出窗口的水平开始位置
    MOV     R2,  #0            ;// 列计数初始值
    LDRH    R11, [R0, #PXx1]   ;// 弹出窗口的水平开始位置
    LDRH    R12, [R0, #PWx1]   ;// 弹出窗口的水平宽度
    ADD     R12, R11, R12      ;// 弹出窗口的水平结束位置
    ADD     R10,  R0,  #P_BUF  ;// 出窗口缓冲区指针初始值

;//----------- 画背景 ----------//
Draw_Loop
    CMP     R2,  #0
    ITT     EQ
    BLEQ    Buld_0             ;// 建立外沿列缓冲区的背景数据 
    BEQ     Draw_Wave
    ADDS    R3,  R1, #2        ;// WIDTH+2
    CMP     R2,  R3
    ITT     EQ
    BLEQ    Buld_0             ;// 建立外沿列缓冲区的背景数据
    BEQ     Draw_Wave

    CMP     R2,  #1
    ITT     EQ
    BLEQ    Buld_1             ;// 建立边线列缓冲区的背景数据
    BEQ     Draw_Wave
    ADDS    R3,  R1, #1        ;// WIDTH+1
    CMP     R2,  R3
    ITT     EQ
    BLEQ    Buld_1             ;// 建立边线列缓冲区的背景数据
    BEQ     Draw_Wave

    SUBS    R3,  R2, #1
    MOVS    R6,  #25
    UDIV    R5,  R3, R6                
    MULS    R5,  R5, R6
    SUBS    R5,  R3, R5
    ITT     EQ
    BLEQ    Buld_4             ;// 建立格线列缓冲区的背景数据
    BEQ     Draw_Wave

    MOVS    R6,  #5
    UDIV    R5,  R3, R6
    MULS    R5,  R5, R6
    SUBS    R5,  R3, R5
    ITT     EQ
    BLEQ    Buld_3             ;// 建立格点列缓冲区的背景数据
    BEQ     Draw_Wave
    BL      Buld_2             ;// 建立格内列缓冲区的背景数据

;//--------- 画波形曲线 --------//
Draw_Wave
    CMP     R2,  #3            ;// 从3~299
    BCC     Horizontal
    CMP     R2,  R1            ;// WIDTH
    BCS     Horizontal

    LDRH    R3,  [R0, #CH_A]   ;// 取 CH_A 波形曲线的消隐标志
    TST     R3,  #W_HID
    ITTT    EQ
    MOVEQ   R3,  #CCHA         ;// R3 = CH_A 波形曲线颜色表偏移
    ADDEQ   R4,  R0, #A_BUF
    BLEQ    Draw_Analog

    LDRH    R3,  [R0, #CH_B]   ;// 取 CH_B 波形曲线的消隐标志
    TST     R3,  #W_HID
    ITTT    EQ
    MOVEQ   R3,  #CCHB         ;// R3 = CH_B 波形曲线颜色表偏移
    ADDEQ   R4,  R0, #B_BUF
    BLEQ    Draw_Analog

    LDRH    R3,  [R0, #CH_C]   ;// 取 CH_C 波形曲线的消隐标志
    TST     R3,  #W_HID
    ITTT    EQ
    MOVEQ   R3,  #CCHC         ;// R3 = CH_C 波形曲线颜色表偏移
    ADDEQ   R4,  R0, #C_BUF
    BLEQ    Draw_Analog

;//------- 画水平方向游标 ------//
Horizontal
    CMP     R2,  #0
    ITT     EQ
    BLEQ    Cursor_0           ;// 外沿列画游标端点
    BEQ     Vertical
    ADDS    R3,  R1, #2        ;// WIDTH+2
    CMP     R2,  R3             
    ITT     EQ
    BLEQ    Cursor_0           ;// 外沿列画游标端点
    BEQ     Vertical

    CMP     R2,  #1
    ITT     EQ
    BLEQ    Cursor_1           ;// 边线列画游标端点
    BEQ     Vertical
    ADDS    R3,  R1, #1        ;// WIDTH+1
    CMP     R2,  R3             
    ITT     EQ
    BLEQ    Cursor_1           ;// 边线列画游标端点
    BEQ     Vertical

    CMP     R2,  #2
    ITT     EQ
    BLEQ    Cursor_2           ;// 内沿列画游标端点
    BEQ     Vertical
    CMP     R2,  R1            ;// WIDTH 
    IT      EQ
    BLEQ    Cursor_2           ;// 内沿列画游标端点
    BEQ     Vertical
    BL      Cursor_3           ;// 其余列画游标线

;//------- 画垂直方向游标 ------//
Vertical
    BL      Cursor_4

;//--------- 画弹出窗口 --------//

    LDRH    R3,  [R0, #POPF]   ;// 取弹出窗口的消隐标志
    TST     R3,  #P_HID
    BNE     Send
    CMP     R2,  R11           ;// 判断弹出窗口列处理开始
    BLT     Send
    CMP     R2,  R12           ;// 判断弹出窗口列处理结束
    IT      LT
    BLLT    Draw_Pop           ;// 列计数在弹出窗口内

;//--------- 显示列数据 --------//
Send
    BL      Send_LCD           ;// 从缓冲区传送一列数据到 LCD
    ADDS    R3,  R1, #2        ;// WIDTH+2
    CMP     R2,  R3             
    ITT     NE
    ADDNE   R2,  R2, #1
    BNE     Draw_Loop          ;// 处理下1列显示数据

    POP     {R4-R12,PC}
;//=============================================================================
; Draw_Analog(R2:Col, R3:ColorNum, R4:pDat)   画模拟波形曲线  Used: R3-R7
;//=============================================================================
    NOP.W
    NOP.W
    NOP
Draw_Analog
    ADD     R4,  R4, R2
    LDRB    R5,  [R4]          ;// 取当前列波形数据数据 n1
    LDRB    R4,  [R4, #-1]     ;// 取上一列波形数据数据 n0
Analog0
    CMP     R4,  #199          ;// 上端截尾 R4 >= 200
    IT      HI
    BXHI    LR
    CMP     R4,  #0            ;// 下端截尾 R4 = 0
    IT      EQ
    BXEQ    LR

    CMP     R5,  R4
    ITTEE   CS                 ;// R5 = | n1 - n0 |
    MOVCS   R6,  R4
    SUBCS   R5,  R5, R4
    MOVCC   R6,  R5            ;// n1, n0 中最小的送 R6
    SUBCC   R5,  R4, R5

    CMP     R6,  #198          ;// 起点超上界则消隐 R6 > 198
    IT      HI
    BXHI    LR
    ADDS    R4,  R5, R6
    CMP     R4,  #198          ;// 终点超上界则限幅 R6 > 198
    IT      HI
    RSBHI   R5,  R6, #198
    BGT     Analog2

    CMP     R4,  #1            ;// 终点超下界则消隐 R4 <= 1
    IT      LS
    BXLS    LR
    CMP     R6,  #2            ;// 起点超下界则限幅 R6 <= 2
    ITTT    LS
    MOVLS   R6,  #2
    SUBLS   R5,  R4, #2
    BLS     Analog2

    CMP     R5,  #0            ;// 水平线加粗
    ITT     EQ
    SUBEQ   R6,  R6, #1
    ADDEQ   R5,  R5, #2

Analog2
    CMP     R5,  #20           ;// 选择颜色
    IT      GE
    ADDGE   R3,  R3, #18       ;// 选择低亮度颜色组
    LDRH    R3,  [R0, R3]

    LSL     R6,  R6, #1
    ADD     R6,  R0, R6        ;// 确定显示位置
Align00    
Analog3
    STRH    R3,  [R6], #2      ;// 画波形点
    SUBS    R5,  R5, #1
    BCS     Analog3
    BX      LR
;//=============================================================================
; Cursor_4(R1:pTab, R2:Col)// 外沿列画游标端点  Used: R3-R8
;//=============================================================================
    NOP.W
Cursor_4
    MOVS    R3,  #P_TAB+6*2    ;// 6~8项为垂直方向游标
Cursor40
    MOV     R4,  R0
    LDRH    R5,  [R0, R3]      ;// 取该游标变量的消隐标志
    TST     R5,  #D_HID
    BNE     Cursor49           ;// 是游标消隐转
Cursor41
    ADDS    R3,  R3, #9*2
    LDRH    R5,  [R0, R3]      ;// 取该游标变量的显示位置
    ADDS    R3,  R3, #9*2
    LDRH    R6,  [R0, R3]      ;// 取该游标变量的显示颜色
    SUBS    R3,  R3, #18*2     ;// 恢复变量表指针

    SUBS    R8,  R5, #2
    CMP     R2,  R8            ;// 画左外沿行游标端点
    BNE     Cursor42
    STRH    R6,  [R4]          ;// 画左下外沿
    ADDS    R4,  R4, #404
    STRH    R6,  [R4]          ;// 画左上外沿
    B       Cursor49
Cursor42
    ADDS    R8,  R8, #1
    CMP     R2,  R8            ;// 画左边线行游标端点
    BNE     Cursor43
    STRH    R6,  [R4], #2      ;// 画左下外沿
    STRH    R6,  [R4]          ;// 画左下边线
    ADDS    R4,  R4, #400
    STRH    R6,  [R4], #2      ;// 画左上边线
    STRH    R6,  [R4]          ;// 画左上外沿
    B       Cursor49
Cursor43
    ADDS    R8,  R8, #1
    CMP     R2,  R8            ;// 画游标端点, 画游标线
    BNE     Cursor45
    STRH    R6,  [R4], #2      ;// 画下外沿线
    STRH    R6,  [R4], #2      ;// 画下边线更
    STRH    R6,  [R4]          ;// 画下内沿线
    ADDS    R4,  R4, #396
    STRH    R6,  [R4], #2      ;// 画上内沿线
    STRH    R6,  [R4], #2      ;// 画上边线
    STRH    R6,  [R4]          ;// 画上外沿线

    LDRH    R5,  [R0, R3]      ;// 取该游标变量的消隐标志
    TST     R5,  #2
    BNE     Cursor45           ;// 是游标线消隐则转
    MOVS    R4,  R0
    ADDS    R7,  R4, #400
Align01    
Cursor44
    STRH    R6,  [R4], #8      ;// 画游标线
    CMP     R7,  R4
    BCS     Cursor44
    B       Cursor49
Cursor45
    ADDS    R8,  R8, #1
    CMP     R2,  R8            ;// 画右边线行游标端点
    BNE     Cursor46
    STRH    R6,  [R4], #2      ;// 画右下外沿
    STRH    R6,  [R4]          ;// 画右下边线
    ADDS    R4,  R4, #400
    STRH    R6,  [R4], #2      ;// 画右上边线
    STRH    R6,  [R4]          ;// 画右上外沿
    B       Cursor49
Cursor46
    ADDS    R8,  R8, #1
    CMP     R2,  R8            ;// 画右外沿行游标端点
    BNE     Cursor49
    STRH    R6,  [R4]          ;// 画右下外沿
    ADDS    R4,  R4, #404
    STRH    R6,  [R4]          ;// 画右上外沿
Cursor49
    ADDS    R3,  R3, #1*2
    CMP     R3,  #P_TAB+9*2    ;//10
    BNE     Cursor40
    BX      LR
;//=============================================================================
; Cursor_3(R1:pTab, R2:Col)// 画其余列游标线  Used: R3-R6
;//=============================================================================
    NOP
Align02    
Cursor_3
    MOVS    R3,  #P_TAB+5*2    ;// 0~5项为水平方向游标
    MOVS    R4,  R0
Cursor31
    LDRH    R5,  [R0, R3]      ;// 取该游标变量的消隐标志
    TST     R5,  #L_HID
    BNE     Cursor32           ;// 是游标线消隐则转
    SUBS    R5,  R2, #1
    ANDS    R5,  R5, #3
    BNE     Cursor32           ;// 是游标线虚线位置则转
    ADDS    R3,  R3, #9*2
    LDRH    R5,  [R0, R3]      ;// 取该游标变量的显示位置
    ADDS    R4,  R0, R5
    ADDS    R3,  R3, #9*2
    LDRH    R6,  [R0, R3]      ;// 取该游标变量的显示颜色
    STRH    R6,  [R4]          ;// 画游标线
    SUBS    R3,  R3, #18*2     ;// 恢复变量表指针
Cursor32
    SUBS    R3,  R3, #1*2
    CMP     R3,  #P_TAB     
    BPL     Cursor31           ;// 处理下1个游标端点
    BX      LR
;//=============================================================================
; Cursor_0(R1:pTab, R2:Col)// 画外沿列游标端点  Used: R3-R6
;//=============================================================================
    NOP
Align03
Cursor_0
    MOVS    R3,  #P_TAB+5*2    ;// 0~5项为水平方向游标
    MOVS    R4,  R0
Cursor01
    LDRH    R5,  [R0, R3]      ;// 取该游标变量的消隐标志
    TST     R5,  #1
    BNE     Cursor02           ;// 是游标端点消隐则转
    ADDS    R3,  R3, #9*2
    LDRH    R5,  [R0, R3]      ;// 取该游标变量的显示位置
    ADDS    R4,  R0, R5
    ADDS    R3,  R3, #9*2
    LDRH    R6,  [R0, R3]      ;// 取该游标变量的显示颜色
    SUB     R4,  R4, #4
    STRH    R6,  [R4], #2
    STRH    R6,  [R4], #2
    STRH    R6,  [R4], #2      ;// 画外沿列游标端点
    STRH    R6,  [R4], #2
    STRH    R6,  [R4], #2
    SUBS    R3,  R3, #18*2     ;// 恢复变量表指针
Cursor02
    SUBS    R3,  R3, #1*2
    CMP     R3,  #P_TAB       
    BPL     Cursor01           ;// 处理下1个游标端点
    BX      LR
;//=============================================================================
; Cursor_1(R1:pTab, R2:Col)// 画边线列游标端点  Used: R3-R6
;//=============================================================================
    NOP.W
Align04
Cursor_1
    MOVS    R3,  #P_TAB+5*2    ;// 0~5项为水平方向游标
    MOVS    R4,  R0
Cursor11
    LDRH    R5,  [R0, R3]      ;// 取该游标变量的消隐标志
    TST     R5,  #1
    BNE     Cursor12           ;// 是游标端点消隐则转
    ADDS    R3,  R3, #9*2
    LDRH    R5,  [R0, R3]      ;// 取该游标变量的显示位置
    ADDS    R4,  R0, R5
    ADDS    R3,  R3, #9*2
    LDRH    R6,  [R0, R3]      ;// 取该游标变量的显示颜色
    SUBS    R4,  R4, #2
    STRH    R6,  [R4], #2
    STRH    R6,  [R4], #2      ;// 画边线列游标端点
    STRH    R6,  [R4], #2
    SUBS    R3,  R3, #18*2     ;// 恢复变量表指针
Cursor12
    SUBS    R3,  R3, #1*2
    CMP     R3,  #P_TAB     
    BPL     Cursor11           ;// 处理下1个游标端点
    BX      LR
;//=============================================================================
; Cursor_2(R1:pTab, R2:Col)// 画内沿列游标端点  Used: R3-R6
;//=============================================================================
    NOP.W
    NOP.W
    NOP.W
    NOP
Align05
Cursor_2
    MOVS    R3,  #P_TAB+5*2    ;// 0~5项为水平方向游标
    MOVS    R4,  R0
Cursor21
    LDRH    R5,  [R0, R3]      ;// 取该游标变量的消隐标志
    TST     R5,  #1
    BNE     Cursor22           ;// 是游标端点消隐则转
    ADDS    R3,  R3, #9*2
    LDRH    R5,  [R0, R3]      ;// 取该游标变量的显示位置
    ADDS    R4,  R0, R5
    ADDS    R3,  R3, #9*2
    LDRH    R6,  [R0, R3]      ;// 取该游标变量的显示颜色
    STRH    R6,  [R4]          ;// 画内沿列游标端点
    SUBS    R3,  R3, #18*2     ;// 恢复变量表指针
Cursor22
    SUBS    R3,  R3, #1*2
    CMP     R3,  #P_TAB   
    BPL     Cursor21           ;// 处理下1个游标端点
    BX      LR
;//=============================================================================
;// R0:pDat, R1:pTab, R2:Col, R3:Var, R4:pBuf, R5:Cnt, R6:Tmp,
;//=============================================================================
; void Fill_Base(R3 = u32 Color)// 列缓冲区填底色 RET: R4=R0+2   Used: R3-R5
;//=============================================================================
    NOP
    EXPORT  Fill_Base
Fill_Base
    MOV.W   R4,  R0
    MOV.W   R5,  #102          ;// 1+202行/2 共404 Bytes
Align06    
Fill_Loop_0
    STR     R3,  [R4], #4      ;// 传送完成后指针加4
    SUBS    R5,  #1
    BNE     Fill_Loop_0
    ADD     R4,  R0, #2        ;// 指针对齐
    MOV     R3,  #GRID_COLOR   ;// 预装载网格颜色值
    BX      LR
;//=============================================================================
; Draw_Pop(R0:pWork) // 画弹出窗口                                  Used: R3-R8
;//=============================================================================
    NOP.W
    NOP.W
    NOP.W
    NOP
Align07
Draw_Pop
    LDRH    R5,  [R0, #PYx2]   ;// 取弹出窗口的垂直开始位置
    ADDS    R5,   R0,  R5
    LDRH    R6,  [R0, #PHx2]   ;// 取弹出窗口的垂直高度
    ADD     R7,   R0, #PHx2    ;// HYx2+2 是存放颜色表指针 CPTR
Pop_Loop
    LDRH    R4,  [R10], #2     ;// 取 Pop 数据(双字节共4个点)
    ANDS    R3,  R4,  #0x0E
    ITT     NE                 ;// 是0号透明色则跳过
    LDRHNE  R3,  [R7, R3]      ;// 查表取1~7号色
    STRHNE  R3,  [R5]          ;// 画第1点
    ADDS    R5,  R5,  #2
    LSR     R4,  R4,  #4
    ANDS    R3,  R4,  #0x0E
    ITT     NE                 ;// 是0号透明色则跳过
    LDRHNE  R3,  [R7, R3]      ;// 查表取1~7号色
    STRHNE  R3,  [R5]          ;// 画第2点
    ADDS    R5,  R5,  #2
    LSR     R4,  R4,  #4
    ANDS    R3,  R4,  #0x0E
    ITT     NE                 ;// 是0号透明色则跳过
    LDRHNE  R3,  [R7, R3]      ;// 查表取1~7号色
    STRHNE  R3,  [R5]          ;// 画第3点
    ADDS    R5,  R5,  #2
    LSR     R4,  R4,  #4
    ANDS    R3,  R4,  #0x0E
    ITT     NE                 ;// 是0号透明色则跳过
    LDRHNE  R3,  [R7, R3]      ;// 查表取1~7号色
    STRHNE  R3,  [R5]          ;// 画第4点
    ADDS    R5,  R5, #2
    SUBS    R6,  R6, #8
    BNE     Pop_Loop
    BX      LR                 ;// 该列显示完成
    NOP
;//=============================================================================
; void Buld_0(R4 = u16* pCol)   // 建立外沿列缓冲区的背景数据 Used: R3-R5
;//=============================================================================
Buld_0
    MOV     R3,  #BACKGROUND   ;// 背景颜色
    B       Fill_Base
;//=============================================================================
; void Buld_2(R4 = u16* pCol)   // 建立格内列缓冲区的背景数据 Used: R3-R6
;//=============================================================================
Buld_2
    MOV     R6,  LR
    MOV     R3,  #BACKGROUND   ;// 背景颜色
    BL      Fill_Base
    STRH    R3,  [R4, #400]    ;// 上边线
    STRH    R3,  [R4]          ;// 下边线
    BX      R6
;//=============================================================================
; void Buld_4(R4 = u16* pCol)   // 建立格线列缓冲区的背景数据
;//=============================================================================
    NOP.W
    NOP
Buld_4
    MOV     R6,  LR
    MOV     R3,  #BACKGROUND   ;// 背景颜色
    BL      Fill_Base
    MOVS    R5,  #41           ;// 41点  P_TAB
Align08    
Loop7
    STRH    R3, [R4], #5*2     ;// 每5行画1格点
    SUBS    R5,  R5,  #1
    BNE     Loop7
    BX      R6
;//=============================================================================
; void Buld_3(R4 = u16* pCol)   // 建立格点列缓冲区的背景数据 Used: R3-R6
;//=============================================================================
    NOP.W
    NOP.W
    NOP
Buld_3
    MOV     R6,  LR
    MOV     R3,  #BACKGROUND   ;// 背景颜色
    BL      Fill_Base
    MOVS    R5,  #9            ;// 9格点
Align09    
Loop3
    STRH    R3, [R4], #50      ;// 每25行画1格点
    SUBS    R5,  R5,  #1
    BNE     Loop3
    BX      R6
;//=============================================================================
; void Buld_1(R4 = u16* pCol)   // 建立边线列缓冲区的背景数据 Used: R3-R6
;//=============================================================================
Buld_1
    MOV     R6,  LR
    MOV.W   R3,  #GRID_COLOR
    MOVT    R3,  #GRID_COLOR   ;// 为提高传输效率，取32bits格线颜色
    BL      Fill_Base          ;// RET: R4=R0+2
    MOV     R3,  #BACKGROUND   ;// 背景颜色
    STRH    R3,  [R4, #402]            
    STRH    R3,  [R4, #-2]     ;// 下边线外留空白
    BX      R6
;//=============================================================================
; void __Mem32Fill(u32* pMem, u32 Data, u32 n)
;//=============================================================================
    NOP.W
    NOP.W
    NOP
    EXPORT  __Mem32Fill
Align10    
__Mem32Fill
    STR     R1, [R0], #4
    SUBS    R2, R2, #1  
    BNE     __Mem32Fill  
    BX      LR         
;//=============================================================================
; void Send_LCD(u16* pBuf, u16 Row) // 送一列缓冲区数据到 LCD     Used: R3-R8
;//=============================================================================
    NOP.W
    NOP
    EXPORT  Send_LCD
Send_LCD
    MOVS    R5,  R0
    PUSH    {R0-R3, LR}
    LDRH    R1, [R0, #M_Y0]
    LDRH    R0, [R0, #M_X0]
    ADDS    R0,  R0, R2
    BL      __SetPosi
    MOVS    R0, #0x0800
    MOVT    R0, #0x4800        ;// Port C Base 0x48000800
    MOVS    R1, #WR 
    MOVS    R2, #203           ;// 1+202
Align11    
Loop9    
    LDRH    R3, [R5], #2
    STRH    R3, [R0,  #0x814]  ;// Port E ODR
    STRH    R1, [R0,  #0x028]  ;// Port C BRR
    SUBS    R2,  R2,  #1  
    STRH    R1, [R0,  #0x018]  ;// Port C BSRR
    BNE     Loop9
    POP     {R0-R3, PC}
;//=============================================================================
   END

;******************************* END OF FILE ***********************************


