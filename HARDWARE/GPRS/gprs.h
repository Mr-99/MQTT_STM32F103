#ifndef __GPRS_H__
#define __GPRS_H__

#include "string.h"
#include "delay.h"
#include "usart.h"
#include "gpio.h"


//GPRS����ģʽ
#define GPRS_DEBUG_MODE	0
/********GPRS���Ӳ���*****************************************/
#define		GPRS_GET_CSQ			0
#define		GPRS_WAIT_REGNET		1
#define		GPRS_CONFIG_PARA		2
#define		GPRS_CONFIG_SOCKET		3
#define		GPRS_DATA_RW			4

/********GPRS�źŲ��ȶ�ʱ�����ִ�д���************************/
#define	 GPRS_SIMID_TIMES (10)				//��ȡSIM������������ִ�д���
#define	 GPRS_IMEI_TIMES (20)				//��ȡ���к���������ִ�д���
#define   GPRS_CSQ_TIMES		(10)	     //��ѯ�ź�ǿ����������ִ�д���
#define	 GPRS_REGNET_TIMES (10)	 //ע��������������ִ�д���
#define	 GPRS_CONFIG_TIMES (10)	//ͨѶ����������������ִ�д���
#define	 GPRS_SOCKET_TIMES (5)	  //Socket������������ִ�д���

/********GPRS״̬*****************************************/
#define	 GPRS_READY			(0)		    //׼������״̬
#define   GPRS_SUCCESS		(1)		  //���ͳɹ�
#define	 GPRS_CSQ_ERR		(2)		//���ź�
#define   GPRS_REGNET_ERR	(3)		//ע������ʧ��
#define	 GPRS_CONFIG_ERR	(4)		//����ͨѶ����ʧ��
#define 	 GPRS_SOCKET_ERR	(5)		//����Socketʧ��
#define	 GPRS_SEND_ERR		(6)		//��������ʧ��
/*************************************************************/

extern u8 TypeNum1[11];
extern u8 GsmID[14];
extern u8 GsmCsq[3];
extern u8 GPRS_ERROR;
extern u8	GPRS_once;
extern u8	GPRS_init_sta;
extern u8	GPRSSendSta;
extern u8	GPRS_INIT;
extern u8	SendSTA;
extern u8	GetCSQ_EN;

extern u8  AlarmSend;

extern u16 SerNumb;

u8 GetGsmID(void);

void GPRS_init(void);
void GPRS_send(void);
void GetCSQ(void);
void ShowCSQ(u8 csqH,u8 csqL);

#endif
