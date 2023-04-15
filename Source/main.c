#include "main.h"

UINT8 SeriesNum = 16;

// ��ͬ����ά���ı��
// ��ӱ
const unsigned char SeriesSelect_AFE1[16][16] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	   // 1��
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	   // 2��
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

// main.c�ļ�ֻ��IO���ƺ�������Ķ�����д�������Ȼ̫���Ӵ����ࡣ
int main(void)
{
	InitDevice(); // ��ʼ������
	InitVar();	  // ��ʼ������

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
		App_SleepDeal(); // �ر�������ܵĻ�����InitVar()��System_OnOFF_Func������㣬����ֱ������
		App_SOC();
		App_LightBar();
		App_WarnCtrl();

		App_FlashUpdate();
		App_LogRecord();
		App_ProID_Deal();
		Feed_WatchDog;

// App_Heat_Cool_Ctrl();
// App_RTC();
// App_DAC();			//CBC�ã�����Ҫ���̸�
// App_PWM();			//CBC�ã�����Ҫ���̸�
// App_BlueTooth();		//���������������̭����д��
#endif
	}
}

void InitDevice(void)
{
	SystemInit(); // HSEĬ�ϱ�Ƶ��72MHz�����ûHSE�л�HSI��ô����Ŀǰ��û�˽�

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
	InitE2PROM(); // �������������ǰ�棬���ȼ���ߣ���Ϊ�ͻ����ڳ�ʼ�����п���Ҫ�����Լ�������
	InitCan();
	InitADC();
	InitSci();
	InitData_SOC();		  // ������ڶ���eeprom���ݺ���
	InitData_SleepFunc(); // ������ڶ���eeprom���ݺ���
	InitAFE1();

	Init_IWDG();
#endif
}

void InitVar(void)
{
	UINT16 i;

	// SystemMonitorResetData_EEPROM();							//��������ĳ�ʼ��Ĭ���������޸��ˣ�Ҫ�޸�EEPROM���ϵ��־λ
	InitSystemMonitorData_EEPROM();
	SystemStatus.bits.b1Status_MOS_DSG = CLOSE;

	// Switch����

	// ��ϵͳ������ϵͳ��ʼ��
	for (i = 0; i < ERROR_NUM; ++i)
	{
		//*(&System_ErrFlag.u8ErrFlag_Com_AFE1+i)  =  0;		//������ͱ������ˣ���ʼ���������ֲ�������
	}

	// ������־λ��ʼ��
	g_stCellInfoReport.unMdlFault_First.all = 0;
	g_stCellInfoReport.unMdlFault_Second.all = 0;
	g_stCellInfoReport.unMdlFault_Third.all = 0;
	// ���α�����¼��ʼ��
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

	// �̵�������������ʼ��							//�������������
	// RelayCtrl_Command = RELAY_PRE_DET;	//�������������
	// HeatCtrl_Command = ST_HEAT_DET_SELF;
	// CoolCtrl_Command = ST_COOL_DET_SELF;

	// �������

	// ����д�Ͳ��ù�ǰ�浽�׶��������Ǹ�λ��(��EEPROM�ܶ���ط���)
	SeriesNum = OtherElement.u16Sys_SeriesNum;
	g_u32CS_Res_AFE = ((UINT32)OtherElement.u16Sys_CS_Res_Num * 1000) / OtherElement.u16Sys_CS_Res;

	SystemStatus.bits.b1StartUpBMS = 0; // ȥ������ʱ��

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
	MCUO_AFE_CTLC = 0; // ���ϵ磬Ĭ�ϸ���̬�����Բ���AFE�տ���˲���MOS
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
