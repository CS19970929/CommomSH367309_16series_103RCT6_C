#include "main.h"


//UINT8 RTC_Faultcnt = 0;
__IO UINT8 TimeDisplay = 0;	//秒中断标志，进入秒中断时置1，当时间被刷新之后清0

struct RTC_ELEMENT RTC_time;

struct RTC_ELEMENT Systmtime = {2018,12,31,23,59,30};

UINT8 month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
#define	Days_in_month(a) (month_days[(a) - 1])


#ifdef _WEEK
void GregorianDay(struct RTC_ELEMENT * tm) {
	int leapsToDate;
	int lastYear;
	int day;
	int MonthOffset[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };

	lastYear = tm->RTC_Time_Year-1;
	leapsToDate = lastYear/4 - lastYear/100 + lastYear/400;   	//计算从公元元年到计数的前一年之中一共经历了多少个闰年


	if((tm->RTC_Time_Year%4 == 0)				 //如若计数的这一年为闰年，且计数的月份在2月之后，则日数加1，否则不加1
		&& ((tm->RTC_Time_Year%100 != 0) || (tm->RTC_Time_Year%400 == 0))
		&& (tm->RTC_Time_Month > 2)) {
		day = 1;
	} else {
		day = 0;
	}

	day += lastYear*365 + leapsToDate + MonthOffset[tm->RTC_Time_Month - 1] + tm->RTC_Time_Day;		//计算从公元元年元旦到计数日期一共有多少天

	tm->tm_wday=day%7;
}

#endif


//此函数为——Linux源码中的mktime算法
UINT32 mktimev(struct RTC_ELEMENT *tm) {
	if (0 >= (int) (tm->RTC_Time_Month -= 2)) {	/* 1..12 -> 11,12,1..10 */
		tm->RTC_Time_Month += 12;		/* Puts Feb last since it has leap day */
		tm->RTC_Time_Year -= 1;
	}

	return (((
		(UINT32) (tm->RTC_Time_Year/4 - tm->RTC_Time_Year/100 + tm->RTC_Time_Year/400 + 367*tm->RTC_Time_Month/12 + tm->RTC_Time_Day)+tm->RTC_Time_Year*365 - 719499
	    )*24 + tm->RTC_Time_Hour /* now have hours */
	  )*60 + tm->RTC_Time_Minute /* now have minutes */
	)*60 + tm->RTC_Time_Second; /* finally seconds */
}


void Second_To_RTCtime(UINT32 AllSecond, struct RTC_ELEMENT* RTCtime) {

	UINT32 i;
	UINT32 Second_res, Day;

	Day = AllSecond / SEC_DAY;			/* 有多少天 */
	Second_res = AllSecond % SEC_DAY;			/* 今天的时间，单位s */

	/* Hours, minutes, seconds are easy */
	RTCtime->RTC_Time_Hour = Second_res / 3600;
	RTCtime->RTC_Time_Minute = (Second_res % 3600) / 60;
	RTCtime->RTC_Time_Second = (Second_res % 3600) % 60;

	for (i = UNIX_TIME_YEAR; Day >= Days_in_year(i); i++) {		//算出当前年份，起始的计数年份为1970年
		Day -= Days_in_year(i);
	}
	i %= 100;								//只保留后两位
	RTCtime->RTC_Time_Year = (UINT8)i;

	if (Leapyear(RTCtime->RTC_Time_Year)) {		//计算当前的月份
		Days_in_month(2) = 29;
	}
	for (i = 1; Day >= Days_in_month(i); i++) {
		Day -= Days_in_month(i);
	}
	Days_in_month(2) = 28;
	RTCtime->RTC_Time_Month = (UINT8)i;

	RTCtime->RTC_Time_Day = (UINT8)Day + 1;				//计算当前日期

	//GregorianDay(tm);						//计算星期几

}

void Get_RTC_Time(void) {
	UINT32 u32BJ_SecondTimeVar;

	u32BJ_SecondTimeVar = RTC_GetCounter() + TIME_ZOOM;
	Second_To_RTCtime(u32BJ_SecondTimeVar, &RTC_time);		//把定时器的值转换为北京时间
}


