#include "main.h"


//UINT8 RTC_Faultcnt = 0;
__IO UINT8 TimeDisplay = 0;	//���жϱ�־���������ж�ʱ��1����ʱ�䱻ˢ��֮����0

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
	leapsToDate = lastYear/4 - lastYear/100 + lastYear/400;   	//����ӹ�ԪԪ�굽������ǰһ��֮��һ�������˶��ٸ�����


	if((tm->RTC_Time_Year%4 == 0)				 //������������һ��Ϊ���꣬�Ҽ������·���2��֮����������1�����򲻼�1
		&& ((tm->RTC_Time_Year%100 != 0) || (tm->RTC_Time_Year%400 == 0))
		&& (tm->RTC_Time_Month > 2)) {
		day = 1;
	} else {
		day = 0;
	}

	day += lastYear*365 + leapsToDate + MonthOffset[tm->RTC_Time_Month - 1] + tm->RTC_Time_Day;		//����ӹ�ԪԪ��Ԫ������������һ���ж�����

	tm->tm_wday=day%7;
}

#endif


//�˺���Ϊ����LinuxԴ���е�mktime�㷨
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

	Day = AllSecond / SEC_DAY;			/* �ж����� */
	Second_res = AllSecond % SEC_DAY;			/* �����ʱ�䣬��λs */

	/* Hours, minutes, seconds are easy */
	RTCtime->RTC_Time_Hour = Second_res / 3600;
	RTCtime->RTC_Time_Minute = (Second_res % 3600) / 60;
	RTCtime->RTC_Time_Second = (Second_res % 3600) % 60;

	for (i = UNIX_TIME_YEAR; Day >= Days_in_year(i); i++) {		//�����ǰ��ݣ���ʼ�ļ������Ϊ1970��
		Day -= Days_in_year(i);
	}
	i %= 100;								//ֻ��������λ
	RTCtime->RTC_Time_Year = (UINT8)i;

	if (Leapyear(RTCtime->RTC_Time_Year)) {		//���㵱ǰ���·�
		Days_in_month(2) = 29;
	}
	for (i = 1; Day >= Days_in_month(i); i++) {
		Day -= Days_in_month(i);
	}
	Days_in_month(2) = 28;
	RTCtime->RTC_Time_Month = (UINT8)i;

	RTCtime->RTC_Time_Day = (UINT8)Day + 1;				//���㵱ǰ����

	//GregorianDay(tm);						//�������ڼ�

}

void Get_RTC_Time(void) {
	UINT32 u32BJ_SecondTimeVar;

	u32BJ_SecondTimeVar = RTC_GetCounter() + TIME_ZOOM;
	Second_To_RTCtime(u32BJ_SecondTimeVar, &RTC_time);		//�Ѷ�ʱ����ֵת��Ϊ����ʱ��
}


//�˺���Ϊ����LinuxԴ���е�mktime�㷨�޸�Ϊ�����׶�����
//1970��1��1��0ʱΪUNIX TIME�ļ�Ԫʱ��
UINT32 Seccond_Cal(struct RTC_ELEMENT *RTC_t) {
	UINT32 Y_day,M_day,D_day;
	UINT32 X_day,T_sec;
	
	if (0 >= (int) (RTC_t->RTC_Time_Month -= 2)) {		//ǰ�����±��Ƶ���һ��ȥ
		RTC_t->RTC_Time_Month += 12;
		RTC_t->RTC_Time_Year -= 1;
	}
	Y_day = (RTC_t->RTC_Time_Year - 1) * 365 + RTC_t->RTC_Time_Year / 4 - RTC_t->RTC_Time_Year / 100 + RTC_t->RTC_Time_Year / 400;
	M_day = 367 * RTC_t->RTC_Time_Month / 12 - 30 + 59;
	D_day = RTC_t->RTC_Time_Day - 1;
	X_day = (UINT32)(Y_day + M_day + D_day) - 719162;		//719162Ϊ0��0��0��0ʱ����Ԫʱ�������
	T_sec = ((X_day * 24 + RTC_t->RTC_Time_Hour) * 60 + RTC_t->RTC_Time_Minute) * 60 + RTC_t->RTC_Time_Second;

	return T_sec;
}


