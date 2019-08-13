#include "sys.h"
#include "usart.h"	
#include "malloc.h"
#include "string.h"
#include "gpio.h"
//#include "timer.h"
//#include "led.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 
#if EN_USART1_RX   //如果使能了接收


//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART_TX_BUF[USART_SEND_LEN];		//发送缓冲

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


u16 USART_RX_STA=0;       //接收状态标记	  
u16 USART_TX_NUM=0;				//待发送数据个数
u8	USART_TX_STA=0;
u8	USART_TX_P = 0;
u8  USART_RX_P = 0;
u8  LastRcv = 0;
  
void uart_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);//开启串口接受中断
	USART_ITConfig(USART1, USART_IT_TC , ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}

/****************************************************************************************
** 函数名称: UART2_Init
** 功能描述: 串口2初始化函数
** 参      数: u32 bound 波特率
** 返  回 值: none
****************************************************************************************/
void USART2_Init(u32 bound)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;
	 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	
  //USART2_TX -> PA2 , USART2_RX -> PA3                              
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;	    //USART2 TX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);		    //A端口 

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;	    //USART2 RX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //复用开漏输入
  GPIO_Init(GPIOA, &GPIO_InitStruct);		    //A端口 

  //Usart2 NVIC 配置
  NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;		
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
  NVIC_Init(&NVIC_InitStruct);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART2
  
  //USART 初始化设置
  USART_InitStruct.USART_BaudRate = bound;//设置串口波特率
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;//设置数据位
  USART_InitStruct.USART_StopBits = USART_StopBits_1;//设置停止位
  USART_InitStruct.USART_Parity = USART_Parity_No;//设置效验位
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//设置流控制
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//设置工作模式
  USART_Init(USART2, &USART_InitStruct); //配置入结构体

  //USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE , ENABLE);//开启串口接受中断
	USART_ITConfig(USART2, USART_IT_TC , ENABLE);//开启串口接受中断
	
  USART_Cmd(USART2, ENABLE);//使能串口2
  USART_ClearITPendingBit(USART2,USART_IT_TXE);//清除中断标志
	
}

/****************************************************************************************
** 函数名称: UART3_Init
** 功能描述: 串口3初始化函数
** 参      数: u32 bound 波特率
** 返  回 值: none
****************************************************************************************/
void USART3_Init(u32 bound)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;
	 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  
  //USART3_TX -> PB10 , USART3_RX -> PB11                             
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;	         		 //USART3 TX
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;    		 //复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStruct);		    		 //A端口 

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;	         	 	 //USART3 RX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 //复用开漏输入
  GPIO_Init(GPIOB, &GPIO_InitStruct);		         	 //A端口 

  //Usart3 NVIC 配置
  NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority= 4;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 4;		
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
  NVIC_Init(&NVIC_InitStruct);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART2
  
  //USART3 初始化设置
  USART_InitStruct.USART_BaudRate = bound;//设置串口波特率
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;//设置数据位
  USART_InitStruct.USART_StopBits = USART_StopBits_1;//设置停止位
  USART_InitStruct.USART_Parity = USART_Parity_No;//设置效验位
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//设置流控制
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//设置工作模式
  USART_Init(USART3, &USART_InitStruct); //配置入结构体

  //USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART3, USART_IT_RXNE , ENABLE);//开启串口接受中断
	USART_ITConfig(USART3, USART_IT_TC , ENABLE);//开启串口接受中断
  USART_Cmd(USART3, ENABLE);//使能串口3
//  USART_ClearITPendingBit(USART3,USART_IT_TXE);//清除中断标志
}


