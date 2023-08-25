# TS80固件更新日志

## V1.30

- 调整LIS2DH12、SC7A20传感器震动判断值
- SC7A20传感器数据开启低通滤波

- Adjust the vibration judgment value of LIS2DH12 and SC7A20 sensors
- SC7A20 sensor data enable low-pass filtering

## V1.29

- 增加兼容SC7A20加速度传感器

- Add compatible SC7A20 acceleration sensor

## V1.28

- 删除温度校准功能
- 修改U盘参数注释

- Remove temperature calibration function
- Modify U disk parameter comment

## V1.27

- 修改过压保护电压范围，最高14V
- 修复显示电阻代码BUG
- 2020/8/11上传整理

- Modify the overvoltage protection voltage range, up to 14V
- Fix the display resistance code BUG

## V1.26

- 增加兼容MSA301 G-sensor
- 2020/7/31上传整理

- Added compatibility with MSA301 G-sensor

## V1.24

- 完善电源类型检测
- 2020/6/12上传整理

- Improve power supply type detection

## V1.23

- 稳定温度显示
- 2020/5/20上传整理

- Stable temperature display

## V1.22

- 整理升压代码
- 增加申请电压检测和申请失败后会重新申请
- 2020/4/28上传整理

- Organize boost code
- Increase the application voltage test and reapply after the application fails

## V1.21

- 修改上一版本遗漏的移动电源唤醒字符部分代码

- Modify the part of the code of the mobile power wake-up character missing from the previous version

## V1.20

- 修改移动电源唤醒选项字符，改为LowCur
- 2020/4/22上传整理

- Modify the mobile power wakeup option character to LowCur

## V1.19

- 完善移动电源唤醒功能，修改唤醒周期，周期5s，加热30ms
- 完善唤醒功能，在修改菜单参数时也在唤醒
- 进入休眠时，关闭唤醒
- 修复因为唤醒功能导致的功率检测和电阻检测不准问题
- 在待机界面，温度过高不关屏
- 修改的临时温度不断电不丢失(修改菜单的工作温度会清临时温度)
- 完善QC电源识别和QC申请电压
- 2020/4/8上传整理

- Improve the wake-up function of the mobile power supply, modify the wake-up cycle, the cycle is 5s, and the heating is 30ms
- Improve the wake-up function, and wake up when the menu parameters are modified
- When entering sleep, turn off wake up
- Fix the problem of inaccurate power detection and resistance detection caused by the wake-up function
- In the standby interface, the screen will not turn off if the temperature is too high
- The modified temporary temperature will not be lost without powering up (modifying the working temperature of the menu will clear the temporary temperature)
- Improve QC power identification and QC application voltage

## V1.14

- 工作功率改为1s平均功率
- 温控和状态进入休眠状态时温度下降过程中增加移动电源唤醒功能
- 加速度传感器检测更灵敏

- Working power changed to 1s average power
- Add mobile power wake-up function during temperature drop when temperature control and state enter sleep state

## V1.13

- 完善温控状态下供电类型检测，增加直流供电符号
- 增加温控状态下显示电压或者功率可选功能
- 完善进入休眠模式后，电压不降，再进入待机状态电压降压处理
- 完善U盘部分代码，U盘保存数据不卡死重启
- 2020/3/4上传整理

- Improve the detection of power supply type under temperature control, and add DC power supply symbol
- Add the optional function of displaying voltage or power in temperature control state
- After entering the sleep mode, the voltage does not drop, and then enters the standby state voltage step-down processing
- Improve some codes of the USB flash drive, save the data on the USB flash drive without freezing and restarting

## V1.11

- 完善FUSB302初始化功能重试
- 修复温控状态下，电压显示BUG(显示两个V)
- PD供电，菜单页面功率不可调
- PD供电，进入菜单页面升压
- 2020/2/25上传整理

- Improve FUSB302 initialization function to retry
- Repair the voltage display BUG under temperature control status (display two V)
- PD power supply, menu page power is not adjustable
- PD power supply, enter the menu page to boost

## V1.10

- 完善移动电源唤醒功能
- 增加USB连接电脑检测功能，在USB连接电脑时没有唤醒功能和PD QC不申请电压
- 2020/2/19上传整理

- Improve the power bank wake-up function
- Added USB connection to computer detection function, there is no wake-up function when USB is connected to computer, and PD QC does not apply for voltage

## v1.07

- 唤醒程序bug修复
- 低电压警告bug修复
- PID控制加热机制更改，升温更快捷温度更稳定稳定速度更快

## v1.00

- TS80P第一版程序(待用户测试)
- 2020/1/19上传整理
