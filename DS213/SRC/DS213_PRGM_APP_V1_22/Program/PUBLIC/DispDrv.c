/********************* (C) COPYRIGHT 2018 e-Design Co.,Ltd. ********************
  DispDrv.c Ver: 2.0                                             Author : bure
*******************************************************************************/
void ClrScrn(u16 Color);
void DispPosn8x14(u16 x, u16 y);
void DispStr8x14(u16 ForeGrnd, u16 BackGrnd, u8 Mode, u32 StrBase);
void DispLogo(u16 x0, u16 y0);

uc16 FONT_8x14[] = {    // 标准字库
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // _  
    0x0000,0x0000,0x1C00,0x3F60,0x3F60,0x1C00,0x0000,0x0000, // !  
    0x0000,0x7000,0x7800,0x0000,0x0000,0x0000,0x7800,0x7000, // "  
    0x0000,0x0880,0x3FE0,0x3FE0,0x0880,0x3FE0,0x3FE0,0x0880, // #  
    0x0000,0x1CC0,0x3E60,0x2220,0xE238,0xE238,0x33E0,0x19C0, // $  
    0x0000,0x0C20,0x0C60,0x00C0,0x0180,0x0300,0x0660,0x0C60, // %  
    0x0000,0x01C0,0x1BE0,0x3E20,0x2720,0x3DC0,0x1BE0,0x0220, // &  
    0x0000,0x0800,0x7800,0x7000,0x0000,0x0000,0x0000,0x0000, // '  
    0x0000,0x0000,0x0F80,0x1FC0,0x3060,0x2020,0x0000,0x0000, // (  
    0x0000,0x0000,0x0000,0x2020,0x3060,0x1FC0,0x0F80,0x0000, // )  
    0x0200,0x0A80,0x0F80,0x0700,0x0700,0x0F80,0x0A80,0x0200, // *  
    0x0200,0x0200,0x0200,0x1FC0,0x1FC0,0x0200,0x0200,0x0200, // +  
    0x0000,0x0000,0x0010,0x00F0,0x00E0,0x0000,0x0000,0x0000, // ,  
    0x0200,0x0200,0x0200,0x0200,0x0200,0x0200,0x0200,0x0200, // -  
    0x0000,0x0000,0x0000,0x0060,0x0060,0x0000,0x0000,0x0000, // .  
    0x0000,0x00C0,0x0180,0x0300,0x0600,0x0C00,0x1800,0x3000, // /  
    0x0000,0x1FC0,0x3FE0,0x2320,0x2620,0x2C20,0x3FE0,0x1FC0, // 0  
    0x0000,0x0000,0x0820,0x1820,0x3FE0,0x3FE0,0x0020,0x0020, // 1  
    0x0000,0x1060,0x30E0,0x21A0,0x2320,0x2620,0x3C60,0x1860, // 2  
    0x0000,0x1040,0x3060,0x2220,0x2220,0x2220,0x3FE0,0x1DC0, // 3  
    0x0000,0x0300,0x0700,0x0D00,0x1920,0x3FE0,0x3FE0,0x0120, // 4  
    0x0000,0x3E40,0x3E60,0x2220,0x2220,0x2220,0x23E0,0x21C0, // 5  
    0x0000,0x0FC0,0x1FE0,0x3220,0x2220,0x2220,0x03E0,0x01C0, // 6  
    0x0000,0x3000,0x3000,0x21E0,0x23E0,0x2600,0x3C00,0x3800, // 7  
    0x0000,0x1DC0,0x3FE0,0x2220,0x2220,0x2220,0x3FE0,0x1DC0, // 8  
    0x0000,0x1C00,0x3E20,0x2220,0x2220,0x2260,0x3FC0,0x1F80, // 9  
    0x0000,0x0000,0x0000,0x18C0,0x18C0,0x0000,0x0000,0x0000, // :  
    0x0000,0x0000,0x0020,0x18E0,0x18C0,0x0000,0x0000,0x0000, // ;  
    0x0000,0x0200,0x0700,0x0D80,0x18C0,0x3060,0x2020,0x0000, // <  
    0x0000,0x0480,0x0480,0x0480,0x0480,0x0480,0x0480,0x0000, // =  
    0x0000,0x2020,0x3060,0x18C0,0x0D80,0x0700,0x0200,0x0000, // >  
    0x0000,0x1800,0x3800,0x2000,0x2360,0x2760,0x3C00,0x1800, // ?  
    0x0000,0x1FC0,0x3FE0,0x2020,0x27A0,0x27A0,0x3FA0,0x1F00, // @  
    0x0000,0x07E0,0x0FE0,0x1900,0x3100,0x1900,0x0FE0,0x07E0, // A  
    0x0000,0x2020,0x3FE0,0x3FE0,0x2220,0x2220,0x3FE0,0x1DC0, // B  
    0x0000,0x0F80,0x1FC0,0x3060,0x2020,0x2020,0x3060,0x18C0, // C  
    0x0000,0x2020,0x3FE0,0x3FE0,0x2020,0x3060,0x1FC0,0x0F80, // D  
    0x0000,0x2020,0x3FE0,0x3FE0,0x2220,0x2720,0x3060,0x38E0, // E  
    0x0000,0x2020,0x3FE0,0x3FE0,0x2220,0x2700,0x3000,0x3800, // F  
    0x0000,0x0F80,0x1FC0,0x3060,0x2120,0x2120,0x31C0,0x19E0, // G  
    0x0000,0x3FE0,0x3FE0,0x0200,0x0200,0x0200,0x3FE0,0x3FE0, // H  
    0x0000,0x0000,0x2020,0x3FE0,0x3FE0,0x2020,0x0000,0x0000, // I  
    0x0000,0x00C0,0x00E0,0x0020,0x2020,0x3FE0,0x3FC0,0x2000, // J  
    0x0000,0x2020,0x3FE0,0x3FE0,0x0200,0x0F80,0x3DE0,0x3060, // K  
    0x0000,0x2020,0x3FE0,0x3FE0,0x2020,0x0020,0x0060,0x00E0, // L  
    0x3FE0,0x3FE0,0x1800,0x0C00,0x0C00,0x1800,0x3FE0,0x3FE0, // M  
    0x0000,0x3FE0,0x3FE0,0x1C00,0x0E00,0x0700,0x3FE0,0x3FE0, // N  
    0x0000,0x0F80,0x1FC0,0x3060,0x2020,0x3060,0x1FC0,0x0F80, // O  
    0x0000,0x2020,0x3FE0,0x3FE0,0x2220,0x2200,0x3E00,0x1C00, // P  
    0x0000,0x1F80,0x3FC0,0x2040,0x21C0,0x20F0,0x3FF0,0x1F90, // Q  
    0x0000,0x2020,0x3FE0,0x3FE0,0x2200,0x2300,0x3FE0,0x1CE0, // R  
    0x0000,0x18C0,0x3CE0,0x2620,0x2220,0x2320,0x39E0,0x18C0, // S  
    0x3800,0x3000,0x2020,0x3FE0,0x3FE0,0x2020,0x3000,0x3800, // T  
    0x0000,0x3FC0,0x3FE0,0x0020,0x0020,0x0020,0x3FE0,0x3FC0, // U  
    0x3F00,0x3F80,0x00C0,0x0060,0x0060,0x00C0,0x3F80,0x3F00, // V  
    0x3F80,0x3FE0,0x00E0,0x0380,0x0380,0x00E0,0x3FE0,0x3F80, // W  
    0x3060,0x38E0,0x0D80,0x0700,0x0700,0x0D80,0x38E0,0x3060, // X  
    0x3800,0x3C00,0x0620,0x03E0,0x03E0,0x0620,0x3C00,0x3800, // Y  
    0x3860,0x30E0,0x21A0,0x2320,0x2620,0x2C20,0x3860,0x30E0, // Z  
    0x0000,0x0000,0x3FE0,0x3FE0,0x2020,0x2020,0x0000,0x0000, // [  
    0x0000,0x3800,0x1C00,0x0E00,0x0700,0x0380,0x01C0,0x00E0, // '\'    
    0x0000,0x0000,0x2020,0x2020,0x3FE0,0x3FE0,0x0000,0x0000, // ]  
    0x0000,0x1000,0x3000,0x6000,0xC000,0x6000,0x3000,0x1000, // ^  
    0x0008,0x0008,0x0008,0x0008,0x0008,0x0008,0x0008,0x0008, // _  
    0x0000,0x0000,0x0000,0xC000,0xE000,0x2000,0x0000,0x0000, // `  
    0x0000,0x00C0,0x05E0,0x0520,0x0520,0x07C0,0x03E0,0x0020, // a  
    0x0000,0x2020,0x3FE0,0x3FC0,0x0420,0x0620,0x03E0,0x01C0, // b  
    0x0000,0x03C0,0x07E0,0x0420,0x0420,0x0420,0x0660,0x0240, // c  
    0x0000,0x01C0,0x03E0,0x0620,0x2420,0x3FC0,0x3FE0,0x0020, // d  
    0x0000,0x03C0,0x07E0,0x0520,0x0520,0x0520,0x0760,0x0340, // e  
    0x0000,0x0220,0x1FE0,0x3FE0,0x2220,0x3200,0x1800,0x0000, // f  
    0x0000,0x0390,0x07D8,0x0448,0x0448,0x03F8,0x07F0,0x0400, // g  
    0x0000,0x2020,0x3FE0,0x3FE0,0x0200,0x0400,0x07E0,0x03E0, // h  
    0x0000,0x0000,0x0000,0x0420,0x37E0,0x37E0,0x0020,0x0000, // i  
    0x0000,0x0030,0x0038,0x0008,0x0408,0x37F8,0x37F0,0x0000, // j  
    0x0000,0x2020,0x3FE0,0x3FE0,0x0100,0x0380,0x06E0,0x0460, // k  
    0x0000,0x0000,0x0000,0x2020,0x3FE0,0x3FE0,0x0020,0x0000, // l  
    0x07E0,0x07E0,0x0600,0x03E0,0x03E0,0x0600,0x07E0,0x03E0, // m  
    0x0000,0x0400,0x07E0,0x03E0,0x0400,0x0400,0x07E0,0x03E0, // n  
    0x0000,0x03C0,0x07E0,0x0420,0x0420,0x0420,0x07E0,0x03C0, // o  
    0x0000,0x0408,0x07F8,0x03F8,0x0448,0x0440,0x07C0,0x0380, // p  
    0x0000,0x0380,0x07C0,0x0440,0x0448,0x03F8,0x07F8,0x0408, // q  
    0x0000,0x0420,0x07E0,0x03E0,0x0620,0x0400,0x0700,0x0300, // r  
    0x0000,0x0240,0x0760,0x0520,0x05A0,0x04A0,0x06E0,0x0240, // s  
    0x0000,0x0400,0x0400,0x1FC0,0x3FE0,0x0420,0x0460,0x0040, // t  
    0x0000,0x07C0,0x07E0,0x0020,0x0020,0x07C0,0x07E0,0x0020, // u  
    0x0700,0x0780,0x00C0,0x0060,0x0060,0x00C0,0x0780,0x0700, // v  
    0x07C0,0x07E0,0x0060,0x01C0,0x01C0,0x0060,0x07E0,0x07C0, // w  
    0x0000,0x0420,0x0660,0x03C0,0x0180,0x03C0,0x0660,0x0420, // x  
    0x0000,0x0780,0x07C8,0x0048,0x0048,0x0058,0x07F0,0x07E0, // y  
    0x0000,0x0620,0x0660,0x04E0,0x05A0,0x0720,0x0660,0x0460, // z  
    0x0000,0x0200,0x0200,0x1FC0,0x3DE0,0x2020,0x2020,0x0000, // {  
    0x0000,0x0000,0x0000,0x3DE0,0x3DE0,0x0000,0x0000,0x0000, // |  
    0x0000,0x2020,0x2020,0x3DE0,0x1FC0,0x0200,0x0200,0x0000, // }  
    0x0000,0x1000,0x3000,0x2000,0x3000,0x1000,0x3000,0x2000, // ~  
    0x0000,0x01C0,0x03C0,0x0640,0x0C40,0x0640,0x03C0,0x01C0,// tri 
};

