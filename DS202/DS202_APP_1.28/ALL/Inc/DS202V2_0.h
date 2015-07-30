/********************* (C) COPYRIGHT 2014 e-Design Co.,Ltd. ********************
 Brief   : 底层硬件配置                                          Author : bure
*******************************************************************************/

#define PRODSTR          "DS202"
#define SCH_STR          "2.21"
#define MCU_STR          "STM32F303VC"
#define ADC_STR          "MCU's ADC"
#define FPGASTR          "None"

#define APP_BASE          0x08008000
#define APP_SECD          0x08020000
#define VAL_BASE          0x08040000 - 0x3000 /*  2K Bytes */
#define LOGO_BASE         0x08040000 - 0x2800 /* 10K Bytes */
#define VRAM_PTR         (u8*)0x10000000
#define SN_ADDR           0x1FFFF7AC


//------------------------------ KEY 相关宏定义 --------------------------------

#define PWR_KEY           GPIO_Pin_15 /* GPIO_C */
#define PWR_KEY_ST()     (!(GPIOC->IDR & PWR_KEY))
#define RUN_KEY           GPIO_Pin_6  /* GPIO_D */
#define RUN_KEY_ST()     (!(GPIOD->IDR & RUN_KEY))

//------------------------------ RCC 相关宏定义 --------------------------------

#define APB1_PERIPHS      RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3  | \
                          RCC_APB1Periph_SPI2 | RCC_APB1Periph_I2C1  | \
                          RCC_APB1Periph_USB  | RCC_APB1Periph_DAC   | \
                          RCC_APB1Periph_PWR  | RCC_APB1Periph_TIM4  | \
                          RCC_APB1Periph_TIM6 | RCC_APB1Periph_TIM7
#define APB2_PERIPHS      RCC_APB2Periph_TIM1 | RCC_APB2Periph_TIM15 | \
                          RCC_APB2Periph_TIM8 | RCC_APB2Periph_TIM17 | \
                          RCC_APB2Periph_SYSCFG
#define AHB_PERIPHS       RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB  | \
                          RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD  | \
                          RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOF  | \
                          RCC_AHBPeriph_ADC12 | RCC_AHBPeriph_ADC34  | \
                          RCC_AHBPeriph_DMA2  | RCC_AHBPeriph_DMA1

//------------------------------ USB 相关宏定义 --------------------------------

#define USB_DN            GPIO_Pin_11 /*GPIO_A*/
#define USB_DP            GPIO_Pin_12 /*GPIO_A*/
#define MAP_DN            GPIOA, GPIO_PinSource11, GPIO_AF_14
#define MAP_DP            GPIOA, GPIO_PinSource12, GPIO_AF_14
#define USB_IRQ           USB_LP_CAN1_RX0_IRQn

//---------------------------- LCD 显示相关宏定义 ------------------------------

#define LCD_nRD           GPIO_Pin_10 /*GPIO_C*/
#define LCD_nWR           GPIO_Pin_11 /*GPIO_C*/
#define LCD_RS            GPIO_Pin_12 /*GPIO_C*/
#define LCD_nRST          GPIO_Pin_13 /*GPIO_C*/
#define LCD_DATA          GPIO_Pin_All
#define READ_LCD()        GPIOE->IDR  
#define LCD_PORT_IN()     GPIOE->MODER = 0x00000000
#define LCD_PORT_OUT()    GPIOE->MODER = 0x55555555

//u8* WorkArea = ; // CCM 显示缓冲区

//------------------------------ I2C 相关宏定义 --------------------------------

#define I2C_SCL           GPIO_Pin_8 /*GPIO_B*/
#define I2C_SDA           GPIO_Pin_9 /*GPIO_B*/
#define MAP_SCL           GPIOB, GPIO_PinSource8, GPIO_AF_4
#define MAP_SDA           GPIOB, GPIO_PinSource9, GPIO_AF_4

