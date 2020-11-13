# ReleaseNotes

## MDP_P905_v1.20.067.hex(based on MDP_P905_v1.20_20190627_svn56.hex)

1. 修复QC协议不稳定导致输出保护的问题。
   Fix the problem that the QC protocol is unstable and cause output protection.
2. 修复输入电压电流偶尔出现异常报警的问题。
   Fix the problem that the input voltage and current occasionally have an abnormal alarm.
3. 提升各类QC充电器的兼容性。
   Improve the compatibility of various QC chargers.
4. 修复非QC协议充电器可能导致灯光异常及界面卡顿的问题。
   Repairing non-QC protocol chargers may cause problems with lighting anomalies and interface jams.
5. 修复off状态显控端设置电压电流时，子模块没有同步刷新的问题。
   When the off-state display terminal sets the voltage and current, the sub-module does not have the problem of synchronous refresh.
6. 优化输出倒灌保护的响应。
   Optimize the response of the output backflow protection.

## Why and How to flash a .bin firmware

DFU V3.64D can not update large .hex file.

1. Get in DFU mode.
2. Copy `MDP_ADR.ADR` to DFU disk, and the disk will restart, then will have a file named `MDP_ADR.SET`
3. Copy `MDP*.bin` to DFU disk.
4. If you can see the `MDP*.RDY` , The firmware is already install.


## 注意：

1. 凡是从P905_v1.21的版本升级到P905_v1.22及以后的版本，需要用户先备份文件到电脑上，然后升级，升级完要格式化U盘，接着把备份文件放回U盘。
2. 凡是从P905_v1.20的版本升级到P905_v1.22及以后的版本，需要用户先备份文件Q_SET.TXT（除Q_SET.TXT之外的文件可以删掉）到电脑上，然后升级，升级完要格式化U盘，接着把备份文件Q_SET.TXT放回U盘。
3. 若通过hex文件无法升级，请尝试通过bin文件升级。

## Note:

1. Any upgrade from P905_v1.21 to P905_v1.22 and later versions requires the user to back up the files to the computer first, then upgrade, format the U disk after the upgrade, and then put the backup files back into the U disk.
2. When upgrading from P905_v1.20 to P905_v1.22 and later, the user needs to back up the file Q_SET.TXT (files other than Q_SET.TXT can be deleted) to the computer, and then upgrade. Format the U disk, and then put the backup file Q_SET.TXT back into the U disk.
3. If you cannot upgrade through the hex file, please try to upgrade through the bin file.

## 升级bin的方法：

1. 先把MDP_ADR.ADR放进DFU盘，当成功后，原文件名变成MDP_ADR.SET，就代表该文件已成效了；
2. 再把后缀名为bin的固件放进DFU盘，当成功后，原文件的后缀名变成RDY，就代表升级成功。

## Method to upgrade bin:

1. Put MDP_ADR.ADR into the DFU disk first, and when successful, the original file name becomes MDP_ADR.SET, which means that the file has worked;
2. Put the firmware with the suffix named bin into the DFU disk. When it is successful, the suffix of the original file becomes RDY, which means the upgrade is successful.