//此函数为——Linux源码中的mktime算法修改为——易读懂型
//1970年1月1日0时为UNIX TIME的纪元时间
UINT32 Seccond_Cal(struct RTC_ELEMENT *RTC_t) {
	UINT32 Y_day,M_day,D_day;
	UINT32 X_day,T_sec;
	
	if (0 >= (int) (RTC_t->RTC_Time_Month -= 2)) {		//前两个月被推到上一年去
		RTC_t->RTC_Time_Month += 12;
		RTC_t->RTC_Time_Year -= 1;
	}
	Y_day = (RTC_t->RTC_Time_Year - 1) * 365 + RTC_t->RTC_Time_Year / 4 - RTC_t->RTC_Time_Year / 100 + RTC_t->RTC_Time_Year / 400;
	M_day = 367 * RTC_t->RTC_Time_Month / 12 - 30 + 59;
	D_day = RTC_t->RTC_Time_Day - 1;
	X_day = (UINT32)(Y_day + M_day + D_day) - 719162;		//719162为0年0月0日0时到纪元时间的天数
	T_sec = ((X_day * 24 + RTC_t->RTC_Time_Hour) * 60 + RTC_t->RTC_Time_Minute) * 60 + RTC_t->RTC_Time_Second;

	return T_sec;
}


UINT8 RTC_ClockConfig(void) {
	__IO UINT16 StartUpCounter = 0, HSEStatus = 0;
	INT8 result = 0;
	PWR_BackupAccessCmd(ENABLE);    						//允许访问RTC
	BKP_DeInit();											//好像少了这句话导致各种wait函数卡死
	RCC_LSEConfig(RCC_LSE_ON);								//使能外部LSE晶振，RCC_LSE_Bypass旁路的意思应该是使能这个LSE时钟，但是单片机不用，外围电路用?
	do {
		HSEStatus = RCC_GetFlagStatus(RCC_FLAG_LSERDY);
		StartUpCounter++;
	}while((HSEStatus == RESET)&&(StartUpCounter < LSE_START_TIMEOUT));		//等待到 LSE 预备
	
	if(StartUpCounter < LSE_START_TIMEOUT) {
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);					//把RTC 时钟源配置为LSE
		RCC_RTCCLKCmd(ENABLE);									//使能RTC时钟
		RTC_WaitForSynchro();									//等待 RTC APB 寄存器同步
		RTC_WaitForLastTask();									//确保上一次 RTC 的操作完成
		RTC_ITConfig(RTC_IT_SEC, ENABLE);						//使能 RTC 秒中断
		RTC_WaitForLastTask();									//确保上一次 RTC 的操作完成
		RTC_SetPrescaler(LSE_FREQUENT); 						//设置 RTC 分频: 使 RTC 周期为1s
																//RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) = 1HZ
		RTC_WaitForLastTask();									//确保上一次 RTC 的操作完成
	}
	else {
		//++RTC_Faultcnt;											//RTC错误单数为LSE出错
		System_ERROR_UserCallback(ERROR_LSE);
		RCC_LSICmd(ENABLE); 									//使能 LSI 振荡
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET); 	//等待到 LSI 预备
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI); 				//把RTC 时钟源配置为LSI
		RCC_RTCCLKCmd(ENABLE);									//使能RTC时钟
		RTC_WaitForSynchro();									//等待 RTC APB 寄存器同步	
		RTC_WaitForLastTask();									//确保上一次 RTC 的操作完成
		RTC_ITConfig(RTC_IT_SEC, ENABLE);						//使能 RTC 秒中断
		RTC_WaitForLastTask();									//确保上一次 RTC 的操作完成
		RTC_SetPrescaler(40000-1); 								//RTC period = RTCCLK/RTC_PR = (40 KHz)/(40000-1+1) = 1HZ
		RTC_WaitForLastTask();									//确保上一次 RTC 的操作完成
		result = 1;
	}
	
	return result;
}

