//********************* (C) COPYRIGHT 2009 e-Design Co.,Ltd. *********************
// 文件名  : ASM_Funtion.s 文件说明 : 汇编语言程序头文件  硬件环境 : DS0201V1.1
// 作者    : bure          当前版本 : Ver 1.0.1           完成日期 : 2009/08/06   
//*******************************************************************************

#ifndef __ASM_FUNTION_H
#define __ASM_FUNTION_H

void __MSD_Init(void);  
void __Get_Medium_Characteristics(void);  
void __USB_Init(void);  
void __CTR_HP(void);
void __USB_Istr(void);

void __Char_to_Str(unsigned char *p, unsigned char n);
void __Char_to_Hex(unsigned char *p, unsigned char n);
int  __Int_sqrt(unsigned int n);

void __Set_System(void);
void __NVIC_Configuration(void);
void __GPIO_Config(void);
void __SPI_Config(void);
void __DMA_Configuration(void);
void __ADC_Configuration(void);

u8 __MSD_WriteBlock(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
u8 __MSD_ReadBlock(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
unsigned short __Get_TAB_8x14(unsigned char Code, unsigned short Row);
unsigned short __Get_TAB_6x12(unsigned char Code, unsigned short Row);
void __Display_Logo(unsigned short Dot_x, unsigned short Dot_y);
void __LCD9325_Initial(void); 
void __LCD7781_Initial(void);
unsigned char __Get_Ref_Wave(unsigned short i);

void __LCD_WR_REG(unsigned short Reg, unsigned short Data);
void __Point_SCR(unsigned short x0, unsigned short y0);
void __Set_Pixel(unsigned short Color);
void __Add_Pixel(unsigned short x0, unsigned short y0, unsigned short Color);
unsigned short __Get_Pixel(unsigned short x0, unsigned short y0);

void __Erase_Color(unsigned short x,unsigned short y,unsigned short Color);
void __Add_Color(unsigned short x,unsigned short y,unsigned short Color);

#endif /* __ASM_FUNTION_H */

//****************************** END OF FILE ***********************************