//------------------------------ SPI 相关宏定义 --------------------------------
#define SPIx              SPI2
#define SPI_RST           GPIO_Pin_13 /*GPIO_D*/
#define SPI_CS            GPIO_Pin_10 /*GPIO_D*/
#define SPI_SCK           GPIO_Pin_13 /*GPIO_B*/
#define SPI_MISO          GPIO_Pin_14 /*GPIO_B*/
#define SPI_MOSI          GPIO_Pin_15 /*GPIO_B*/
#define MAP_SCK           GPIOB, GPIO_PinSource13, GPIO_AF_5
#define MAP_MISO          GPIOB, GPIO_PinSource14, GPIO_AF_5
#define MAP_MOSI          GPIOB, GPIO_PinSource15, GPIO_AF_5

//------------------------------ UART 相关宏定义 -------------------------------
#define OUT_TX            GPIO_Pin_9  /*GPIO_A*/
#define OUT_RX            GPIO_Pin_10 /*GPIO_A*/
#define MAP_TX            GPIOA, GPIO_PinSource9,  GPIO_AF_6
#define MAP_RX            GPIOA, GPIO_PinSource10, GPIO_AF_6
                            
//-------------------------- 背光及蜂鸣器相关宏定义 ----------------------------

#define TIM_BL            TIM8
#define OUT_BL            GPIO_Pin_8 /*GPIO_C*/
#define MAP_BL            GPIOC, GPIO_PinSource8, GPIO_AF_4
#define BACKLIGHT(Val)    TIM_BL->CCR3 = Val
#define TIM_BZ            TIM3
#define OUT_BZ            GPIO_Pin_9 /*GPIO_C*/
#define MAP_BZ            GPIOC, GPIO_PinSource9, GPIO_AF_2
#define BUZZER(Val)       TIM_BZ->CCR4 = Val/2

//--------------------------- 触摸板控制相关宏定义 -----------------------------

#define HC640_ID          0x4E 
#define INT_TP            GPIO_Pin_8  /*GPIO_D*/
#define RST_TP            GPIO_Pin_9  /*GPIO_D*/

//---------------------------- 电源控制相关宏定义 ------------------------------

#define PM_IRQ            GPIO_Pin_0  /*GPIO_C*/
#define PM_ID            (0x68)

#define PM_INFO           0x00
  #define CHG_ING         0x0004
  #define VUSB_ON         0x0020
  #define BAT_ON          0x2000
  #define CHG_OK          0x4000
  #define TEMP_OV         0x8000

#define DC1_LDO23         0x12
  #define DC1_EN          0x01
  #define LDO2_EN         0x04
  #define LDO3_EN         0x08

#define DC1_VO            0x26
  #define DC1_2V8         0x54
  #define DC1_3V0         0x5C
  #define DC1_3V3         0x68

#define LDO23_VO          0x28
  #define LDO2_3V0        0x0C
  #define LDO3_3V0        0xC0

#define PWR_VOFF          0x31
  #define VOFF_3V0        0x04

#define PWR_CTRL          0x32
  #define CHG_ACT         0x40
  #define FLASH1HZ        0x58
  #define FLASH4HZ        0x68
  #define PWR_OFF         0x80

#define CHG_CTRL          0x33
  #define ICHG360MA       0xC3
  #define ICHG550MA       0xC5

#define KEY_PWR          0x46
  #define LONG_KP         0x01
  #define SHORTKP         0x02

#define QNT_REG           0xB8
  #define QNT_EN          0x80
  #define QNT_CLR         0x20

#define PWR_ADEN          0x82
  #define ADC_EN          0xCE

// 12 bits 小端模式存放
#define V_USB             0x5A
#define I_USB             0x5C
#define _TEMP             0x5E
#define V_BAT             0x78
#define V_APS             0x7E
// 13 bits 小端模式存放
#define I_CHG             0x7A
#define I_BAT             0x7C
// 32 bits 小端模式存放
#define Q_CHG             0xB0
#define Q_CNT             0xB4

//-------------------------- 信号通道控制相关宏定义 ----------------------------

