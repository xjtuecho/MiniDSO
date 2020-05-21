/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
 Brief   : 底层硬件配置                                          Author : bure
*******************************************************************************/

#define PRODSTR          "DS212"

#define APP_BASE          0x08008000
#define APP_SECD          0x08020000

#define VAL_BASE          0x08040000 - 0x3000 /* 0x0803D000   2K Bytes */
#define LOGO_BASE         0x08040000 - 0x2800 /* 0x0803D800  10K Bytes */
#define OEM_BASE          0x08040000 - 0x5000 /* 0x0803B000  10K Bytes */

#define VRAM_PTR          (u8*)0x10000000
#define SN_ADDR           0x1FFFF7AC



//------------------------------ KEY 相关宏定义 --------------------------------

#define KU_BIT            GPIO_Pin_7 /* GPIO_B */
#define KD_BIT            GPIO_Pin_4 /* GPIO_B */
#define KL_BIT            GPIO_Pin_3 /* GPIO_B */
#define KR_BIT            GPIO_Pin_8 /* GPIO_B */

//------------------------------ RCC 相关宏定义 --------------------------------

#define APB1_PERIPHS      RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3  | \
                          RCC_APB1Periph_SPI2 | RCC_APB1Periph_I2C1  | \
                          RCC_APB1Periph_USB  | RCC_APB1Periph_DAC   | \
                          RCC_APB1Periph_PWR  | RCC_APB1Periph_TIM4  | \
                          RCC_APB1Periph_TIM6 | RCC_APB1Periph_TIM7
#define APB2_PERIPHS      RCC_APB2Periph_TIM1 | RCC_APB2Periph_TIM15 | \
                          RCC_APB2Periph_TIM8 | RCC_APB2Periph_TIM16 | \
                          RCC_APB2Periph_USART1|RCC_APB2Periph_SYSCFG
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

#define SWD               GPIO_Pin_13 /*GPIO_A*/
#define SWC               GPIO_Pin_14 /*GPIO_A*/
#define MAP_SWD           GPIOA, GPIO_PinSource13,  GPIO_AF_0
#define MAP_SWC           GPIOA, GPIO_PinSource14,  GPIO_AF_0     
#define SWD_EVENTOUT      GPIOA, GPIO_PinSource13,  GPIO_AF_15
#define SWC_EVENTOUT      GPIOA, GPIO_PinSource14,  GPIO_AF_15                              
//---------------------------- LCD 显示相关宏定义 ------------------------------

#define LCD_nRD           GPIO_Pin_4 /*GPIO_D*/
#define LCD_nWR           GPIO_Pin_5 /*GPIO_D*/
#define LCD_RS            GPIO_Pin_6 /*GPIO_D*/
#define LCD_nRST          GPIO_Pin_3 /*GPIO_D*/
#define LCD_DATA          GPIO_Pin_All
#define READ_LCD()        GPIOE->IDR  
#define LCD_nWR_ACT()     GPIOD->BSRR = GPIOD->BRR  = GPIO_Pin_5
#define LDC_DATA_OUT      GPIOE->ODR
#define LDC_DATA_INP      GPIOE->IDR
#define LCD_PORT_IN()     GPIOE->MODER = 0x00000000
#define LCD_PORT_OUT()    GPIOE->MODER = 0x55555555


//------------------------------ 电池 相关宏定义 --------------------------------
#define V_BAT             GPIO_Pin_10 /*GPIO_D*/
#define V_USB             GPIO_Pin_9  /*GPIO_D*/                           
                            
//------------------------------ SPI 相关宏定义 --------------------------------
#define SPIx              SPI2
#define SPI_RST           GPIO_Pin_8  /*GPIO_D*/
#define SPI_CS            GPIO_Pin_12 /*GPIO_B*/
#define SPI_SCK           GPIO_Pin_13 /*GPIO_B*/
#define SPI_MISO          GPIO_Pin_14 /*GPIO_B*/
#define SPI_MOSI          GPIO_Pin_15 /*GPIO_B*/
#define MAP_SCK           GPIOB, GPIO_PinSource13, GPIO_AF_5
#define MAP_MISO          GPIOB, GPIO_PinSource14, GPIO_AF_5
#define MAP_MOSI          GPIOB, GPIO_PinSource15, GPIO_AF_5
                           
