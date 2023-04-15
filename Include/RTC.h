#ifndef RTC_H
#define RTC_H


#define RTC_BKP_DATA        0xA5A5

#define LSE_START_TIMEOUT   ((uint16_t)0x5000) /*!< Time out for LSE start up */
#define LSE_FREQUENT		32767

#define TIME_ZOOM			(8*60*60)			//相对中时区的北京时间
#define	UNIX_TIME_YEAR		1970				//纪元时间
#define SEC_DAY				86400L          	//一天有多少s 

#define ALARM_TIME_SEC		(UINT32)10       //s，休眠唤醒时间 

#define	Leapyear(year)		((year) % 4 == 0)	//反正不会超过2100，没问题
#define	Days_in_year(a) 	(Leapyear(a) ? 366 : 365)

/*
struct RTC_TIME {
	UINT16 RTC_year;		//为什么这个为16位是因为计算中需要16位，但上传不需要
	UINT8 RTC_mon;
	UINT8 RTC_day;
	UINT8 RTC_hour;
	UINT8 RTC_min;
	UINT8 RTC_sec;
};
*/

struct RTC_ELEMENT {
	UINT16  RTC_Time_Year;
	UINT16  RTC_Time_Month;
	UINT16  RTC_Time_Day;
	UINT16  RTC_Time_Hour;
	
	UINT16  RTC_Time_Minute;
	UINT16  RTC_Time_Second;
	UINT16  RTC_Alarm_Year;
	UINT16  RTC_Alarm_Month;
	
	UINT16  RTC_Alarm_Day;
	UINT16  RTC_Alarm_Hour;
	UINT16  RTC_Alarm_Minute;
	UINT16  RTC_Alarm_Second;
};

#define RTC_element_min     {0, 1, 1, 0,\
								0, 0, 0, 0,\
								0, 0, 0, 0}

#define RTC_element_default {18, 10, 31, 23,\
								59, 30, 0, 0,\
								0, 9, 1, 4}

#define RTC_element_max     {99, 12, 31, 23,\
								59, 59, 99, 12,\
								31, 23, 59, 59}

extern struct RTC_ELEMENT RTC_time;
//extern UINT8 RTC_Faultcnt;


void RTC_WKTimeConfig(void);
void InitRTC(void);
void App_RTC(void);

#endif	/* RTC_H */

