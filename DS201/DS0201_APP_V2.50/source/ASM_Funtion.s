;********************* (C) COPYRIGHT 2009 e-Design Co.,Ltd. *********************
; 文件名  : ASM_Funtion.s 文件说明 : 汇编语言子程序      硬件环境 : DS0201V1.1~1.6
; 作者    : bure          当前版本 : Ver 1.0.1           完成日期 : 2009/08/06    
;*******************************************************************************

  THUMB
  REQUIRE8
  PRESERVE8
    
  AREA |.text|, CODE, READONLY, ALIGN=2
; RSEG CODE:CODE(2)

  EXPORT __MSD_Init  
  EXPORT __Get_Medium_Characteristics  
  EXPORT __USB_Init  
  EXPORT __CTR_HP
  EXPORT __USB_Istr
  
  EXPORT __Char_to_Str
  EXPORT __Char_to_Hex
  EXPORT __Int_sqrt
  
  EXPORT __Set_System
  EXPORT __NVIC_Configuration
  EXPORT __GPIO_Config
  EXPORT __SPI_Config
  EXPORT __DMA_Configuration
  EXPORT __ADC_Configuration
  
  EXPORT __LCD_WR_REG
  EXPORT __Point_SCR
  EXPORT __Set_Pixel
  EXPORT __Add_Pixel
  EXPORT __Erase_Pixel

  EXPORT __MSD_WriteBlock
  EXPORT __MSD_ReadBlock
  EXPORT __Get_TAB_8x14
  EXPORT __Get_TAB_6x12
  EXPORT __Display_Logo
  EXPORT __LCD9325_Initial 
  EXPORT __LCD7781_Initial
  EXPORT __Get_Ref_Wave

  EXPORT __Erase_Color
  EXPORT __Add_Color
  EXPORT __Get_Pixel
  
;*******************************************************************************
; 库函数入口地址向量表 
;*******************************************************************************
__MSD_Init                ;void __MSD_Init(void)
    B       0x08004151
__Get_Medium_Characteristics  ;void __Get_Medium_Characteristics(void)
    B       0x08004155
__USB_Init                ;void __USB_Init(void)
    B       0x08004159
__CTR_HP                  ;void __CTR_HP(void)
    B       0x0800415D
__USB_Istr                ;void __USB_Istr(void);
    B       0x08004161
;-------------------------------------------------------------------------------    
__Char_to_Str             ;void __Char_to_Str(unsigned char *p, unsigned char n)
    B       0x08004165
__Char_to_Hex             ;void __Char_to_Hex(unsigned char *p, unsigned char n)
    B       0x08004169
__Int_sqrt                ;int __Int_sqrt(unsigned int n)
    B       0x0800416D
;-------------------------------------------------------------------------------
__Set_System              ;void __Set_System(void)
    B       0x08004171
__NVIC_Configuration      ;void __NVIC_Configuration(void)
    B       0x08004175
__GPIO_Config             ;void __GPIO_Config(void)
    B       0x08004179
__SPI_Config              ;void __SPI_Config(void)
    B       0x0800417D
__DMA_Configuration       ;void __DMA_Configuration(void)
    B       0x08004181
__ADC_Configuration       ;void __ADC_Configuration(void)
    B       0x08004185
;-------------------------------------------------------------------------------
__MSD_WriteBlock          ;u8 __MSD_WriteBlock(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
    B       0x08004189 
__MSD_ReadBlock           ;u8 __MSD_ReadBlock(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
    B       0x0800418D 
__Get_TAB_8x14            ;unsigned short __Get_TAB_8x14(unsigned char Code, unsigned short Row)
    B       0x08004191
__Get_TAB_6x12            ;unsigned short __Get_TAB_6x12(unsigned char Code, unsigned short Row)
    B       0x08004195
__Display_Logo            ;void __Display_Logo(unsigned short Dot_x, unsigned short Dot_y)
    B       0x08004199
__LCD9325_Initial         ;void __LCD9325_Initial(void) 
    B       0x0800419D
__LCD7781_Initial         ;void __LCD7781_Initial(void)
    B       0x080041A1
__Get_Ref_Wave            ;unsigned char __Get_Ref_Wave(unsigned short i)
    B       0x080041A5
;-------------------------------------------------------------------------------
__LCD_WR_REG              ;void __LCD_WR_REG(unsigned short Reg, unsigned short Data)
    B       0x08000151
__Add_Pixel               ;void __Add_Pixel(unsigned short x0, unsigned short y0, unsigned short Color)
    B       0x08000227
__Point_SCR               ;void __Point_SCR(unsigned short x0, unsigned short y0)
    B       0x08000189
__Set_Pixel               ;void __Set_Pixel(unsigned short Color)
    B       0x08000205
__Erase_Pixel             ;void __Erase_Pixel(unsigned short x0, unsigned short y0, unsigned short Color)
    B       0x08000375


;*******************************************************************************
__Add_Color             ;void __Add_Color(unsigned short x,unsigned short y,unsigned short Color)

;*******************************************************************************
__Erase_Color             ;void __Erase_Color(unsigned short x,unsigned short y,unsigned short Color)

;*******************************************************************************
__Get_Pixel     ;unsigned short __Get_Pixel(unsigned short x0,unsigned short y0)

  END

;******************************* END OF FILE ***********************************
  