/*******************************************************************************
 Clear_Screen: 清LCD显示器屏幕。
*******************************************************************************/
void ClrScrn(u16 Color)
{
  vu16 Temp = Color;
  LCD_W_Block(0, 0, COL_SIZE, ROW_SIZE);
  u32 n = COL_SIZE * ROW_SIZE;
  DMA_FILL((u32)&Temp, n);
  LCD_DmaWait();
}
/*******************************************************************************
  屏幕显示字符串  Mode: 0/1 => CHAR/INVR
*******************************************************************************/
void DispPosn8x14(u16 x, u16 y)
{
  LCD_W_Block(x, y, COL_SIZE-1, y+13);
}
/*******************************************************************************
  屏幕显示字符串  Mode: 0/1 => CHAR/INVR
*******************************************************************************/
void DispStr8x14(u16 ForeGrnd, u16 BackGrnd, u8 Mode, u32 StrBase)
{
  u8 *Str = (u8*)StrBase;
  while(*Str != 0){
    for(u16 i = 0; i < 8; ++i){
      u16 k = FONT_8x14[(*Str - ' ')*8+i];
      for(u16 j = 0; j < 14; ++j){
        if(k&4)  *pLCD_DB_W = (Mode & INVR) ? BackGrnd : ForeGrnd;
        else     *pLCD_DB_W = (Mode & INVR) ? ForeGrnd : BackGrnd;
        k >>= 1;
      }
    }
    Str++;
  }
  u8 n = 14;
  while(n--) *pLCD_DB_W = BLK;  // 字符串后增加一空白列
}
/*******************************************************************************
 Display_Logo: 在指定位置显示商标图案   输入: X、Y坐标
--------------------------------------------------------------------------------
LOGO文件格式为 16 色 64*256 的 BMP 文件，但扩展名改为 .INF
 0x00~0x09: 文件类型 2B,大小 4B, 保留 4B, 0x0A~0x0D: 文件头到实际图像数据的偏移
 0x36~0x75: 调色板(每色 4B, 顺序为 B, G, R, Alpha)
 0x0076~0x2075: 8kB, 16k 点图像数据
*******************************************************************************/
void DispLogo(u16 x0, u16 y0)
{
  u16 Color[16];
  u8 *p = (u8*)0x0807D800;//&LOGO_BASE;
  u16 x = x0+256;
  for(u16 i = 0; i < 16; i++){ // 0x36~0x75: 调色板(每色 4B, 顺序为 B, G, R, Alpha)
    Color[i] = ((p[0x36+i*4]>> 3)<< 11)+((p[0x37+i*4]>> 2)<< 5)+(p[0x38+i*4]>> 3);
  }
  p += 0x76;
  for(u16 i = 0; i < 128; i++){
    x = x0+i*2;
    LCD_Posn(x, y0);
    for(u16 j = 0; j < 64; j++) *pLCD_DB_W = Color[p[i+j*128]>> 4]; // 高半字节
    LCD_Posn(x + 1, y0);
    for(u16 j = 0; j < 64; j++) *pLCD_DB_W = Color[p[i+j*128]& 15]; // 低半字节
  }
}

/******************************** END OF FILE *********************************/
