#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  			100  	//定义最大接收字节数 200
#define USART_SEND_LEN      100		//定义最大发送缓冲区 100
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
#define EN_USART1_TX			1   //使能（1）/禁止（0）串口1发送

	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u8  USART_TX_BUF[USART_SEND_LEN];
extern u16 USART_RX_STA;         		//接收状态标记
extern u8  USART_TX_STA;
extern u16 USART_TX_NUM;
extern u8	 USART_TX_P;


extern u8 USART2_RX_BUF[32];
extern u8 USART2_RX_DEAL[18];
extern u8 USART2_TX_BUF[64];
extern u8 USART2_TX_P;
extern u8 USART2_TX_NUM;
extern u8 USART2_RX_P;
extern u8	USART2_BUSY;
extern u8 USART2_RXED;

extern u8 USART3_RX_BUF[32];
extern u8 USART3_RX_DEAL[18];
extern u8 USART3_TX_BUF[64];
extern u8 USART3_RXED;
extern u8 USART3_RX_P;
extern u8 USART3_TX_P;
extern u8 USART3_TX_NUM;
extern u8 USART3_BUSY;

extern u8 UART4_RX_BUF[16];
extern u8 UART4_RX_DEAL[16];
extern u8 UART4_RX_P;
extern u8 UART4_RXED;
extern u8 UART4_TX_BUF[16];
extern u8 UART4_TX_P;
extern u8 UART4_TX_NUM;
extern u8 UART4_BUSY;

extern u8 UART5_RX_BUF[200];
extern u8 UART5_RX_DEAL[16];
extern u8 UART5_RX_P;
extern u8 UART5_RXED;
extern u8 UART5_TX_BUF[16];
extern u8 UART5_TX_P;
extern u8 UART5_TX_NUM;
extern u8 UART5_BUSY;

//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void USART2_Init(u32 bound);//串口2初始化
void USART3_Init(u32 bound);//串口3初始化
void UART4_Init(u32 bound);
void UART5_Init(u32 bound);
void USART1_SendString(uint8_t* p,u8 Len);

void USART2_SendString(char *s);
void USART2_SendBytes(u8 *s,u8 len);
void USART2_SendData(u8 dat);
void USART3_SendString(char *s);
void USART3_SendBytes(u8 *s,u8 len);
void USART3_SendData(u8 dat);
void UART4_SendBytes(u8 *s,u8 len);
void UART5_SendBytes(u8 *s,u8 len);
#endif


