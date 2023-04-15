#include "main.h"

CHARGERLOAD_FUNC ChargerLoad_Func;


//电枪插入解除类型，全系列都有，PA0信号，下降沿起作用
void AllSeriesDeal_Charger_ON(void) {
	static UINT8 su8_ChargerON_All_WakeFlag = 0;

	if(ChargerLoad_Func.bits.b1OFFDriver_Uvp || ChargerLoad_Func.bits.b1OFFDriver_DsgOcp ||ChargerLoad_Func.bits.b1OFFDriver_CBC) {
		switch(su8_ChargerON_All_WakeFlag) {
			case 0:
				ChargerLoad_Func.bits.b1ON_Charger_AllSeries = 0;
				su8_ChargerON_All_WakeFlag = 1;
				break;
				
			case 1:
				//电枪插入，则解除
				if(ChargerLoad_Func.bits.b1ON_Charger_AllSeries) {
					ChargerLoad_Func.bits.b1ON_Charger_AllSeries = 0;
					su8_ChargerON_All_WakeFlag = 2;
				}
				break;
				
			case 2:
				//作出操作，使能驱动功能
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


//电枪移除，解除类型
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
				//作出操作，使能驱动功能
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
		//上位机强制关不作处理
	}

	if(ChargerLoad_Func.bits.b1OFFDriver_AFE_ERR || ChargerLoad_Func.bits.b1OFFDriver_EEPROM_ERR) {
		//Sleep_Mode.bits.b1ForceToSleep = 1;
		//直接进入休眠，这里不作处理，万一起来了就好了呢
	}

	if(ChargerLoad_Func.bits.b1OFFDriver_Vdelta) {
		if(++su16_Sleep_Tcnt >= 300) {
			su16_Sleep_Tcnt = 0;
			SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1;
		}
	}

	//这个应该是检测到电枪移除有效果的，但是因为MOS没法产生低电平(必须断掉放电管，这么做有缺陷)，只能有个下降沿产生
	//基于全系列考虑，只能移动到休眠处理。
	//MOS不会有三次过压现象，接触器都有。
	if(ChargerLoad_Func.bits.b1OFFDriver_Ovp || ChargerLoad_Func.bits.b1OFFDriver_ChgOcp) {
		if(++su16_Sleep_Tcnt >= 300) {
			su16_Sleep_Tcnt = 0;
			SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1;
		}
	}
}


void Status_ChargerLoad(void) {
	if(ChargerLoad_Func.bits.b1ON_Charger) {
		//如果电枪在线，作出处理
	}

	if(ChargerLoad_Func.bits.b1ON_Load) {
		//如果负载在线，作出处理
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
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能GPIOA时钟，因为是开机，所以还是要
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//因为是开机，所以还是要

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				//选择要用的GPIO引脚 
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


//第一个，第二个不一定有没有时屏蔽
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
	//电枪插入解除类型，全系列都有，PA0信号，下降沿起作用
	AllSeriesDeal_Charger_ON();		//包括三次低压，三次放电过流，CBC

	//电枪移除，解除类型，全系列都有，PA0信号，低电平起作用
	AllSeriesDeal_Charger_OFF();	//包括三次过压，三次充电过流>>>>移动到休眠

	//不作处理或者进入休眠类型
	AllSeriesDeal_Sleep_Or_None();	//AFE错误，EEPROM错误，压差过大保护(均为5min)为休眠，上位机强制关不作处理

	#ifdef _CHARGER_LOAD
	//选通功能，电枪在线和负载在线，另外一套体系，BMS不一定有
	Status_ChargerLoad();
	#endif
}

