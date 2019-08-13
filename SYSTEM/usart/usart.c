#include "sys.h"
#include "usart.h"	
#include "malloc.h"
#include "string.h"
#include "gpio.h"
//#include "timer.h"
//#include "led.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
#if EN_USART1_RX   //���ʹ���˽���


//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART_TX_BUF[USART_SEND_LEN];		//���ͻ���

u8 USART2_RX_BUF[32];
u8 USART2_RX_DEAL[18];
u8 USART2_RX_P=0;
u8 USART2_RXED=0;
u8 USART2_TX_BUF[64];
u8 USART2_TX_P=0;
u8 USART2_TX_NUM=0;
u8 USART2_BUSY=0;

u8 USART3_RX_BUF[32];
u8 USART3_RX_DEAL[18];
u8 USART3_TX_BUF[64];
u8 USART3_RXED=0;
u8 USART3_RX_P=0;
u8 USART3_TX_P=0;
u8 USART3_TX_NUM=0;
u8 USART3_BUSY=0;

u8 UART4_RX_BUF[16];
u8 UART4_RX_DEAL[16];
u8 UART4_RX_P=0;
u8 UART4_RXED=0;
u8 UART4_TX_BUF[16];
u8 UART4_TX_P=0;
u8 UART4_TX_NUM=0;
u8 UART4_BUSY=0;

u8 UART5_RX_BUF[200];
u8 UART5_RX_DEAL[16];
u8 UART5_RX_P=0;
u8 UART5_RXED=0;
u8 UART5_TX_BUF[16];
u8 UART5_TX_P=0;
u8 UART5_TX_NUM=0;
u8 UART5_BUSY=0;


u16 USART_RX_STA=0;       //����״̬���	  
u16 USART_TX_NUM=0;				//���������ݸ���
u8	USART_TX_STA=0;
u8	USART_TX_P = 0;
u8  USART_RX_P = 0;
u8  LastRcv = 0;
  
void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);//�������ڽ����ж�
	USART_ITConfig(USART1, USART_IT_TC , ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

/****************************************************************************************
** ��������: UART2_Init
** ��������: ����2��ʼ������
** ��      ��: u32 bound ������
** ��  �� ֵ: none
****************************************************************************************/
void USART2_Init(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;
	 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	
  //USART2_TX -> PA2 , USART2_RX -> PA3                              
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;	    //USART2 TX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);		    //A�˿� 

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;	    //USART2 RX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //���ÿ�©����
  GPIO_Init(GPIOA, &GPIO_InitStruct);		    //A�˿� 

  //Usart2 NVIC ����
  NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;		
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStruct);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART2
  
  //USART ��ʼ������
  USART_InitStruct.USART_BaudRate = bound;//���ô��ڲ�����
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;//��������λ
  USART_InitStruct.USART_StopBits = USART_StopBits_1;//����ֹͣλ
  USART_InitStruct.USART_Parity = USART_Parity_No;//����Ч��λ
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//����������
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ù���ģʽ
  USART_Init(USART2, &USART_InitStruct); //������ṹ��

  //USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE , ENABLE);//�������ڽ����ж�
	USART_ITConfig(USART2, USART_IT_TC , ENABLE);//�������ڽ����ж�
	
  USART_Cmd(USART2, ENABLE);//ʹ�ܴ���2
  USART_ClearITPendingBit(USART2,USART_IT_TXE);//����жϱ�־
	
}

