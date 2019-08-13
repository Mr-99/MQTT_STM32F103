
#include "adc.h"
#include "delay.h"
#include "timer.h"

#define K   				257.4					//AD值转电压值系数

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)
#define ADC2_DR_Address    ((u32)0x40012800+0x4c)
// ADC1转换的电压值通过DMA方式传到SRAM
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
u8  ZXWDADDFlog = 0;	//0为+，1为-
u8  QXWDADD = 10;
u8  QXWDADDFlog = 1;
u8	ADDBuf[7] = {"+00+00"};


u16 ADC_ConvertedValueLocal[6]={0};

/*
 * 函数名：ADC1_GPIO_Config
 * 描述  ：使能ADC1和DMA1的时钟，初始化相关引脚
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
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

/* 函数名：ADC1_Mode_Config
 * 描述  ：配置ADC1的工作模式为DMA模式
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 //ADC地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC1_ConvertedValue;//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 6;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址bu固定
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		//循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	/* ADC1 configuration */	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//独立ADC模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 	 //扫描模式用于多通道采集
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//开启连续转换模式，即不停地进行ADC转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不使用外部触发转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//采集数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 6;	 	//要转换的通道数目12
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/*配置ADC时钟，为PCLK2的8分频，即9MHz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
	/*配置ADC1的通道1~13为55.	5个采样周期，序列为1~13 */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 4, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 5, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 6, ADC_SampleTime_55Cycles5);

	ADC_DMACmd(ADC1, ENABLE);        /* Enable ADC1 DMA */
	ADC_Cmd(ADC1, ENABLE);          /* Enable ADC1 */	 
	ADC_ResetCalibration(ADC1);     /*复位校准寄存器 */  
	while(ADC_GetResetCalibrationStatus(ADC1));	/*等待校准寄存器复位完成 */	
	ADC_StartCalibration(ADC1); /* ADC校准 */
	while(ADC_GetCalibrationStatus(ADC1));	/* 等待校准完成*/	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);/* 由于没有采用外部触发，所以使用软件触发ADC转换 */ 
}



/*
 * 函数名：ADC1_Init
 * 描述  ：无
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}

/*
 * 函数名：ADC1_ConvertedValue_Channel1_20
 * 描述  ：依次获取ADC1的1~20通道的AD值并转换
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void ADC1_ConvertedValue_ALL(void)
{
	uint8_t Channel_num;

	for(Channel_num = 0;Channel_num < 6;Channel_num++)
	{
		ADC_ConvertedValueLocal[Channel_num] = (ADC1_ConvertedValue[Channel_num]); 
	}
}















