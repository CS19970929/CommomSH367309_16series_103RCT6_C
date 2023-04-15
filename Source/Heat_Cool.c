#include "main.h"

enum HEAT_COOL_CTRL_STATUS HeatCtrl_Command = ST_HEAT_DET_SELF;
enum HEAT_COOL_CTRL_STATUS CoolCtrl_Command = ST_COOL_DET_SELF;
union HEAT_COOL_FAULT_FLAG Heat_Cool_FaultFlag;

struct HEAT_COOL_ELEMENT Heat_Cool_Element;

#define HEAT_CLOSE_CUR 	0		//Ĭ��д��1A�رգ�����ʱ�򣬿������Ȼ���ٳ�����
								//��Ϊ0A����Ϊ����Ĥ���յ���֮��(С���ʳ����)�����ܾʹ�2.5A���0.5A��
								//Ȼ��رգ������������������ִ򿪣����������񿪹ء�
								//�ɴ�һ�˰��ˣ�ֻҪ���ˣ��͵��¶������ٹر�
								
void Cool_StartUp_SelfCheck(void) {
	Heat_Cool_FaultFlag.bits.CoolErr_Normal = 1;
	System_Func_StartUp.bits.b1StartUpFlag_Cool = 0;	//���������
	CoolCtrl_Command = ST_COOL_DET_NORMAL;
}


//�������Ժܼ򵥣�ֻҪ�¶ȵ��˾ʹ򿪾��У���Ϊ���ʱ�������ڴ���ʹ�á�
void Cool_OnOFF_Det_Normal(void) {
	if(g_stCellInfoReport.u16TempMax >= Heat_Cool_Element.u16Cool_OpenTemp) {
		SystemStatus.bits.b1Status_Cool = 1;
		CoolCtrl_Command = ST_COOL_CONT;
	}
}


void Cool_OnOFF_CONT(void) {
	static UINT16 su16_CoolTime_Cnt = 0;

	if(g_stCellInfoReport.u16TempMax < Heat_Cool_Element.u16Cool_CloseTemp) {
		if(++su16_CoolTime_Cnt > 3) {
			su16_CoolTime_Cnt = 0;
			SystemStatus.bits.b1Status_Cool = 0;
			CoolCtrl_Command = ST_COOL_DET_NORMAL;
		}
	}
	else {
		if(su16_CoolTime_Cnt)su16_CoolTime_Cnt = 0;
	}
}


void Cool_OnOFF_Det_Err(void) {

}


void Cool_OnOFF_Err_RecoverDeal(void) {

}


//�������ߣ�
//A�������Լ�û���⡪������normal��ء�����Ҫ�����Ҵ���ɹ�������������normalģʽ���
//B�������Լ�û���⡪������normal��ء�����Ҫ�����Ҵ���ʧ�ܡ���������������������븴ԭ����������ԭ��ϡ��������Լ�
//C�������Լ������⡪��������������������븴ԭ����������ԭ��ϡ��������Լ�
void Cool_Control(void) {
	if(!System_OnOFF_Func.bits.b1OnOFF_Cool) {	//û��������ܣ�������
		return;
	}

	if(STARTUP_CONT == System_FUNC_StartUp(SYSTEM_FUNC_STARTUP_COOL)) {
		return;
	}

	if(0 == g_st_SysTimeFlag.bits.b1Sys1000msFlag2) {
		return;
	}

	switch(CoolCtrl_Command) {
		case ST_COOL_DET_SELF:
			Cool_StartUp_SelfCheck();
			break;
		case ST_COOL_DET_NORMAL:
			Cool_OnOFF_Det_Normal();
			break;
		case ST_COOL_CONT:
			Cool_OnOFF_CONT();
			break;
		case ST_COOL_DET_ERR:
			Cool_OnOFF_Det_Err();
			break;
		case ST_COOL_ERR_RECOVER_DEAL:
			Cool_OnOFF_Err_RecoverDeal();
			break;
		default:
			break;
	
}

	MCUO_RELAY_COOL = SystemStatus.bits.b1Status_Cool;
}



void Heat_StartUp_SelfCheck(void) {
	Heat_Cool_FaultFlag.bits.HeatErr_Normal = 1;
	System_Func_StartUp.bits.b1StartUpFlag_Heat = 0;	//���������
	HeatCtrl_Command = ST_HEAT_DET_NORMAL;
}


/*
��Ϊ���װ�
1���е��������ߵ���ֵ����һ����ֵ(����)
2���¶�С��X���϶�(����)��
*/
void Heat_OnOFF_Det_Normal(void) {
	//���������߼����ȿɵ�ؼ��ȣ�Ҳ�ɳ��ǹ����
	if(g_stCellInfoReport.u16TempMin <= Heat_Cool_Element.u16Heat_OpenTemp) {
		if(g_stCellInfoReport.u16Ichg >= Heat_Cool_Element.u16Heat_OpenCur\
			|| g_stCellInfoReport.u16IDischg >= Heat_Cool_Element.u16Heat_OpenCur) {
			SystemStatus.bits.b1Status_Heat = 1;
			HeatCtrl_Command = ST_HEAT_CONT;
		}
	}

	//���䣬����ǵ��±������϶��������ص������ʱ��ֱ�Ӵ򿪼��ȽӴ����ȴ����ǹ������
	if(g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgUtp || g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp) {
		SystemStatus.bits.b1Status_Heat = 1;
		HeatCtrl_Command = ST_HEAT_DET_ERR;
	}
}


