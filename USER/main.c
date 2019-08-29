#include "delay.h"
#include "gpio.h"
#include "timer.h"
#include "usart.h"
#include "adc.h"
#include "gprs.h"
#include "flash.h"
#include "mmc_sd.h"
#include "mqtt.h"
#define PT100ZERO  		0
/********�洢�������ڲ�FLASH��ַ(STM32F103RCT6��256k��������2k�ֽ����ڴ洢�û�����)********************************/
#define ROM_USER_ADDRESS_START   ((uint32_t)0x0803F800)  			//�ڲ�flash�洢���ݵ���ʼ��ַ



u8	AlarmType[4];
u8	ZdValue[2];

u8	SaveEn = 0;
/*****************EEPROM����**************************/
const u8 TEXT_Buffer[]={"123345678912345678"};
#define SIZE  sizeof(TEXT_Buffer)	
u8 datatemp[SIZE];
/*****************************************************/

u8  RecordDat[512];

/***************************************************/
void System_Init()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();	    	 //��ʱ������ʼ��	 
	ADC1_Init();
	IO_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	TIM3_Int_Init(1000,7200);		//200ms
	TIM2_Int_Init(500,7200);		//50ms
//	SD2_Initialize();
	uart_init(115200);
	
	USART2_Init(115200);
	USART3_Init(115200);
	UART4_Init(115200);
	UART5_Init(115200);
}

void ParaLoad(void)
{
	u8 	ReadTemp[64];
	u16	value;
	u8 i;
	Flash_Read(ROM_USER_ADDRESS_START, (uint8_t*)ReadTemp, 64);
	value = (ReadTemp[1]<<8) + ReadTemp[0];		//�������
	if(value >= 10000)
	{
		Para[0] = 0;
	}
	else
	{
		Para[0] = value;
	}
	value = (ReadTemp[3]<<8) + ReadTemp[2];		//��׼����
	if(value >= 10000)
	{
		Para[1] = 0;
	}
	else
	{
		Para[1] = value;
	}
	value = (ReadTemp[5]<<8) + ReadTemp[4];		//ǿ������
	if(value >= 10000)
	{
		Para[2] = 0;
	}
	else
	{
		Para[2] = value;
	}
	value = (ReadTemp[7]<<8) + ReadTemp[6];		//��ˮ����
	if(value >= 10000)
	{
		Para[3] = 0;
	}
	else
	{
		Para[3] = value;
	}
	value = (ReadTemp[9]<<8) + ReadTemp[8];		//С��������
	if(value >= 10000)
	{
		Para[4] = 0;
	}
	else
	{
		Para[4] = value;
	}
	value = (ReadTemp[11]<<8) + ReadTemp[10];		//��������
	if(value >= 10000)
	{
		Para[5] = 0;
	}
	else
	{
		Para[5] = value;
	}
	value = (ReadTemp[13]<<8) + ReadTemp[12];		//����ʱ��
	if(value >= 10000)
	{
		Para[6] = 0;
	}
	else
	{
		Para[6] = value;
	}
	value = (ReadTemp[15]<<8) + ReadTemp[14];		//����ʱ��
	if(value >= 10000)
	{
		Para[7] = 0;
	}
	else
	{
		Para[7] = value;
	}
	value = (ReadTemp[17]<<8) + ReadTemp[16];		//����ʱ��
	if(value > 59)
	{
		Para[8] = 0;
	}
	else
	{
		Para[8] = value;
	}
	
	value = (ReadTemp[33]<<8) + ReadTemp[32];		//�������
	if(value >= 10000)
	{
		Para[16] = 0;
	}
	else
	{
		Para[16] = value;
	}
	value = (ReadTemp[35]<<8) + ReadTemp[34];		//��׼����
	if(value >= 10000)
	{
		Para[17] = 0;
	}
	else
	{
		Para[17] = value;
	}
	value = (ReadTemp[37]<<8) + ReadTemp[36];		//ǿ������
	if(value >= 10000)
	{
		Para[18] = 0;
	}
	else
	{
		Para[18] = value;
	}
	value = (ReadTemp[39]<<8) + ReadTemp[38];		//��ˮ����
	if(value >= 10000)
	{
		Para[19] = 0;
	}
	else
	{
		Para[19] = value;
	}
	value = (ReadTemp[41]<<8) + ReadTemp[40];		//С��������
	if(value >= 10000)
	{
		Para[20] = 0;
	}
	else
	{
		Para[20] = value;
	}
	value = (ReadTemp[43]<<8) + ReadTemp[42];		//��������
	if(value >= 10000)
	{
		Para[21] = 0;
	}
	else
	{
		Para[21] = value;
	}
	value = (ReadTemp[45]<<8) + ReadTemp[44];		//����ʱ��
	if(value >= 10000)
	{
		Para[22] = 0;
	}
	else
	{
		Para[22] = value;
	}
	value = (ReadTemp[47]<<8) + ReadTemp[46];		//����ʱ��
	if(value >= 10000)
	{
		Para[23] = 0;
	}
	else
	{
		Para[23] = value;
	}
	value = (ReadTemp[49]<<8) + ReadTemp[48];		//����ʱ��
	if(value > 59)
	{
		Para[24] = 0;
	}
	else
	{
		Para[24] = value;
	}
	
}