UINT8 RTC_ClockConfig(void) {
	__IO UINT16 StartUpCounter = 0, HSEStatus = 0;
	INT8 result = 0;
	PWR_BackupAccessCmd(ENABLE);    						//�������RTC
	BKP_DeInit();											//����������仰���¸���wait��������
	RCC_LSEConfig(RCC_LSE_ON);								//ʹ���ⲿLSE����RCC_LSE_Bypass��·����˼Ӧ����ʹ�����LSEʱ�ӣ����ǵ�Ƭ�����ã���Χ��·��?
	do {
		HSEStatus = RCC_GetFlagStatus(RCC_FLAG_LSERDY);
		StartUpCounter++;
	}while((HSEStatus == RESET)&&(StartUpCounter < LSE_START_TIMEOUT));		//�ȴ��� LSE Ԥ��
	
	if(StartUpCounter < LSE_START_TIMEOUT) {
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);					//��RTC ʱ��Դ����ΪLSE
		RCC_RTCCLKCmd(ENABLE);									//ʹ��RTCʱ��
		RTC_WaitForSynchro();									//�ȴ� RTC APB �Ĵ���ͬ��
		RTC_WaitForLastTask();									//ȷ����һ�� RTC �Ĳ������
		RTC_ITConfig(RTC_IT_SEC, ENABLE);						//ʹ�� RTC ���ж�
		RTC_WaitForLastTask();									//ȷ����һ�� RTC �Ĳ������
		RTC_SetPrescaler(LSE_FREQUENT); 						//���� RTC ��Ƶ: ʹ RTC ����Ϊ1s
																//RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) = 1HZ
		RTC_WaitForLastTask();									//ȷ����һ�� RTC �Ĳ������
	}
	else {
		//++RTC_Faultcnt;											//RTC������ΪLSE����
		System_ERROR_UserCallback(ERROR_LSE);
		RCC_LSICmd(ENABLE); 									//ʹ�� LSI ��
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET); 	//�ȴ��� LSI Ԥ��
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI); 				//��RTC ʱ��Դ����ΪLSI
		RCC_RTCCLKCmd(ENABLE);									//ʹ��RTCʱ��
		RTC_WaitForSynchro();									//�ȴ� RTC APB �Ĵ���ͬ��	
		RTC_WaitForLastTask();									//ȷ����һ�� RTC �Ĳ������
		RTC_ITConfig(RTC_IT_SEC, ENABLE);						//ʹ�� RTC ���ж�
		RTC_WaitForLastTask();									//ȷ����һ�� RTC �Ĳ������
		RTC_SetPrescaler(40000-1); 								//RTC period = RTCCLK/RTC_PR = (40 KHz)/(40000-1+1) = 1HZ
		RTC_WaitForLastTask();									//ȷ����һ�� RTC �Ĳ������
		result = 1;
	}
	
	return result;
}

void RTC_TimeConfig(void) {
	//GregorianDay(tm);			//��������
	RTC_SetCounter(Seccond_Cal(&Systmtime)-TIME_ZOOM);			//�����ڼ���ʱ�����д�뵽RTC�����Ĵ���
	RTC_WaitForLastTask();

}


void RTC_AlarmConfig(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

	#if 0		//���������ʼ��
	PWR_BackupAccessCmd(ENABLE);											//�������
	RTC_ITConfig(RTC_IT_SEC, DISABLE);										//��ֹʵʱʱ�����ж�
	RTC_SetAlarm(RTC_GetCounter() + ALARM_TIME_SEC);						//����ʱ��
  	RTC_WaitForLastTask();
  	RTC_ITConfig(RTC_FLAG_ALR, ENABLE);										//�������ж�
	#endif
	
	//------------EXTI17 ���� -------------------
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); 
	//------------���� �ж�-------------------
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


//RTC����ʱ�����ã�
void RTC_WKTimeConfig(void) {
	PWR_BackupAccessCmd(ENABLE);											//�������
	RTC_ITConfig(RTC_IT_SEC, DISABLE);										//��ֹʵʱʱ�����ж�
	RTC_SetAlarm(RTC_GetCounter() + (UINT32)OtherElement.u16Sleep_RTC_WakeUpTime*60);//����ʱ��	
	//RTC_SetAlarm(RTC_GetCounter() + ALARM_TIME_SEC);						//����ʱ��
  	RTC_WaitForLastTask();
  	RTC_ITConfig(RTC_FLAG_ALR, ENABLE);										//�������ж�
}


void InitRTC(void) {
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); 		//ʹ��PWR����ʱ�ӣ�����ģʽ��RTC�����Ź�
	RTC_ClockConfig();											//RTCʱ������
	
	if (BKP_ReadBackupRegister(BKP_DR1) != RTC_BKP_DATA) {		//��ȡ���������ֵ�Ƿ�д����
		RTC_TimeConfig();
		BKP_WriteBackupRegister(BKP_DR1, RTC_BKP_DATA);
		RTC_AlarmConfig();
	}
	else {	//������λ���Ҫ��
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {			//����ɶ
			//("\r\n Power On Reset occurred....\n\r");
			//++RTC_Faultcnt;
		}
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {		//����ɶ
			//printf("\r\n External Reset occurred....\n\r");
		}
		RTC_ClearFlag(RTC_FLAG_ALR);		//Clear the RTC Alarm Flag
		EXTI_ClearITPendingBit(EXTI_Line17);
		RCC_ClearFlag();
		//RTC_TimeShow();				//Display the RTC Time and Alarm�����������õ�
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
   EXTI_ClearITPendingBit(EXTI_Line17);			//������Ҫ����Ȼ������
   //RTC_ClearITPendingBit(RTC_IT_ALR);			//����Ҫ
   //RTC_ITConfig(RTC_FLAG_ALR, DISABLE);
   if(FLASH_COMPLETE == FlashWriteOneHalfWord(FLASH_ADDR_SH367309_FLAG, FLASH_309_RTC_RTC_VALUE)) {
	   //RTC���ѣ���д��־λ
   }
}


void RTC_IRQHandler(void) {
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET) {
		RTC_ClearITPendingBit(RTC_IT_SEC);		//Clear the RTC Second interrupt
		TimeDisplay = 1;						//Enable time update
		RTC_WaitForLastTask();	    			//Wait until last write operation on RTC registers has finished
	}

	if (RTC_GetITStatus(RTC_FLAG_ALR) != RESET) {	//���ˣ�CNM����̫��������
		RTC_ClearITPendingBit(RTC_FLAG_ALR);		//�����е�����ط�������ҲҪ�������־λȥ������Ȼ���������޷�����
		RTC_WaitForLastTask();
	}
}

