/******************** (C) COPYRIGHT 2018 e-Design Co., Ltd. ********************
  USB_pwr.h STM32 USB Disk Ver 2.0        Author : MCD Application Team & bure
*******************************************************************************/
#ifndef __USB_PWR_H
#define __USB_PWR_H

#include "usb_core.h"

typedef enum _RESUME_STATE
{
  RESUME_EXTERNAL,
  RESUME_INTERNAL,
  RESUME_LATER,
  RESUME_WAIT,
  RESUME_START,
  RESUME_ON,
  RESUME_OFF,
  RESUME_ESOF
} RESUME_STATE;

typedef enum _DEVICE_STATE
{
  UNCONNECTED,
  ATTACHED,
  POWERED,
  SUSPENDED,
  ADDRESSED,
  CONFIGURED
} DEVICE_STATE;

void Suspend(void);
void Resume_Init(void);
void Resume(RESUME_STATE eResumeSetVal);
RESULT PowerOn(void);
RESULT PowerOff(void);

extern  vu32 bDeviceState; /* USB device status */
extern  vu8 fSuspendEnabled;  /* true when suspend is possible */

#endif 
/*********************************  END OF FILE  ******************************/
