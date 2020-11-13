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
  
  //-------------------- DevInit(u16 Dev)������ Dev �궨�� ---------------------
  
  enum { 
    PWRCTRL, // ��Դ�����豸
    KEYnDEV, // �����������豸
    BUZZDEV, // �������豸
    DISPDEV, // LCD ��ʾ�豸 
    USB_DEV, // USB ͨ���豸
    USB_SCI, // SCI ͨ���豸
    USBDISK, // USB Disk �豸
    FPGADEV, // FPGA �ӿ��豸
    FPGACFG, // FPGA �ϵ�����
    SGNL_IN, // �ź�����ͨ��
    SGNLOUT, // �ź����ͨ��
    SO_ANLG, // ģ���ź����
    SO_DGTL, // �����ź����
  };

//-------------------------------- ��ʾ��ض��� --------------------------------
  
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

  #define CHAR       0x00   //  Standard Display �ַ���ʾ
  #define INVR       0x01   //  Inverse Display  ��ת��ʾ
  #define SYMB       0x02   //  Symbol Display   ������ʾ
  #define VOID       0x04   //  Dotted Display   ��Ӱ��ʾ
  
#endif
/********************************* END OF FILE ********************************/
