1.Enter DS213 DFU mode.
2.Copy FPGA_CFG.ADR to DFU disk, Then the DFU disk will restart.
3.Copy STMU_213_018.bin to DFU disk, Then the DFU disk will restart.
4.Copy DS213AppV1.26.hex to DFU disk.
5.Restart DS213.

*All files need to be copied one by one.

APP V1.24: ����ADC�ͺ�֧��

APP V1.26: ����DFU�е�ADC�ִ��Զ�������Ӧ�Ĳ�����,��������û��
           ʹ��U�������ò��������⡣
           ������ͨ��A��ͨ��B̽ͷ˥��10��ʱ�ĵ�λ
           �����˲˵���SaveBuf��SaveCSVѡ��ż�����������⡣

APP V1.24: Added ADC model support

APP V1.26: Automatically call the corresponding parameter table according to the ADC string in DFU
           The problem of using U disk to set parameters.
           Increase the gear when the channel A and channel B probe attenuation is 10 times
           Fixed an issue where the SaveBuf and SaveCSV options in the menu would occasionally freeze.