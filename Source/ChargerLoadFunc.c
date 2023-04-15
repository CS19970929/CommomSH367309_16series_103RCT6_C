#include "main.h"

CHARGERLOAD_FUNC ChargerLoad_Func;


//��ǹ���������ͣ�ȫϵ�ж��У�PA0�źţ��½���������
void AllSeriesDeal_Charger_ON(void) {
	static UINT8 su8_ChargerON_All_WakeFlag = 0;

	if(ChargerLoad_Func.bits.b1OFFDriver_Uvp || ChargerLoad_Func.bits.b1OFFDriver_DsgOcp ||ChargerLoad_Func.bits.b1OFFDriver_CBC) {
		switch(su8_ChargerON_All_WakeFlag) {
			case 0:
				ChargerLoad_Func.bits.b1ON_Charger_AllSeries = 0;
				su8_ChargerON_All_WakeFlag = 1;
				break;
				
			case 1:
				//��ǹ���룬����
				if(ChargerLoad_Func.bits.b1ON_Charger_AllSeries) {
					ChargerLoad_Func.bits.b1ON_Charger_AllSeries = 0;
					su8_ChargerON_All_WakeFlag = 2;
				}
				break;
				
			case 2:
				//����������ʹ����������
				System_OnOFF_Func.bits.b1OnOFF_MOS_Relay = 1;
				ChargerLoad_Func.bits.b1OFFDriver_Uvp = 0;
				ChargerLoad_Func.bits.b1OFFDriver_DsgOcp = 0;
				ChargerLoad_Func.bits.b1OFFDriver_CBC = 0;
				su8_ChargerON_All_WakeFlag = 0;
				break;
				
			default:
				su8_ChargerON_All_WakeFlag = 0;
				break;
		}
	}
}


//��ǹ�Ƴ����������
void AllSeriesDeal_Charger_OFF(void) {
	#if 0
	static UINT8 su8_ChargerOFF_All_WakeFlag = 0;
	static UINT16 su16_ChargerOFF_All_Tcnt = 0;

	if(ChargerLoad_Func.bits.b1OFFDriver_Ovp || ChargerLoad_Func.bits.b1OFFDriver_ChgOcp) {
		switch(su8_ChargerOFF_All_WakeFlag) {
			case 0:
				if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_RESET) {
					if(++su16_ChargerOFF_All_Tcnt >= 30) {
						su16_ChargerOFF_All_Tcnt = 0;
						su8_ChargerOFF_All_WakeFlag = 1;
					}
				}
				else {
					su16_ChargerOFF_All_Tcnt = 0;
				}
				break;
				
			case 1:
				//����������ʹ����������
				System_OnOFF_Func.bits.b1OnOFF_MOS_Relay = 1;
				ChargerLoad_Func.bits.b1OFFDriver_Ovp = 0;
				ChargerLoad_Func.bits.b1OFFDriver_ChgOcp = 0;
				su8_ChargerOFF_All_WakeFlag = 0;
				break;
				
			default:
				su8_ChargerOFF_All_WakeFlag = 0;
				break;
		}
	}
	#endif
}


void AllSeriesDeal_Sleep_Or_None(void) {
	static UINT16 su16_Sleep_Tcnt = 0;

	if(ChargerLoad_Func.bits.b1OFFDriver_UpperCom) {
		//��λ��ǿ�ƹز�������
	}

	if(ChargerLoad_Func.bits.b1OFFDriver_AFE_ERR || ChargerLoad_Func.bits.b1OFFDriver_EEPROM_ERR) {
		//Sleep_Mode.bits.b1ForceToSleep = 1;
		//ֱ�ӽ������ߣ����ﲻ��������һ�����˾ͺ�����
	}

	if(ChargerLoad_Func.bits.b1OFFDriver_Vdelta) {
		if(++su16_Sleep_Tcnt >= 300) {
			su16_Sleep_Tcnt = 0;
			SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1;
		}
	}

	//���Ӧ���Ǽ�⵽��ǹ�Ƴ���Ч���ģ�������ΪMOSû�������͵�ƽ(����ϵ��ŵ�ܣ���ô����ȱ��)��ֻ���и��½��ز���
	//����ȫϵ�п��ǣ�ֻ���ƶ������ߴ���
	//MOS���������ι�ѹ���󣬽Ӵ������С�
	if(ChargerLoad_Func.bits.b1OFFDriver_Ovp || ChargerLoad_Func.bits.b1OFFDriver_ChgOcp) {
		if(++su16_Sleep_Tcnt >= 300) {
			su16_Sleep_Tcnt = 0;
			SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1;
		}
	}
}


void Status_ChargerLoad(void) {
	if(ChargerLoad_Func.bits.b1ON_Charger) {
		//�����ǹ���ߣ���������
	}

	if(ChargerLoad_Func.bits.b1ON_Load) {
		//����������ߣ���������
	}		
}


void Init_Load(void) {
	//PB8_DRV-LOAD-DET
	
	//PF5_WK-LOAD
}


void Init_Charger(void) {
	//PF4_DRV-CHG-DET

	//PC6_WK-CHG
}


void Init_Charger_AllSeries(void) {
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��GPIOAʱ�ӣ���Ϊ�ǿ��������Ի���Ҫ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//��Ϊ�ǿ��������Ի���Ҫ

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				//ѡ��Ҫ�õ�GPIO���� 
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
}


//��һ�����ڶ�����һ����û��ʱ����
void Init_ChargerLoad_Det(void) {
	#ifdef _CHARGER_LOAD
	Init_Load();
	Init_Charger();
	#endif
	
	Init_Charger_AllSeries();
	ChargerLoad_Func.all = 0;
}


void App_ChargerLoad_Det(void) {
	if(0 == g_st_SysTimeFlag.bits.b1Sys1000msFlag2) {
		return;
	}
	//��ǹ���������ͣ�ȫϵ�ж��У�PA0�źţ��½���������
	AllSeriesDeal_Charger_ON();		//�������ε�ѹ�����ηŵ������CBC

	//��ǹ�Ƴ���������ͣ�ȫϵ�ж��У�PA0�źţ��͵�ƽ������
	AllSeriesDeal_Charger_OFF();	//�������ι�ѹ�����γ�����>>>>�ƶ�������

	//����������߽�����������
	AllSeriesDeal_Sleep_Or_None();	//AFE����EEPROM����ѹ����󱣻�(��Ϊ5min)Ϊ���ߣ���λ��ǿ�ƹز�������

	#ifdef _CHARGER_LOAD
	//ѡͨ���ܣ���ǹ���ߺ͸������ߣ�����һ����ϵ��BMS��һ����
	Status_ChargerLoad();
	#endif
}

