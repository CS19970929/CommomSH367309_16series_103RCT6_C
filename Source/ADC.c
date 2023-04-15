#include "main.h"

__IO UINT16 g_u16ADCValFilter[ADC_NUM]; // ���λ�����ܸ�

INT32 g_u32ADCValFilter2[ADC_NUM]; // ADC���ݻ���2���������ˣ�ԭ����UINT32���ڼ�����̳����ˣ�
								   // ���ܸ�UINT32
INT32 g_i32ADCResult[ADC_NUM]; // ADC�������

UINT16 g_u16IoutOffsetAD;
UINT16 gu16_BusCurr_CHG; // A*10
UINT16 gu16_BusCurr_DSG; // A*10

// 12λ��4096���
const UINT16 iSheldTemp_10K[LENGTH_TBLTEMP_PORT_10K] = {
	// AD		(Temp+40)*10
	3771, 100, //-30
	3683, 150, //-25
	3580, 200, //-20
	3460, 250, //-15
	3323, 300, //-10
	3169, 350, //-5
	3004, 400, // 0
	2820, 450, // 5
	2633, 500, // 10
	2437, 550, // 15
	2241, 600, // 20
	2048, 650, // 25
	1859, 700, // 30
	1679, 750, // 35
	1509, 800, // 40
	1351, 850, // 45
	1204, 900, // 50
	1073, 950, // 55
	953, 1000, // 60
	845, 1050, // 65
	749, 1100, // 70
	664, 1150, // 75
	588, 1200, // 80
	522, 1250, // 85
	463, 1300, // 90
	411, 1350, // 95
	366, 1400, // 100
	326, 1450, // 105

};

// 030��103����һ��
void InitADC_DMA(void)
{
	DMA_InitTypeDef DMA_InitStruct;
	// NVIC_InitTypeDef  		 NVIC_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); // ����DMA1����ʱ�ӣ����ڶ�ȡADC1

	/*
	//DMA�ж�����
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;       //ѡ��DMA1ͨ���ж�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //�ж�ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;                //���ȼ���Ϊ0
	NVIC_Init(&NVIC_InitStructure);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);		//ʹ��DMA�жϣ����濴���Ƿ���Ҫ�����˸о�����Ҫ
	*/

	// DMA��ʼ��
	// ������SYSCFG_CFGR1��ADC_DMA_RMPλ��Ϊ0��ADC�ź�Channel1����һ�𣬶�1ʱ��Channel2����һ��(ADCҪôChannel1Ҫô2)
	// �����������û���ù���resetֵΪ0�����Բ��ù�
	DMA_DeInit(DMA1_Channel1);												 // ѡ��Ƶ��
	DMA_StructInit(&DMA_InitStruct);										 // ��ʼ��DMA�ṹ��
	DMA_InitStruct.DMA_PeripheralBaseAddr = (UINT32)(&(ADC1->DR));			 // ���������ַ
	DMA_InitStruct.DMA_MemoryBaseAddr = (UINT32)(&g_u16ADCValFilter[0]);	 // �����ڴ�ӳ���ַ
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;							 // ���ݴ��䷽��0�����������1���Ӵ洢����
	DMA_InitStruct.DMA_BufferSize = AD_Used_amount;							 // ���������DMA���������С����
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			 // �����ַ���䣬�����̫�����ĸ������ַ
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;					 // �ڴ��ַ����
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // ������ִ���16λ
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;		 // �ڴ���ִ���16λ
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;							 // ѭ��ģʽ
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;						 // �����ȼ�����ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;								 // ���ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel1, &DMA_InitStruct);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

// ��ʼ��PC5��������485����2û��ͨѶ����������Բ���ʼ��PC5��ʵ�����ֲ�Ӱ�����
// ���Ǹ�BUG�������۲�
void InitADC_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

#if 0 // ��Ч
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);		//������ӳ��1����CH2/PB3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
}

void InitADC_TIMER(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_Cmd(TIM2, DISABLE); // ��ֹͣTIM2ʱ��
	if (0 == System_ErrFlag.u8ErrFlag_HSE)
	{
		TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; // ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ����������Ƶ
	}
	else
	{
		TIM_TimeBaseStructure.TIM_Prescaler = 8 - 1; // ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ����������Ƶ
	}
	TIM_TimeBaseStructure.TIM_Period = 999; // ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ

	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ���ϼ���
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x00;			// ���ָ��(+1)����������ж�

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_Cmd(TIM2, ENABLE);
}

