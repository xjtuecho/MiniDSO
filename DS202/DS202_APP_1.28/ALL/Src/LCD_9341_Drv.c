/********************* (C) COPYRIGHT 2014 e-Design Co.,Ltd. ********************
 Brief   : ILI9341 LCD Drivers                                    Author : bure
********************************************************************************
   本模块按通用LCD驱动模块规划设计，可根据实际硬件配置加载不同的LCD驱动模块

 * 调用时需要外部提供的硬件相关宏定义如下：
    LCD_nRST_LOW()
    LCD_nRST_HIGH()
    LCD_RS_LOW()
    LCD_RS_HIGH()
    LCD_nRD_LOW()
    LCD_nRD_HIGH()
    LCD_nWR_LOW()
    LCD_nWR_HIGH()
    READ_LCD()
    LCD_DAT_W(Data)
    LCD_CMD_W(Cmd)
    LCD_PORT_IN()
    LCD_PORT_OUT()

 * 采用统一名称可供外部调用的函数如下：
    void LCD_Init(void);
    void SetBlock(u16 x1, u16 x2, u16 y1, u16 y2);
    void SetPosi(u16 x0, u16 y0);
    void SetPixel(u16 Color);
    u16  ReadPixel(u16 x0, u16 y0);
    void Direction(u8 Direction);

*******************************************************************************/