/****************************************************************************************
** 函数名称: UART4_Init
** 功能描述: 串口4初始化函数
** 参      数: u32 bound 波特率
** 返  回 值: none
****************************************************************************************/
void UART4_Init(u32 bound)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;
	 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
  
  //USART4_TX -> PC10 , USART4_RX -> PC11                             
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;	         		 //USART4 TX
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;    		 //复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStruct);		    		 //A端口 

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;	         	 	 //USART4 RX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 //复用开漏输入
  GPIO_Init(GPIOC, &GPIO_InitStruct);		         	 //A端口 

  //Usart4 NVIC 配置
  NVIC_InitStruct.NVIC_IRQChannel = UART4_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority= 4;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;		
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
  NVIC_Init(&NVIC_InitStruct);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART2
  
  //USART4 初始化设置
  USART_InitStruct.USART_BaudRate = bound;//设置串口波特率
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;//设置数据位
  USART_InitStruct.USART_StopBits = USART_StopBits_1;//设置停止位
  USART_InitStruct.USART_Parity = USART_Parity_No;//设置效验位
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//设置流控制
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//设置工作模式
  USART_Init(UART4, &USART_InitStruct); //配置入结构体

  //USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	USART_ITConfig(UART4, USART_IT_RXNE , ENABLE);//开启串口接受中断
	USART_ITConfig(UART4, USART_IT_TC , ENABLE);//开启串口接受中断
  USART_Cmd(UART4, ENABLE);//使能串口4
//  USART_ClearITPendingBit(USART3,USART_IT_TXE);//清除中断标志
}


/****************************************************************************************
** 函数名称: UART5_Init
** 功能描述: 串口5初始化函数
** 参      数: u32 bound 波特率
** 返  回 值: none
****************************************************************************************/
void UART5_Init(u32 bound)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  NVIC_InitTypeDef NVIC_InitStruct;
	 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
  
  //USART5_TX -> PC12 , USART5_RX -> PD2                             
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;	         		 //USART5 TX
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;    		 //复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStruct);		    		 //

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;	         	 	 //USART5 RX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 //复用开漏输入
  GPIO_Init(GPIOD, &GPIO_InitStruct);		         	 //

  //Usart5 NVIC 配置
  NVIC_InitStruct.NVIC_IRQChannel = UART5_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority= 2;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;		
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
  NVIC_Init(&NVIC_InitStruct);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART2
  
  //USART5 初始化设置
  USART_InitStruct.USART_BaudRate = bound;//设置串口波特率
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;//设置数据位
  USART_InitStruct.USART_StopBits = USART_StopBits_1;//设置停止位
  USART_InitStruct.USART_Parity = USART_Parity_No;//设置效验位
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//设置流控制
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//设置工作模式
  USART_Init(UART5, &USART_InitStruct); //配置入结构体

	USART_ITConfig(UART5, USART_IT_RXNE , ENABLE);//开启串口接受中断
	//USART_ITConfig(UART5, USART_IT_TC , ENABLE);		//开启发送中断
  USART_Cmd(UART5, ENABLE);//使能串口5
//  USART_ClearITPendingBit(USART3,USART_IT_TXE);//清除中断标志
}


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		Res =USART_ReceiveData(USART1);	//读取接收到的数据

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
	
	while(USART_TX_STA);   //等待发空
	for(i=0;i<Len;i++)
		USART_TX_BUF[i] = *(p+i);
	USART_TX_NUM = Len-1;
	USART_TX_STA = 1;
	USART_TX_P= 0;
	USART_SendData(USART1, USART_TX_BUF[USART_TX_P]);
}


void USART2_IRQHandler(void)                	//串口2中断服务程序
{
	u8 Res2;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		Res2 =USART_ReceiveData(USART2);					//读取接收到的数据
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


void USART3_IRQHandler(void)                	//串口3中断服务程序
{
	u8 Res3;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		Res3 =USART_ReceiveData(USART3);	//读取接收到的数据
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


void UART5_IRQHandler(void)                	//串口5中断服务程序
{
	u8 Res5;
	
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART_ClearITPendingBit(UART5,USART_IT_RXNE);
		Res5 =USART_ReceiveData(UART5);					//读取接收到的数据
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

void UART4_IRQHandler(void)                	//串口5中断服务程序
{
	u8 Res4;
	
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);
		Res4 =USART_ReceiveData(UART4);					//读取接收到的数据
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

