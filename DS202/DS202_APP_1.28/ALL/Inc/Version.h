/********************* (C) COPYRIGHT 2014 e-Design Co.,Ltd. ********************
 Brief : Device version description                              Author : bure
*******************************************************************************/
#ifndef __VERSION_H
#define __VERSION_H

#include "STM32F30x.h"
#include <string.h>
#include <stdlib.h>
#include "Option.h"

  #define M_VER              'V','3'
  #define S_VER              '4','0'
  #define OEM_TYPE           "0"

/*----------------------------------------------------------------------------*/
  #ifdef TYPE_DS202

    #include "DS202V2_0.h"
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
    #define TP_ID             HC640_ID
    #define SERIAL_NO1       (*(u32*)SN_ADDR+0)
    #define SERIAL_NO2       (*(u32*)SN_ADDR+4)
    #define SERIAL_NO3       (*(u32*)SN_ADDR+8)

  #endif
/*----------------------------------------------------------------------------*/
  #ifdef LCD_ILI_9341
    #define LCD_STR          "ILI 9341"
    #define LCD_M            'D'
  #endif
/*----------------------------------------------------------------------------*/
  #ifdef OEM_AES
    #define BG    WHT
    #define FG    BLK
    #define DEVICE_SN        SERIAL_NO3
  #else
    #define BG    BLK
    #define FG    WHT
    #define DEVICE_SN        SERIAL_NO1 + SERIAL_NO2 + SERIAL_NO3
  #endif
/*----------------------------------------------------------------------------*/
  #ifdef SEEED
    #define OEM_STR          "1"
  #elif  OEM_AES
    #define OEM_STR          "2"
  #else
    #define OEM_STR          "0"
  #endif
/*----------------------------------------------------------------------------*/
  #define VERSION_STR        'D','F','U',' ',M_VER,'_',S_VER,'_',LCD_M, 8
  #define DFU_STR            {M_VER,'.',S_VER,LCD_M, 0}

#endif /* VERSION_H */
/*********************************  END OF FILE  ******************************/
