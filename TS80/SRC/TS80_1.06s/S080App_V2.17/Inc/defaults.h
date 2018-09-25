/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
File Name :     defaults.h
Version :        TS80 APP v1.x
Description:
Author :         Joker
Data:            2018.06
History:
*******************************************************************************/
#ifndef __CFGDEFAULTS_H
#define __CFGDEFAULTS_H

#define EQU_OPR			=

//working temperature
#define WKTEMP_STR       	WkTemp
#define WKTEMP_VAL       	300

//stand-by temperature
#define STBTEMP_STR      	StbTemp
#define STBTEMP_VAL      	200

//sleep latency time
#define SLPTIME_STR      	SlpTime
#define SLPTIME_VAL      	180

//rated power
#define RP_STR      		Rated_P
#define RP_VAL      		18

//temperature setting step
#define TEMPSTP_STR      	TempStp
#define TEMPSTP_VAL      	10

//Voltage protection (unit: 0.1V)
#define OFFVOLT_STR      	OffVolt
#define OFFVOLT_VAL      	13

//Temperature units(0:Celsius ¡æ, 1:Fahrenheit ¨H)
#define TEMPUNIT_STR     	Temp
#define TEMPUNIT_VAL     	0

//usage mode(0:Right-hand , 1:Left-hand)
#define USMODE_STR      	Hand
#define USMODE_VAL      	0

//portable source mode(0:not portable , 1:portable)
#define PORTABLE_STR      	Portable
#define PORTABLE_VAL            0

//Zero Calibration (Do not modify!)
#define ZEROCALI_STR      	ZeroP_Ad
#define ZEROCALI_VAL      	102

#define _STR(s)     #s
#define STR(s)       _STR(s)

#define CONS(a,b)  STR(a)##STR(EQU_OPR)##STR(b)
#endif