void ParaSave(void)
{
	Flash_Write(ROM_USER_ADDRESS_START,(uint8_t*)Para, 64);
}


void Uart2Process(void)
{
	u8	testBack[64];
	u8	i;
	//������ķ���
	if(USART2_RX_DEAL[0]=='R' && USART2_RX_DEAL[2]=='?')	//������
	{
		switch(USART2_RX_DEAL[1])
		{
			case '0':		//ˮλ
				testBack[0] = 'B';
				testBack[1] = '0';
				testBack[2] = '=';
				testBack[3] = 0x30+WaterState1;
				testBack[4] = 0x0d;
				testBack[5] = 0x0a;
				USART2_SendBytes(testBack,6);
				break;
			case '1':		//�¶�
				testBack[0] = 'B';
				testBack[1] = '1';
				testBack[2] = '=';
				if(ZXPT100 >= 100)
				{
					testBack[3] = 0x39;
					testBack[4] = 0x39;
				}
				{
					testBack[3] = 0x30+(ZXPT100/10);
					testBack[4] = 0x30+(ZXPT100%10);
				}
				testBack[5] = 0x0d;
				testBack[6] = 0x0a;
				USART2_SendBytes(testBack,7);
				break;
			case '2':		//ˮ�Ƕ�
				testBack[0] = 'B';
				testBack[1] = '2';
				testBack[2] = '=';
				if(ZdValue[0] >= 100)
				{
					testBack[3] = 0x39;
					testBack[4] = 0x39;
				}
				{
					testBack[3] = 0x30+(ZdValue[0]/10);
					testBack[4] = 0x30+(ZdValue[0]%10);
				}
				testBack[5] = 0x0d;
				testBack[6] = 0x0a;
				USART2_SendBytes(testBack,7);
				break;
			case 'A':
				testBack[0] = 'B';
				testBack[1] = 'A';
				testBack[2] = '=';
				for(i=0;i<9;i++)
				{
					testBack[3+(i*5)] =0x30 + (Para[i]/1000);
					testBack[4+(i*5)] =0x30 + ((Para[i]/100)%10);
					testBack[5+(i*5)] =0x30 + ((Para[i]/10)%10);
					testBack[6+(i*5)] =0x30 + ((Para[i]/1)%10);
					testBack[7+(i*5)] = ',';
				}
				testBack[38] =0x30 + (Para[9]/1000);
				testBack[39] =0x30 + ((Para[9]/100)%10);
				testBack[40] =0x30 + ((Para[9]/10)%10);
				testBack[41] =0x30 + ((Para[9]/1)%10);
				testBack[48] = 0x0d;
				testBack[49] = 0x0a;
				USART2_SendBytes(testBack,50);
				break;
		}
	}
	else if(USART2_RX_DEAL[0]=='W' && USART2_RX_DEAL[2]=='=')	//д����
	{
		switch(USART2_RX_DEAL[1])
		{
			case '0':			//�������
				if(USART2_RX_DEAL[3] == '0')
					BQFJ1 = 0;
				else
					BQFJ1 = 1;
				break;
			case '1':			//
				if(USART2_RX_DEAL[3] == '0')
					HXF1 = 0;
				else
					HXF1 = 1;
				break;
			case '2':
				if(USART2_RX_DEAL[3] == '0')
				{
					FSQ1 = 0;
					ch0En = 0;
				}
				else
				{
					FSQ1 = 1;
					ch0En = 1;
				}
				break;
			case '3':
				if(USART2_RX_DEAL[3] == '0')
					SSF1 = 0;
				else
					SSF1 = 1;
				break;
			case '4':
				if(USART2_RX_DEAL[3] == '0')
					PAIS1 = 0;
				else
					PAIS1 = 1;
				break;
			case '5':
				if(USART2_RX_DEAL[3] == '0')
					JR1 = 0;
				else
					JR1 = 1;
				break;
			case '6':
				if(USART2_RX_DEAL[3] == '0')
					SB1 = 0;
				else
					SB1 = 1;
				break;
			case '8':		//���Ά��
				if(USART2_RX_DEAL[3] == '1')
				{
					if(Para[0] >= 10000)
						Para[0] = 0;
					else
						Para[0] ++;
				}
				break;
			case '9':		//��׼����
				if(USART2_RX_DEAL[3] == '1')
				{
					if(Para[1] >= 10000)
						Para[1] = 0;
					else
						Para[1] ++;
				}
				break;
			case 'A':		//ǿ������
				if(USART2_RX_DEAL[3] == '1')
				{
					if(Para[2] >= 10000)
						Para[2] = 0;
					else
						Para[2] ++;
				}
				break;
			case 'B':		//��ˮ
				if(USART2_RX_DEAL[3] == '1')
				{
					if(Para[3] >= 10000)
						Para[3] = 0;
					else
						Para[3] ++;
				}
				break;
			case 'C':		//С����
				if(USART2_RX_DEAL[3] == '1')
				{
					if(Para[4] >= 10000)
						Para[4] = 0;
					else
						Para[4] ++;
					Para[9] = Para[8];
				}
				break;
			case 'D':		//����
				if(USART2_RX_DEAL[3] == '1')
				{
					if(Para[5] >= 10000)
						Para[5] = 0;
					else
						Para[5] ++;
					Para[9] = Para[8];
				}
				break;
			case 'F':		//�ۼ�����
				if(USART2_RX_DEAL[3] == '1')
				{
					if(Para[6] >= 10000)
						Para[6] = 0;
					else
						Para[6] ++;
				}
				break;
			case 'G':		//�������
				if(USART2_RX_DEAL[3] == '1')
				{
					Para[0] = 0;
					Para[1] = 0;
					Para[2] = 0;
					Para[3] = 0;
					Para[4] = 0;
					Para[6] = 0;
					Para[7] = 0;		//����
					Para[8] = 0;		//Сʱ	
					Para[9] = 0;
					Para[5]++;
					SaveEn = 1;
				}
				break;
			case 'S':		//save
				if(USART2_RX_DEAL[3] == '1')
				{
					SaveEn = 1;
				}
				break;
		}
	}
	memset(USART2_RX_DEAL,0,sizeof(USART2_RX_DEAL));
}

