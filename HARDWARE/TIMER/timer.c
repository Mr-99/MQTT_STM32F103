#include "timer.h"
#include "gpio.h"
#include "usart.h"
#include "gprs.h"


u8	WaterState1 = 0;	//水位状态 0 都没水 1 高水位没水 2 低水位没水 3 都有水
u8	WaterState2 = 0;	//水位状态 0 都没水 1 高水位没水 2 低水位没水 3 都有水

u16	Para[32];						//参数:  0:轻柔次数low_count，1:标准次数middle_count 2:强力hight_count
												//			 3:节水save_water		   4:小保养min_care   5:大保养max_care		
												//       6:累计使用时间sum_time         7:累计使用天数sum_day   8:秒second

u8 	timeslice =20;
u8 	ADC_en=0;
u8 	AlarmCnt=0;

u8	time3slice = 10;
u8	ss1 = 60;  	//秒
u8	ss2 = 60;		//秒
u8	mm1 = 0;		//分钟计时
u8	mm2 = 0;		//分钟计时
u8	ch0En = 0;
u8	ch1En = 0;
//u16	hh = 0;		//小时计时
u8	time2slice = 19;
u8	OpenTime = 0;


void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitS;
	NVIC_InitTypeDef NVIC_InitTim;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_TimeBaseInitS.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitS.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitS.TIM_Period=arr-1;
	TIM_TimeBaseInitS.TIM_Prescaler=psc-1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitS);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	NVIC_InitTim.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitTim.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitTim.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitTim.NVIC_IRQChannelSubPriority=2;
	NVIC_Init(&NVIC_InitTim);
	
	TIM_Cmd(TIM3,ENABLE);
}

void TIM2_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitS;
	NVIC_InitTypeDef NVIC_InitTim;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	TIM_TimeBaseInitS.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitS.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitS.TIM_Period=arr-1;
	TIM_TimeBaseInitS.TIM_Prescaler=psc-1;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitS);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	NVIC_InitTim.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitTim.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitTim.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitTim.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitTim);
	
	TIM_Cmd(TIM2,ENABLE);
}

//void TIM4_Int_Init(u16 arr,u16 psc)
//{
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitS;
//	NVIC_InitTypeDef NVIC_InitTim;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
//	
//	TIM_TimeBaseInitS.TIM_ClockDivision=TIM_CKD_DIV1;
//	TIM_TimeBaseInitS.TIM_CounterMode=TIM_CounterMode_Up;
//	TIM_TimeBaseInitS.TIM_Period=arr-1;
//	TIM_TimeBaseInitS.TIM_Prescaler=psc-1;
//	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitS);
//	
//	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
//	
//	NVIC_InitTim.NVIC_IRQChannel=TIM4_IRQn;
//	NVIC_InitTim.NVIC_IRQChannelCmd=ENABLE;
//	NVIC_InitTim.NVIC_IRQChannelPreemptionPriority=0;
//	NVIC_InitTim.NVIC_IRQChannelSubPriority=2;
//	NVIC_Init(&NVIC_InitTim);
//	
//	TIM_Cmd(TIM4,DISABLE);
//}



void TIM3_IRQHandler(void)
{

	if(TIM_GetFlagStatus(TIM3,TIM_FLAG_Update))
	{
		TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
		if(time3slice != 0)
		{
			time3slice--;
		}
		else
		{
			time3slice = 10;
			if(ch0En)
			{
				if(ss1 != 0)
				{
					ss1--;
				}
				else
				{
					ss1 = 60;
					if(Para[7] >=  60)
					{
						Para[7] = 0;
						Para[8]++;
					}
					else
						Para[7]++;
				}
			}
			
			if(ch1En)
			{
				if(ss2 != 0)
				{
					ss2--;
				}
				else
				{
					ss2 = 60;
					if(Para[23] >=  60)
					{
						Para[23] = 0;
						Para[24]++;
					}
					else
						Para[23]++;
				}
			}
		}
	}
	
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM2,TIM_FLAG_Update))
	{
		ADC_en = 1;
		if(time2slice != 0)
		{
			time2slice--;
		}
		else
		{
			time2slice = 19;
			if(OpenTime != 0)
				OpenTime--;
		}
		TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);
	}
}



