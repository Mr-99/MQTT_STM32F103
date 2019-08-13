#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//ADC 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
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
extern u8  ZXWDADDFlog;	//0为+，1为-
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
