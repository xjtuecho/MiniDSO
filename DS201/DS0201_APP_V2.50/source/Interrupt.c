/********************* (C) COPYRIGHT 2010 e-Design Co.,Ltd. ********************
 File Name : Interrupt.c
 Version   : DS203_APP Ver 2.3x                                  Author : bure
*******************************************************************************/
#include "Interrupt.h"
#include "HW_V1_Config.h"
#include "Function.h"
#include "ASM_Funtion.h"

void NMIException(void)
{}

void HardFaultException(void)
{
  while (1) {}
}

void MemManageException(void)
{
  while (1) {}
}

void BusFaultException(void)
{
  while (1) {}
}
void UsageFaultException(void)
{
  while (1) {}
}

void DebugMonitor(void)
{}

void SVCHandler(void)
{}

void PendSVC(void)
{}

void SysTickHandler(void)
{}

void WWDG_IRQHandler(void)
{}

void PVD_IRQHandler(void)
{}

void TAMPER_IRQHandler(void)
{}

void RTC_IRQHandler(void)
{}

void FLASH_IRQHandler(void)
{}

void RCC_IRQHandler(void)
{}

void EXTI0_IRQHandler(void)
{}

void EXTI1_IRQHandler(void)
{}

void EXTI2_IRQHandler(void)
{}

void EXTI3_IRQHandler(void)
{}

void EXTI4_IRQHandler(void)
{}

void DMA1_Channel1_IRQHandler(void)
{}

void DMA1_Channel2_IRQHandler(void)
{}

void DMA1_Channel3_IRQHandler(void)
{}

void DMA1_Channel4_IRQHandler(void)
{}

void DMA1_Channel5_IRQHandler(void)
{}

void DMA1_Channel6_IRQHandler(void)
{}

void DMA1_Channel7_IRQHandler(void)
{}

void ADC1_2_IRQHandler(void)
{}
//void ADC_IRQHandler(void)
//{}

void USB_HP_CAN_TX_IRQHandler(void)
{
  __CTR_HP();
}

void USB_LP_CAN_RX0_IRQHandler(void)
{
  __USB_Istr();
}

void CAN_RX1_IRQHandler(void)
{}

void CAN_SCE_IRQHandler(void)
{}

void EXTI9_5_IRQHandler(void)
{}

void TIM1_BRK_IRQHandler(void)
{}

void TIM1_UP_IRQHandler(void)
{}

void TIM1_TRG_COM_IRQHandler(void)
{}

void TIM1_CC_IRQHandler(void)
{}

void TIM2_IRQHandler(void)
{}

void TIM3_IRQHandler(void)
{
  unsigned char KeyCode;
  TIM3_SR = 0;                             //清中断标志
  if(Delay_Counter>0)  Delay_Counter--;
  if(Counter_20mS>0)   Counter_20mS--;
  else {                                   //每20mS读1次键盘
    Cursor_Counter++;
    if(Cursor_Counter>24) {               //25*20mS=500mS
      Cursor_Counter=0;
      Update[Item_Index[CURRENT_ITEM]]=1;
      Update[MEASUR_KIND]=1;
      Type=1-Type;
    }
    Counter_20mS =20;
    if(Key_Wait_Counter)    Key_Wait_Counter--;
    if(Key_Repeat_Counter)  Key_Repeat_Counter--;
    KeyCode=KeyScan();
    if(KeyCode !=0) Key_Buffer = KeyCode;
  }
}

void TIM4_IRQHandler(void)
{}

void I2C1_EV_IRQHandler(void)
{}

void I2C1_ER_IRQHandler(void)
{}

void I2C2_EV_IRQHandler(void)
{}

void I2C2_ER_IRQHandler(void)
{}

void SPI1_IRQHandler(void)
{}

void SPI2_IRQHandler(void)
{}

void USART1_IRQHandler(void)
{}

void USART2_IRQHandler(void)
{}

void USART3_IRQHandler(void)
{}

void EXTI15_10_IRQHandler(void)
{}

void RTCAlarm_IRQHandler(void)
{}

void USBWakeUp_IRQHandler(void)
{}
/*******************************************************************************
* Function Name  : TIM8_BRK_IRQHandler
* Description    : This function handles TIM8 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_BRK_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : TIM8_UP_IRQHandler
* Description    : This function handles TIM8 overflow and update interrupt
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_UP_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : TIM8_TRG_COM_IRQHandler
* Description    : This function handles TIM8 Trigger and commutation interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_TRG_COM_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : TIM8_CC_IRQHandler
* Description    : This function handles TIM8 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_CC_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : ADC3_IRQHandler
* Description    : This function handles ADC3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC3_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : FSMC_IRQHandler
* Description    : This function handles FSMC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FSMC_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : SDIO_IRQHandler
* Description    : This function handles SDIO global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SDIO_IRQHandler(void)
{
  /* Process All SDIO Interrupt Sources */
//  SD_ProcessIRQSrc();

}

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : This function handles TIM5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : SPI3_IRQHandler
* Description    : This function handles SPI3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI3_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : UART4_IRQHandler
* Description    : This function handles UART4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART4_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : UART5_IRQHandler
* Description    : This function handles UART5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART5_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : TIM6_IRQHandler
* Description    : This function handles TIM6 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM6_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : This function handles TIM7 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM7_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : DMA2_Channel1_IRQHandler
* Description    : This function handles DMA2 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel1_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : DMA2_Channel2_IRQHandler
* Description    : This function handles DMA2 Channel 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel2_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : DMA2_Channel3_IRQHandler
* Description    : This function handles DMA2 Channel 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel3_IRQHandler(void)
{}

/*******************************************************************************
* Function Name  : DMA2_Channel4_5_IRQHandler
* Description    : This function handles DMA2 Channel 4 and DMA2 Channel 5
*                  interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel4_5_IRQHandler(void)
{}

/********************************* END OF FILE ********************************/
