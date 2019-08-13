#ifndef __GPIO_H
#define __GPIO_H	 
#include "sys.h"


#define RUN_LED 				PBout(9)			//���е�		out
#define	RS485_EN2				PEout(14)			//
#define	RS485_EN3				PEout(15)			//
#define FengJi					PCout(6)

#define PAIS2						PDout(12)			//2#��ˮ
#define BQFJ2						PBout(12)			//2#�������
#define HXF2						PDout(14)			//2#����
#define FSQ2						PCout(7)			//2#������
#define SSF2						PDout(10)			//2#��ˮ��
#define JR2							PBout(14)     //����
#define SB2							PDout(8)			//2#ˮ��

#define PAIS1						PDout(13)			//2#��ˮ
#define BQFJ1						PBout(13)			//2#�������
#define HXF1						PDout(15)			//2#����
#define FSQ1						PCout(8)			//2#������
#define SSF1						PDout(11)			//2#��ˮ��
#define JR1							PBout(15)      //����
#define	SB1							PDout(9)			//ˮ��

#define GSW1						PEin(10)			//��ˮλ	
#define DSW1						PEin(11)			//��ˮ	
#define GSW2						PEin(12)			//��ˮλ	
#define DSW2						PEin(13)			//��ˮ	

#define EN_12V					PDout(0)		//12Vʹ��				out
#define EN_GPRS					PCout(9)		//GPRS ��Դʹ��	out
#define START						PDout(3)		//GPRS ��ʼ�ź�
#define EN_GPS					PAout(12)		//GPS  ʹ���ź�

void IO_Init(void);//��ʼ��

		 				    
#endif //GPIO_H
