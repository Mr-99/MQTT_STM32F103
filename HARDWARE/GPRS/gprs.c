#include "gprs.h"
#include "usart.h"
#include "timer.h"
#include "24cxx.h"


u8 GsmSend1[198] = 
  {0x00 ,0x00 ,0x00 ,0xC2 ,0x80 ,0x01 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x07 ,0x65 ,0x78 ,0x65  //14
	,0x63 ,0x75 ,0x74 ,0x65 ,0x00 ,0x00 ,0x00 ,0x01 ,0x0B ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0xA7  //29
    
	,0x7B ,0x22 ,0x6D ,0x65 ,0x74 ,0x68 ,0x6F ,0x64 ,0x22 ,0x3A ,0x22 ,0x64 ,0x65 ,0x76 ,0x69  //44
	,0x63 ,0x65 ,0x5F ,0x73 ,0x74 ,0x61 ,0x74 ,0x75 ,0x73 ,0x22 ,0x2C ,0x22 ,0x75 ,0x75 ,0x69  //59
	,0x64 ,0x22 ,0x3A ,0x22 ,0x31 ,0x33 ,0x38 ,0x32 ,0x30 ,0x31 ,0x35 ,0x30 ,0x30 ,0x30 ,0x31 	//74 
	,0x31 ,0x31 ,0x22 ,0x2C ,0x22 ,0x64 ,0x65 ,0x76 ,0x69 ,0x63 ,0x65 ,0x5F ,0x74 ,0x79 ,0x70  //89
	,0x65 ,0x22 ,0x3A ,0x22 ,0x31 ,0x22 ,0x2C ,0x22 ,0x64 ,0x65 ,0x76 ,0x69 ,0x63 ,0x65 ,0x5F //104
	,0x70 ,0x72 ,0x6F ,0x64 ,0x75 ,0x63 ,0x74 ,0x5F ,0x63 ,0x6F ,0x64 ,0x65 ,0x22 ,0x3A ,0x22 //SD700C   119
	,0x53 ,0x44 ,0x37 ,0x30 ,0x30 ,0x43 ,0x22 ,0x2C ,0x22 ,0x64 ,0x61 ,0x74 ,0x61 ,0x49 ,0x6E  //134
	,0x66 ,0x6F ,0x22 ,0x3A ,0x7B ,0x22 ,0x64 ,0x65 ,0x76 ,0x69 ,0x63 ,0x65 ,0x5F ,0x73 ,0x74 //149
	,0x61 ,0x74 ,0x75 ,0x73 ,0x22 ,0x3A ,0x7B ,0x22 ,0x68 ,0x65 ,0x61 ,0x6C ,0x74 ,0x68 ,0x5F //164
	,0x63 ,0x6F ,0x64 ,0x65 ,0x22 ,0x3A ,0x22 ,0x30 ,0x22 ,0x2C ,0x73 ,0x65 ,0x72 ,0x69 ,0x61 //179
	,0x6C ,0x5F ,0x6E ,0x75 ,0x6D ,0x62 ,0x65 ,0x72 ,0x3A ,0x22 ,0x30 ,0x30 ,0x30 ,0x22 ,0x7D  //194
	,0x7D ,0x7D ,0x00};							//177

