#ifndef _AFX_
#define _AFX_

#include "stm32f10x.h"

typedef enum WORK_STATUS
{  
  DMOCONNECT = 1,
  DMOSETGROUP,
  DMOAUTOPOWCONTR,
  DMOSETVOX,  
  DMOSETMIC,
  DMOMES ,
  DMOVERQ,
  DMOMESSEND,
}WORK_STATUS;

#define   NO_KEY    0x0
#define   KEY_V1    0x0080
#define   KEY_CN    0X8000

#define USB_DN_OUT()    GPIOA->CRH = (GPIOA->CRH & 0xFFFF3FFF) | 0x00003000
#define USB_DP_OUT()    GPIOA->CRH = (GPIOA->CRH & 0xFFF3FFFF) | 0x00030000

#define USB_DN_EN()     GPIOA->CRH = (GPIOA->CRH & 0xFFFFBFFF) | 0x0000B000
#define USB_DP_EN()     GPIOA->CRH = (GPIOA->CRH & 0xFFFBFFFF) | 0x000B0000

#define USB_DP_PD()     GPIOA->CRH = (GPIOA->CRH & 0xFFF3FFFF) | 0x00030000

#define USB_DN_LOW()    GPIOA->BRR  = GPIO_Pin_11
#define USB_DP_LOW()    GPIOA->BRR  = GPIO_Pin_12

#define SECTOR_SIZE     512
#define SECTOR_CNT      4096

#define SERIAL_NO1         (*(u32*)0x1FFFF7E8)
#define SERIAL_NO2         (*(u32*)0x1FFFF7EC)
#define SERIAL_NO3         (*(u32*)0x1FFFF7F0)
//--------------------------------- RCC ÉèÖÃ ---------------------------------//

#define RCC_PLL_EN()        RCC->CR   |= 0x01000000;// PLL En

#define RCC_CFGR_CFG()      RCC->CFGR |= 0x0068840A;/*RCC peripheral clock config
                                           |||||||+--Bits3~0 = 1010 PLL used as sys clock
                                           ||||||+---Bits7~4 = 0000 AHB clock = SYSCLK
                                           |||||+----Bits10~8  = 100 PCLK1=HCLK divided by 2
                                           ||||++----Bits13~11 = 000 PCLK2=HCLK
                                           ||||+-----Bits15~14 = 10 ADC prescaler PCLK2 divided by 6
                                           |||+------Bit17~16 = 00 HSI/2 clock selected as PLL input clock
                                           ||++------Bits21~18 = 1010 PLL input clock x12
                                           ||+-------Bit22 = 1 USB prescaler is PLL clock
                                           ++--------Bits31~27 Reserved*/
#endif
/******************************** END OF FILE *********************************/
