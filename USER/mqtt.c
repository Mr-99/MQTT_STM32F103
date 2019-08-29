#include "mqtt.h"
#include "usart.h"
#include "delay.h"
//MQTT 部分
//协议头文件
#include "MqttKit.h"
#include "dStream.h"
#include<string.h>
#include<stdio.h>
//数据处理包含的头文件
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//定义at出错跳转
#define error (34)
//引用的外部变量
extern uint16_t at_count;
uint8_t AT_error = 0;
//发送数据的结构体定义
//需要发送的数据流
uint8_t Led1 = 21,Led2 = 22;
#define RX5_BUFF_LENG 200 
//外部变量声明
extern struct _gps_original gps1;
//END
//GPS相关
float lon;
float lat;
typedef struct
{

    char lon[16];
    char lat[16];

    _Bool flag;

} GPS_INFO;
//END
typedef unsigned char Flag_Type;
extern u16	Para[32];
GPS_INFO gps = {"116.28669146096","39.834073314787",1};
unsigned char imie[30] = "20190727S902BD0001";
unsigned char date[30] = "2019.03.08";
unsigned char device_id[30] = "BSA-S902BD";
DATA_STREAM dataStream[] = {
    {"low_count", &Para[0], TYPE_UCHAR, 1},
    {"middle_count", &Para[1], TYPE_UCHAR, 1},
		{"hight_count", &Para[2], TYPE_UCHAR, 1},
		{"save_water", &Para[3], TYPE_UCHAR, 1},
		{"min_care", &Para[4], TYPE_UCHAR, 1},
		{"max_care", &Para[5], TYPE_UCHAR, 1},
		{"sum_time", &Para[6], TYPE_UCHAR, 1},
		{"sum_day", &Para[7], TYPE_UCHAR, 1},
		{"second", &Para[8], TYPE_UCHAR, 1},
		{"NET_State", &Led1, TYPE_UCHAR, 1},
		{"IMIE", imie, TYPE_STRING, 1},
		{"DATE", date, TYPE_STRING, 1},
		{"DEVICE_ID", device_id, TYPE_STRING, 1},
		{"GPS", &gps, TYPE_GPS, 1},
};

unsigned char dataStreamCnt = sizeof(dataStream) / sizeof(dataStream[0]);
void OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt);
void get_GPS(void);
  //处理逗号
volatile unsigned char* Seek_comma(volatile unsigned char *prt,unsigned char n);//把接受数据逗号间数据分离出来
//==========================================================
void no_wait_OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt);
///////////END
//申请延时
void fill_at_count(uint16_t * count,uint16_t delay)
{
	*count = 	delay;
}
//判断延时是否到达
uint8_t check_at_count(uint16_t * count)
{
		if(0==(*count))
		{
		return 1;
		}
		else
		{
		return 0;
		}
}
//////////////////////////////////////////////////////////////////
void clear_rx_buff(void)
{
uint8_t i=0;	
		UART5_RX_P = 0;
for(i = 0;i<RX5_BUFF_LENG;i++)
{
UART5_RX_BUF[i] = 0;
}	
}
//@重定义fputc函数 
void uart5_send_string(uint8_t *p)
{  
uint8_t i=0;	
UART5_RX_P = 0;
for(i = 0;i<RX5_BUFF_LENG;i++)
{
UART5_RX_BUF[i] = 0;
}	
while(1)
{
	while((UART5->SR&0X40)==0);//循环发送,直到发送完毕   
  UART5->DR = (u8) *p; 
	p++;
	if(0==*p)
	{
	break;
	}
}	
}
//@只发送数据函数
void NET_DEVICE_SendData(unsigned char *data, unsigned int len)
{
	unsigned int i = 0;
	uint8_t *p = data;
 for(i=0;i<3;i++)
	{
	uart5_send_string("AT+CIPSEND\r\n");
	delay_ms(1000);
if(strstr((const char*)UART5_RX_BUF, (const char*)">"))
{
									for(i =0;i<len;i++)
								{
									while((UART5->SR&0X40)==0);//循环发送,直到发送完毕   
									UART5->DR = (u8) *p; 
									p++;
								}	
							while((UART5->SR&0X40)==0);//循环发送,直到发送完毕
							 delay_ms(1);
							 USART_SendData(UART5,0x1A);//确认发送
							while((UART5->SR&0X40)==0);//循环发送,直到发送完毕
							 delay_ms(1);
							 USART_SendData(UART5,0x0d);//确认发送
							while((UART5->SR&0X40)==0);//循环发送,直到发送完毕
							 delay_ms(1);
							 USART_SendData(UART5,0x0a);//确认发送
}
else
{
continue;
}
	}
	}

	//@无等待发送数据函数
