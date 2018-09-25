/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
File Name :      MMA8652FC.c
Version :        TS080 APP 1.02
Description:
Author :         Ning
Data:            2017/11/24
History:
2017/11/24       传感器硬件判断;
2017/11/24       添加新的加速度传感计配置;
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "APP_Version.h"
#include "Bios.h"
#include "Oled.h"
#include "MMA8652FC.h"
#include "I2C.h"
#include "CTRL.h"
#include "UI.h"


extern vu8 MMA_INT1;
extern vu32 Timer_Counter;
static int Read_ZYXDr(void);

vu16 gactive = 0, gShift = 0;
u8 Accelerometer_pos = 1;//加速计型号标志

typedef struct
{
    u8  hi;
    u8  lo;
} DRByte;
typedef struct
{
    DRByte  Byte;

} DR_Value;

DR_Value gX_value, gY_value, gZ_value;

/*******************************************************************************
函数名: Get_MmaActive
函数作用:获取加速度传感器静动状态
输入参数:NULL
返回参数:加速度传感器状态
*******************************************************************************/
u16 Get_MmaActive(void)
{
    return gactive;
}

void Clear_MMA_INT(void)
{
    static u32 start;
    if( MMA_INT1)
    {
        gactive = 1;
        start = Timer_StartValue();
        MMA_INT1 = 0;
        IIC_RegRead(0x31);
    }
    if (Timer_ElapseValue(start) > 15)
        gactive = 0;

    return ;
}
/*******************************************************************************
函数名: Get_MmaActive
函数作用:XXXXXXXXXXXXXXXXXXXXXX
输入参数:XXXXXXXXXXXXXXddddddXXXXXXXX
返回参数:XXXXXXXXXXXXXXXXXXXXXX
*******************************************************************************/
/*
u16 Get_MmaShift(void)
{
    return gShift;
}
*/
/*******************************************************************************
函数名: Get_MmaActive
函数作用:XXXXXXXXXXXXXXXXXXXXXX
输入参数:XXXXXXXXXXXXXXXXXXXXXX
返回参数:XXXXXXXXXXXXXXXXXXXXXX
*******************************************************************************/
void Set_MmaShift(u16 shift)
{
    gShift = shift;
}

/*******************************************************************************
函数名: IIC_RegWrite
函数作用:向Reg地址写入Data
输入参数:Reg 设置中的地址，Data数据
返回参数: 成功；
*******************************************************************************/
void IIC_RegWrite(u8 reg, u8 data)
{
    Sim_I2C_Start();
    Sim_I2C_WR_Byte(LIS_DEVICE_ADDR<<1);
    Sim_I2C_WR_Byte(reg);	
    Sim_I2C_WR_Byte(data);
    Sim_I2C_Stop();
}
/*******************************************************************************
函数名: IIC_RegRead
函数作用:从设置Reg处读出数据
输入参数:Reg 设置中的地址，存入gMmatxdata中
返回参数: 成功；
*******************************************************************************/
int IIC_RegRead(u8 reg)
{
    u8 tx_data[20];
    tx_data[0] = reg;
    if(Accelerometer_pos)
    {
        I2C_PageRead(tx_data, 1, LIS_DEVICE_ADDR, reg);
    }
    else
    {
        I2C_PageRead(tx_data, 1, DEVICE_ADDR, reg);
    }
    return tx_data[0];
}
/*******************************************************************************
函数名: StartUp_Accelerated
函数作用:设置Reg数据
输入参数:NULL
返回参数:NULL
*******************************************************************************/
void StartUp_Accelerated(void)
{
    if(Accelerometer_pos)
    {
        //        IIC_RegWrite(ST_CTRL_REG1,0x57);
        //        IIC_RegWrite(ST_CTRL_REG2,0x00);
        //        IIC_RegWrite(ST_CTRL_REG3,0x00);
        //        IIC_RegWrite(ST_CTRL_REG4,0x00);
        //        IIC_RegWrite(ST_CTRL_REG5,0x00);
        IIC_RegWrite(0x21, 0x00);
        IIC_RegWrite(0x22, 0x40);
        IIC_RegWrite(0x23, 0x00);
        IIC_RegWrite(0x30, 0x7F);
        IIC_RegWrite(0x32, 0x2A);
        IIC_RegWrite(0x33, 0x3);
        IIC_RegWrite(0x20, 0x37);
    }
    else
    {
        IIC_RegWrite(CTRL_REG1, 0);
        //---- 设置测量范围4g----------------------//
        IIC_RegWrite(XYZ_DATA_CFG_REG, FULL_SCALE_8G ); //(IIC_RegRead(XYZ_DATA_CFG_REG) & ~FS_MASK)
        //--- 设置正常操作时数据速率100HZ------------------------------------//
        IIC_RegWrite(CTRL_REG1,  DataRateValue); //IIC_RegRead(CTRL_REG1)|
        //----设置采样工作模式------------------------------------------------------//
        IIC_RegWrite(CTRL_REG2, 0);//(IIC_RegRead(CTRL_REG2) & ~MODS_MASK)
        //---------启动测量模式------------------------------------//
        IIC_RegWrite(CTRL_REG1, ACTIVE_MASK );
    }
}
/*******************************************************************************
函数名: Is_ST_LIS2DH12
函数作用:检测加速计是否是LIS2DH12
输入参数:NULL
返回参数:x,y,z的方向
*******************************************************************************/
void Is_ST_LIS2DH12(void)
{
    if(IIC_RegRead(ST_WHO_AM_I) == 51)//加速计判断
    {
        Accelerometer_pos = 1;
    }
    else
    {
        Accelerometer_pos = 0;
    }
}
/*******************************************************************************
函数名: Read_ZYXDr
函数作用:读取XYZ方向
输入参数:NULL
返回参数:x,y,z的方向
*******************************************************************************/