u8 GsmSend2[203] = 
	{0x00 ,0x00 ,0x00 ,0xC7 ,0x80 ,0x01 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x07 ,0x65 ,0x78 ,0x65 	//14
	,0x63 ,0x75 ,0x74 ,0x65 ,0x00 ,0x00 ,0x00 ,0x01 ,0x0B ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0xAC 
    
	,0x7B ,0x22 ,0x6D ,0x65 ,0x74 ,0x68 ,0x6F ,0x64 ,0x22 ,0x3A ,0x22 ,0x75 ,0x73 ,0x65 ,0x5F 
	,0x63 ,0x6F ,0x75 ,0x6E ,0x74 ,0x5F ,0x64 ,0x69 ,0x73 ,0x68 ,0x22 ,0x2C ,0x22 ,0x75 ,0x75 
	,0x69 ,0x64 ,0x22 ,0x3A ,0x22 ,0x31 ,0x33 ,0x38 ,0x32 ,0x30 ,0x31 ,0x35 ,0x30 ,0x30 ,0x30
	,0x31 ,0x31 ,0x31 ,0x22 ,0x2C ,0x22 ,0x64 ,0x65 ,0x76 ,0x69 ,0x63 ,0x65 ,0x5F ,0x74 ,0x79 
	,0x70 ,0x65 ,0x22 ,0x3A ,0x22 ,0x31 ,0x22 ,0x2C ,0x22 ,0x64 ,0x65 ,0x76 ,0x69 ,0x63 ,0x65 
	,0x5F ,0x70 ,0x72 ,0x6F ,0x64 ,0x75 ,0x63 ,0x74 ,0x5F ,0x63 ,0x6F ,0x64 ,0x65 ,0x22 ,0x3A 
	,0x22 ,0x53 ,0x44 ,0x37 ,0x30 ,0x30 ,0x43 ,0x22 ,0x2C ,0x22 ,0x64 ,0x61 ,0x74 ,0x61 ,0x49 
	,0x6E ,0x66 ,0x6F ,0x22 ,0x3A ,0x7B ,0x22 ,0x75 ,0x73 ,0x65 ,0x5F ,0x63 ,0x6F ,0x75 ,0x6E 
	,0x74 ,0x5F ,0x64 ,0x69 ,0x73 ,0x68 ,0x22 ,0x3A ,0x7B ,0x22 ,0x75 ,0x73 ,0x65 ,0x5F ,0x63 
	,0x6F ,0x75 ,0x6E ,0x74 ,0x22 ,0x3A ,0x22 ,0x30 ,0x30 ,0x30 ,0x30 ,0x30 ,0x30 ,0x22 ,0x2C 
	,0x73 ,0x65 ,0x72 ,0x69 ,0x61 ,0x6C ,0x5F ,0x6E ,0x75 ,0x6D ,0x62 ,0x65 ,0x72 ,0x3A ,0x22  //194
	,0x30 ,0x30 ,0x30 ,0x22 ,0x7D ,0x7D ,0x7D ,0x00 };
	
	
const u8 eofs[4] = {0x0D,0x0A,0x0D,0x0A};
u8 GsmID[14] = {0};
u8 GsmCsq[3] = {0};
u8 SerNumBuf[4];

u8 	GPRS_ERROR=0;
u8	GPRS_once=1;
u8	GPRS_init_sta=0;
u8	GPRSSendSta = 0;
u8	GPRS_INIT=0;
u8	SendSTA = 1;
u8	GetCSQ_EN = 0;

u8  AlarmSend = 0;

u16 SerNumb = 0;

extern u8 asc_ShowCnt[7];



void GPRS_init(void)
{
	if(GPRS_INIT == 0)
	{
		//先发"ATE0\r\n"				关回显
		if(GPRS_init_sta == 0)
		{
			if(GPRS_once)
			{
				USART2_SendString("ATE0\r\n");
				GPRS_once = 0;
				SecondDelay = 40;
			}
			if(USART2_RXED == 1)			//接收到回车符
			{
				GPRS_once = 1;					//清空发送标识
				USART2_RXED = 0;				//清除接收完成标识
				if (strstr((char*)USART2_RX_DEAL, "OK")) //判断有无OK字符
				{
					GPRS_init_sta = 1;		//进行下一步操作
					SecondDelay = 0;			//延时结束
				}
				memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
			}
			else if(SecondDelay == 0)	//超时还未接收到回车符
			{
				GPRS_once = 1;					//清空发送标识 重新发送
				GPRS_ERROR = 1;					//错误信息
			}
		}
		else if(GPRS_init_sta == 1)
		{
			if(GPRS_once)
			{
				USART2_SendString("AT+CNUM\r\n");		//再发"AT+CNUM\r\n"			获取卡号
				GPRS_once = 0;
				SecondDelay = 40;
			}
			if(USART2_RXED == 1)			//接收到回车符
			{
			//	GPRS_once = 1;					//清空发送标识
				USART2_RXED = 0;				//清除接收完成标识
				if (strstr((char*)USART2_RX_DEAL, "+CNUM:")) //判断有无OK字符
				{
					GPRS_init_sta = 1;		//进行下一步操作
					SecondDelay = 0;			//延时结束
					GPRS_once = 1;
					if(GetGsmID() == 0)
					{
						GPRS_INIT = 1;
						GPRS_ERROR = 0;
					}
				}
				memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
			}
			else if(SecondDelay == 0)	//超时还未接收到回车符
			{
				GPRS_once = 1;					//清空发送标识 重新发送
				GPRS_ERROR = 2;					//错误信息
			}
		}
	}
	
}

