#ifndef __GPRS_H__
#define __GPRS_H__

#include "string.h"
#include "delay.h"
#include "usart.h"
#include "gpio.h"


//GPRS调试模式
#define GPRS_DEBUG_MODE	0
/********GPRS连接步骤*****************************************/
#define		GPRS_GET_CSQ			0
#define		GPRS_WAIT_REGNET		1
#define		GPRS_CONFIG_PARA		2
#define		GPRS_CONFIG_SOCKET		3
#define		GPRS_DATA_RW			4

/********GPRS信号不稳定时的最大执行次数************************/
#define	 GPRS_SIMID_TIMES (10)				//获取SIM卡号允许的最大执行次数
#define	 GPRS_IMEI_TIMES (20)				//获取序列号允许的最大执行次数
#define   GPRS_CSQ_TIMES		(10)	     //查询信号强度允许的最大执行次数
#define	 GPRS_REGNET_TIMES (10)	 //注册网络允许的最大执行次数
#define	 GPRS_CONFIG_TIMES (10)	//通讯参数配置允许的最大执行次数
#define	 GPRS_SOCKET_TIMES (5)	  //Socket连接允许的最大执行次数

/********GPRS状态*****************************************/
#define	 GPRS_READY			(0)		    //准备就绪状态
#define   GPRS_SUCCESS		(1)		  //发送成功
#define	 GPRS_CSQ_ERR		(2)		//无信号
#define   GPRS_REGNET_ERR	(3)		//注册网络失败
#define	 GPRS_CONFIG_ERR	(4)		//配置通讯参数失败
#define 	 GPRS_SOCKET_ERR	(5)		//配置Socket失败
#define	 GPRS_SEND_ERR		(6)		//发送数据失败
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