/****************************************************************************************
** ��������: UART3_Init
** ��������: ����3��ʼ������
** ��      ��: u32 bound ������
** ��  �� ֵ: none
****************************************************************************************/
void USART3_Init(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;
	 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  
  //USART3_TX -> PB10 , USART3_RX -> PB11                             
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;	         		 //USART3 TX
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;    		 //�����������
  GPIO_Init(GPIOB, &GPIO_InitStruct);		    		 //A�˿� 

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;	         	 	 //USART3 RX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 //���ÿ�©����
  GPIO_Init(GPIOB, &GPIO_InitStruct);		         	 //A�˿� 

  //Usart3 NVIC ����
  NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority= 4;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 4;		
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStruct);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART2
  
  //USART3 ��ʼ������
  USART_InitStruct.USART_BaudRate = bound;//���ô��ڲ�����
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;//��������λ
  USART_InitStruct.USART_StopBits = USART_StopBits_1;//����ֹͣλ
  USART_InitStruct.USART_Parity = USART_Parity_No;//����Ч��λ
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//����������
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ù���ģʽ
  USART_Init(USART3, &USART_InitStruct); //������ṹ��

  //USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART3, USART_IT_RXNE , ENABLE);//�������ڽ����ж�
	USART_ITConfig(USART3, USART_IT_TC , ENABLE);//�������ڽ����ж�
  USART_Cmd(USART3, ENABLE);//ʹ�ܴ���3
//  USART_ClearITPendingBit(USART3,USART_IT_TXE);//����жϱ�־
}


/****************************************************************************************
** ��������: UART4_Init
** ��������: ����4��ʼ������
** ��      ��: u32 bound ������
** ��  �� ֵ: none
****************************************************************************************/
void UART4_Init(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;
	 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
  
  //USART4_TX -> PC10 , USART4_RX -> PC11                             
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;	         		 //USART4 TX
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;    		 //�����������
  GPIO_Init(GPIOC, &GPIO_InitStruct);		    		 //A�˿� 

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;	         	 	 //USART4 RX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 //���ÿ�©����
  GPIO_Init(GPIOC, &GPIO_InitStruct);		         	 //A�˿� 

  //Usart4 NVIC ����
  NVIC_InitStruct.NVIC_IRQChannel = UART4_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority= 4;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;		
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStruct);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART2
  
  //USART4 ��ʼ������
  USART_InitStruct.USART_BaudRate = bound;//���ô��ڲ�����
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;//��������λ
  USART_InitStruct.USART_StopBits = USART_StopBits_1;//����ֹͣλ
  USART_InitStruct.USART_Parity = USART_Parity_No;//����Ч��λ
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//����������
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ù���ģʽ
  USART_Init(UART4, &USART_InitStruct); //������ṹ��

  //USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	USART_ITConfig(UART4, USART_IT_RXNE , ENABLE);//�������ڽ����ж�
	USART_ITConfig(UART4, USART_IT_TC , ENABLE);//�������ڽ����ж�
  USART_Cmd(UART4, ENABLE);//ʹ�ܴ���4
//  USART_ClearITPendingBit(USART3,USART_IT_TXE);//����жϱ�־
}


/****************************************************************************************
** ��������: UART5_Init
** ��������: ����5��ʼ������
** ��      ��: u32 bound ������
** ��  �� ֵ: none
****************************************************************************************/
void UART5_Init(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;
	 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
  
  //USART5_TX -> PC12 , USART5_RX -> PD2                             
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;	         		 //USART5 TX
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;    		 //�����������
  GPIO_Init(GPIOC, &GPIO_InitStruct);		    		 //

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;	         	 	 //USART5 RX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 //���ÿ�©����
  GPIO_Init(GPIOD, &GPIO_InitStruct);		         	 //

  //Usart5 NVIC ����
  NVIC_InitStruct.NVIC_IRQChannel = UART5_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority= 2;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;		
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStruct);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART2
  
  //USART5 ��ʼ������
  USART_InitStruct.USART_BaudRate = bound;//���ô��ڲ�����
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;//��������λ
  USART_InitStruct.USART_StopBits = USART_StopBits_1;//����ֹͣλ
  USART_InitStruct.USART_Parity = USART_Parity_No;//����Ч��λ
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//����������
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ù���ģʽ
  USART_Init(UART5, &USART_InitStruct); //������ṹ��

	USART_ITConfig(UART5, USART_IT_RXNE , ENABLE);//�������ڽ����ж�
	//USART_ITConfig(UART5, USART_IT_TC , ENABLE);		//���������ж�
  USART_Cmd(UART5, ENABLE);//ʹ�ܴ���5
//  USART_ClearITPendingBit(USART3,USART_IT_TXE);//����жϱ�־
}