void Uart3Process(void)
{
	u8	testBack2[64];
	u8	i;
	//������ķ���
	if(USART3_RX_DEAL[0] == 'R' && USART3_RX_DEAL[2]=='?')	//������
	{
		switch(USART3_RX_DEAL[1])
		{
			case '0':
				testBack2[0] = 'B';
				testBack2[1] = '0';
				testBack2[2] = '=';
				testBack2[3] = 0x30+WaterState2;
				testBack2[4] = 0x0d;
				testBack2[5] = 0x0a;
				USART3_SendBytes(testBack2,6);
				break;
			case '1':
				testBack2[0] = 'B';
				testBack2[1] = '1';
				testBack2[2] = '=';
				if(QXPT100 >= 100)
				{
					testBack2[3] = 0x39;
					testBack2[4] = 0x39;
				}
				{
					testBack2[3] = 0x30+(QXPT100/10);
					testBack2[4] = 0x30+(QXPT100%10);
				}
				testBack2[5] = 0x0d;
				testBack2[6] = 0x0a;
				USART3_SendBytes(testBack2,7);
				break;
			case '2':
				testBack2[0] = 'B';
				testBack2[1] = '2';
				testBack2[2] = '=';
				if(ZdValue[1] >= 100)
				{
					testBack2[3] = 0x39;
					testBack2[4] = 0x39;
				}
				{
					testBack2[3] = 0x30+(ZdValue[1]/10);
					testBack2[4] = 0x30+(ZdValue[1]%10);
				}
				testBack2[5] = 0x0d;
				testBack2[6] = 0x0a;
				USART3_SendBytes(testBack2,7);
				break;
			case 'A':
				testBack2[0] = 'B';
				testBack2[1] = 'A';
				testBack2[2] = '=';
				for(i=0;i<9;i++)
				{
					testBack2[3+(i*5)] =0x30 + (Para[i+16]/1000);
					testBack2[4+(i*5)] =0x30 + ((Para[i+16]/100)%10);
					testBack2[5+(i*5)] =0x30 + ((Para[i+16]/10)%10);
					testBack2[6+(i*5)] =0x30 + ((Para[i+16]/1)%10);
					testBack2[7+(i*5)] = ',';
				}
				testBack2[38] =0x30 + (Para[25]/1000);
				testBack2[39] =0x30 + ((Para[25]/100)%10);
				testBack2[40] =0x30 + ((Para[25]/10)%10);
				testBack2[41] =0x30 + ((Para[25]/1)%10);
				testBack2[48] = 0x0d;
				testBack2[49] = 0x0a;
				USART3_SendBytes(testBack2,50);
				break;
		}
	}
	else if(USART3_RX_DEAL[0]=='W' && USART3_RX_DEAL[2]=='=')	//д����
	{
		switch(USART3_RX_DEAL[1])
		{
			case '0':			//�������
				if(USART3_RX_DEAL[3] == '0')
					BQFJ2 = 0;
				else
					BQFJ2 = 1;
				break;
			case '1':			//
				if(USART3_RX_DEAL[3] == '0')
					HXF2 = 0;
				else
					HXF2 = 1;
				break;
			case '2':
				if(USART3_RX_DEAL[3] == '0')
				{
					FSQ2 = 0;
					ch1En = 0;
				}
				else
				{
					FSQ2 = 1;
					ch1En = 1;
				}
				break;
			case '3':
				if(USART3_RX_DEAL[3] == '0')
					SSF2 = 0;
				else
					SSF2 = 1;
				break;
			case '4':
				if(USART3_RX_DEAL[3] == '0')
					PAIS2 = 0;
				else
					PAIS2 = 1;
				break;
			case '5':
				if(USART3_RX_DEAL[3] == '0')
					JR2 = 0;
				else
					JR2 = 1;
				break;
			case '6':
				if(USART3_RX_DEAL[3] == '0')
					SB2 = 0;
				else
					SB2 = 1;
				break;
			case '8':		//���Ά��
				if(USART3_RX_DEAL[3] == '1')
				{
					if(Para[16] >= 10000)
						Para[16] = 0;
					else
						Para[16] ++;
				}
				break;
			case '9':		//��׼����
				if(USART3_RX_DEAL[3] == '1')
				{
					if(Para[17] >= 10000)
						Para[17] = 0;
					else
						Para[17] ++;
				}
				break;
			case 'A':		//ǿ������
				if(USART3_RX_DEAL[3] == '1')
				{
					if(Para[18] >= 10000)
						Para[18] = 0;
					else
						Para[18] ++;
				}
				break;
			case 'B':		//��ˮ
				if(USART3_RX_DEAL[3] == '1')
				{
					if(Para[19] >= 10000)
						Para[19] = 0;
					else
						Para[19] ++;
				}
				break;
			case 'C':		//С����
				if(USART3_RX_DEAL[3] == '1')
				{
					if(Para[20] >= 10000)
						Para[20] = 0;
					else
						Para[20] ++;
					Para[25] = Para[24];
				}
				break;
			case 'D':		//����
				if(USART3_RX_DEAL[3] == '1')
				{
					if(Para[21] >= 10000)
						Para[21] = 0;
					else
						Para[21] ++;
					Para[25] = Para[24];
				}
				break;
			case 'F':		//�ۼ�����
				if(USART3_RX_DEAL[3] == '1')
				{
					if(Para[22] >= 10000)
						Para[22] = 0;
					else
						Para[22] ++;
				}
				break;
			case 'G':		//�������
				if(USART3_RX_DEAL[3] == '1')
				{
					Para[16] = 0;
					Para[17] = 0;
					Para[18] = 0;
					Para[19] = 0;
					Para[20] = 0;
					Para[22] = 0;
					Para[23] = 0;		//����
					Para[24] = 0;		//Сʱ
					Para[25] = 0;
					Para[21]++;
					SaveEn = 1;
				}
				break;
			case 'S':		//save
				if(USART3_RX_DEAL[3] == '1')
				{
					SaveEn = 1;
				}
				break;
		}
	}
	memset(USART3_RX_DEAL,0,sizeof(USART3_RX_DEAL));
}

