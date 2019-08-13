#include "serial.h"
#include "gpio.h"
#include "24cxx.h" 
#include "usart.h"

u8 recvYwfg = 0;
u8  prYwbuf[10];


u8  recvYwbuf[11];
u8 ywlen = 0;


u8 recvStat = COM_STOP_BIT;
u8 recvData = 0;
u8 sbusy = 0;	//send busy  1��Ч
u8 TXDBUF=0;	//���ͼĴ���
u8 sendYwbuf[11];
u8 sendflog=0;
u8 sendLEN=0;

extern u32 count;
extern u8  ZXWDADD;
extern u8  ZXWDADDFlog;	//0Ϊ+��1Ϊ-
extern u8  QXWDADD;
extern u8  QXWDADDFlog;
extern u8	ADDBuf[7];


void send_Byte(u8 Data)
{

	sbusy = 1;
	TIM_Cmd(TIM4, ENABLE);
	IO_TXD = 0;
	TXDBUF = Data;  
}
	
void USART_Send(u8 *buf, u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		sendYwbuf[i] = buf[i];
	}
	sbusy = 1;
	TIM_Cmd(TIM4, ENABLE);
	IO_TXD = 0;
	TXDBUF = sendYwbuf[0];  
	sendLEN = len;
	sendflog = 0;
}


void SerialConfig(void)
{
	u8 i;
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 	EXTI_InitTypeDef EXTI_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB,PC�˿�ʱ�� 
	 
	 //SoftWare Serial TXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz	 
  GPIO_Init(GPIOC, &GPIO_InitStructure);	  				
  GPIO_SetBits(GPIOC,GPIO_Pin_11); 						
	 
	 
	//SoftWare Serial RXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	 

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource10);
	EXTI_InitStruct.EXTI_Line = EXTI_Line10;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling; //�½��ش����ж�
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	NVIC_InitStructure.NVIC_IRQChannel= EXTI15_10_IRQn ; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;  
	NVIC_Init(&NVIC_InitStructure);  
	
	for(i=0;i<10;i++)
	{
		recvYwbuf[i] = 0x30+i;
	}
	
}
 

void TIM4_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM4��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���			 
}



