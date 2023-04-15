#ifndef MAIN_H
#define MAIN_H

//#define USE_STDPERIPH_DRIVER	//没有这个报一大堆assert_param()函数的错误，要在工程文件中配置
#include <math.h>
#include "stm32f10x.h"
#include "stm32f10x_it.h"			//里面有一些硬件错误之类的中断，还是需要的
#include "string.h"

#include "DataDeal.h"				//放在Sci_Upper.h前面才行
#include "Sci_Upper.h"
#include "System_Init.h"
#include "System_Monitor.h"
#include "EEPROM.h"
#include "Fault.h"
#include "SOC.h"
#include "ADC.h"
#include "RTC.h"
#include "PubFunc.h"
//#include "Can_GW_SH.h"
#include "Can_HDX.h"
#include "I2C_AFE1.h"
#include "Flash.h"
#include "Cell_balance.h"
#include "SleepDeal.h"
#include "Uart_Client.h"
#include "LCD.h"
#include "Heat_Cool.h"
#include "TestLib.h"
#include "LED_Buzzer.h"
#include "ProductionID.h"
#include "SH367309_Func.h"
#include "ChargerLoadFunc.h"
#include "SH367309_DataDeal.h"
#include "LogRecord.h"


#define UPDNLMT16(Var,Max,Min)	{(Var)=((Var)>=(Max))?(Max):(Var);(Var)=((Var)<=(Min))?(Min):(Var);}
#define S2U(x)   (*((volatile UINT16*)(&(x))))
#define MCU_RESET()	NVIC_SystemReset()
#define CRC_KEY 	7
#define I2C_RW_W	0
#define I2C_RW_R	1

typedef enum _IO_STATUS {
OPEN = 1, CLOSE = 0
}IO_STATUS;

typedef enum _BOOL {
FALSE = 0, TRUE
}BOOL;


//10ms时基计数器
#define DELAYB10MS_0MS       ((UINT16)0)            //0ms
#define DELAYB10MS_30MS      ((UINT16)3)            //30ms
#define DELAYB10MS_50MS      ((UINT16)5)            //50ms
#define DELAYB10MS_100MS     ((UINT16)10)           //100ms
#define DELAYB10MS_200MS     ((UINT16)20)           //200ms
#define DELAYB10MS_250MS     ((UINT16)25)           //250ms
#define DELAYB10MS_500MS     ((UINT16)50)           //500ms
#define DELAYB10MS_1S        ((UINT16)100)          //1s
#define DELAYB10MS_1S5       ((UINT16)150)          //1.5s
#define DELAYB10MS_2S        ((UINT16)200)          //2s
#define DELAYB10MS_2S5       ((UINT16)250)          //2.5s
#define DELAYB10MS_3S        ((UINT16)300)          //3s
#define DELAYB10MS_4S        ((UINT16)400)          // 4s
#define DELAYB10MS_5S        ((UINT16)500)          // 5s
#define DELAYB10MS_10S       ((UINT16)1000)         //10s
#define DELAYB10MS_30S       ((UINT16)3000)         //30s
#define DELAYB10MS_2MIN      ((UINT16)12000)         //30s


//#define _DEBUG_CODE
#define _IAP					//如果工程文件地址变的话，加上这个和烧IAP的project才行
//#define _SLEEP_WITH_CURRENT

//#define _DI_SWITCH_SYS_ONOFF	//DI开关用作休眠
//#define _DI_SWITCH_DSG_ONOFF	//DI开关用作控制放电接触器或者MOS


/*============Sci===========*/
//每个串口只能选一种通讯
#define _COMMOM_UPPER_SCI1
//#define _CLIENT_SCI1
//#define _LCD_SCI1


#define _COMMOM_UPPER_SCI2
//#define _CLIENT_SCI2
//#define _LCD_SCI2


//这个版本没有串口3
//#define _COMMOM_UPPER_SCI3
//#define _CLIENT_SCI3
//#define _LCD_SCI3
/*============Sci===========*/

void InitSystemWakeUp(void);


extern const unsigned char SeriesSelect_AFE1[16][16];
extern UINT8 SeriesNum;

#endif	/* MAIN_H */