#define TIM_PA           TIM15
#define OUT_PA           GPIO_Pin_10 /*GPIO_F*/
#define MAP_PA           GPIOF, GPIO_PinSource10, GPIO_AF_3

#define TIM_PB           TIM4
#define OUT_PB           GPIO_Pin_12 /*GPIO_D*/
#define MAP_PB           GPIOD, GPIO_PinSource12, GPIO_AF_2 

#define OP1_AP           GPIO_Pin_1  /*GPIO_A*/
#define OP1_AN           GPIO_Pin_3  /*GPIO_A*/
#define OP1_AO           GPIO_Pin_2  /*GPIO_A*/
#define MAP_AO           GPIOA, GPIO_PinSource2, GPIO_AF_10
#define CHANNEL_A0       ADC_Channel_3 
#define CHANNEL_A1       ADC_Channel_2 
#define INP_AI0          GPIO_Pin_2  /*GPIO_A*/
#define INP_AI1          GPIO_Pin_5  /*GPIO_A*/

#define OP2_BP           GPIO_Pin_0  /*GPIO_B*/
#define OP2_BN           GPIO_Pin_5  /*GPIO_C*/
#define OP2_BO           GPIO_Pin_6  /*GPIO_A*/
#define MAP_BO           GPIOA, GPIO_PinSource6, GPIO_AF_7
#define CHANNEL_B0       ADC_Channel_1
#define CHANNEL_B1       ADC_Channel_3
#define INP_BI0          GPIO_Pin_1  /*GPIO_B*/
#define INP_BI1          GPIO_Pin_12 /*GPIO_B*/

#define OUT_AN           GPIO_Pin_4  /*GPIO_A*/
#define OUT_PO           GPIO_Pin_10 /*GPIO_B*/
#define MAP_PO           GPIOB, GPIO_PinSource10, GPIO_AF_1

#define SEL_AX           GPIO_Pin_1  /*GPIO_C*/
#define SEL_AY           GPIO_Pin_2  /*GPIO_C*/
#define SEL_AZ           GPIO_Pin_3  /*GPIO_C*/
#define SEL_BX           GPIO_Pin_7  /*GPIO_A*/
#define SEL_BY           GPIO_Pin_4  /*GPIO_C*/ 
#define SEL_BZ           GPIO_Pin_2  /*GPIO_B*/

#define AX_HIGH          GPIOC->BSRR = SEL_AX
#define AX_LOW           GPIOC->BRR  = SEL_AX
#define AY_HIGH          GPIOC->BSRR = SEL_AY
#define AY_LOW           GPIOC->BRR  = SEL_AY
#define AZ_HIGH          GPIOC->BSRR = SEL_AZ
#define AZ_LOW           GPIOC->BRR  = SEL_AZ
#define BX_HIGH          GPIOA->BSRR = SEL_BX
#define BX_LOW           GPIOA->BRR  = SEL_BX
#define BY_HIGH          GPIOC->BSRR = SEL_BY
#define BY_LOW           GPIOC->BRR  = SEL_BY
#define BZ_HIGH          GPIOB->BSRR = SEL_BZ
#define BZ_LOW           GPIOB->BRR  = SEL_BZ

#define CH_C_IN          GPIO_Pin_6  /*GPIO_C*/
#define CH_D_IN          GPIO_Pin_7  /*GPIO_C*/

//------------------------------  采样定时宏定义 -------------------------------

#define TIM_AD           TIM1
#define TIM_AD_CCR       TIM_AD->CCR3

//------------------------------  采样传输宏定义 -------------------------------

#define DMA_CH_A         DMA1_Channel1
#define DMA_CH_B         DMA2_Channel5

//------------------------------  信号输出宏定义 -------------------------------

#define TIM_PO           TIM2
#define TIM_DA           TIM7
#define OUT_ANALG        DAC->DHR12R1
#define OUT_DMA          DMA2_Channel4

//------------------------------  相关宏定义 --------------------------------




/******************************** END OF FILE *********************************/
