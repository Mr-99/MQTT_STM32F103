#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//ADC ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

extern u16 AdcCh0Buf[16];
extern u16 AdcCh1Buf[16];
extern u8	 AdcP;
extern u8	 TrigCnt;
extern u16 ADC_ConvertedValueLocal[6];

extern u8	ZXPT100;
extern u8	QXPT100;
extern u8  ZXWDfilte[4];
extern u8  QXWDfilte[4];
extern u8  FilteFlog;
extern u8  ZXWDADD;
extern u8  ZXWDADDFlog;	//0Ϊ+��1Ϊ-
extern u8  QXWDADD;
extern u8  QXWDADDFlog;
extern u8	ADDBuf[7];

extern u16 Zd0Buf[16];
extern u16 Zd1Buf[16];
extern u8	ZdP;
extern u8	TrigZd;

void ADC1_Init(void);
void ADC1_ConvertedValue_ALL(void);
 
#endif 
