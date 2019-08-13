#ifndef __GPIO_H
#define __GPIO_H	 
#include "sys.h"


#define RUN_LED 				PBout(9)			//运行灯		out
#define	RS485_EN2				PEout(14)			//
#define	RS485_EN3				PEout(15)			//
#define FengJi					PCout(6)

#define PAIS2						PDout(12)			//2#排水
#define BQFJ2						PBout(12)			//2#曝气风机
#define HXF2						PDout(14)			//2#换向阀
#define FSQ2						PCout(7)			//2#发生器
#define SSF2						PDout(10)			//2#上水阀
#define JR2							PBout(14)     //加热
#define SB2							PDout(8)			//2#水泵

#define PAIS1						PDout(13)			//2#排水
#define BQFJ1						PBout(13)			//2#曝气风机
#define HXF1						PDout(15)			//2#换向阀
#define FSQ1						PCout(8)			//2#发生器
#define SSF1						PDout(11)			//2#上水阀
#define JR1							PBout(15)      //加热
#define	SB1							PDout(9)			//水泵

#define GSW1						PEin(10)			//高水位	
#define DSW1						PEin(11)			//低水	
#define GSW2						PEin(12)			//高水位	
#define DSW2						PEin(13)			//低水	

#define EN_12V					PDout(0)		//12V使能				out
#define EN_GPRS					PCout(9)		//GPRS 电源使能	out
#define START						PDout(3)		//GPRS 开始信号
#define EN_GPS					PAout(12)		//GPS  使能信号

void IO_Init(void);//初始化

		 				    
#endif //GPIO_H