void no_wait_NET_DEVICE_SendData(unsigned char *data, unsigned int len)
{
	unsigned int i = 0;
	uint8_t *p = data;
									for(i =0;i<len;i++)
								{
									while((UART5->SR&0X40)==0);//循环发送,直到发送完毕   
									UART5->DR = (u8) *p; 
									p++;
								}	
							while((UART5->SR&0X40)==0);//循环发送,直到发送完毕
							 delay_ms(1);
							 USART_SendData(UART5,0x1A);//确认发送
							while((UART5->SR&0X40)==0);//循环发送,直到发送完毕
							 delay_ms(1);
							 USART_SendData(UART5,0x0d);//确认发送
							while((UART5->SR&0X40)==0);//循环发送,直到发送完毕
							 delay_ms(1);
							 USART_SendData(UART5,0x0a);//确认发送
}
uint8_t check_AT_ok(uint8_t *p,uint8_t *prt_match,uint16_t delay)
{
uint16_t i=0;
uart5_send_string(p);
for(i=0;i<delay;i++)
{
delay_ms(1);
}
if(strstr((const char*)UART5_RX_BUF, (const char*)prt_match))
  {
	return 0;
  }
  return 1;
}
//@初始化引脚
void gpio_init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);	 //使能PB,PC,PD,PE端口时钟
	
	//PC组配置
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA
 GPIO_SetBits(GPIOC,GPIO_Pin_9);	
		//PA12组配置
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA
 GPIO_SetBits(GPIOA,GPIO_Pin_12);	
			//PD3组配置
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA
 GPIO_SetBits(GPIOD,GPIO_Pin_3);	
}
//switch方法进行连接
void no_wait_connect(void)
{
MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
static uint8_t state = 0;
unsigned char i=0;
volatile unsigned char *prt=0;
static Flag_Type frist_send = 0;
u32 lon_inter = 0,lat_inter = 0,j=0;
Led1++;
switch (state)
{
//一组新命令
	case 0:
uart5_send_string("AT\r\n");
fill_at_count(&at_count,20);//延时10s
	  state = 1;
		break;
	case 1: 
		if(check_at_count(&at_count))
{
		if(strstr((const char*)UART5_RX_BUF, (const char*)"OK"))
		{
				state = 2;//如果匹配成功,切换到状态2
		}
		else
		{
			  state = 0;
		}
}
		break;
//一组AT命令结束
//新的一组开始
	case 2:
uart5_send_string("AT+CREG?\r\n");
fill_at_count(&at_count,20);//延时10s
	  state = 3;
		break;
	case 3: 
		if(check_at_count(&at_count))
{
		if(strstr((const char*)UART5_RX_BUF, (const char*)"+CREG: 0,1"))
		{
				state = 4;//如果匹配成功,切换到状态2
		}
		else
		{
			  state = 0;
		}
}
		break;
//一组AT命令结束
//新的一组开始
	case 4:
uart5_send_string("AT+CDNSORIP=0\r\n");
fill_at_count(&at_count,40);//延时10s
	  state = 5;
		break;
	case 5: 
		if(check_at_count(&at_count))
{
				state = 6;//如果匹配成功,切换到状态2
}
		break;
//一组AT命令结束
//新的一组开始
	  case 6:
		uart5_send_string("AT+CIPHEAD=1\r\n");
		fill_at_count(&at_count,40);//延时10s
	  state = 7;
		break;
	  case 7: 
		if(check_at_count(&at_count))
      {
		if(strstr((const char*)UART5_RX_BUF, (const char*)"OK"))
		{
				state = 8;//如果匹配成功,切换到状态2
		}
		else
		{
			  state = 0;
		}
      }
		break;
//一组AT命令结束
//新的一组开始
	case 8:
uart5_send_string("AT+CIPSTART=\"TCP\",\"183.230.40.39\",\"6002\"\r\n");
fill_at_count(&at_count,200);//延时10s
	  state = 9;
		break;
	case 9: 
		if(check_at_count(&at_count))
{
		if(strstr((const char*)UART5_RX_BUF, (const char*)"CONNECT"))
		{
				state = 10;//如果匹配成功,切换到状态2
		}
		else
		{
			  state = error;
		}
}
		break;			
//一组AT命令结束
//新的一组开始
	case 10:
uart5_send_string("AT+CIPSEND\r\n");
fill_at_count(&at_count,40);//延时10s
	  state = 11;
		break;
	case 11: 
		if(check_at_count(&at_count))
{
		if(strstr((const char*)UART5_RX_BUF, (const char*)">"))
		{
				state = 12;//如果匹配成功,切换到状态2
		}
		else
		{
			  state = 0;
		}
}
		break;	
//发送数据
		case 12:
					//如果没错误,继续发送
									//组装连接OneNet包
			  	if(MQTT_PacketConnect("267582", "sFov2oSSbvwoP2QrE1nnCp3I0Po=", "541228864", 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
				  {
				//---------------------------------------------步骤二：发送数据-----------------------------------------
						no_wait_NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
						MQTT_DeleteBuffer(&mqttPacket);									//删包
					  state = 13;
					}
		break;	
//一组AT命令结束
//新的一组开始
	case 13:
    fill_at_count(&at_count,200);//延时8s
	  state = 14;
		break;
	case 14: 
		if(check_at_count(&at_count))
{
				state = 15;//如果匹配成功,切换到状态2
}
		break;					
//一组AT命令结束
//新的一组开始									
	case 15:
uart5_send_string("AT+CIPSEND\r\n");
fill_at_count(&at_count,40);//延时2s
	  state = 16;
		break;
	case 16: 
		if(check_at_count(&at_count))
{
		if(strstr((const char*)UART5_RX_BUF, (const char*)">"))
		{
				state = 17;//如果匹配成功,切换到状态2
		}
		else
		{
			  state = 0;
		}
}
		break;
//可以发送数据
		case 17:
			 //组装数据包	
		no_wait_OneNet_SendData(FORMAT_TYPE3, NULL, NULL, dataStream, dataStreamCnt);	
		fill_at_count(&at_count,200);//延时10
	  state = 18;
		break;	
		case 18:
		if(check_at_count(&at_count))
				{
						state = 19;
				}
		break;
//获取GPS
	 case 19:
 if(!frist_send)
 {
	 frist_send = 1;
uart5_send_string("AT+CGPSPWR=1\r\n");
	  delay_ms(5);
uart5_send_string("AT+CGNSTST=1\r\n");
	  delay_ms(5);
uart5_send_string("AT+CGNSIPR=115200\r\n");
	  delay_ms(5);
uart5_send_string("AT+CGNSTST=0\r\n");
	  delay_ms(5);
uart5_send_string("AT+CGNSURC=1\r\n"); 
	  delay_ms(5);
fill_at_count(&at_count,40);//延时2s
 }
	  state = 20;
		break;
	 case 20: 
		 state = 21;
//		if(check_at_count(&at_count))
//{
//		if(strstr((const char*)UART5_RX_BUF, (const char*)"OK"))
//		{
//				state = 21;//如果匹配成功,切换到状态2
//		}
//		else
//		{
//			  state = 0;
//		}
//}
//数据处理 转换mmm到ddd
	 case 21:
		 		for( i = 0;i<16;i++)
				{
				gps.lon[i] = 0;
				gps.lat[i] = 0;
				}  
		   for( i = 0;i<16;i++)
				{
				gps.lon[i] = gps1.E[i];
				gps.lat[i] = gps1.N[i];
				}  
				for( i = 0;i<16;i++)
				{
          if('.'==gps.lon[i])
					{
					lon = atof(&gps.lon[i]-2); 
					lon = lon/60;
					for(j = 0;j<i-2;j++)
						{
			   lon_inter = lon_inter*10+(gps.lon[j]-0x30);
						}
						lon = lon_inter+lon;
					sprintf(gps.lon, "%f", lon); //产生"3.141593"
					}
          if('.'==gps.lat[i])
					{
					lat = atof(&gps.lat[i]-2); 
					lat = lat/60;
					for(j = 0;j<i-2;j++)//取出dd
						{
			   lat_inter = lat_inter*10+(gps.lat[j]-0x30);
						}
						lat = lat_inter+lat;
					sprintf(gps.lat, "%f", lat); //产生"3.141593"
					}
				}
      

  
      
uart5_send_string("AT\r\n");
fill_at_count(&at_count,40);//延时2s
	  state = 22;
		break;
	 case 22: 
		if(check_at_count(&at_count))
{
		if(strstr((const char*)UART5_RX_BUF, (const char*)"OK"))
		{
				state = 23;//如果匹配成功,切换到状态2
		}
		else
		{
			  state = 0;
		}
}
		break;
//配置输出到串口波特率
	 case 23:
		uart5_send_string("AT+CGNSIPR=115200\r\n");
		fill_at_count(&at_count,40);//延时2s
	  state = 24;
		break;
	 case 24: 
		if(check_at_count(&at_count))
{
		if(strstr((const char*)UART5_RX_BUF, (const char*)"OK"))
		{
				state = 25;//如果匹配成功,切换到状态2
		}
		else
		{
			  state = 0;
		}
}
		break;
//获取
	 case 25:
//uart5_send_string("AT+CGNSINF\r\n");
//fill_at_count(&at_count,40);//延时2s
	  state = 26;
		break;
	 case 26: 
//		if(check_at_count(&at_count))
//{
//		if(strstr((const char*)UART5_RX_BUF, (const char*)"CGNSINF"))
//		{
//						prt=Seek_comma(UART5_RX_BUF,1);
//				for(++prt;*prt!=',';prt++)
//					{
//						gps.lat[i++]=*prt;
//					}
//					i=0;
//				prt=Seek_comma(UART5_RX_BUF,2);
//				for(++prt;*prt!=',';prt++)
//					{
//						gps.lon[i++]=*prt;
//					}
//					i=0;
//						gps.lat[9]='\0';
//						gps.lon[10]='\0';
//					
//						state = 0;//完成切换到0
//		}
//		else
//		{
//			  state = 0;
//		}
//}
    state = 0; 
		break;
/////////////////////////////////////////////////出错处理//////////////////////////////
//连接出错
		case error: 
uart5_send_string("AT+CIPCLOSE\r\n");
fill_at_count(&at_count,40);//延时2s
	  state = error+1;
		break;
	case error+1: 
		if(check_at_count(&at_count))
			{
	  state = error+2; 
			}
		break;
//连接出错处理
		case error+2: 
uart5_send_string("AT+CIPSHUT\r\n");
fill_at_count(&at_count,40);//延时2s
	  state = error+3;
		break;
	case error+3: 
		if(check_at_count(&at_count))
			{
	  state = 0; 
			}
		break;			
	default:
		break;
}
}
//@tcp/ip连接
uint8_t tcp_ip_connect(void)
{
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
		uint8_t net_state = 0;
		uint16_t i = 0;
		Led1++;
		Led2++;
if(check_AT_ok("AT\r\n", "OK",1000))
		{
     return 0;
		}
		get_GPS();
		if(check_AT_ok("AT+CREG?\r\n", "+CREG: 0,1",1000))
		{
      net_state = 0;
			return 0;
		}	
		else
		{
	    net_state = 1;	
		}
	if(1==net_state)
	{
		if(check_AT_ok("AT+CDNSORIP=0\r\n", "OK",1000))
		{
		//出错
		  AT_error++;
		}
		if(check_AT_ok("AT+CIPHEAD=1\r\n", "OK",1000))
		{
		//出错
		  AT_error++;
		}
		if(check_AT_ok("AT+CIPSTART=\"TCP\",\"183.230.40.39\",\"6002\"\r\n", "CONNECT",10000))//如果连接成功
		     {
					 //如果出错
									if(check_AT_ok("AT+CIPCLOSE\r\n", "OK",2000))
								{
								//出错
									AT_error++;

								}
								if(check_AT_ok("AT+CIPSHUT\r\n", "OK",8000))
								{
								//出错
									AT_error++;

								}
		     }	
				else
				{
					//如果没错误,继续发送
									//组装连接OneNet包
			  	if(MQTT_PacketConnect("267582", "sFov2oSSbvwoP2QrE1nnCp3I0Po=", "540067462", 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
				  {
				//---------------------------------------------步骤二：发送数据-----------------------------------------
						NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
						MQTT_DeleteBuffer(&mqttPacket);									//删包  
							 for(i=0;i<8000;i++)
								{
								delay_ms(1);
								}
			 //组装数据包	
					  OneNet_SendData(FORMAT_TYPE3, NULL, NULL, dataStream, dataStreamCnt);		
							 for(i=0;i<8000;i++)
								{
								delay_ms(1);
								}
				  }
				}	


	}
return 0;
}
//@发送数据
//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//				devid：设备ID
//				apikey：设备apikey
//				streamArray：数据流
//				streamArrayNum：数据流个数
//
//	返回参数：	SEND_TYPE_OK-发送成功	SEND_TYPE_DATA-需要重送
//
//	说明：		
//==========================================================
void no_wait_OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};											//协议包
	short body_len = 0;
	body_len = DSTREAM_GetDataStream_Body_Measure(type, streamArray, streamArrayCnt, 0);		//获取当前需要发送的数据流的总长度
	if(body_len > 0)
	{
		if(MQTT_PacketSaveData(devid, body_len, NULL, (uint8)type, &mqttPacket) == 0)
		{
			body_len = DSTREAM_GetDataStream_Body(type, streamArray, streamArrayCnt, mqttPacket._data, mqttPacket._size, mqttPacket._len);
			
			if(body_len > 0)
			{
				mqttPacket._len += body_len;
				 no_wait_NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);						//上传数据到平台
			}
				
			MQTT_DeleteBuffer(&mqttPacket);														//删包
		}

	}	
}
//@发送数据
//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//				devid：设备ID
//				apikey：设备apikey
//				streamArray：数据流
//				streamArrayNum：数据流个数
//
//	返回参数：	SEND_TYPE_OK-发送成功	SEND_TYPE_DATA-需要重送
//
//	说明：		
//==========================================================
void OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};											//协议包
	short body_len = 0;
	body_len = DSTREAM_GetDataStream_Body_Measure(type, streamArray, streamArrayCnt, 0);		//获取当前需要发送的数据流的总长度
	if(body_len > 0)
	{
		if(MQTT_PacketSaveData(devid, body_len, NULL, (uint8)type, &mqttPacket) == 0)
		{
			body_len = DSTREAM_GetDataStream_Body(type, streamArray, streamArrayCnt, mqttPacket._data, mqttPacket._size, mqttPacket._len);
			
			if(body_len > 0)
			{
				mqttPacket._len += body_len;
				NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);						//上传数据到平台
			}
				
			MQTT_DeleteBuffer(&mqttPacket);														//删包
		}

	}	
}
  //处理逗号
