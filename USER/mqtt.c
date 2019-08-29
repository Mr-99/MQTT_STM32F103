#include "mqtt.h"
#include "usart.h"
#include "delay.h"
//MQTT ����
//Э��ͷ�ļ�
#include "MqttKit.h"
#include "dStream.h"
#include<string.h>
#include<stdio.h>
//���ݴ��������ͷ�ļ�
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//����at������ת
#define error (34)
//���õ��ⲿ����
extern uint16_t at_count;
uint8_t AT_error = 0;
//�������ݵĽṹ�嶨��
//��Ҫ���͵�������
uint8_t Led1 = 21,Led2 = 22;
#define RX5_BUFF_LENG 200 
//�ⲿ��������
extern struct _gps_original gps1;
//END
//GPS���
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
  //������
volatile unsigned char* Seek_comma(volatile unsigned char *prt,unsigned char n);//�ѽ������ݶ��ż����ݷ������
//==========================================================
void no_wait_OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt);
///////////END
//������ʱ
void fill_at_count(uint16_t * count,uint16_t delay)
{
	*count = 	delay;
}
//�ж���ʱ�Ƿ񵽴�
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
//@�ض���fputc���� 
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
	while((UART5->SR&0X40)==0);//ѭ������,ֱ���������   
  UART5->DR = (u8) *p; 
	p++;
	if(0==*p)
	{
	break;
	}
}	
}
//@ֻ�������ݺ���
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
									while((UART5->SR&0X40)==0);//ѭ������,ֱ���������   
									UART5->DR = (u8) *p; 
									p++;
								}	
							while((UART5->SR&0X40)==0);//ѭ������,ֱ���������
							 delay_ms(1);
							 USART_SendData(UART5,0x1A);//ȷ�Ϸ���
							while((UART5->SR&0X40)==0);//ѭ������,ֱ���������
							 delay_ms(1);
							 USART_SendData(UART5,0x0d);//ȷ�Ϸ���
							while((UART5->SR&0X40)==0);//ѭ������,ֱ���������
							 delay_ms(1);
							 USART_SendData(UART5,0x0a);//ȷ�Ϸ���
}
else
{
continue;
}
	}
	}

	//@�޵ȴ��������ݺ���
void no_wait_NET_DEVICE_SendData(unsigned char *data, unsigned int len)
{
	unsigned int i = 0;
	uint8_t *p = data;
									for(i =0;i<len;i++)
								{
									while((UART5->SR&0X40)==0);//ѭ������,ֱ���������   
									UART5->DR = (u8) *p; 
									p++;
								}	
							while((UART5->SR&0X40)==0);//ѭ������,ֱ���������
							 delay_ms(1);
							 USART_SendData(UART5,0x1A);//ȷ�Ϸ���
							while((UART5->SR&0X40)==0);//ѭ������,ֱ���������
							 delay_ms(1);
							 USART_SendData(UART5,0x0d);//ȷ�Ϸ���
							while((UART5->SR&0X40)==0);//ѭ������,ֱ���������
							 delay_ms(1);
							 USART_SendData(UART5,0x0a);//ȷ�Ϸ���
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
//@��ʼ������
void gpio_init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��PB,PC,PD,PE�˿�ʱ��
	
	//PC������
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA
 GPIO_SetBits(GPIOC,GPIO_Pin_9);	
		//PA12������
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA
 GPIO_SetBits(GPIOA,GPIO_Pin_12);	
			//PD3������
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOD, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA
 GPIO_SetBits(GPIOD,GPIO_Pin_3);	
}
//switch������������
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
//һ��������
	case 0:
uart5_send_string("AT\r\n");
fill_at_count(&at_count,20);//��ʱ10s
	  state = 1;
		break;
	case 1: 
		if(check_at_count(&at_count))
{
		if(strstr((const char*)UART5_RX_BUF, (const char*)"OK"))
		{
				state = 2;//���ƥ��ɹ�,�л���״̬2
		}
		else
		{
			  state = 0;
		}
}
		break;
//һ��AT�������
//�µ�һ�鿪ʼ
	case 2:
uart5_send_string("AT+CREG?\r\n");
fill_at_count(&at_count,20);//��ʱ10s
	  state = 3;
		break;
	case 3: 
		if(check_at_count(&at_count))
{
		if(strstr((const char*)UART5_RX_BUF, (const char*)"+CREG: 0,1"))
		{
				state = 4;//���ƥ��ɹ�,�л���״̬2
		}
		else
		{
			  state = 0;
		}
}
		break;
//һ��AT�������
//�µ�һ�鿪ʼ
	case 4:
uart5_send_string("AT+CDNSORIP=0\r\n");
fill_at_count(&at_count,40);//��ʱ10s
	  state = 5;
		break;
	case 5: 
		if(check_at_count(&at_count))
{
				state = 6;//���ƥ��ɹ�,�л���״̬2
}
		break;