//�رպ͵������ҹ�(֤����ʼʹ����)���򿪱���ҹ�����Ȼ�ͳ���������ģ��ѵ�غĵ���ѹ������
void Heat_OnOFF_CONT(void) {
	static UINT16 su16_HeatTime_Cnt = 0;
	//UINT16 Cur_Temp = 0;
	/*
	if((g_stCellInfoReport.u16Ichg <= HEAT_CLOSE_CUR\
		&& g_stCellInfoReport.u16IDischg <= HEAT_CLOSE_CUR)\
		|| g_stCellInfoReport.u16TempMin > Heat_Cool_Element.u16Heat_CloseTemp) {
		if(++su16_HeatTime_Cnt > 3) {
			su16_HeatTime_Cnt = 0;
			SystemStatus.bits.b1Status_Heat = 0;
			HeatCtrl_Command = ST_HEAT_DET_NORMAL;
		}
	}
	*/
	//��Ϊ����Ĥ���յ���֮��(С���ʳ����)�����ܾʹ�2.5A���0.5A��
	//Ȼ��رգ������������������ִ򿪣����������񿪹ء�
	//�ɴ�һ�˰��ˣ�ֻҪ���ˣ��¶ȱ���������Źرգ�������Ҫ�޸ģ���������
	if(g_stCellInfoReport.u16TempMin > Heat_Cool_Element.u16Heat_CloseTemp) {
		if(++su16_HeatTime_Cnt > 3) {
			su16_HeatTime_Cnt = 0;
			SystemStatus.bits.b1Status_Heat = 0;
			HeatCtrl_Command = ST_HEAT_DET_NORMAL;
		}
	}
	else {
		if(su16_HeatTime_Cnt)su16_HeatTime_Cnt = 0;
	}

	if(g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgUtp || g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp) {
		SystemStatus.bits.b1Status_Heat = 1;
		HeatCtrl_Command = ST_HEAT_DET_ERR;
	}
}


//���±�����Ȼ�������ǹ�����ȵ����±����ָ������ʱ��Ͽ����ȽӴ�����Ȼ������������ģʽ
//1�����������ֱ�Ӽ��ȵ�ֹͣ���ȵ㣬������ż��ţ�Ȼ��Ͽ����ǹ������ɵ�طŵ���ȣ�������ʱ���¶ȶ�̬ƽ�⣬�ѵ�صĵ��û��
//2������Ƕϵ������������߼��жϣ��ͻ��е��û�����Ĳ��ã��ͻ���ͨ�����ǹ�Ѽ��ȵ���ԭ�����ǲ�û������
//����2��ʵ��������Ҳ���󣬼��ȵ����±����ָ��ˣ����������������ˣ��������ͻ�����Ϊû���⣬���ʱ������е����ٽ��м��ȣ����ⲻ��
void Heat_OnOFF_Det_Err(void) {
	static UINT16 su16_Recovery_Tcnt = 0;

	if(!g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgUtp && !g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp) {
		if(++su16_Recovery_Tcnt > 10) {						//��������󣬵ȴ�ϵͳ�ȶ�10s
			su16_Recovery_Tcnt = 0;
			SystemStatus.bits.b1Status_Heat = 0;			//�ȹص����ȽӴ��������������жϡ�
			HeatCtrl_Command = ST_HEAT_ERR_RECOVER_DEAL;	//���븴ԭ������
		}
	}
}


void Heat_OnOFF_Err_RecoverDeal(void) {
	HeatCtrl_Command = ST_HEAT_DET_NORMAL;
}


void Heat_Control(void) {
	if(!System_OnOFF_Func.bits.b1OnOFF_Heat) {	//û��������ܣ�������
		return;
	}

	if(STARTUP_CONT == System_FUNC_StartUp(SYSTEM_FUNC_STARTUP_HEAT)) {
		return;
	}

	if(0 == g_st_SysTimeFlag.bits.b1Sys1000msFlag2) {
		return;
	}

	switch(HeatCtrl_Command) {
		case ST_HEAT_DET_SELF:
			Heat_StartUp_SelfCheck();
			break;
		case ST_HEAT_DET_NORMAL:
			Heat_OnOFF_Det_Normal();
			break;
		case ST_HEAT_CONT:
			Heat_OnOFF_CONT();
			break;
		case ST_HEAT_DET_ERR:
			Heat_OnOFF_Det_Err();
			break;
		case ST_HEAT_ERR_RECOVER_DEAL:
			Heat_OnOFF_Err_RecoverDeal();
			break;
		default:
			break;
	}

	MCUO_RELAY_HEAT = SystemStatus.bits.b1Status_Heat;
}



void InitHeat_Cool(void) {
	GPIO_InitTypeDef  GPIO_InitStructure;

	//PB14_MCUO_RELAY_HEAT
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		//IO���ٶ�Ϊ2MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//PC6_MCUO_RELAY_COOL
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		//IO���ٶ�Ϊ2MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}


//���������������޷����
void App_Heat_Cool_Ctrl(void) {
	Heat_Control();
	Cool_Control();
}

