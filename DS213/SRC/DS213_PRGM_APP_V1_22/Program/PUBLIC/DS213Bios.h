/********************* (C) COPYRIGHT 2018 e-Design Co.,Ltd. ********************
  Bios.h for DS213 DFU & APP                                     Author : bure
*******************************************************************************/
#ifndef __BIOS_H
#define __BIOS_H

  #ifdef STM32F30X
    #include "STM32F30x.h"
  #elif STM32F10X_HD
    #include "STM32F10x.h"
  #endif

  #include <stdlib.h>
  #include <string.h>
  #include "Func.h"
  #include "Ident.h"        
  #include "USB_bot.h"
  #include "usb_lib.h"
  #include "USB_pwr.h"
  #include "DS213HwDriver.h"

  extern HwDrvDef  Hw;

  void __BiosInit(u32 pDrv);
  void SetLicence(u16 x, u16 y);
  
  //-------------------- DevInit(u16 Dev)函数中 Dev 宏定义 ---------------------
  
  enum { 
    PWRCTRL, // 电源控制设备
    KEYnDEV, // 按键类输入设备
    BUZZDEV, // 蜂鸣器设备
    DISPDEV, // LCD 显示设备 
    USB_DEV, // USB 通信设备
    USB_SCI, // SCI 通信设备
    USBDISK, // USB Disk 设备
    FPGADEV, // FPGA 接口设备
    FPGACFG, // FPGA 上电配置
    SGNL_IN, // 信号输入通道
    SGNLOUT, // 信号输出通道
    SO_ANLG, // 模拟信号输出
    SO_DGTL, // 数字信号输出
  };

//-------------------------------- 显示相关定义 --------------------------------
  
  #define WHT        0xFFFF
  #define CYAN       0xFFE0
  #define YEL        0x07FF
  #define PRPL       0xF81F
  #define GRN        0x07E0
  #define GRAY       0x7BEF
  #define DARK       0x39E7
  #define BLUE       0xF800
  #define ORNG       0x051F
  #define RED        0x001F
  #define BLK        0x0000

  #define CHAR       0x00   //  Standard Display 字符显示
  #define INVR       0x01   //  Inverse Display  反转显示
  #define SYMB       0x02   //  Symbol Display   符号显示
  #define VOID       0x04   //  Dotted Display   虚影显示
  
#endif
/********************************* END OF FILE ********************************/
