/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
 Brief : Device version description                              Author : bure
*******************************************************************************/
#ifndef __VERSION_H
#define __VERSION_H

#include "STM32F30x.h"
#include <string.h>
#include <stdlib.h>
#include "Option.h"

/*----------------------------------------------------------------------------*/
  #ifdef TYPE_DS212

    #include "DS212V1_0.h"
    #define DISKSTR          "8MB Internal"
    #define PAGE_SIZE         256
    #define SEC_SIZE          4096
    #define SEC_AMNT          2048
    #define FLASH_PAGE        0x0800
    #define LCD_COL           240
    #define LCD_ROW           320
    #define LOGO_X            4*8
    #define X_SIZE            300
    #define Y_SIZE            200
    #define SERIAL_NO1       (*(u32*)SN_ADDR+0)
    #define SERIAL_NO2       (*(u32*)SN_ADDR+4)
    #define SERIAL_NO3       (*(u32*)SN_ADDR+8)

  #endif

#endif /* VERSION_H */
/*********************************  END OF FILE  ******************************/