/************************PT100  *********************************************/
void PT100C(void)
{
	u32 temp;
//	u8 testbuf[5];
	u8 cnt;
	u8 WDTEMP;
	
	AdcCh0Buf[AdcP] = ADC_ConvertedValueLocal[0];
	AdcCh1Buf[AdcP] = ADC_ConvertedValueLocal[1];
	AdcP++;
	if(AdcP>=16)
	{
		AdcP=0;
		TrigCnt=1;			//�Ѿ��ɼ���16��
	}
	if(TrigCnt == 1)
	{
		if(AdcP == 0)
		{
			temp=0;
			for(cnt=0;cnt<16;cnt++)
				temp = temp + AdcCh0Buf[cnt];
			temp = temp/16;
			WDTEMP = temp/41.62;
			
			//add for filter
			ZXWDfilte[FilteFlog] = WDTEMP;
			ZXPT100 = (ZXWDfilte[0] + ZXWDfilte[1] + ZXWDfilte[2] + ZXWDfilte[3])/4;
			if(ZXWDADDFlog == 1 && ZXPT100>= ZXWDADD)
			{
				ZXPT100 = ZXPT100 - ZXWDADD;
			}
			else
			{
				ZXPT100 = ZXPT100 + ZXWDADD;
			}
			//end add
			temp=0;
			for(cnt=0;cnt<16;cnt++)
				temp += AdcCh1Buf[cnt];
			temp = temp/16;
			
			WDTEMP = temp/41.62;			//23.347
			//add for filter
			QXWDfilte[FilteFlog] = WDTEMP;
			QXPT100 = (QXWDfilte[0] + QXWDfilte[1] + QXWDfilte[2] + QXWDfilte[3])/4;
			if(QXWDADDFlog == 1 && QXPT100 >= QXWDADD)
			{
				QXPT100 = QXPT100 - QXWDADD;
			}
			else
			{
				QXPT100 = QXPT100 + QXWDADD;
			}
			//end add
			if(FilteFlog >= 3)
				FilteFlog = 0;
			else
				FilteFlog ++;
		}
	}
}	
/**************************************************************/

