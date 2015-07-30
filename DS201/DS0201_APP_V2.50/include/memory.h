/******************** (C) COPYRIGHT 2009 e-Design Co.,Ltd. *********************
* File Name: memory.h    Author: bure   Hardware: DS0201V1.1   Version: Ver 1.0
*******************************************************************************/
#ifndef __memory_H
#define __memory_H

#include "usb_lib.h"

void Read_Memory(void);
void Write_Memory(void);
bool Address_Management_Test(u8 Cmd);

#endif

/********************************* END OF FILE ********************************/
