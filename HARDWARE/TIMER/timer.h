#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f10x.h"
#include "gpio.h"

#define ScanTime 5




extern u8 timeslice;
extern u8 ADC_en;
extern u32 count;		//洗涤计数值
extern u8	WaterState1;	//水位状态 0 都没水 1 高水位没水 2 低水位没水 3 都有水
extern u8	WaterState2;	//水位状态 0 都没水 1 高水位没水 2 低水位没水 3 都有水

extern u8 time3slice;
extern u8	ss1;  	//秒
extern u8	ss2;		//秒
extern u8	mm1;		//分钟计时
extern u8	mm2;		//分钟计时
extern u8	ch0En;
extern u8	ch1En;

extern u16 Para[32];		//
extern u8	OpenTime;



void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 arr,u16 psc);

#endif
