#ifndef __SERIAL_H
#define __SERIAL_H

#include "sys.h"
#include "delay.h"
#include "string.h"

#define IO_TXD	PCout(11)
#define IO_RXD	PCin(10)
#define BuadRate_9600	100


extern u8 recvYwfg;
extern u8  prYwbuf[10];

enum{
	COM_START_BIT,
	COM_D0_BIT,
	COM_D1_BIT,
	COM_D2_BIT,
	COM_D3_BIT,
	COM_D4_BIT,
	COM_D5_BIT,
	COM_D6_BIT,
	COM_D7_BIT,
	COM_STOP_BIT,
};



void SerialConfig(void);
void TIM4_Int_Init(u16 arr,u16 psc);
void Serial_SendByte(u8 Data);
void Serial_Send(u8 *buf, u8 len);
u8 CheckYw(void);
void send_Byte(u8 Data);
void USART_Send(u8 *buf, u8 len);




#endif //SERIAL_H
