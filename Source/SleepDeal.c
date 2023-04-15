#include "main.h"

void SleepTest(void);

UINT8 RTC_ExtComCnt;

// ͨѶ���Ѷ�������߲���Ч����������Base����ͨѶ���ѡ�
void InitWakeUp_Base(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // ʹ��GPIOAʱ�ӣ���Ϊ�ǿ��������Ի���Ҫ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // ��Ϊ�ǿ��������Ի���Ҫ

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // ѡ��Ҫ�õ�GPIO����,PA0Ҳ���Ի���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; // ѡ��Ҫ�õ�GPIO����,PA0Ҳ���Ի���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);
	EXTI_InitStruct.EXTI_Line = EXTI_Line5;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void InitWakeUp_NormalMode(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	InitWakeUp_Base();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // ʹ��GPIOAʱ�ӣ���Ϊ�ǿ��������Ի���Ҫ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // ��Ϊ�ǿ��������Ի���Ҫ

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // ѡ��Ҫ�õ�GPIO����,PA0Ҳ���Ի���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);
	EXTI_InitStruct.EXTI_Line = EXTI_Line9;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void InitWakeUp_RTCMode(void)
{
	// InitWakeUp_Base();
	InitWakeUp_NormalMode(); // ������Base�Ļ��ѷ�ʽ
	RTC_WKTimeConfig();
}

// �����standbyģʽ�Ļ���PA0��wkup������
// ͨѶ���Ѷ�������߲�����Ч����
void InitWakeUp_DeepMode(void)
{
	InitWakeUp_Base();
}

void IOstatus_Base(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // ʹ��GPIOBʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // ʹ��GPIOCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); // ʹ��GPIODʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); // ʹ��GPIOEʱ��

	ADC_DeInit(ADC1); // �ٶ�˵���ã�ʵ���ϲ�֪���Ƿ�����

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	// ����ʱ��ģ��
	// ��Դ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; // MCUO_PWSV_LDO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_InitStructure.GPIO_Pin);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; // MCUO_PWSV_CTR
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC, GPIO_InitStructure.GPIO_Pin);

	__delay_ms(100);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // MCUO_PW_SPS_STB
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_InitStructure.GPIO_Pin);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; // MCUO_PW_WK_EN
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_InitStructure.GPIO_Pin);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // MCUO_DRV_WLM_PW
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_InitStructure.GPIO_Pin);

	// AFE��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		 // MCUO_AFE_SHIP
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // Ҫ��ȷ��ƽ����Ȼ�ǻ���(��Ϊ�ⲿû������)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_InitStructure.GPIO_Pin);

	// PB3ΪJTAG��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;		 // DRV_CHG_DET
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // û����Ҳû�������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_InitStructure.GPIO_Pin);

	__delay_ms(100);
}

void IOstatus_RTCMode(void)
{
	InitIO();
	InitDelay();
	InitSystemWakeUp();
	InitE2PROM(); // �ڲ�EEPROM������Ҫ��ʼ��
	InitRTC();

	InitAFE1_Sleep(1);
	AFE_IDLE();
	IOstatus_Base();
}

void IOstatus_NormalMode(void)
{
	/*
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);		//ʹ��PWR����ʱ�ӣ�����ģʽ��RTC�����Ź�
	PWR_BackupAccessCmd(ENABLE);    						//�������RTC
	RCC_LSEConfig(RCC_LSE_ON);								//ʹ���ⲿLSE����RCC_LSE_Bypass��·����˼Ӧ����ʹ�����LSEʱ�ӣ����ǵ�Ƭ�����ã���Χ��·��?
	//RCC_LSICmd(DISABLE);
	*/

	InitIO();
	InitDelay();
	InitSystemWakeUp();
	InitRTC(); // ��������ܽ���150uA�Ĺ��ģ����������������仰������ԭ����
	InitAFE1_Sleep(0);
	AFE_Sleep();
	IOstatus_Base();
}

void IOstatus_DeepMode(void)
{
	IOstatus_NormalMode();
	// IOstatus_Base();
}

