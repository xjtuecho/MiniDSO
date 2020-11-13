/********************* (C) COPYRIGHT 2018 e-Design Co.,Ltd. ********************
  Interrupt.h Ver 2.0                                            Author : bure
*******************************************************************************/
#ifndef __INTERRUPT_H
#define __INTERRUPT_H

  void NMI_Handler(void);
  void HardFault_Handler(void);
  void MemManage_Handler(void);
  void BusFault_Handler(void);
  void UsageFault_Handler(void);
  void SVC_Handler(void);
  void DebugMon_Handler(void);
  void PendSV_Handler(void);

  void SysTick_Handler(void);
  void USB_LP_CAN1_RX0_IRQHandler(void);

#endif /* __INTERRUPT_H */

/*********************************  END OF FILE  ******************************/