//һ��AT�������
//�µ�һ�鿪ʼ
	  case 6:
		uart5_send_string("AT+CIPHEAD=1\r\n");
		fill_at_count(&at_count,40);//��ʱ10s
	  state = 7;
		break;
	  case 7: 
		if(check_at_count(&at_count))
      {
		if(strstr((const char*)UART5_RX_BUF, (const char*)"OK"))
		{
				state = 8;//���ƥ��ɹ�,�л���״̬2
		}
		else
		{
			  state = 0;
		}
      }
		break;
//һ��AT�������
//�µ�һ�鿪ʼ
	case 8:
uart5_send_string("AT+CIPSTART=\"TCP\",\"183.230.40.39\",\"6002\"\r\n");
fill_at_count(&at_count,200);//��ʱ10s
	  state = 9;
		break;
	case 9: 
		if(check_at_count(&at_count))
{
		if(strstr((const char*)UART5_RX_BUF, (const char*)"CONNECT"))
		{
				state = 10;//���ƥ��ɹ�,�л���״̬2
		}
		else
		{
			  state = error;
		}
}
		break;			
//һ��AT�������
//�µ�һ�鿪ʼ
	case 10:
uart5_send_string("AT+CIPSEND\r\n");
fill_at_count(&at_count,40);//��ʱ10s
	  state = 11;
		break;
	case 11: 
		if(check_at_count(&at_count))
{
		if(strstr((const char*)UART5_RX_BUF, (const char*)">"))
		{
				state = 12;//���ƥ��ɹ�,�л���״̬2
		}
		else
		{
			  state = 0;
		}
}
		break;	
//��������
		case 12:
					//���û����,��������
									//��װ����OneNet��
			  	if(MQTT_PacketConnect("267582", "sFov2oSSbvwoP2QrE1nnCp3I0Po=", "541228864", 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
				  {
				//---------------------------------------------���������������-----------------------------------------
						no_wait_NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
						MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
					  state = 13;
					}
		break;	
//һ��AT�������
//�µ�һ�鿪ʼ
	case 13:
    fill_at_count(&at_count,200);//��ʱ8s
	  state = 14;
		break;
	case 14: 
		if(check_at_count(&at_count))
{
				state = 15;//���ƥ��ɹ�,�л���״̬2
}
		break;					
//һ��AT�������
//�µ�һ�鿪ʼ									
	case 15:
uart5_send_string("AT+CIPSEND\r\n");
fill_at_count(&at_count,40);//��ʱ2s
	  state = 16;
		break;
	case 16: 
		if(check_at_count(&at_count))
{
		if(strstr((const char*)UART5_RX_BUF, (const char*)">"))
		{
				state = 17;//���ƥ��ɹ�,�л���״̬2
		}
		else
		{
			  state = 0;
		}
}
		break;
//���Է�������
		case 17:
			 //��װ���ݰ�	
		no_wait_OneNet_SendData(FORMAT_TYPE3, NULL, NULL, dataStream, dataStreamCnt);	
		fill_at_count(&at_count,200);//��ʱ10
	  state = 18;
		break;	
		case 18:
		if(check_at_count(&at_count))
				{
						state = 19;
				}
		break;
//��ȡGPS
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
fill_at_count(&at_count,40);//��ʱ2s
 }
	  state = 20;
		break;
	 case 20: 
		 state = 21;
//		if(check_at_count(&at_count))
//{
//		if(strstr((const char*)UART5_RX_BUF, (const char*)"OK"))
//		{
//				state = 21;//���ƥ��ɹ�,�л���״̬2
//		}
//		else
//		{
//			  state = 0;
//		}
//}
//���ݴ��� ת��mmm��ddd
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
					sprintf(gps.lon, "%f", lon); //����"3.141593"
					}
          if('.'==gps.lat[i])
					{
					lat = atof(&gps.lat[i]-2); 
					lat = lat/60;
					for(j = 0;j<i-2;j++)//ȡ��dd
						{
			   lat_inter = lat_inter*10+(gps.lat[j]-0x30);
						}
						lat = lat_inter+lat;
					sprintf(gps.lat, "%f", lat); //����"3.141593"
					}
				}
      

  
      
uart5_send_string("AT\r\n");
fill_at_count(&at_count,40);//��ʱ2s
	  state = 22;
		break;
	 case 22: 
		if(check_at_count(&at_count))
{
		if(strstr((const char*)UART5_RX_BUF, (const char*)"OK"))
		{
				state = 23;//���ƥ��ɹ�,�л���״̬2
		}
		else
		{
			  state = 0;
		}
}
		break;
//������������ڲ�����
	 case 23:
		uart5_send_string("AT+CGNSIPR=115200\r\n");
		fill_at_count(&at_count,40);//��ʱ2s
	  state = 24;
		break;
	 case 24: 
		if(check_at_count(&at_count))
{
		if(strstr((const char*)UART5_RX_BUF, (const char*)"OK"))
		{
				state = 25;//���ƥ��ɹ�,�л���״̬2
		}
		else
		{
			  state = 0;
		}
}
		break;
