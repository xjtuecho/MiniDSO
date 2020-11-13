/********************* (C) COPYRIGHT 2017 e-Design Co.,Ltd. ********************
  DS213_APP Fpga.h                                               Author : bure
*******************************************************************************/
#ifndef __FPGADRV_H
#define __FPGADRV_H


//=================== FPGA������ض��� ===================

  #define A_C_CH         0
  #define B_D_CH         1

  //FPGA ��������
  #define CHIP_RST       0x80  /* FPGA ��ʼ��λ */


  //FPGA ����״̬��־λ����
  #define SMPL_Din       0x01    /* �����źŶ���   */
  #define PSMPL          0x02    /* �ź�Ԥ������� */
  #define TRIGG          0x04    /* ������������   */
  #define FULL           0x08    /* ���ݻ�����д�� */
  #define EMPTY          0x10    /* ���ݻ��������� */
  #define TRIGG_ASK      0x20    /* ���ش�������   */
  #define SCRNF          0x40    /* ��һ������     */

  //�������Ʋ�����������
  #define SMPL_MODE      0x40    /* ����ģʽ����             */
  #define SMPL_PSCL      0x41    /* ��������Ԥ��Ƶ   0~255   */
  #define SMPL_TDIV      0x42    /* ������������     0~65535 */
  #define SMPL_PSMP      0x43    /* Ԥ�����������   0~4095  */
  #define SMPL_RPTR      0x45    /* ���ݶ�ָ������   0~4095  */ 
  #define SMPL_RD        0x46    /* ����������״̬�� 9 bits  */
  #define SMPL_ST        0x47    /* FPGA ״̬����    6Bits   */

  //����ģʽ SMPL_MODE ����
  #define SEPARATE       0x00    /* ADC ���в��� */
  #define INTERLACE      0x01    /* ADC ������� */

  //�������Ʋ�����������
  #define TRIG_KIND      0x20    /* ������������      0~31     1 */
  #define TRIG_VOLT      0x21    /* ������ƽ��ֵ      0~255  100 */
  #define TRIG_WDTH      0x22    /* ����������ֵ     0~4095  35 */

  //�������ݶ���ѡ����
  #define VMIN           0x30    /* ģ����Сֵ 1B */
  #define VMAX           0x31    /* ģ�����ֵ 1B */
  #define VAVG           0x32    /* ģ��ƽ��ֵ 1B */
  #define ATWL           0x34    /* ģ���źŸ�����*16      2B*1 */
  #define ATWH           0x35    /* ģ���ź�������*16      2B*1 */
  #define DTWL           0x36    /* �����źŸ�����*16      2B*1 */
  #define DTWH           0x37    /* �����ź�������*16      2B*1 */

  //�������� TRIG_KIND ��������
  #define TRIG_NONE      0x00    /* ֹͣ����               */
  #define TRIG_ANY       0x01    /* ���ⴥ��               */

  #define TRIG_AiDN      0x10    /* ģ���źŸ���Ծ����     */
  #define TRIG_AiUP      0x11    /* ģ���ź�����Ծ����     */
  #define TRIG_AiLT      0x12    /* ģ���ƽС����ֵ����   */
  #define TRIG_AiGT      0x13    /* ģ���ƽ������ֵ����   */
  #define TRIG_AnLT      0x14    /* ģ�⸺����С����ֵ���� */
  #define TRIG_AnGT      0x15    /* ģ�⸺���������ֵ���� */
  #define TRIG_ApLT      0x16    /* ģ��������С����ֵ���� */
  #define TRIG_ApGT      0x17    /* ģ�������������ֵ���� */

  #define TRIG_DiDN      0x18    /* �����źŸ���Ծ����     */
  #define TRIG_DiUP      0x19    /* �����ź�����Ծ����     */
  #define TRIG_DiLT      0x1A    /* �����źŵ͵�ƽ����     */
  #define TRIG_DiGT      0x1B    /* �����źŸߵ�ƽ����     */
  #define TRIG_DnLT      0x1C    /* ���ָ�����С����ֵ���� */
  #define TRIG_DnGT      0x1D    /* ���ָ����������ֵ���� */
  #define TRIG_DpLT      0x1E    /* ����������С����ֵ���� */
  #define TRIG_DpGT      0x1F    /* ���������������ֵ���� */

#endif
