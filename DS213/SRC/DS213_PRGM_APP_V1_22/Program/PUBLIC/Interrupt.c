/********************* (C) COPYRIGHT 2018 e-Design Co.,Ltd. ********************
  Interrupt.c Ver 2.0                                            Author : bure
*******************************************************************************/
#include "Interrupt.h"

void SysInt(void);
void USB_Istr(void);

/*******************************************************************************
              Cortex-M4 Processor Exceptions Handlers
*******************************************************************************/


void NMI_Handler(void){;}

void HardFault_Handler(void) {while (1);}

void MemManage_Handler(void) {while (1);}

void BusFault_Handler(void)  {while (1);}

void UsageFault_Handler(void){while (1);}

void SVC_Handler(void){}

void DebugMon_Handler(void){}

void PendSV_Handler(void){}

void SysTick_Handler(void) 
{
  SysInt();
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
  USB_Istr();
}

/******************************  END OF FILE  *********************************/

