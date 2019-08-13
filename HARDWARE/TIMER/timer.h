#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f10x.h"
#include "gpio.h"

#define ScanTime 5




extern u8 timeslice;
extern u8 ADC_en;
extern u32 count;		//ϴ�Ӽ���ֵ
extern u8	WaterState1;	//ˮλ״̬ 0 ��ûˮ 1 ��ˮλûˮ 2 ��ˮλûˮ 3 ����ˮ
extern u8	WaterState2;	//ˮλ״̬ 0 ��ûˮ 1 ��ˮλûˮ 2 ��ˮλûˮ 3 ����ˮ

extern u8 time3slice;
extern u8	ss1;  	//��
extern u8	ss2;		//��
extern u8	mm1;		//���Ӽ�ʱ
extern u8	mm2;		//���Ӽ�ʱ
extern u8	ch0En;
extern u8	ch1En;

extern u16 Para[32];		//
extern u8	OpenTime;



void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 arr,u16 psc);

#endif