/**************************************************************/
void ZdCalc(void)
{
	u32 temp;
	u8	i;
	Zd0Buf[AdcP] = ADC_ConvertedValueLocal[4];
	Zd1Buf[AdcP] = ADC_ConvertedValueLocal[5];
	ZdP++;
	if(ZdP>=16)
	{
		ZdP=0;
		TrigZd=1;			//�Ѿ��ɼ���16��
	}
	if(TrigZd)
	{
		if(ZdP == 0)
		{
			temp=0;
			for(i=0;i<16;i++)
				temp = temp + Zd0Buf[i];
			temp = temp/16;
			ZdValue[0] = temp/310;
			
			temp=0;
			for(i=0;i<16;i++)
				temp = temp + Zd1Buf[i];
			temp = temp/16;
			ZdValue[1] = temp/310;
		}
	}
}
/**************************************************************/


int main(void)
{
	u16 ui=0;
	u8 uart_testBuf[16];
  u16 i=0;
	System_Init();
	EN_GPRS = 0;
	START = 0;
	
	RS485_EN2 = 0;
	RS485_EN3 = 0;
	RUN_LED = 1;

	AlarmType[0] = 0;
	AlarmType[1] = 0;
	AlarmType[2] = 0;
	AlarmType[3] = 0;		
	ParaLoad();
	
	EN_12V = 1;
	
/*----------------------------------------*/
	for(ui=0;ui<512;ui++)
	{
		RecordDat[ui] = ui;
	}
//	USART1_SendString(RecordDat,16);

/*----------------------------------------*/
//�޸�����
gpio_init();//��ʼ��gpio
//	EN_12V = 1;
//	EN_GPRS= 1;
//	START= 1;
//	EN_GPS= 1;
	for(i = 0;i<10000;i++)
	{
	delay_ms(1);
	}
//END
	while(1)
	{
		//�޸����£�����onenet
		no_wait_connect();
		//tcp_ip_connect();
//	if(tcp_ip_connect())
//	{		
//	onenet_send_data();
//	}
		if(ADC_en)
		{
			ADC_en = 0;
			RUN_LED = !RUN_LED;
			ADC1_ConvertedValue_ALL();
			PT100C();
			ZdCalc();
		}

		if(USART2_RXED)				//��������ֵ
		{
			USART2_RXED = 0;
			Uart2Process();
		}
		
		if(USART3_RXED)				//��������ֵ
		{
			USART3_RXED = 0;
			Uart3Process();
		}
		
		if(UART4_RXED == 1)		//
		{
			UART4_RXED = 0;
			
		}
		
		if(UART5_RXED == 1)		//����������
		{
			UART5_RXED = 0;
			
		}
		
		if(USART_RX_STA)			//���洦��
		{
			USART_RX_STA = 0;
		}
		
		if(DSW1 == 0)					//��ˮ
			WaterState1 = WaterState1|0x01;
		else
			WaterState1 = WaterState1&0x02;
		if(GSW1 == 0)					//��ˮ
			WaterState1 = WaterState1|0x02;
		else
			WaterState1 = WaterState1&0x01;
		
		if(DSW2 == 0)					//��ˮ
			WaterState2 = WaterState2|0x01;
		else
			WaterState2 = WaterState2&0x02;
		if(GSW2 == 0)					//��ˮ
			WaterState2 = WaterState2|0x02;
		else
			WaterState2 = WaterState2&0x01;
		
		if(SaveEn)
		{
			SaveEn = 0;
			ParaSave();
		}
		
//		if((AlarmCnt<=100 && AlarmCnt > 80) || (AlarmCnt<=60 && AlarmCnt > 40) || (AlarmCnt<=20 && AlarmCnt > 0))		//ϴ�ӽ�����ķ�������
//			BEEP = 1;
//		else
//			BEEP = 0;
	
		//testLed();

	}
}
