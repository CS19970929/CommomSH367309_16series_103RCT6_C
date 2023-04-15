#ifndef SLEEPFUNCTION_H
#define SLEEPFUNCTION_H

#include "stm32f10x.h"

//一个地址能存16位一个字
#define FLASH_ADDR_SLEEP_FLAG           0x0803F800			//休眠关键指令，2K
#define FLASH_NORMAL_SLEEP_VALUE    	((UINT16)0x1234)
#define FLASH_DEEP_SLEEP_VALUE    		((UINT16)0x1235)
#define FLASH_HICCUP_SLEEP_VALUE    	((UINT16)0x1236)
#define FLASH_SLEEP_RESET_VALUE    		((UINT16)0xFFFF)


//砍掉这个功能，化繁为简
/*
#define FLASH_ADDR_RTC_WK_TIME 			0x0803C000			//RTC休眠唤醒时间参数
#define FLASH_VALUE_RTC_WK_TIME_RESET	((UINT16)0xFFFF)
#define FLASH_ADDR_SLEEP_ELEMENT 		0x0803C800			//休眠相关参数
#define FLASH_VALUE_SLEEP_ELEMENT_RESET	((UINT16)0xFFFF)
*/

//以下这个联合体标志位其实可以用一个状态机替代(因任何时间SleepMode只有一种情况而不会出现多种情况)
//但是我决定不改，因为未来项目有可能出现多种情况同时进行
typedef union SLEEP_MODE{
    UINT16   all;
    struct SleepModeFlagBit {
		UINT8 b1TestSleep        	:1;		//b1ForceToSleep可以代替，取消
		UINT8 b1NormalSleep_L1      :1;
		UINT8 b1NormalSleep_L2      :1;
		UINT8 b1NormalSleep_L3      :1;
		
		UINT8 b1ForceToSleep_L1     :1;		//外部操控进入第一级休眠
        UINT8 b1ForceToSleep_L2     :1;		//外部操控进入第二级休眠
		UINT8 b1ForceToSleep_L3     :1;		//外部操控进入第三级休眠
        UINT8 b1ForceToSleep_L1_Out :1;		//外部操控第一级休眠退出打嗝模式标志位

		UINT8 b1OverCurSleep        :1;
        UINT8 b1OverVdeltaSleep     :1;
		UINT8 b1CBCSleep       		:1;
		UINT8 b1VcellOVP			:1;
		
		UINT8 b1VcellUVP			:1;
		UINT8 Res2					:3;		//8L必须得反过来才和STM32一致
     }bits;
}SleepMode;


typedef struct SLEEP_ELEMENT {
	//只需要赋值一次的参数
	UINT16 u16_TimeRTC_First;		//min，第一次进入RTC休眠时间，0表示不进入休眠
	UINT16 u16_TimeRTC_Hiccup;		//min，打嗝模式进入RTC休眠时间
	UINT16 u16_RTC_WakeUpTime;		//min，RTC唤醒时间	
    UINT16 u16_VNormal;      		//mV
	UINT16 u16_TimeNormal;			//min
    UINT16 u16_Vlow;     			//mV
	UINT16 u16_TimeVlow;			//min
	UINT16 u16_VirCur_Chg;     		//A*10
    UINT16 u16_VirCur_Dsg;    		//A*10
	//只需要赋值一次的参数，新接口
	UINT16 u16_Force_RTC_First;		//未使用，min，第一次进入RTC休眠时间，0表示不进入休眠
	UINT16 u16_Force_RTC_Hiccup;	//未使用，min，打嗝模式进入RTC休眠时间
	UINT16 u16_Force_RTC_WKtime;	//未使用，min，RTC唤醒时间
	UINT16 u16_Force_Normal_T;		//min
	UINT16 u16_Force_Vlow_T;		//min


	//需要不间断赋值的参数
	UINT8  u8_ExtComCnt;			//外部通讯次数
	UINT16 u16_VCellMax;        	//mV
	UINT16 u16_VCellMin;        	//mV
	UINT16 u16_CurChg;				//A*10
	UINT16 u16_CurDsg;				//A*10

	//能获取结果信息的参数
	//Driver_OnOFF_Etc MosRelay_Status;	//还是决定省空间，目标是8L和103均能用
	UINT8 u8_ToSleepFlag;			//当为1时，1s后进入休眠，别的时间为0
	
	//信息交换区
	volatile SleepMode Sleep_Mode;		//用于外部控制进入休眠标志位
}Sleep_Element;


extern UINT16 aaaa1;
extern UINT16 aaaa2;
extern UINT16 aaaa3;
extern UINT16 aaaa4;
extern Sleep_Element SleepElement;

void IsSleepStartUp(void);
void SleepCtrl(UINT8 TimeBase_1s, UINT8 OnOFF_Ctrl);


//因为需要外部的RTC两个中断，所以干脆初始化也调用了，不集成
extern void InitRTC(void);

extern void InitWakeUp_RTCMode(void);
extern void InitWakeUp_NormalMode(void);
extern void InitWakeUp_DeepMode(void);


extern void IOstatus_RTCMode(void);
extern void IOstatus_NormalMode(void);
extern void IOstatus_DeepMode(void);

extern FLASH_Status FlashWriteOneHalfWord(uint32_t StartAddr,uint16_t Buffer);
extern UINT16 FlashReadOneHalfWord(UINT32 faddr);

#endif	/* SLEEPFUNCTION_H */



/*
1，过压，低压不一定是进入RTC模式
>>进入深度休眠
2，留出直接进入休眠，进入三级休眠模式(快速还是按照时间呢？)的接口
>>已留，目标时间也能改
3，直接进入RTC
>>同问题2
4，直接进入深度休眠？
>>同问题2
5，别的保护状况如过流，压差过大，驱动功能被取消情况，进入休眠？
>>与问题2不扣在一起，再集成自己的逻辑
3，Test休眠要加上，测试调试用
>>不使用，b1ForceToSleep，取消

*/
