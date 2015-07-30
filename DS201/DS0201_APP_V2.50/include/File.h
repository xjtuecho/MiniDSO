/******************** (C) COPYRIGHT 2009 e-Design Co.,Ltd. *********************
* File Name: file.h      Author: bure      Hardware: DS0201V1.1    Version: V1.0
*******************************************************************************/
#ifndef __FILE_H
#define __FILE_H
//================================= º¯ÊýÉùÃ÷ ===================================
char FAT_Info(void);
char Open_BMP_File(unsigned char *n);
char Open_DAT_File(unsigned char *n);
char Writ_BMP_File(void);
//char Open_CFG_File(void);
char Read_File(void);
char Write_File(void);
void Read_Parameter(void);
char Write_Parameter(void);
#endif
/********************************* END OF FILE ********************************/