void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������

		if((Res == 0xA5) && (LastRcv == 0x5A))
		{
			USART_RX_P = 0;
//			CGJ = 0;
		}
		else
		{
			USART_RX_BUF[USART_RX_P] = Res;
			USART_RX_P++;
			if(USART_RX_P > USART_RX_BUF[0])
			{
				USART_RX_P=0;
				USART_RX_STA = 1;
			}
		}
		LastRcv = Res;

  } 
	
	if(USART_GetITStatus(USART1, USART_IT_TC))
	{
		
		if(USART_TX_NUM != 0)
		{
			USART_TX_P++;
		//	USART_TX_STA = 1;
			USART_TX_NUM--;
			USART_SendData(USART1,USART_TX_BUF[USART_TX_P]);
		}
		else
		{
			USART_TX_STA = 0;
		}
		USART_ClearITPendingBit(USART1,USART_IT_TC);
	}
	
} 

void USART1_SendString(uint8_t* p,u8 Len)
{
	u8 i;
	
	while(USART_TX_STA);   //�ȴ�����
	for(i=0;i<Len;i++)
		USART_TX_BUF[i] = *(p+i);
	USART_TX_NUM = Len-1;
	USART_TX_STA = 1;
	USART_TX_P= 0;
	USART_SendData(USART1, USART_TX_BUF[USART_TX_P]);
}


void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 Res2;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		Res2 =USART_ReceiveData(USART2);					//��ȡ���յ�������
		if(0x0A == Res2 && USART2_RX_P >= 4)
		{
			USART2_RXED = 1;
			memcpy(USART2_RX_DEAL,USART2_RX_BUF,USART2_RX_P);
			USART2_RX_P = 0;
		}
		else
		{
			USART2_RX_BUF[USART2_RX_P] = Res2;
			USART2_RX_P++;
			if(USART2_RX_P >= 64)
				USART2_RX_P = 0;
		}
  } 
	
	if(USART_GetITStatus(USART2, USART_IT_TC))
	{
		if(USART2_TX_NUM != 0)
		{
			USART2_TX_P++;
			USART2_TX_NUM--;
			USART_SendData(USART2, USART2_TX_BUF[USART2_TX_P]);
		}
		else
		{
			USART2_BUSY = 0;
			RS485_EN2 = 0;
		}	
		USART_ClearITPendingBit(USART2,USART_IT_TC);
	}
	
} 


void USART3_IRQHandler(void)                	//����3�жϷ������
{
	u8 Res3;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		Res3 =USART_ReceiveData(USART3);	//��ȡ���յ�������
		if(0x0A == Res3 && USART3_RX_P >= 4)
		{
			USART3_RXED = 1;
			memcpy(USART3_RX_DEAL,USART3_RX_BUF,USART3_RX_P);
			USART3_RX_P = 0;
		}
		else
		{
			USART3_RX_BUF[USART3_RX_P] = Res3;
			USART3_RX_P++;
			if(USART3_RX_P >= 32)
			USART3_RX_P = 0;
		}
  } 
	
	if(USART_GetITStatus(USART3, USART_IT_TC))
	{
		if(USART3_TX_NUM != 0)
		{
			USART3_TX_P++;
			USART3_TX_NUM--;
			USART_SendData(USART3, USART3_TX_BUF[USART3_TX_P]);
		}
		else
		{
			USART3_BUSY = 0;
			RS485_EN3 = 0;
		}	
		USART_ClearITPendingBit(USART3,USART_IT_TC);
	}
	
} 


void UART5_IRQHandler(void)                	//����5�жϷ������
{
	u8 Res5;
	
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		USART_ClearITPendingBit(UART5,USART_IT_RXNE);
		Res5 =USART_ReceiveData(UART5);					//��ȡ���յ�������
			UART5_RX_BUF[UART5_RX_P] = Res5;
			UART5_RX_P++;
	
			if(UART5_RX_P >= 200)
			{
				UART5_RX_P = 0;
			}
  } 
	
	if(USART_GetITStatus(UART5, USART_IT_TC))
	{
		if(UART5_TX_NUM != 0)
		{
			UART5_TX_P++;
			UART5_TX_NUM--;
		//	USART_SendData(UART5, UART5_TX_BUF[UART5_TX_P]);
		}
		else
		{
			UART5_BUSY = 0;
		}	
		USART_ClearITPendingBit(UART5,USART_IT_TC);
	}
	
}