int Read_ZYXDr(void)
{
    u8 reg_flag;
    u8 ptr, i;
    u8 value[7];

    memset((u8 *)&gX_value, 0, 6);
    //Poll the ZYXDR status bit and wait for it to set
    if(Accelerometer_pos)
    {
        reg_flag = IIC_RegRead(ST_STATUS_REG);
        if((reg_flag & ZYXDR_BIT) != 0)
        {
            //Read 12/10-bit XYZ results using a 6 byte IIC access
            ptr = ST_OUT_X_L;
            for(i = 0; i < 6; i++)
            {
                value[i] = IIC_RegRead(ptr++);
            }
            //Copy and save each result as a 16-bit left-justified value
            gX_value.Byte.lo = value[0];
            gX_value.Byte.hi = value[1];
            gY_value.Byte.lo = value[2];
            gY_value.Byte.hi = value[3];
            gZ_value.Byte.lo = value[4];
            gZ_value.Byte.hi = value[5];
            return 1;
        }
        return 0;
    }
    else
    {
        reg_flag = IIC_RegRead(STATUS_REG);
        if((reg_flag & ZYXDR_BIT) != 0)
        {
            //Read 12/10-bit XYZ results using a 6 byte IIC access
            ptr = X_MSB_REG;
            for(i = 0; i < 6; i++)
            {
                value[i] = IIC_RegRead(ptr++);
            }
            //Copy and save each result as a 16-bit left-justified value
            gX_value.Byte.hi = value[0];
            gX_value.Byte.lo = value[1];
            gY_value.Byte.hi = value[2];
            gY_value.Byte.lo = value[3];
            gZ_value.Byte.hi = value[4];
            gZ_value.Byte.lo = value[5];
            return 1;
        }
        return 0;
    }
}

/*******************************************************************************
函数名: Cheak_XYData
函数作用:检查x，y的方向变化
输入参数:前一xy，跟现在xy坐标对比
返回参数:是否移动
*******************************************************************************/

u16 Cheak_XYData(u16 x0, u16 y0, u16 x1, u16 y1)
{
    u16 active = 0;
    gShift = 0;

    if((x1 > (x0 + 16)) || (x1 < (x0 - 16)))  active = 1;
    if((y1 > (y0 + 16)) || (y1 < (y0 - 16)))  active = 1;

    if((x1 > (x0 + 32)) || (x1 < (x0 - 32)))  gShift = 1;
    if((y1 > (y0 + 32)) || (y1 < (y0 - 32)))  gShift = 1;

    return active;
}

/*******************************************************************************
函数名: Update_X
函数作用:更新数据
输入参数:返回x
返回参数:NULL
*******************************************************************************/

u16 Update_X(void)
{
    u16 value, x;

    value = ((gX_value.Byte.hi << 8) | (gX_value.Byte.lo & 0xf0 )) >> 4;
    if(gX_value.Byte.hi > 0x7f)    x = (~value + 1) & 0xfff;
    else                        x = value & 0xfff;

    return x;
}

/*******************************************************************************
函数名: Update_Y
函数作用:更新数据
输入参数:NULL
返回参数:返回y
*******************************************************************************/

u16 Update_Y(void)
{
    u16 value, y;

    value = ((gY_value.Byte.hi << 8) | (gY_value.Byte.lo & 0xf0 )) >> 4;
    if(gY_value.Byte.hi > 0x7f)    y = (~value + 1) & 0xfff;
    else                          y = value & 0xfff;

    return y;
}

/*******************************************************************************
函数名: Update_z
函数作用:更新数据
输入参数:NULL
返回参数:返回z
*******************************************************************************/

u16 Update_Z(void)
{
    u16 value, z;

    value = ((gZ_value.Byte.hi << 8) | (gZ_value.Byte.lo & 0xf0 )) >> 4;
    if(gZ_value.Byte.hi > 0x7f)     z = (~value + 1) & 0xfff;
    else                          z = value & 0xfff;

    return z;
}

/*******************************************************************************
函数名: Check_Accelerated
函数作用:检查加速度传感器是否移动
输入参数:NULL
返回参数:NULL
*******************************************************************************/

void Check_Accelerated(void)
{
    static u16 x0 = 0, y0 = 0;
    u16 x1, y1;

    if(Read_ZYXDr())   //读数据，更新数据
    {
        x1 = Update_X();
        y1 = Update_Y();
    }
    else  x1 = x0;
    y1 = y0;
    gactive = Cheak_XYData(x0, y0, x1, y1); //检查是否移动
    x0 = x1;
    y0 = y1;
}

/******************************** END OF FILE *********************************/
