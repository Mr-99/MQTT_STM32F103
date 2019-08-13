#include "mqtt.h"
#include "usart.h"
#include "delay.h"
//MQTT 部分
//协议头文件
#include "MqttKit.h"
#include "dStream.h"
#include<string.h>
#include<stdio.h>
uint8_t AT_error = 0;
//发送数据的结构体定义
//需要发送的数据流
uint8_t Led1 = 21,Led2 = 22;
#define RX5_BUFF_LENG 200 
typedef struct
{

    char lon[16];
    char lat[16];

    _Bool flag;

} GPS_INFO;
//END
extern u16	Para[32];
GPS_INFO gps = {"0","0",1};
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
		{"GPS", &gps, TYPE_GPS, 1},
};

unsigned char dataStreamCnt = sizeof(dataStream) / sizeof(dataStream[0]);
void OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt);
void get_GPS(void);
///////////END
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
void NET_DEVICE_SendData(unsigned char *data, unsigned short len)
{
	uint8_t i = 0;
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
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA
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
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA
 GPIO_SetBits(GPIOD,GPIO_Pin_3);	
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
		if(!check_AT_ok("AT+CIPSTART=\"TCP\",\"183.230.40.39\",\"6002\"\r\n", "CONNECT",10000))//如果连接成功
		{
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
					  OneNet_SendData(FORMAT_TYPE3, NULL, NULL, dataStream, dataStreamCnt);		
							 for(i=0;i<8000;i++)
								{
								delay_ms(1);
								}
				}	
			  //组装数据包
				
		}	
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
   if(!check_AT_ok("AT+CGPSINF=0\r\n","OK",1000))//
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