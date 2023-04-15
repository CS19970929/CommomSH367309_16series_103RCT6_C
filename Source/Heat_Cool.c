#include "main.h"

enum HEAT_COOL_CTRL_STATUS HeatCtrl_Command = ST_HEAT_DET_SELF;
enum HEAT_COOL_CTRL_STATUS CoolCtrl_Command = ST_COOL_DET_SELF;
union HEAT_COOL_FAULT_FLAG Heat_Cool_FaultFlag;

struct HEAT_COOL_ELEMENT Heat_Cool_Element;

#define HEAT_CLOSE_CUR 	0		//默认写死1A关闭，充电的时候，开启加热会减少充电电流
								//改为0A，因为加热膜吸收电流之后(小功率充电器)，可能就从2.5A变成0.5A，
								//然后关闭，电流又起来，加热又打开，就在这里疯狂开关。
								//干脆一了百了，只要打开了，就等温度上来再关闭
								
void Cool_StartUp_SelfCheck(void) {
	Heat_Cool_FaultFlag.bits.CoolErr_Normal = 1;
	System_Func_StartUp.bits.b1StartUpFlag_Cool = 0;	//明天回来补
	CoolCtrl_Command = ST_COOL_DET_NORMAL;
}


//冷凝策略很简单，只要温度到了就打开就行，因为这个时候大概率在大功率使用。
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


//三条主线：
//A，开机自检没问题——进入normal监控——需要处理且处理成功——继续跳回normal模式监控
//B，开机自检没问题——进入normal监控——需要处理且处理失败——进入错误处理函数——进入复原函数——复原完毕——返回自检
//C，开机自检有问题——进入错误处理函数——进入复原函数——复原完毕——返回自检
void Cool_Control(void) {
	if(!System_OnOFF_Func.bits.b1OnOFF_Cool) {	//没有这个功能，不进来
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
	System_Func_StartUp.bits.b1StartUpFlag_Heat = 0;	//明天回来补
	HeatCtrl_Command = ST_HEAT_DET_NORMAL;
}


/*
改为简易版
1、有电流，或者电流值大于一定的值(可设)
2、温度小于X摄氏度(可设)。
*/
void Heat_OnOFF_Det_Normal(void) {
	//正常加热逻辑，既可电池加热，也可充电枪加热
	if(g_stCellInfoReport.u16TempMin <= Heat_Cool_Element.u16Heat_OpenTemp) {
		if(g_stCellInfoReport.u16Ichg >= Heat_Cool_Element.u16Heat_OpenCur\
			|| g_stCellInfoReport.u16IDischg >= Heat_Cool_Element.u16Heat_OpenCur) {
			SystemStatus.bits.b1Status_Heat = 1;
			HeatCtrl_Command = ST_HEAT_CONT;
		}
	}

	//补充，如果是低温保护，肯定是驱动关掉，这个时候直接打开加热接触器等待充电枪插入便可
	if(g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgUtp || g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp) {
		SystemStatus.bits.b1Status_Heat = 1;
		HeatCtrl_Command = ST_HEAT_DET_ERR;
	}
}


//关闭和电流不挂钩(证明开始使用了)，打开必须挂钩，不然就长期在那里耗，把电池耗到低压保护。
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
	//因为加热膜吸收电流之后(小功率充电器)，可能就从2.5A变成0.5A，
	//然后关闭，电流又起来，加热又打开，就在这里疯狂开关。
	//干脆一了百了，只要打开了，温度必须回来，才关闭，后续需要修改，再作定制
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


//低温保护，然后插入充电枪，加热到低温保护恢复，这个时候断开加热接触器，然后进入正常检测模式
//1，如果不关想直接加热到停止加热点，如果加着加着，然后断开充电枪，则会变成电池放电加热，如果这个时候温度动态平衡，把电池的电耗没了
//2，如果是断掉，进入正常逻辑判断，就会有点用户体验的不好，客户想通过充电枪把加热到复原，但是并没做到。
//基于2，实际上问题也不大，加热到低温保护恢复了，就能正常跑起来了，跑起来客户就认为没问题，这个时候如果有电流再进行加热，问题不大。
void Heat_OnOFF_Det_Err(void) {
	static UINT16 su16_Recovery_Tcnt = 0;

	if(!g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgUtp && !g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp) {
		if(++su16_Recovery_Tcnt > 10) {						//解除保护后，等待系统稳定10s
			su16_Recovery_Tcnt = 0;
			SystemStatus.bits.b1Status_Heat = 0;			//先关掉加热接触器，进入正常判断。
			HeatCtrl_Command = ST_HEAT_ERR_RECOVER_DEAL;	//进入复原处理函数
		}
	}
}


void Heat_OnOFF_Err_RecoverDeal(void) {
	HeatCtrl_Command = ST_HEAT_DET_NORMAL;
}


void Heat_Control(void) {
	if(!System_OnOFF_Func.bits.b1OnOFF_Heat) {	//没有这个功能，不进来
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		//IO口速度为2MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//PC6_MCUO_RELAY_COOL
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		//IO口速度为2MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}


//加热冷凝电流均无法检测
void App_Heat_Cool_Ctrl(void) {
	Heat_Control();
	Cool_Control();
}