//��ȡ
	 case 25:
//uart5_send_string("AT+CGNSINF\r\n");
//fill_at_count(&at_count,40);//��ʱ2s
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
//						state = 0;//����л���0
//		}
//		else
//		{
//			  state = 0;
//		}
//}
    state = 0; 
		break;
/////////////////////////////////////////////////������//////////////////////////////
//���ӳ���
		case error: 
uart5_send_string("AT+CIPCLOSE\r\n");
fill_at_count(&at_count,40);//��ʱ2s
	  state = error+1;
		break;
	case error+1: 
		if(check_at_count(&at_count))
			{
	  state = error+2; 
			}
		break;
//���ӳ�����
		case error+2: 
uart5_send_string("AT+CIPSHUT\r\n");
fill_at_count(&at_count,40);//��ʱ2s
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
//@tcp/ip����
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
		//����
		  AT_error++;
		}
		if(check_AT_ok("AT+CIPHEAD=1\r\n", "OK",1000))
		{
		//����
		  AT_error++;
		}
		if(check_AT_ok("AT+CIPSTART=\"TCP\",\"183.230.40.39\",\"6002\"\r\n", "CONNECT",10000))//������ӳɹ�
		     {
					 //�������
									if(check_AT_ok("AT+CIPCLOSE\r\n", "OK",2000))
								{
								//����
									AT_error++;

								}
								if(check_AT_ok("AT+CIPSHUT\r\n", "OK",8000))
								{
								//����
									AT_error++;

								}
		     }	
				else
				{
					//���û����,��������
									//��װ����OneNet��
			  	if(MQTT_PacketConnect("267582", "sFov2oSSbvwoP2QrE1nnCp3I0Po=", "540067462", 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
				  {
				//---------------------------------------------���������������-----------------------------------------
						NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);
						MQTT_DeleteBuffer(&mqttPacket);									//ɾ��  
							 for(i=0;i<8000;i++)
								{
								delay_ms(1);
								}
			 //��װ���ݰ�	
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
//@��������
//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//				devid���豸ID
//				apikey���豸apikey
//				streamArray��������
//				streamArrayNum������������
//
//	���ز�����	SEND_TYPE_OK-���ͳɹ�	SEND_TYPE_DATA-��Ҫ����
//
//	˵����		
//==========================================================
void no_wait_OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};											//Э���
	short body_len = 0;
	body_len = DSTREAM_GetDataStream_Body_Measure(type, streamArray, streamArrayCnt, 0);		//��ȡ��ǰ��Ҫ���͵����������ܳ���
	if(body_len > 0)
	{
		if(MQTT_PacketSaveData(devid, body_len, NULL, (uint8)type, &mqttPacket) == 0)
		{
			body_len = DSTREAM_GetDataStream_Body(type, streamArray, streamArrayCnt, mqttPacket._data, mqttPacket._size, mqttPacket._len);
			
			if(body_len > 0)
			{
				mqttPacket._len += body_len;
				 no_wait_NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);						//�ϴ����ݵ�ƽ̨
			}
				
			MQTT_DeleteBuffer(&mqttPacket);														//ɾ��
		}

	}	
}
//@��������
//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//				devid���豸ID
//				apikey���豸apikey
//				streamArray��������
//				streamArrayNum������������
//
//	���ز�����	SEND_TYPE_OK-���ͳɹ�	SEND_TYPE_DATA-��Ҫ����
//
//	˵����		
//==========================================================
void OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};											//Э���
	short body_len = 0;
	body_len = DSTREAM_GetDataStream_Body_Measure(type, streamArray, streamArrayCnt, 0);		//��ȡ��ǰ��Ҫ���͵����������ܳ���
	if(body_len > 0)
	{
		if(MQTT_PacketSaveData(devid, body_len, NULL, (uint8)type, &mqttPacket) == 0)
		{
			body_len = DSTREAM_GetDataStream_Body(type, streamArray, streamArrayCnt, mqttPacket._data, mqttPacket._size, mqttPacket._len);
			
			if(body_len > 0)
			{
				mqttPacket._len += body_len;
				NET_DEVICE_SendData(mqttPacket._data, mqttPacket._len);						//�ϴ����ݵ�ƽ̨
			}
				
			MQTT_DeleteBuffer(&mqttPacket);														//ɾ��
		}

	}	
}
  //������
volatile unsigned char* Seek_comma(volatile unsigned char *prt,unsigned char n)//�ѽ������ݶ��ż����ݷ������
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
//��GPS���ݱ��浽�ṹ��
 void get_GPS(void)
{
    volatile unsigned char *prt;
    unsigned char i=0;
	 if(check_AT_ok("AT+CGPSPWR=1\r\n","OK",1000))//ָʾGPRS����״̬
    {
   //����
    }
			 if(check_AT_ok("AT+CGNSSEQ=\"RMC\"\r\n","OK",1000))//ָʾGPRS����״̬
    {
   //����
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