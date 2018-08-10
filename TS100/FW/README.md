# TS100烙铁固件修改说明

===============================================================================

## v2.15-2.17 更新内容整合(20170512)

1. 在控温模式下的设置模式改为以前版本的只能调节温度，关机后参数不保存；
2. 在待机模式单击B进入可改变参数的设置模式(前版本为温度计模式)；
3. 进入设置模式后单击A/B可控制左右移动选择需要修改项，长按A/B键进入该选择项的
   修改(在同屏显示温度电压项长按A/B经行校准)，同时长按A，B键返回待机模式；
4. 增加恢复出厂设备功能还原所有参数为默认值，此功能在设置模式Factory Reset选项
   时长按任意键恢复出厂设置。
5. 优化了控温模式下温度设置界面，添加温度设置时双击A,B键进入电压显示界面，
   进入后长按A,B即可返回控温模式；
6. 扩大了保护电压的范围和精度，可以精确到0.1V；
7. 从待机模式进入设置模式后第一项固定为温度调校和电压显示；
8. 在设置模式长按任意键进入选择项的参数调整时添加双键长按退出功能；
9. 添加当TempStp步长设置为1时，温控模式下温度设置长按有快速修改效果。
10. 对切换为左手模式进行优化(A/B键也会对应左右手模式调整)；

## V2.15-2.17 update content integration (20170512)

1. in the temperature control mode settings mode changed to the previous version, can only adjust the temperature, after shutdown parameters are not saved;
2. In standby mode, click B to enter the settings mode with changing parameters (previous version is thermometer mode);
3. enter the settings mode, click A/B to control the move around, select the need to modify the item, long press the A/B key to enter the selection of changes (displayed at the same screen temperature
   The voltage term is calibrated by A/B, and the A and B keys are returned to standby mode at the same time;
4. increase the restore factory device function, restore all parameters as default values, this function in the set mode Factory Reset option, long press any key to restore
   Factory settings.
5. optimize the temperature control mode temperature setting interface, add temperature settings, double-click A, B key into the voltage display interface, enter after long press A, B can return Temperature control mode;
6. expand the scope and accuracy of protection voltage, can be accurate to 0.1V;
7. after the standby mode enters the set mode, the first item is fixed to the temperature adjustment and voltage display;
8, in setting mode long press any key to enter the selection of parameter adjustment, add double bond, long press the exit function;
9. add, when the TempStp step size is set to 1, the temperature control mode temperature setting is long, according to have the fast modification effect.
10. to switch to left hand mode optimization (A/B key will also correspond to left and right hand mode adjustment);

## v2.16 更新内容(20170427)

1. 优化了控温模式下温度设置界面，添加温度设置时双击A,B键进入电压显示界面，
   进入后长按A,B即可返回控温模式；
2. 扩大了保护电压的范围和精度，可以精确到0.1V；
3. 从待机模式进入设置模式后第一项固定为温度调校和电压显示；
4. 在设置模式长按任意键进入选择项的参数调整时添加双键长按退出功能；
5. 添加当TempStp步长设置为1时，温度设置长按有快速修改效果。

## v2.15 更新内容(20170401)

1. 在控温模式下的设置模式改为以前版本的只能调节温度，关机后参数不保存；
2. 在待机模式单击B进入可改变参数的设置模式(前版本为温度计模式)，温度/电压显示
   以及校准都放在了设置模式里；
3. 进入设置模式后单击A/B可控制左右移动选择需要修改项，长按A键进入该选择项的修改
   (显示温度电压项长按A经行校准)，同时长按A/B返回待机模式；
4. 增加恢复出厂设备功能还原所有参数，此功能在设置模式Factory Reset选项时长按A
   恢复出厂设置。

## v2.14 更新内容(20170328)

1. 根据意见设置模式自动退出时间加长并可以通过长按B键离开设置模式；
2. 进入设置模式后单击A/B可控制左右移项，长按A键进入该选择项的修改；
3. 从设置模式长按A进入修改模式后，单击A/B可对数值进行一个设置单位的减/增，
   长按A/B   则会持续减/增直到松开按键或者超出范围，修改到适宜的参数后不操作
   等待4s即可回到设置模式。

## v2.13 更新内容(20170215)

1. 添加左右手模式，可在设置模式种选择惯用手以调整屏幕方向；
2. 添加设置模式，在温控模式时长按任意键进入，设置模式可以设置休眠温度，工作温
   度，休眠时间，待机时间，保护电压，温度递进，温度单位设置和左右控制选择；
3. 设置模式长按A/B进行修改项的选择，单击A/B进行选中项参数的修改，不操作3s后
   自动返回温控模式并保存相关参数。

## v2.12 更新内容(20160606)

1. 设置温度可保存

## v2.11 更新内容(20150803)

1. 优化按键延时处理；
2. 优化显示处理
3. 优化高温报警；
4. 提高报警的最高电压；
5. 新加入屏幕保护功能，进入待机界面一段时间后，屏幕会自己关闭，按键或者移动
   烙铁屏幕会自动点亮；
6. 优化移动判定

## v2.10 更新内容(20150714)

1. 温度校准时没有刷新的bug；
2. 取消USB供电时进入温度计的功能；

## v2.09 更新内容(20150626)

1. 优化恒温算法；
2. 提高报警的最高电压。

## v2.08 更新内容(20150616)

1. 在温度计模式下，校准后还是留在温度计模式。

## v2.07 更新内容(20150605)

1. 修复温度计返回待机按键BUG；
2. U盘文件兼容前面固件生成的TXT文件；
3. 休眠状态下，烙铁指示反色显示；
4. U盘放入文件名为LOGOIN.BMP的文件，将在烙铁开机中显示出来(LOGOIN.BMP 为单色，
   尺寸96*16)；
5. USB供电也也能进入温度计模式。

## v2.06 更新内容(20150528) 
1. 优化U盘读写，现在可放入其他文件，且不会删除(存在config文件时)并且U盘大小为
   实际大小(8K)； 
2. 优化温度计功能(计算、显示、状态转换)； 
3. 在温度计状态下，长按任意键返回待机状态，同时长按双键为温度校准(需要在常温下
   校准)，校准成功和失败有相应提示，校准完成后，返回待机状态； 
4. 烙铁头故障(sen-err)、超温故障(waring)都会区分开； 
5. U盘文件最后一项为校准AD值，不可更改； 

## v2.05 更新内容(20150521) 

1. 硬件(2.44)到硬件(2.45)相应固件变更

## v2.04 更新内容(20150515) 

1. 优化温度计、电压相关操作以及状态转换；  

## v2.03 更新内容(20150513)

1. 增加简易温度计功能； 

## v2.02 更新内容(20150512) 

1. 增加最低电压关断加热，返回待机界面，可通过配置文件配置； 

## v2.01 更新内容(20150512) 

1. 优化U盘文件(由于 U盘文件而导致的一系列问题)； 
2. 配置文件中加入温度设置步长参数； 
3. 开机显示版本号； 
4. 优化降温提示； 