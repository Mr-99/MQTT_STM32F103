
#include "adc.h"
#include "delay.h"
#include "timer.h"

#define K   				257.4					//ADֵת��ѹֵϵ��

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)
#define ADC2_DR_Address    ((u32)0x40012800+0x4c)
// ADC1ת���ĵ�ѹֵͨ��DMA��ʽ����SRAM
__IO uint16_t ADC1_ConvertedValue[6];

u16 AdcCh0Buf[16];
u16 AdcCh1Buf[16];
u8	AdcP = 0;
u8	TrigCnt = 0;

u16 Zd0Buf[16];
u16 Zd1Buf[16];
u8	ZdP = 0;
u8	TrigZd = 0;

u8	ZXPT100;
u8	QXPT100;
u8  ZXWDfilte[4];
u8  QXWDfilte[4];
u8  FilteFlog = 0;
u8  ZXWDADD = 0;
u8  ZXWDADDFlog = 0;	//0Ϊ+��1Ϊ-
u8  QXWDADD = 10;
u8  QXWDADDFlog = 1;
u8	ADDBuf[7] = {"+00+00"};


u16 ADC_ConvertedValueLocal[6]={0};

/*
 * ��������ADC1_GPIO_Config
 * ����  ��ʹ��ADC1��DMA1��ʱ�ӣ���ʼ���������
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	

	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

	/* Configure PA.00~01   as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure PC.00~03   as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

}

/* ��������ADC1_Mode_Config
 * ����  ������ADC1�Ĺ���ģʽΪDMAģʽ
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 //ADC��ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC1_ConvertedValue;//�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 6;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַbu�̶�
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//����
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//ѭ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	/* ADC1 configuration */	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//����ADCģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 	 //ɨ��ģʽ���ڶ�ͨ���ɼ�
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//��������ת��ģʽ������ͣ�ؽ���ADCת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//��ʹ���ⲿ����ת��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//�ɼ������Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 6;	 	//Ҫת����ͨ����Ŀ12
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/*����ADCʱ�ӣ�ΪPCLK2��8��Ƶ����9MHz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
	/*����ADC1��ͨ��1~13Ϊ55.	5���������ڣ�����Ϊ1~13 */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 4, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 5, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 6, ADC_SampleTime_55Cycles5);

	ADC_DMACmd(ADC1, ENABLE);        /* Enable ADC1 DMA */
	ADC_Cmd(ADC1, ENABLE);          /* Enable ADC1 */	 
	ADC_ResetCalibration(ADC1);     /*��λУ׼�Ĵ��� */  
	while(ADC_GetResetCalibrationStatus(ADC1));	/*�ȴ�У׼�Ĵ�����λ��� */	
	ADC_StartCalibration(ADC1); /* ADCУ׼ */
	while(ADC_GetCalibrationStatus(ADC1));	/* �ȴ�У׼���*/	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);/* ����û�в����ⲿ����������ʹ���������ADCת�� */ 
}



/*
 * ��������ADC1_Init
 * ����  ����
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}

/*
 * ��������ADC1_ConvertedValue_Channel1_20
 * ����  �����λ�ȡADC1��1~20ͨ����ADֵ��ת��
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void ADC1_ConvertedValue_ALL(void)
{
	uint8_t Channel_num;

	for(Channel_num = 0;Channel_num < 6;Channel_num++)
	{
		ADC_ConvertedValueLocal[Channel_num] = (ADC1_ConvertedValue[Channel_num]); 
	}
}