void CheckYw(void)	
{
	u8 tempbuf[10];
	u8 temp;
	u8 sendback;
	if(recvYwfg == 1)
	{
		if(recvYwbuf[0] == 'w')
		{
			RUN_LED = 0;
			count = (recvYwbuf[1]-0x30)*100000 +
							(recvYwbuf[2]-0x30)*10000	+
							(recvYwbuf[3]-0x30)*1000	+
							(recvYwbuf[4]-0x30)*100	+
							(recvYwbuf[5]-0x30)*10	+
							(recvYwbuf[6]-0x30)*1	;
			//count = 999999;
			tempbuf[0]=count>>24;
			tempbuf[1]=count>>16;
			tempbuf[2]=count>>8;
			tempbuf[3]=count;
			tempbuf[4] = tempbuf[0]|tempbuf[1]|tempbuf[2]|tempbuf[3];
			AT24CXX_Write(0,(u8*)tempbuf,5);
			VGUS_Send(tempbuf,0x01,0x0a,4);
			recvYwbuf[0] = 'b';
			recvYwbuf[7] = 0x0d;
			recvYwbuf[8] = 0x0a;
			USART_Send(recvYwbuf,9);
		}
		else if(recvYwbuf[0] == 'r' && recvYwbuf[1] == 'e')
		{
			tempbuf[0] = 'b';
			tempbuf[1] = 0x30+(count/100000)%10;
			tempbuf[2] = 0x30+(count/10000)%10;
			tempbuf[3] = 0x30+(count/1000)%10;
			tempbuf[4] = 0x30+(count/100)%10;
			tempbuf[5] = 0x30+(count/10)%10;
			tempbuf[6] = 0x30+(count/1)%10;
			tempbuf[7] = 0x0d;
			tempbuf[8] = 0x0a;
			USART_Send(tempbuf,9);
		}
		else if(recvYwbuf[0] == 's' && recvYwbuf[1] == '?')
		{
			tempbuf[0] = 's';
			tempbuf[1] = '=';
			if(ZXWDADDFlog == 0)
				tempbuf[2] = '+';
			else
				tempbuf[2] = '-';
			tempbuf[3] = 0x30+ZXWDADD/10;
			tempbuf[4] = 0x30+ZXWDADD%10;
			if(QXWDADDFlog == 0)
				tempbuf[5] = '+';
			else
				tempbuf[5] = '-';
			tempbuf[6] = 0x30+QXWDADD/10;
			tempbuf[7] = 0x30+QXWDADD%10;
			tempbuf[8] = 0x0d;
			tempbuf[9] = 0x0a;
			USART_Send(tempbuf,10);
		}
		else if(recvYwbuf[0] == 's' && recvYwbuf[1] == '=')
		{
			sendback = 0;
			if(recvYwbuf[2] == '+')
				ZXWDADDFlog = 0;
			else if(recvYwbuf[2] == '-')
				ZXWDADDFlog = 1;
			else
				sendback = 1;
			temp = (recvYwbuf[3]-0x30)*10 + (recvYwbuf[4]-0x30);
			if(sendback==0 && temp<=15)
			{
				ZXWDADD = temp;
			}
			else
			{
				sendback = 1;
			}
			if(recvYwbuf[5] == '+')
				QXWDADDFlog = 0;
			else if(recvYwbuf[5] == '-')
				QXWDADDFlog = 1;
			else
				sendback = 1;
			temp = (recvYwbuf[6]-0x30)*10 + (recvYwbuf[7]-0x30);
			if(sendback==0 && temp<=15)
			{
				QXWDADD = temp;
			}
			else
			{
				sendback = 1;
			}
			if(sendback == 0)
			{
				AT24CXX_Write(64,recvYwbuf,8);
				tempbuf[0] = 'o';
				tempbuf[1] = 'k';
				tempbuf[2] = 0x0d;
				tempbuf[3] = 0x0a;
				USART_Send(tempbuf,4);
			}
			else
			{
				tempbuf[0] = 'e';
				tempbuf[1] = 'r';
				tempbuf[2] = 'r';
				tempbuf[3] = 'o';
				tempbuf[4] = 'r';
				tempbuf[5] = 0x0d;
				tempbuf[6] = 0x0a;
				USART_Send(tempbuf,7);
			}
		}
		recvYwfg = 0;
		ywlen = 0;
		memcpy(prYwbuf, recvYwbuf, 10);
	}		
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line10) != RESET)
	{
	//	RUN_LED = 0;
		if(IO_RXD == 0) 
		{
			if(recvStat == COM_STOP_BIT)
			{
				recvStat = COM_START_BIT;
				TIM_Cmd(TIM4, ENABLE);
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line10);
	}
}

void TIM4_IRQHandler(void)
{  
	unsigned char temp;
//	unsigned char lrecv;
	static u8 bitcnt=0;
	if(TIM_GetFlagStatus(TIM4, TIM_FLAG_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);
		if(sbusy == 0)
		{
			recvStat++;
			if(recvStat == COM_STOP_BIT)
			{
				TIM_Cmd(TIM4, DISABLE);
				temp = recvData;
				recvYwbuf[ywlen++] = recvData;			
				//if((temp == 0x0D) && (lrecv == 0x0A) && (ywlen < 11))
				//if((temp == 0x0A )&& (lrecv == 0x0A))
				if(temp == 0x0A)
				{
			 // 	RUN_LED = 0;
					recvYwfg = 1;
				}
//				lrecv = temp;
				return;
			}
			if(IO_RXD)
			{
				recvData |= (1 << (recvStat - 1));
			}else
			{
				recvData &= ~(1 << (recvStat - 1));
			}
		}
		else
		{
			bitcnt++;
			if(bitcnt<=8)
			{
				if(TXDBUF&0x01)
					IO_TXD = 1;  
				else
					IO_TXD = 0;
				TXDBUF = TXDBUF>>1;
			}
			else if(bitcnt==9)		//����
			{
				IO_TXD = 1;
			}
			else
			{
				sendflog++;
				if(sendflog<sendLEN)
				{
					TXDBUF = sendYwbuf[sendflog];
					IO_TXD = 0;
					bitcnt=0;
				}
				else
				{
					bitcnt=0;
					sbusy=0;
					TIM_Cmd(TIM4, DISABLE);
				}
				
			}
			
		}
  }		
}