// ���ڸ�������
void RefreshData_SleepFunction(void)
{
	SleepElement.u8_ExtComCnt = RTC_ExtComCnt;
	SleepElement.u16_VCellMax = g_stCellInfoReport.u16VCellMax;
	SleepElement.u16_VCellMin = g_stCellInfoReport.u16VCellMin;
	SleepElement.u16_CurChg = g_stCellInfoReport.u16Ichg;
	SleepElement.u16_CurDsg = g_stCellInfoReport.u16IDischg;
}

void GetData_SleepFunction(void)
{
	// SleepElement.u8_ToSleepFlag;
}

// һ���Ը�ֵ
void InitData_SleepFunc(void)
{
	SleepElement.u16_TimeRTC_First = OtherElement.u16Sleep_TimeRTC;
	SleepElement.u16_TimeRTC_Hiccup = OtherElement.u16Sleep_TimeRTC;
	SleepElement.u16_RTC_WakeUpTime = OtherElement.u16Sleep_RTC_WakeUpTime;
	SleepElement.u16_VNormal = OtherElement.u16Sleep_VNormal;
	SleepElement.u16_TimeNormal = OtherElement.u16Sleep_TimeNormal;
	SleepElement.u16_Vlow = OtherElement.u16Sleep_Vlow;
	SleepElement.u16_TimeVlow = OtherElement.u16Sleep_TimeVlow;
	SleepElement.u16_VirCur_Chg = OtherElement.u16Sleep_VirCur_Chg;
	SleepElement.u16_VirCur_Dsg = OtherElement.u16Sleep_VirCur_Dsg;

	// ���ư洦��ʱ���޸ĵط�
	SleepElement.u16_Force_RTC_First = 0;  // δʹ�ã�����
	SleepElement.u16_Force_RTC_Hiccup = 0; // δʹ�ã�����
	SleepElement.u16_Force_RTC_WKtime = 0; // δʹ�ã�����
	SleepElement.u16_Force_Normal_T = 0;   // ��ζ��ֱ�ӽ���
	SleepElement.u16_Force_Vlow_T = 0;	   // ��ζ��ֱ�ӽ���
}

void EnterSleepJudge(void)
{
	IsSleepStartUp();
}

void App_SleepDeal(void)
{
	if (SystemStatus.bits.b1StartUpBMS)
	{ // ��������ٽ���
		return;
	}
	else
	{
		SystemStatus.bits.b1Status_ToSleep = 1;
	}

	if (SleepElement.u8_ToSleepFlag)
	{
		LogRecord_Flag.bits.Log_Sleep = 1;
		return;
	}

	RefreshData_SleepFunction();
	SleepCtrl(g_st_SysTimeFlag.bits.b1Sys1000msFlag1, System_OnOFF_Func.bits.b1OnOFF_Sleep);
	// SleepTest();		//��Ҫ���̽��룬��ȡ�����μ���
}

void SleepTest(void)
{
	static UINT8 su8_aaa = 0;
	if (0 == g_st_SysTimeFlag.bits.b1Sys1000msFlag1)
	{
		return;
	}
	if (++su8_aaa > 3)
	{
		SleepElement.Sleep_Mode.bits.b1ForceToSleep_L1 = 1;
	}
}

void App_NormalSleepTest(void)
{
	static UINT16 s_u16HaltTestCnt = 0;

	if (0 == g_st_SysTimeFlag.bits.b1Sys1000msFlag1)
	{ // ���������ȴ�ϵͳ��ʼ�����
		return;
	}

	if (++s_u16HaltTestCnt >= 5)
	{ // 10s����Test
		s_u16HaltTestCnt = 0;
		IOstatus_NormalMode();
		InitWakeUp_NormalMode();
		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
		// Sys_StandbyMode();
		MCU_RESET();
	}
}

void App_RTCSleepTest(void)
{
	static UINT16 s_u16HaltTestCnt = 0;

	if (0 == g_st_SysTimeFlag.bits.b1Sys1000msFlag1)
	{ // ���������ȴ�ϵͳ��ʼ�����
		return;
	}

	if (++s_u16HaltTestCnt >= 5)
	{ // 10s����Test
		s_u16HaltTestCnt = 0;
		IOstatus_RTCMode();
		InitWakeUp_RTCMode();
		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
		MCU_RESET();
	}
}
