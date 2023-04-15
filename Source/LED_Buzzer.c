#include "main.h"

volatile static UINT8 u8BarFlag[6] = {0};


//һ����2��
void Flash2(volatile UINT8 *BarLedFlag) {
	static UINT8 s_u8F2Cnt = 0;
	if((++s_u8F2Cnt) > 25){
		*BarLedFlag = ~*BarLedFlag;
		s_u8F2Cnt = 0;
	}
}

//һ����3��
void Flash3(volatile UINT8 *BarLedFlag){
	static UINT8 s_u8F3Cnt = 0;
	if((++s_u8F3Cnt) > 16){
		*BarLedFlag = ~*BarLedFlag;
		s_u8F3Cnt = 0;
	}
}


void LEDCtrl_SOC(UINT8 idx){
	UINT8 i;
	for(i = 1; i < idx; i++) {	//����Ϊ0��λ�ã�����ALM��
		u8BarFlag[i] = LB_LED_ON;
	}

	//����ʱ�����һ����˸
	if(g_stCellInfoReport.u16Ichg > OtherElement.u16Sleep_VirCur_Chg) {
		Flash2(&u8BarFlag[i++]);
	}
	else if(0 == idx) {		//��ѹ��������SOC������ȫ��
		u8BarFlag[i++] = LB_LED_OFF;
	}
	else {
		u8BarFlag[i++] = LB_LED_ON;
	}
	
	for(; i < LIGHTBAR_LED_NUM; i++) {
		u8BarFlag[i] = LB_LED_OFF;
	}
}


void LED_SOC(void){
	UINT8 idx = 0;
	if (g_stCellInfoReport.SocElement.u16Soc == 0) idx = 0;
	else if (g_stCellInfoReport.SocElement.u16Soc <= 20) idx = 1;
	else if (g_stCellInfoReport.SocElement.u16Soc <= 40) idx = 2;
	else if (g_stCellInfoReport.SocElement.u16Soc <= 60) idx = 3;
	else if (g_stCellInfoReport.SocElement.u16Soc <= 80) idx = 4;
	else if (g_stCellInfoReport.SocElement.u16Soc <= 100) idx = 5;

	if(g_stCellInfoReport.unMdlFault_Third.bits.b1BatOvp		//��ѹȫ��
		||g_stCellInfoReport.unMdlFault_Third.bits.b1CellOvp) {
		idx = 5;
	}
	else if(g_stCellInfoReport.unMdlFault_Third.bits.b1BatUvp	//��ѹȫ��
		|| g_stCellInfoReport.unMdlFault_Third.bits.b1CellUvp) {
		idx = 0;
	}

	LEDCtrl_SOC(idx);
}

void LED_ALM(void){
	//���˹�ѹ��ѹ��SOC��������������������
	if(g_stCellInfoReport.unMdlFault_Third.all&0x27F0
		|| System_ErrFlag.u8ErrFlag_CBC_DSG
		|| System_ErrFlag.u8ErrFlag_Com_AFE1
		|| System_ErrFlag.u8ErrFlag_Com_EEPROM
	) {
		u8BarFlag[e_LED_ALM] = LB_LED_ON;
	}
	//����ѹ��ѹ������������������
	else if((g_stCellInfoReport.unMdlFault_First.all&0x27F0)
		|| (g_stCellInfoReport.unMdlFault_Second.all&0x27F0)) {
		Flash3(&u8BarFlag[e_LED_ALM]);
	}
	else{
		u8BarFlag[e_LED_ALM] = LB_LED_OFF;
	}
}

void App_LightBar(void) {
	if(0 == g_st_SysTimeFlag.bits.b1Sys10msFlag2) {
		return;
	}
	LED_SOC();
	LED_ALM();

	MCUO_LED_ALM = u8BarFlag[e_LED_ALM];
	MCUO_LED_SOC20 = u8BarFlag[e_LED_SOC20];
	MCUO_LED_SOC40 = u8BarFlag[e_LED_SOC40];
	MCUO_LED_SOC60 = u8BarFlag[e_LED_SOC60];
	MCUO_LED_SOC80 = u8BarFlag[e_LED_SOC80];
	MCUO_LED_SOC100 = u8BarFlag[e_LED_SOC100];
}