/*******************************************************************************

*******************************************************************************/
void Direction(u8 Direction)
{
  LCD_CMD_W(0x36);           // Memory Access Control
  LCD_DAT_W(Direction << 5); // Bit7/6/5: Y/X/V Mirror
}
/*******************************************************************************
  Init_LCD : ILI9341 LCD 显示模块初始化
*******************************************************************************/
void LCD_Init(void)
{
//--------------------------------------------------------
  LCD_RS_HIGH();        //RS=1
  LCD_nRST_HIGH();
  Ctrl(DELAYmS, 1);     //RST=1，Delay 1ms
  LCD_nRST_LOW();
  Ctrl(DELAYmS, 10);    //RST=0 Reset LCD，Delay 1ms
  LCD_nRST_HIGH();
  Ctrl(DELAYmS, 150);   //RST=1，Delay 120 ms
//--------------------------------------------------------
  
//*****ILI9341V + TM2.8 MCU 16bit*****//

LCD_CMD_W(0xCF);  
LCD_DAT_W (0x00); 
LCD_DAT_W (0x83); 
LCD_DAT_W (0X30); 
 
LCD_CMD_W(0xED);  
LCD_DAT_W (0x64); 
LCD_DAT_W (0x03); 
LCD_DAT_W (0X12); 
LCD_DAT_W (0X81); 
 
LCD_CMD_W(0xE8);  
LCD_DAT_W (0x85); 
LCD_DAT_W (0x01); 
LCD_DAT_W (0x79); 
 
LCD_CMD_W(0xCB);  
LCD_DAT_W (0x39); 
LCD_DAT_W (0x2C); 
LCD_DAT_W (0x00); 
LCD_DAT_W (0x34); 
LCD_DAT_W (0x02); 
 
LCD_CMD_W(0xF7);  
LCD_DAT_W (0x20); 

LCD_CMD_W(0xEA);  
LCD_DAT_W (0x00); 
LCD_DAT_W (0x00); 


LCD_CMD_W(0xb6);  
LCD_DAT_W(0x0a); 
LCD_DAT_W(0xa2); 

LCD_CMD_W(0xC0);    //Power control 
LCD_DAT_W (0x1D);   //VRH[5:0]   21
 
LCD_CMD_W(0xC1);    //Power control 
LCD_DAT_W (0x11);   //SAP[2:0];BT[3:0] 
 
LCD_CMD_W(0xC5);    //VCM control 
LCD_DAT_W (0x33);    //
LCD_DAT_W (0x34);    //
 
LCD_CMD_W(0xC7);    //VCM control2 
LCD_DAT_W (0xad); 
 
LCD_CMD_W(0x36);    // Memory Access Control 
LCD_DAT_W (0x04);  // RGB

LCD_CMD_W(0x3A);    
LCD_DAT_W (0x55); 
 
//LCD_CMD_W(0xF2);    // 3Gamma Function Disable 
//LCD_DAT_W (0x00); 
 
LCD_CMD_W(0x26);    //Gamma curve selected 
LCD_DAT_W (0x01); 

LCD_CMD_W(0xB1);    //
LCD_DAT_W (0x00); 
LCD_DAT_W (0x1B); 

LCD_CMD_W(0xF2);    // 3Gamma Function Disable 
LCD_DAT_W (0x00); 

//LCD_CMD_W(0xB4);    //
//LCD_DAT_W (0x07); 
 
LCD_CMD_W(0xE0);    //Set Gamma 
LCD_DAT_W (0x0F); 
LCD_DAT_W (0x23); 
LCD_DAT_W (0x1F); 
LCD_DAT_W (0x09); 
LCD_DAT_W (0x08); 
LCD_DAT_W (0x4B); 
LCD_DAT_W (0xF2); 
LCD_DAT_W (0x38); 
LCD_DAT_W (0x09); 
LCD_DAT_W (0x13); 
LCD_DAT_W (0x03); 
LCD_DAT_W (0x12); 
LCD_DAT_W (0x07); 
LCD_DAT_W (0x04); 
 
LCD_CMD_W(0XE1);    //Set Gamma 
LCD_DAT_W (0x00); 
LCD_DAT_W (0x1D); 
LCD_DAT_W (0x20); 
LCD_DAT_W (0x02); 
LCD_DAT_W (0x11); 
LCD_DAT_W (0x07); 
LCD_DAT_W (0x34); 
LCD_DAT_W (0x81); 
LCD_DAT_W (0x46); 
LCD_DAT_W (0x06); 
LCD_DAT_W (0x0E); 
LCD_DAT_W (0x0C); 
LCD_DAT_W (0x32); 
LCD_DAT_W (0x38); 
LCD_DAT_W (0x0F); 	 

LCD_CMD_W(0x11);    //Exit Sleep 
Ctrl(DELAYmS, 120); 
LCD_CMD_W(0x29);    //Display on 
}
/*******************************************************************************
  Set Block : 指定显示块   输入: X、Y坐标
*******************************************************************************/
void SetBlock(u16 x1, u16 y1, u16 x2, u16 y2)
{
  LCD_CMD_W(0x2A);
  LCD_DAT_W(y1 >> 8);
  LCD_DAT_W(y1 & 255);
  LCD_DAT_W(y2 >> 8);
  LCD_DAT_W(y2 & 255);

  LCD_CMD_W(0x2B);
  LCD_DAT_W(x1 >> 8);
  LCD_DAT_W(x1 & 255);
  LCD_DAT_W(x2 >> 8);
  LCD_DAT_W(x2 & 255);

  LCD_CMD_W(0x2C);           // Ready to write GRAM
}
/*******************************************************************************
  Set Position : 指定显示位置   输入: X、Y坐标
*******************************************************************************/
void SetPosi(u16 x0, u16 y0)
{
  LCD_CMD_W(0x2A);
  LCD_DAT_W(0x00);
  LCD_DAT_W(y0 & 255);
  LCD_DAT_W(0x00);
  LCD_DAT_W(239);

  LCD_CMD_W(0x2B);
  LCD_DAT_W(x0 >> 8);
  LCD_DAT_W(x0 & 255);
  LCD_DAT_W(0x01);
  LCD_DAT_W(0x3F);

  LCD_CMD_W(0x2C);
}
/*******************************************************************************
 SendPixels:
*******************************************************************************/
void SendPixels(u16* pBuf, u16 n)
{
  while(n--) {LCD_DAT_W(*pBuf++);};
}
/*******************************************************************************
 Set Pixel: 设置屏幕上一个指定位置点的颜色。 输入: 颜色值
*******************************************************************************/
void SetPixel(u16 Color)
{
  asm("MOVS    R2, #0x0800     ");
  asm("MOVT    R2, #0x4800     "); // GPIO Port C Base 0x48000800
  asm("MOVS    R3, #0x0800     "); // GPIO_Pin_11
  asm("STRH    R0, [R2, #0x814]"); // GPIO Port E ODR  0x48001014  
  asm("STRH    R3, [R2, #0x028]"); // GPIO Port C BRR  0x48000828
  asm("NOP");
  asm("STRH    R3, [R2, #0x018]"); // GPIO Port C BSRR 0x48000818
  asm("BX      LR");
}
/*******************************************************************************
 Read Pixel: 读取当前屏幕上一个指定位置点的颜色。 输出: 颜色值
*******************************************************************************/
u16 ReadPixel(void )
{
  u32 Tmp;

  LCD_CMD_W(0x2E);
  LCD_PORT_IN();
  LCD_nRD_LOW();
  asm("NOP");
  asm("NOP"); 
  LCD_nRD_HIGH();
  asm("NOP");
  asm("NOP"); 
  LCD_nRD_LOW();
  asm("NOP");
  asm("NOP");
  asm("NOP");
  asm("NOP"); 
  Tmp = READ_LCD();
  LCD_nRD_HIGH();
  Tmp = (Tmp & 0xF800)+((Tmp << 3)& 0x07E0);
  LCD_nRD_LOW();
  asm("NOP"); 
  asm("NOP"); 
  asm("NOP"); 
  asm("NOP"); 
  Tmp |= READ_LCD() >> 11;
  LCD_nRD_HIGH();
  LCD_PORT_OUT();
  return Tmp;
}

/******************************** END OF FILE *********************************/
