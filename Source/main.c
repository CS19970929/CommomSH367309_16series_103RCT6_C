#include "main.h"

UINT8 SeriesNum = 16;

// 不同串数维护的表格
// 中颖
const unsigned char SeriesSelect_AFE1[16][16] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	   // 1串
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	   // 2串
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	   // 3
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	   // 4
	{0, 1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	   // 5
	{0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	   // 6
	{0, 1, 2, 3, 4, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0},	   // 7
	{0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0},	   // 8
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 0, 0, 0, 0, 0, 0, 0},	   // 9
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0},	   // 10
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0, 0, 0, 0, 0},	   // 11
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 0, 0, 0},	   // 12
	{0, 1, 2, 3, 4, 5, 6, 7, 9, 9, 10, 11, 12, 0, 0, 0},   // 13
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0, 0},  // 14
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0}, // 15
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} // 16
};

void InitVar(void);
void InitDevice(void);
void InitSci(void);
void App_Sci(void);
void InitSystemWakeUp(void);

// main.c文件只留IO控制函数，别的都不能写在这里，不然太过庞大冗余。
int main(void)
{
	InitDevice(); // 初始化外设
	InitVar();	  // 初始化变量

	while (1)
	{
#if (defined _DEBUG_CODE)
		App_SysTime();
		// App_RTCSleepTest();
		// App_NormalSleepTest();
		SleepTest();
		App_SleepDeal();
		Feed_WatchDog;

#else
		App_SysTime();
		App_AFEGet();
		App_SH367309();

		App_Sci();
		App_AnlogCal();
		App_E2promDeal();
		App_CellBalance();
		App_Can();
		App_SleepDeal(); // 关闭这个功能的话，在InitVar()中System_OnOFF_Func相关置零，或者直接屏蔽
		App_SOC();
		App_LightBar();
		App_WarnCtrl();

		App_FlashUpdate();
		App_LogRecord();
		App_ProID_Deal();
		Feed_WatchDog;

// App_Heat_Cool_Ctrl();
// App_RTC();
// App_DAC();			//CBC用，不需要即刻改
// App_PWM();			//CBC用，不需要即刻改
// App_BlueTooth();		//这玩意后面大概率淘汰，白写了
#endif
	}
}

void InitDevice(void)
{
	SystemInit(); // HSE默认倍频到72MHz，如果没HSE切回HSI怎么处理目前还没了解

#if (defined _DEBUG_CODE)
	InitDelay();
	EnterSleepJudge();
	InitNVIC();
	InitIO();
	InitTimer();
	InitSystemWakeUp();
	InitE2PROM();
	InitData_SleepFunc();
	// Init_IWDG();

#else
	InitDelay();
	EnterSleepJudge();
	InitNVIC();
	InitIO();
	InitTimer();
	InitSystemWakeUp();
	InitE2PROM(); // 决定把这个放在前面，优先级提高，因为客户串口初始化，有可能要读其自己的数据
	InitCan();
	InitADC();
	InitSci();
	InitData_SOC();		  // 必须放在读完eeprom数据后面
	InitData_SleepFunc(); // 必须放在读完eeprom数据后面
	InitAFE1();

	Init_IWDG();
#endif
}

void InitVar(void)
{
	UINT16 i;

	// SystemMonitorResetData_EEPROM();							//这个函数的初始化默认需求功能修改了，要修改EEPROM的上电标志位
	InitSystemMonitorData_EEPROM();
	SystemStatus.bits.b1Status_MOS_DSG = CLOSE;

	// Switch功能

	// 总系统错误监控系统初始化
	for (i = 0; i < ERROR_NUM; ++i)
	{
		//*(&System_ErrFlag.u8ErrFlag_Com_AFE1+i)  =  0;		//在这里就被覆盖了，初始化错误体现不出来了
	}

	// 保护标志位初始化
	g_stCellInfoReport.unMdlFault_First.all = 0;
	g_stCellInfoReport.unMdlFault_Second.all = 0;
	g_stCellInfoReport.unMdlFault_Third.all = 0;
	// 当次保护记录初始化
	FaultPoint_First = 0;
	FaultPoint_Second = 0;
	FaultPoint_Third = 0;

	FaultPoint_First2 = 0;
	FaultPoint_Second2 = 0;
	FaultPoint_Third2 = 0;
	for (i = 0; i < Record_len; ++i)
	{
		Fault_record_First[i] = 0;
		Fault_record_Second[i] = 0;
		Fault_record_Third[i] = 0;

		Fault_record_First2[i] = 0;
		Fault_record_Second2[i] = 0;
		Fault_record_Third2[i] = 0;
	}
	Fault_Flag_Fisrt.all = 0;
	Fault_Flag_Second.all = 0;
	Fault_Flag_Third.all = 0;

	// 继电器驱动开启初始化							//不打算放在这里
	// RelayCtrl_Command = RELAY_PRE_DET;	//不打算放在这里
	// HeatCtrl_Command = ST_HEAT_DET_SELF;
	// CoolCtrl_Command = ST_COOL_DET_SELF;

	// 休眠相关

	// 这样写就不用管前面到底读出来还是复位了(在EEPROM很多个地方算)
	SeriesNum = OtherElement.u16Sys_SeriesNum;
	g_u32CS_Res_AFE = ((UINT32)OtherElement.u16Sys_CS_Res_Num * 1000) / OtherElement.u16Sys_CS_Res;

	SystemStatus.bits.b1StartUpBMS = 0; // 去掉开机时序

	SystemStatus.bits.b4Status_ProjectVer = 1;
	LogRecord_Flag.bits.Log_StartUp = 1;
}

void InitSystemWakeUp(void)
{
	// MCUO_SD_DRV_CHG = 0;
	MCUO_PWSV_STB = 0;

	MCUO_PWSV_LDO = 1;
	MCUO_PWSV_CTR = 1;

	MCUO_DRV_WLM_PW = 1;

	MCUO_AFE_SHIP = 0;
	MCUO_AFE_MODE = 0;
	MCUO_AFE_CTLC = 0; // 刚上电，默认高阻态，所以不慌AFE刚开机瞬间打开MOS
	__delay_ms(10);
}

void InitSci(void)
{
	InitUSART_CommonUpper();
	InitUSART_UartClient();
	InitUSART_LCD();
}

void App_Sci(void)
{
	App_CommonUpper();
	App_UartClient_Updata();
	App_USART_LCD();
}