void InitADC_ADC1(void)
{
	ADC_InitTypeDef ADC_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // ����ADC1����ʱ��

	// ADC��ʼ��
	ADC_DeInit(ADC1);				 // ADC�ָ�Ĭ������
	ADC_StructInit(&ADC_InitStruct); // ��ʼ��ADC�ṹ��

	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;					   // ����ģʽ
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;						   // ɨ��ģʽ
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;				   // ����ת��ģʽ
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2; // T2_CC2����
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;				   // ת������Ҷ���
	ADC_InitStruct.ADC_NbrOfChannel = AD_Used_amount;				   // ת��ͨ������
	ADC_Init(ADC1, &ADC_InitStruct);								   // ��ʼ��ADC

	RCC_ADCCLKConfig(RCC_PCLK2_Div8); // ����ADCʱ��PCLK2��8��Ƶ����9MHz

	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5); // ����ADCͨ����ת��˳��Ͳ���ʱ��
	// ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 2, ADC_SampleTime_55Cycles5);	//����ADCͨ����ת��˳��Ͳ���ʱ��
	// ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 3, ADC_SampleTime_55Cycles5);
	// ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 4, ADC_SampleTime_55Cycles5);

	ADC_Cmd(ADC1, ENABLE);	  // ����ADC������ʼת��
	ADC_DMACmd(ADC1, ENABLE); // ʹ��ADC DMA ����

	ADC_ResetCalibration(ADC1); // ��ʼ��ADC У׼�Ĵ�������ȻҪ�����������仰�������
	while (ADC_GetResetCalibrationStatus(ADC1))
		;						// �ȴ�У׼�Ĵ�����ʼ�����
	ADC_StartCalibration(ADC1); // ADC��ʼУ׼
	while (ADC_GetCalibrationStatus(ADC1))
		; // �ȴ�У׼���

	// ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//����û�в����ⲿ����������ʹ���������ADCת��
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);
}

void ADC_Current_Smooth(void)
{
	static UINT8 su8_ADcnt = 0;
	INT32 t_i32temp = 0;
	UINT32 u32_ADC_CUR_AMP = 0;
	UINT32 u32_AD_VREF_AD = 0;

	if (su8_ADcnt++ < AD_CalNum_Cur)
	{
		g_u32ADCValFilter2[ADC_CUR_AMP] += g_u16ADCValFilter[ADC_CUR_AMP];
		g_u32ADCValFilter2[AD_VREF_AD] += g_u16ADCValFilter[AD_VREF_AD];
	}
	else
	{
		su8_ADcnt = 0;
		u32_ADC_CUR_AMP = g_u32ADCValFilter2[ADC_CUR_AMP] >> AD_CalNum_Cur_2;
		u32_AD_VREF_AD = g_u32ADCValFilter2[AD_VREF_AD] >> AD_CalNum_Cur_2;
		g_u32ADCValFilter2[ADC_CUR_AMP] = 0;
		g_u32ADCValFilter2[AD_VREF_AD] = 0;

		u32_ADC_CUR_AMP = u32_ADC_CUR_AMP * 151 / 100;
		if (u32_AD_VREF_AD > u32_ADC_CUR_AMP)
		{
			t_i32temp = ModulusSub(u32_AD_VREF_AD, u32_ADC_CUR_AMP) * 3300 / 4096;					  // mV
			t_i32temp = t_i32temp * OtherElement.u16Sys_CS_Res_Num / OtherElement.u16Sys_CS_Res >> 1; // A*10	//A*10/Gain = >>1
			g_u32ADCValFilter2[ADC_CURR] = ((t_i32temp - g_u32ADCValFilter2[ADC_CURR]) >> 3) + g_u32ADCValFilter2[ADC_CURR];
			gu16_BusCurr_DSG = (UINT16)g_u32ADCValFilter2[ADC_CURR];
			gu16_BusCurr_CHG = 0;
		}
		else
		{
			t_i32temp = ModulusSub(u32_AD_VREF_AD, u32_ADC_CUR_AMP) * 3300 / 4096;					  // mV
			t_i32temp = t_i32temp * OtherElement.u16Sys_CS_Res_Num / OtherElement.u16Sys_CS_Res >> 1; // A*10	//A*10/Gain = >>1
			g_u32ADCValFilter2[ADC_CURR] = ((t_i32temp - g_u32ADCValFilter2[ADC_CURR]) >> 3) + g_u32ADCValFilter2[ADC_CURR];
			gu16_BusCurr_DSG = 0;
			gu16_BusCurr_CHG = (UINT16)g_u32ADCValFilter2[ADC_CURR];
		}
	}
}