void UART4_IRQHandler(void)                	//����5�жϷ������
{
	u8 Res4;
	
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);
		Res4 =USART_ReceiveData(UART4);					//��ȡ���յ�������
		if(0x0A == Res4)
		{
			UART4_RXED = 1;
			memcpy(UART4_RX_DEAL,UART4_RX_BUF,UART4_RX_P);
			UART4_RX_P = 0;
		}
		else
		{
			UART4_RX_BUF[UART4_RX_P] = Res4;
			UART4_RX_P++;
			if(UART4_RX_P >= 32)
			UART4_RX_P = 0;
		}
  } 
	
	if(USART_GetITStatus(UART4, USART_IT_TC))
	{
		if(UART4_TX_NUM != 0)
		{
			UART4_TX_P++;
			UART4_TX_NUM--;
			USART_SendData(UART4, UART4_TX_BUF[UART4_TX_P]);
		}
		else
		{
			UART4_BUSY = 0;
		}	
		USART_ClearITPendingBit(UART4,USART_IT_TC);
	}
	
}

void UART4_SendBytes(u8 *s,u8 len)
{
	while (UART4_BUSY);
	memcpy(UART4_TX_BUF,s,len);
	UART4_TX_NUM = len-1;
	UART4_TX_P = 0;
	USART_SendData(UART4, UART4_TX_BUF[0]);
	UART4_BUSY = 1;
}


void UART5_SendBytes(u8 *s,u8 len)
{
	while (UART5_BUSY); 
	memcpy(UART5_TX_BUF,s,len);
	UART5_TX_NUM = len-1;
	UART5_TX_P = 0;
	USART_SendData(UART5, UART5_TX_BUF[0]);
	UART5_BUSY = 1;
}

void USART2_SendString(char *s)
{
	u8 LenStr=0;
	LenStr = strlen(s);
	while (USART2_BUSY); 
	memcpy(USART2_TX_BUF,s,LenStr);
  USART2_TX_NUM = LenStr-1;
  USART2_TX_P = 0;
	RS485_EN2 = 1;
	USART_SendData(USART2, USART2_TX_BUF[0]);
	USART2_BUSY = 1;
}

void USART2_SendBytes(u8 *s,u8 len)
{
	while (USART2_BUSY); 
	memcpy(USART2_TX_BUF,s,len);
	USART2_TX_NUM = len-1;
	USART2_TX_P = 0;
	RS485_EN2 = 1;
	USART_SendData(USART2, USART2_TX_BUF[0]);
	USART2_BUSY = 1;
}

void USART2_SendData(u8 dat)
{
  while (USART2_BUSY);   					// 
	RS485_EN2 = 1;
	USART_SendData(USART2,dat);   	//
  USART2_BUSY = 1;
}

void USART3_SendString(char *s)
{
	u8 LenStr=0;
	LenStr = strlen(s);
	while (USART3_BUSY); 
	memcpy(USART3_TX_BUF,s,LenStr);
  USART3_TX_NUM = LenStr-1;
  USART3_TX_P = 0;
	RS485_EN3 = 1;
	USART_SendData(USART3, USART3_TX_BUF[0]);
	USART3_BUSY = 1;
}

void USART3_SendBytes(u8 *s,u8 len)
{
	while (USART3_BUSY); 
	memcpy(USART3_TX_BUF,s,len);
	USART3_TX_NUM = len-1;
	USART3_TX_P = 0;
	RS485_EN3 = 1;
	USART_SendData(USART3, USART3_TX_BUF[0]);
	USART3_BUSY = 1;
}

void USART3_SendData(u8 dat)
{
  while (USART3_BUSY);   					// 
	RS485_EN3 = 1;	
	USART_SendData(USART3,dat);   	//
  USART3_BUSY = 1;
}

#endif	