void RTC_TimeConfig(void) {
	//GregorianDay(tm);			//计算星期
	RTC_SetCounter(Seccond_Cal(&Systmtime)-TIME_ZOOM);			//由日期计算时间戳并写入到RTC计数寄存器
	RTC_WaitForLastTask();

}


void RTC_AlarmConfig(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

	#if 0		//不在这里初始化
	PWR_BackupAccessCmd(ENABLE);											//后备域解锁
	RTC_ITConfig(RTC_IT_SEC, DISABLE);										//禁止实时时钟秒中断
	RTC_SetAlarm(RTC_GetCounter() + ALARM_TIME_SEC);						//唤醒时间
  	RTC_WaitForLastTask();
  	RTC_ITConfig(RTC_FLAG_ALR, ENABLE);										//打开闹钟中断
	#endif
	
	//------------EXTI17 配置 -------------------
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); 
	//------------设置 中断-------------------
    NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);    
}


void RTC_NVIC_Config(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


//RTC唤醒时间设置，
void RTC_WKTimeConfig(void) {
	PWR_BackupAccessCmd(ENABLE);											//后备域解锁
	RTC_ITConfig(RTC_IT_SEC, DISABLE);										//禁止实时时钟秒中断
	RTC_SetAlarm(RTC_GetCounter() + (UINT32)OtherElement.u16Sleep_RTC_WakeUpTime*60);//唤醒时间	
	//RTC_SetAlarm(RTC_GetCounter() + ALARM_TIME_SEC);						//唤醒时间
  	RTC_WaitForLastTask();
  	RTC_ITConfig(RTC_FLAG_ALR, ENABLE);										//打开闹钟中断
}


void InitRTC(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); 		//使能PWR外设时钟，待机模式，RTC，看门狗
	RTC_ClockConfig();											//RTC时钟配置
	
	if (BKP_ReadBackupRegister(BKP_DR1) != RTC_BKP_DATA) {		//读取备份里面的值是否被写过。
		RTC_TimeConfig();
		BKP_WriteBackupRegister(BKP_DR1, RTC_BKP_DATA);
		RTC_AlarmConfig();
	}
	else {	//以下这段话需要吗？
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {			//这是啥
			//("\r\n Power On Reset occurred....\n\r");
			//++RTC_Faultcnt;
		}
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {		//这是啥
			//printf("\r\n External Reset occurred....\n\r");
		}
		RTC_ClearFlag(RTC_FLAG_ALR);		//Clear the RTC Alarm Flag
		EXTI_ClearITPendingBit(EXTI_Line17);
		RCC_ClearFlag();
		//RTC_TimeShow();				//Display the RTC Time and Alarm，这个后面会用到
		//RTC_AlarmShow();
	}
	RTC_NVIC_Config();
}


void App_RTC(void) {
	if(0 == TimeDisplay) {
		return;
	}
	Get_RTC_Time(); 		
	TimeDisplay = 0;

}


void RTCAlarm_IRQHandler(void) {
   EXTI_ClearITPendingBit(EXTI_Line17);			//两个都要，不然会死机
   //RTC_ClearITPendingBit(RTC_IT_ALR);			//不需要
   //RTC_ITConfig(RTC_FLAG_ALR, DISABLE);
   if(FLASH_COMPLETE == FlashWriteOneHalfWord(FLASH_ADDR_SH367309_FLAG, FLASH_309_RTC_RTC_VALUE)) {
	   //RTC唤醒，则写标志位
   }
}


void RTC_IRQHandler(void) {
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET) {
		RTC_ClearITPendingBit(RTC_IT_SEC);		//Clear the RTC Second interrupt
		TimeDisplay = 1;						//Enable time update
		RTC_WaitForLastTask();	    			//Wait until last write operation on RTC registers has finished
	}

	if (RTC_GetITStatus(RTC_FLAG_ALR) != RESET) {	//成了！CNM！我太难了老铁
		RTC_ClearITPendingBit(RTC_FLAG_ALR);		//会运行到这个地方，这里也要把这个标志位去除，不然卡在这里无法唤醒
		RTC_WaitForLastTask();
	}
}

