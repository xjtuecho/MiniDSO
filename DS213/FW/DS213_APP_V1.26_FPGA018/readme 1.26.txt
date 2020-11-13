1.Enter DS213 DFU mode.
2.Copy FPGA_CFG.ADR to DFU disk, Then the DFU disk will restart.
3.Copy STMU_213_018.bin to DFU disk, Then the DFU disk will restart.
4.Copy DS213AppV1.26.hex to DFU disk.
5.Restart DS213.

*All files need to be copied one by one.

APP V1.24: 新增ADC型号支持

APP V1.26: 根据DFU中的ADC字串自动调用相应的参数表,修正开机没有
           使用U盘中设置参数的问题。
           增加了通道A和通道B探头衰减10倍时的档位
           修正了菜单中SaveBuf和SaveCSV选项偶尔卡死的问题。

APP V1.24: Added ADC model support

APP V1.26: Automatically call the corresponding parameter table according to the ADC string in DFU
           The problem of using U disk to set parameters.
           Increase the gear when the channel A and channel B probe attenuation is 10 times
           Fixed an issue where the SaveBuf and SaveCSV options in the menu would occasionally freeze.