void GetCSQ(void)
{
	if(GetCSQ_EN == 1)
	{
		if(GPRS_once)
		{
			USART2_SendString("AT+CSQ\r\n");
			GPRS_once = 0;
			SecondDelay = 40;
		}
		if(USART2_RXED == 1)			//接收到回车符
		{
		//	GPRS_once = 1;					//清空发送标识
			USART2_RXED = 0;				//清除接收完成标识
			if (strstr((char*)USART2_RX_DEAL, "+CSQ:")) //判断有无OK字符
			{
				//p = (u8*)strstr((char*)USART2_RX_DEAL, "+CSQ:");
				ShowCSQ(USART2_RX_DEAL[8],USART2_RX_DEAL[9]);
				GetCSQ_EN = 0;			//进行下一步操作
				SecondDelay = 0;			//延时结束
				GPRS_once = 1;
			}
			memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
		}
		else if(SecondDelay == 0)	//超时还未接收到回车符
		{
			GPRS_once = 1;					//清空发送标识 重新发送
			GetCSQ_EN = 0;
			GPRS_ERROR = 3;					//错误信息
		}
	}
}


void GPRS_send(void)
{
//	u8 i;
	if(GPRSSendCnt == 0)
	{
		
//		switch(GPRSSendSta)
//		{
//			case 0:					//1、获取信号强度	"AT+CSQ\r\n"
		if(GPRSSendSta == 0)
		{
				if(GPRS_once)
				{
					USART2_SendString("AT+CSQ\r\n");
					GPRS_once = 0;
					SecondDelay = 40;
				}
				if(USART2_RXED == 1)			//接收到回车符
				{
			//		GPRS_once = 1;					//清空发送标识
					USART2_RXED = 0;				//清除接收完成标识
					if (strstr((char*)USART2_RX_DEAL, "+CSQ:")) //判断有无OK字符
					{
						//p = (u8*)strstr((char*)USART2_RX_DEAL, "+CSQ:");
						ShowCSQ(USART2_RX_DEAL[8],USART2_RX_DEAL[9]);
						GPRSSendSta = 1;			//进行下一步操作
						SecondDelay = 0;			//延时结束
						GPRS_once = 1;
					}
					memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
				}
				else if(SecondDelay == 0)	//超时还未接收到回车符
				{
					GPRS_once = 1;					//清空发送标识 重新发送
					GPRSSendSta = 1;
					
					GPRS_ERROR = 3;					//错误信息
				}
			}
			
	//		case 1:							//2、"AT+CGREG?\r\n"   注册
			else if(GPRSSendSta == 1)
			{
				if(GPRS_once)
				{
					USART2_SendString("AT+CGREG?\r\n");
					GPRS_once = 0;
					SecondDelay = 40;
				}
				if(USART2_RXED == 1)			//接收到回车符
				{
					//GPRS_once = 1;					//清空发送标识
					USART2_RXED = 0;				//清除接收完成标识
					if (strstr((char*)USART2_RX_DEAL, "+CGREG:")) //判断有无OK字符
					{
						//p = (u8*)strstr((char*)USART2_RX_DEAL, "+CSQ:");
						//ShowCSQ(USART2_RX_DEAL[6],USART2_RX_DEAL[7]);
						GPRSSendSta = 2;		//进行下一步操作
						SecondDelay = 0;			//延时结束
						GPRS_once = 1;
						GPRS_ERROR &= 0xFE;
					}
					memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
				}
				else if(SecondDelay == 0)	//超时还未接收到回车符
				{
					GPRS_once = 1;					//清空发送标识 重新发送
					GPRSSendSta = 2;
					
					GPRS_ERROR |= 0x01;						//错误信息
				}
			}//	break;
			
		//	case 2:					//3、"AT+CGATT?\r\n"		配置参数
			else if(GPRSSendSta == 2)
			{
				if(GPRS_once)
				{
					USART2_SendString("AT+CGATT?\r\n");
					GPRS_once = 0;
					SecondDelay = 40;
				}
//				if(USART2_RXED == 1)			//接收到回车符
//				{
//					USART2_RXED = 0;				//清除接收完成标识
//					if (strstr((char*)USART2_RX_DEAL, "OK")) //判断有无OK字符
//					{
//						GPRSSendSta = 3;		//进行下一步操作
//						SecondDelay = 0;			//延时结束
//						GPRS_once = 1;
//					}
//					memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
//				}
/*				else*/ if(SecondDelay == 0)	//超时还未接收到回车符
				{
					GPRS_once = 1;					//清空发送标识 重新发送
					GPRSSendSta = 3;
					memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
					//GPRS_ERROR = 5;					//错误信息
				}
			}//	break;
			
		//	case 3:					//4、"AT+CIPSTART=\"TCP\",\"182.92.66.42\",9999\r\n"		发网络
			else if(GPRSSendSta == 3)
			{
				if(GPRS_once)
				{
					USART2_SendString("AT+CIPSTART=\"TCP\",\"182.92.66.42\",\"9999\"\r\n");
					GPRS_once = 0;
					SecondDelay = 320;
				}
				if(USART2_RXED == 1)			//接收到回车符
				{
				//	GPRS_once = 1;					//清空发送标识
					USART2_RXED = 0;				//清除接收完成标识
					if (strstr((char*)USART2_RX_DEAL, "CONNECT OK") || (strstr((char*)USART2_RX_DEAL, "ALREADY CONNECT"))) //判断有无OK字符
					{
						GPRSSendSta = 5;			//进行下一步操作
						SecondDelay = 0;			//延时结束
						GPRS_once = 1;
						GPRS_ERROR &= 0xFD;
					}
					memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
				}
				else if(SecondDelay == 0)	//超时还未接收到回车符
				{
					GPRS_once = 1;					//清空发送标识 重新发送
					GPRSSendSta = 3;
					
					GPRS_ERROR |= 0x02;					//错误信息
				}
			}//	break;
			
//			case 4:					//延时
//				if(GPRS_once)
//				{
//					GPRS_once = 0;
//					SecondDelay = 2;
//				}
//				if(SecondDelay == 0)
//				{
//					GPRS_once = 1;
//					memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
//					GPRSSendSta = 5;
//				}
//				break;
			
		//	case 5:					//5、"AT+CIPSEND\r\n"	发送
			else if(GPRSSendSta == 5)
			{
				if(GPRS_once)
				{
					SecondDelay = 160;
					USART2_SendString("AT+CIPSEND\r\n");
					GPRS_once = 0;
				}
				if(USART2_RXED == 1)			//接收到回车符
				{
				//	GPRS_once = 1;					//清空发送标识
					USART2_RXED = 0;				//清除接收完成标识
					if (strstr((char*)USART2_RX_DEAL, ">")) //判断有无>字符
					{
						GPRSSendSta = 6;			//进行下一步操作
						SecondDelay = 0;			//延时结束
						GPRS_once = 1;
						GPRS_ERROR &= 0xFB;
					}
					memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
//					RUN_LED = 0;
				}
				else if(SecondDelay == 0)	//超时还未接收到回车符
				{
					GPRS_once = 1;					//清空发送标识 重新发送
					GPRSSendSta = 6;
					GPRS_ERROR |= 0x04;				//错误信息
				}
			}//	break;
				
		//	case 6:
			else if(GPRSSendSta == 6)
			{
				if(GPRS_once)
				{
					SecondDelay = 60;
					SerNumb++;
					if(SerNumb>999)
					{
						SerNumb = 1;
					}
					SerNumBuf[0] = (SerNumb/100) + 0x30;
					SerNumBuf[1] = (SerNumb/10)%10 + 0x30;
					SerNumBuf[2] = (SerNumb%10) + 0x30;
					if(SendSTA == 0)
					{
						memcpy(USART2_TX_BUF,GsmSend1,198);
						memcpy(USART2_TX_BUF+64,GsmID,13);
						
					//	USART2_TX_BUF[119] = 0x20;
						USART2_TX_BUF[120] = 'M';   //s
						USART2_TX_BUF[121] = 'O';   //d
						USART2_TX_BUF[122] = 'D';   //-
						USART2_TX_BUF[123] = 'E';   //7
						USART2_TX_BUF[124] = ':';   //0
						USART2_TX_BUF[125] = 'D';   //0
						
						if(AlarmSend > 9)
							USART2_TX_BUF[172] = 0x37 + AlarmSend;
						else
							USART2_TX_BUF[172] = 0x30 + AlarmSend;
						USART2_TX_BUF[190] = SerNumBuf[0];
						USART2_TX_BUF[191] = SerNumBuf[1];
						USART2_TX_BUF[192] = SerNumBuf[2];
						USART2_SendBytes(USART_TX_BUF,198);					//发送状态包
						SendSTA = 1;
					}
					else
					{
						memcpy(USART2_TX_BUF,GsmSend2,203);
						memcpy(USART2_TX_BUF+65,GsmID,13);
						
					//	USART2_TX_BUF[120] = 0x20;
						USART2_TX_BUF[121] = 'M';   //s
						USART2_TX_BUF[122] = 'O';   //d
						USART2_TX_BUF[123] = 'D';   //-
						USART2_TX_BUF[124] = 'E';   //7
						USART2_TX_BUF[125] = ':';   //0
						USART2_TX_BUF[126] = 'D';   //0
						
						USART2_TX_BUF[172] = 0x30+(count/100000)%10;	 //
						USART2_TX_BUF[173] = 0x30+(count/10000)%10;   //
						USART2_TX_BUF[174] = 0x30+(count/1000)%10;   //
						USART2_TX_BUF[175] = 0x30+(count/100)%10;   //
						USART2_TX_BUF[176] = 0x30+(count/10)%10;   //
						USART2_TX_BUF[177] = 0x30+(count/1)%10;   //
				
						USART2_TX_BUF[195] = SerNumBuf[0];
						USART2_TX_BUF[196] = SerNumBuf[1];
						USART2_TX_BUF[197] = SerNumBuf[2];
						USART2_SendBytes(USART2_TX_BUF,203);
						SendSTA = 0;
						AT24CXX_Write(CntAdd2,(u8*)asc_ShowCnt,7);
					}
					AT24CXX_Write(ParaAdd1,(u8*)SerNumBuf,4);
					GPRS_once = 0;
					
				}
				if(SecondDelay == 0)
				{
					GPRSSendSta = 7;			//进行下一步操作
					memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
					GPRS_once = 1;
				}				
			}//	break;
				
			//case 7:
			else if(GPRSSendSta == 7)
			{
				if(GPRS_once)
				{
					GPRS_once = 0;
					SecondDelay = 600;
					USART_SendData(USART2, 0x1a);
				}
				if(USART2_RXED == 1)			//接收到回车符
				{
				//	GPRS_once = 1;					//清空发送标识
					USART2_RXED = 0;				//清除接收完成标识
					if (strstr((char*)USART2_RX_DEAL, "OK")) //判断有无>字符
					{
						GPRSSendSta = 8;			//进行下一步操作
						SecondDelay = 0;			//延时结束
						GPRS_once = 1;
						GPRS_ERROR &= 0xF7;
					}
					memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
				}
				else if(SecondDelay == 0)	//超时还未接收到回车符
				{
					GPRS_once = 1;					//清空发送标识 重新发送
					GPRSSendSta = 8;
					
					GPRS_ERROR |= 0x08;					//错误信息
				}
			}//	break;
				
		//	case 8:
			else if(GPRSSendSta == 8)
			{
				if(GPRS_once)
				{
					GPRS_once = 0;
					USART2_SendString("AT+CIPCLOSE\r\n");
					SecondDelay = 40;
				}
				if(USART2_RXED == 1)			//接收到回车符
				{
				//	GPRS_once = 1;					//清空发送标识
					USART2_RXED = 0;				//清除接收完成标识
					if (strstr((char*)USART2_RX_DEAL, "OK")) //判断有无>字符
					{
						GPRSSendSta = 9;			//进行下一步操作
						SecondDelay = 0;			//延时结束
						GPRS_once = 1;
						GPRS_ERROR &= 0xEF;
					}
					memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
				}
				else if(SecondDelay == 0)	//超时还未接收到回车符
				{
					GPRS_once = 1;					//清空发送标识 重新发送
					GPRSSendSta = 9;
					
					GPRS_ERROR |= 0x10;					//错误信息
				}
			}//break;
			
			//case 9:
			else if(GPRSSendSta == 9)
			{
				if(GPRS_once)
				{
					GPRS_once = 0;
					USART2_SendString("AT+CIPSHUT\r\n");
					SecondDelay = 40;
				}
				if(USART2_RXED == 1)			//接收到回车符
				{
			//		GPRS_once = 1;					//清空发送标识
					USART2_RXED = 0;				//清除接收完成标识
					if (strstr((char*)USART2_RX_DEAL, "OK")) //判断有无>字符
					{
						GPRSSendSta = 10;			//进行下一步操作
						SecondDelay = 0;			//延时结束
						GPRS_once = 1;
						GPRS_ERROR &= 0xDF;
					}
					memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
				}
				else if(SecondDelay == 0)	//超时还未接收到回车符
				{
					GPRS_once = 1;					//清空发送标识 重新发送
					GPRSSendSta = 10;
					
					GPRS_ERROR |= 0x20;					//错误信息
				}
			}//	break;
				
			//case 10:
			else if(GPRSSendSta == 10)
			{
				if(GPRS_once)
				{
					GPRS_once = 0;
					SecondDelay = 100;
				}
				if(SecondDelay == 0)	//超时还未接收到回车符
				{
					GPRS_once = 1;					//清空发送标识 重新发送
					GPRSSendSta = 0;
					GPRSSendCnt = 150;
					USART2_RXED = 0;
					memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
				}
			}//break;
	
		//}
	}
}


//获取SIM卡本机号码
u8 GetGsmID(void)
{
	char *p;
	u8 ui;
	u8 res;
	res = 0;
	p = strstr((char*)USART2_RX_DEAL, "1");
	memcpy(GsmID, p, 13);
	GsmID[13] = 0;
	for(ui=0;ui<13;ui++)
	{
		if(GsmID[ui]>0x39 || GsmID[ui]<0x30)	//校验
		{
			res = 1;
		}
	}
	if(res == 0)
	{
		VGUS_Send(GsmID,0x01,0x10,13);	//sim号
	}
	return res;
}


void ShowCSQ(u8 csqH,u8 csqL)
{
	u8 p[3];
	p[0] = 0;
	if(csqH<=0x33 && csqH>=0x30)
		p[1] = (csqH-0x30)*10;
	else
		p[1] = 0;
	if(csqL<=0x39 && csqL>=0x30)
		p[1] = p[1] + (csqL-0x30);
	VGUS_Send(p,0x01,0x00,2);	//
}
/*
向九洲做的修改如下
*/
//@建立tcp/ip连接
void tcp_ip(void)
{

}
//@连接到OneNet服务器
//@建立MQTT协议的连接
//@利用MQTT协议传输数据