void ADC_TTC(void)
{
	INT32 t_i32temp = 0;
	//-------------TEMP1�¶�(+40)-------------
	t_i32temp = (INT32)g_u16ADCValFilter[ADC_TEMP_EV1]; // ��ȡADֵ
	t_i32temp = GetEndValue(iSheldTemp_10K, (UINT16)LENGTH_TBLTEMP_PORT_10K, (UINT16)t_i32temp);
	g_u32ADCValFilter2[ADC_TEMP_EV1] = (((t_i32temp << 10) - g_u32ADCValFilter2[ADC_TEMP_EV1]) >> 3) + g_u32ADCValFilter2[ADC_TEMP_EV1];
	g_i32ADCResult[ADC_TEMP_EV1] = (UINT16)((g_u32ADCValFilter2[ADC_TEMP_EV1] + 512) >> 10);

	//-------------MOS1�¶�(+40)-------------
	t_i32temp = (INT32)g_u16ADCValFilter[ADC_TEMP_MOS1]; // ��ȡADֵ
	t_i32temp = GetEndValue(iSheldTemp_10K, (UINT16)LENGTH_TBLTEMP_PORT_10K, (UINT16)t_i32temp);
	g_u32ADCValFilter2[ADC_TEMP_MOS1] = (((t_i32temp << 10) - g_u32ADCValFilter2[ADC_TEMP_MOS1]) >> 3) + g_u32ADCValFilter2[ADC_TEMP_MOS1];
	g_i32ADCResult[ADC_TEMP_MOS1] = (UINT16)((g_u32ADCValFilter2[ADC_TEMP_MOS1] + 512) >> 10);

	//-------------MOS2�¶�(+40)-------------
	t_i32temp = (INT32)g_u16ADCValFilter[ADC_TEMP_MOS2]; // ��ȡADֵ
	t_i32temp = GetEndValue(iSheldTemp_10K, (UINT16)LENGTH_TBLTEMP_PORT_10K, (UINT16)t_i32temp);
	g_u32ADCValFilter2[ADC_TEMP_MOS2] = (((t_i32temp << 10) - g_u32ADCValFilter2[ADC_TEMP_MOS2]) >> 3) + g_u32ADCValFilter2[ADC_TEMP_MOS2];
	g_i32ADCResult[ADC_TEMP_MOS2] = (UINT16)((g_u32ADCValFilter2[ADC_TEMP_MOS2] + 512) >> 10);
}

void ADC_Vbc(void)
{
	static UINT8 s8ADcnt = 0;
	INT32 t_i32temp = 0;
	if (s8ADcnt++ < AD_CalNum)
	{
		g_u32ADCValFilter2[ADC_VBC] += (UINT32)g_u16ADCValFilter[ADC_VBC];
	}
	else
	{
		s8ADcnt = 0;
		t_i32temp = g_u32ADCValFilter2[ADC_VBC] >> AD_CalNum_2; // ��ȡADֵ��mV
		g_u32ADCValFilter2[ADC_VBC] = 0;
		// t_i32temp *= 21;							// ADֵת��Ϊģ����
		t_i32temp = ((t_i32temp * 825) >> 10) * Vbc_scale; // 12λ��4096Ϊ��׼
		t_i32temp = t_i32temp > 0 ? t_i32temp : 0;
		g_i32ADCResult[ADC_VBC] = ((t_i32temp - g_i32ADCResult[ADC_VBC]) >> 3) + g_i32ADCResult[ADC_VBC]; // mV
	}
}

// VDDA��VSSAΪAD����ר�Ź��磬VREF+��VREF-ΪAD�����Ĳμӵ�ѹ������Ҫ��������(���Իᷢ��û����������)
// �����Ǹ�����ΪΪ12λ�ֱ��ʣ������������Ϊ4096��
void InitADC(void)
{
	UINT8 i;
	InitADC_GPIO();
	InitADC_TIMER();
	InitADC_DMA();
	InitADC_ADC1();

	for (i = 0; i < ADC_NUM; i++)
	{
		g_u16ADCValFilter[i] = 0;
		g_i32ADCResult[i] = 0;
		g_u32ADCValFilter2[i] = 0;
	}
}

// ��ʱ���ͳ�ʼ���ǲ���Ҫreturn��
void App_AnlogCal(void)
{
	if (STARTUP_CONT == System_FUNC_StartUp(SYSTEM_FUNC_STARTUP_ADC))
	{
		// return;
	}

	// ��Ϊ1msʱ��
	if (0 == g_st_SysTimeFlag.bits.b1Sys1msFlag)
	{
		return;
	}

	// ADC_Vbc();
	ADC_TTC();
	// ADC_Current_Smooth();
}