volatile unsigned char* Seek_comma(volatile unsigned char *prt,unsigned char n)//把接受数据逗号间数据分离出来
  {
    unsigned char i=0;
    for( ;*prt!='\0'; prt++)
         {
        if(*prt==',')
           {
            ++i;
            if(i==n)
             {
            return prt;
             }
           }
         }
    return 0;
  }
//把GPS数据保存到结构体
 void get_GPS(void)
{
    volatile unsigned char *prt;
    unsigned char i=0;
	 if(check_AT_ok("AT+CGPSPWR=1\r\n","OK",1000))//指示GPRS附着状态
    {
   //出错
    }
			 if(check_AT_ok("AT+CGNSSEQ=\"RMC\"\r\n","OK",1000))//指示GPRS附着状态
    {
   //出错
    }
   if(!check_AT_ok("AT+CGPSINF=2\r\n","OK",1000))//
    { 
  prt=Seek_comma(UART5_RX_BUF,1);
    for(++prt;*prt!=',';prt++)
      {
        gps.lat[i++]=*prt;
      }
      i=0;
    prt=Seek_comma(UART5_RX_BUF,2);
    for(++prt;*prt!=',';prt++)
      {
        gps.lon[i++]=*prt;
      }
      i=0;
        gps.lat[9]='\0';
        gps.lon[10]='\0';
			
   }
}