//-------------------------- 背光及蜂鸣器相关宏定义 ----------------------------

#define TIM_BL            TIM8
#define OUT_BL            GPIO_Pin_9 /*GPIO_B*/
#define MAP_BL            GPIOB, GPIO_PinSource9, GPIO_AF_10
#define BACKLIGHT(Val)    TIM_BL->CCR3 = Val
                           
#define TIM_BZ            TIM4
#define OUT_BZ            GPIO_Pin_6 /*GPIO_F*/
#define MAP_BZ            GPIOF, GPIO_PinSource6, GPIO_AF_2                        
#define BUZZER(Val)       TIM_BZ->CCR4 = Val/2


//-------------------------- 信号通道控制相关宏定义 ----------------------------

#define TIM_PA           TIM2
#define OUT_PA           GPIO_Pin_2  /*GPIO_A*/
#define MAP_PA           GPIOA, GPIO_PinSource2, GPIO_AF_1

#define TIM_PB           TIM3
#define OUT_PB           GPIO_Pin_4  /*GPIO_A*/
#define MAP_PB           GPIOA, GPIO_PinSource4, GPIO_AF_2 

#define INP_AI0          GPIO_Pin_0  /*GPIO_C*/
#define INP_AI1          GPIO_Pin_0  /*GPIO_C*/
                         
#define INP_BI0          GPIO_Pin_14  /*GPIO_D*/
#define INP_BI1          GPIO_Pin_14  /*GPIO_D*/

#define OUT_AN           GPIO_Pin_5  /*GPIO_A*/
#define OUT_PO           GPIO_Pin_6  /*GPIO_A*/
#define MAP_PO           GPIOA, GPIO_PinSource6, GPIO_AF_1

#define SEL_AX           GPIO_Pin_2  /*GPIO_C*/
#define SEL_AY           GPIO_Pin_3  /*GPIO_C*/
#define SEL_AZ           GPIO_Pin_1  /*GPIO_C*/
#define SEL_BX           GPIO_Pin_12 /*GPIO_D*/
#define SEL_BY           GPIO_Pin_13 /*GPIO_D*/ 
#define SEL_BZ           GPIO_Pin_11 /*GPIO_D*/

#define AX_HIGH          GPIOC->BSRR = SEL_AX
#define AX_LOW           GPIOC->BRR  = SEL_AX
#define AY_LOW           GPIOC->BSRR = SEL_AY
#define AY_HIGH          GPIOC->BRR  = SEL_AY
#define AZ_LOW           GPIOC->BSRR = SEL_AZ
#define AZ_HIGH          GPIOC->BRR  = SEL_AZ
#define BX_HIGH          GPIOD->BSRR = SEL_BX
#define BX_LOW           GPIOD->BRR  = SEL_BX
#define BY_LOW           GPIOD->BSRR = SEL_BY
#define BY_HIGH          GPIOD->BRR  = SEL_BY
#define BZ_LOW           GPIOD->BSRR = SEL_BZ
#define BZ_HIGH          GPIOD->BRR  = SEL_BZ

//------------------------------  采样定时宏定义 -------------------------------

#define TIM_AD           TIM1
#define TIM_AD_CCR       TIM_AD->CCR3

//------------------------------  采样传输宏定义 -------------------------------

#define DMA_CH_A         DMA1_Channel1
#define DMA_CH_B         DMA2_Channel5

//------------------------------  信号输出宏定义 -------------------------------

#define TIM_PO           TIM16
#define TIM_DA           TIM7
#define OUT_ANALG        DAC->DHR12R2
#define OUT_DMA          DMA2_Channel4

//------------------------------  相关宏定义 --------------------------------




/******************************** END OF FILE *********************************/
