#include "main.h"
#include "SH367309_DataDeal.h"
#include "string.h"

int AFE_PARAM_WRITE_Flag = 1;
int AFE_ResetFlag = 0;

const UINT16 AFE_OCD1V_OCCV[16] = {20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 160, 180, 200};					   // 一级放电过流和充电过流，单位mv
const UINT16 AFE_SCV[16] = {50, 80, 110, 140, 170, 200, 230, 260, 290, 320, 350, 400, 500, 600, 800, 1000};					   // 短路保护电压，单位mv
const UINT16 AFE_OVT_UVT[16] = {100, 200, 300, 400, 600, 800, 1000, 2000, 3000, 4000, 6000, 8000, 10000, 20000, 30000, 40000}; // 过压低压延时时间。单位ms
const UINT16 AFE_SCT[16] = {0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960};					   // 短路延时,单位us。
const UINT16 AFE_OCD1T[16] = {50, 100, 200, 400, 600, 800, 1000, 2000, 4000, 6000, 8000, 10000, 15000, 20000, 30000, 40000};   // 放电过流1延时。单位ms
const UINT16 AFE_OCCT_OCD2T[16] = {10, 20, 40, 60, 80, 100, 200, 400, 600, 800, 1000, 2000, 4000, 8000, 10000, 20000};		   // 放电过流2和充电过流延时。单位ms

AFE_ROM_PARAMETERS_TypeDef AFE_ROM_PARAMETERS_Struction = {0};
AFE_Parameters_RS485_Typedef AFE_Parameters_RS485_Struction = AFE_PARAMETERS_RS485_STRUCTION_DEFAULT;

/* 用到的外部的数据 */
extern UINT8 ucMTPBuffer[26];
extern UINT16 iSheldTemp_10K_NTC[141];

#define DSG_CHG_OCP_DELAY_TIME 30 * 100 // 过流延时时间 (10ms单位)
#define OFF 0
#define ON 1

/* 一级过流保护标志 */
void Ocp_MosJudge(UINT16 Current, UINT16 Ocp_First, UINT16 Ocp_Filter, UINT8 *flag, UINT8 Type)
{
	static UINT8 Ocp_MosCtrl_flag[2] = {0};
	static UINT16 ocpFitleCount[2] = {0};
	static UINT32 ocpRecoverCount[2] = {0};

	switch (Ocp_MosCtrl_flag[Type])
	{
	case 0:
		if (Current > Ocp_First)
		{
			if (Ocp_Filter <= ocpFitleCount[Type]++)
			{
				ocpFitleCount[Type] = 0;
				*flag = 1;
				Ocp_MosCtrl_flag[Type] = 1;
			}
		}
		else
		{
			if (ocpFitleCount[Type])
			{
				ocpFitleCount[Type]--;
			}
		}
		break;
	case 1:
		/* 延时30s控制权交给AFE */
		if (DSG_CHG_OCP_DELAY_TIME <= ocpRecoverCount[Type]++)
		{
			ocpRecoverCount[Type] = 0;
			*flag = 0;
			Ocp_MosCtrl_flag[Type] = 0;
		}
		break;
	}
}

void App_SH367309_Supplement(void)
{
	UINT8 flag = 0;
	static UINT8 IchgOcp_Flag = 0;
	static UINT8 IdischgOcp_Flag = 0;
	static UINT8 TmosOtp_Flag = 0;

	if (g_st_SysTimeFlag.bits.b1Sys10msFlag1 == 0)
	{
		return;
	}

	/* 充电过流判断 */
	flag = g_stCellInfoReport.unMdlFault_First.bits.b1IchgOcp;
	Ocp_MosJudge(g_stCellInfoReport.u16Ichg,
				 AFE_Parameters_RS485_Struction.u16IchgOcp_First.curValue, AFE_Parameters_RS485_Struction.u16IchgOcp_Filter_First.curValue, &flag, 0);
	g_stCellInfoReport.unMdlFault_First.bits.b1IchgOcp = flag;

	/*放电过流判断*/
	flag = g_stCellInfoReport.unMdlFault_First.bits.b1IdischgOcp;
	Ocp_MosJudge(g_stCellInfoReport.u16IDischg,
				 AFE_Parameters_RS485_Struction.u16IdsgOcp_First.curValue, AFE_Parameters_RS485_Struction.u16IdsgOcp_Filter_First.curValue, &flag, 1);
	g_stCellInfoReport.unMdlFault_First.bits.b1IdischgOcp = flag;

	switch (IchgOcp_Flag)
	{
	case 0:
		if (g_stCellInfoReport.unMdlFault_First.bits.b1IchgOcp)
		{
			FaultWarnRecord2(IchgOcp_First);
			/* 强行关闭充电管 */
			SH367309_DriverMos_Ctrl(GPIO_CHG, OFF);
			IchgOcp_Flag = 1;
		}
		break;
	case 1:
		if (!g_stCellInfoReport.unMdlFault_First.bits.b1IchgOcp)
		{
			/* MCU不控制充电管，由AFE控制 */
			SH367309_DriverMos_Ctrl(GPIO_CHG, ON);
			IchgOcp_Flag = 0;
		}
		break;
	default:
		break;
	}

	switch (IdischgOcp_Flag)
	{
	case 0:
		if (g_stCellInfoReport.unMdlFault_First.bits.b1IdischgOcp)
		{
			FaultWarnRecord2(IdischgOcp_First);
			/* 强行关闭放电管 */
			SH367309_DriverMos_Ctrl(GPIO_DSG, OFF);
			IdischgOcp_Flag = 1;
		}
		break;
	case 1:
		if (!g_stCellInfoReport.unMdlFault_First.bits.b1IdischgOcp)
		{
			/* MCU不控制放电管，由AFE控制 */
			SH367309_DriverMos_Ctrl(GPIO_DSG, ON);
			IdischgOcp_Flag = 0;
		}
		break;
	default:
		break;
	}

	switch (TmosOtp_Flag)
	{
	case 0:
		if (g_stCellInfoReport.unMdlFault_Third.bits.b1TmosOtp)
		{
			SH367309_DriverMos_Ctrl(GPIO_CHG, OFF);
			SH367309_DriverMos_Ctrl(GPIO_DSG, OFF);
			TmosOtp_Flag = 1;
		}
		break;

	case 1:
		if (!g_stCellInfoReport.unMdlFault_Third.bits.b1TmosOtp)
		{
			/* MCU不控制放电管，由AFE控制 */

			if (g_stCellInfoReport.unMdlFault_First.bits.b1IdischgOcp)
			{										   // 这个写法其实不太好，用并联判断最好
				SH367309_DriverMos_Ctrl(GPIO_CHG, ON); // 如果后续不添加新保护功能，问题不大。
			}										   // 如果加了，就要思考并联恢复问题了
			else if (g_stCellInfoReport.unMdlFault_First.bits.b1IchgOcp)
			{
				SH367309_DriverMos_Ctrl(GPIO_DSG, ON);
			}
			else
			{
				SH367309_DriverMos_Ctrl(GPIO_CHG, ON);
				SH367309_DriverMos_Ctrl(GPIO_DSG, ON);
			}
			TmosOtp_Flag = 0;
		}
		break;

	default:
		break;
	}
}

/* 找出要写入AFE寄存器的值：参数1：当前要写的值，参数2：AFE参数列表的地址 */
int Choose_Right_Value(UINT16 cur_Value, const UINT16 *AFE_list)
{
	int i = 0;
	for (i = 0; i < 15; i++)
	{
		if (cur_Value <= AFE_list[i])
		{
			break;
		}
	}
	return i;
}

// 全部数据刷新到AFE_ROM_PARAMETERS_Struction结构体里面
// 一部分是默认的配置，从.h文件的#define里面来
// 一部分是可修改的，AFE_Parameters_RS485_Struction里面来
void Refresh_Parameters(void)
{
	int i = 0;
	int temp = 0;
	UINT8 TR = 0;
	UINT16 AFE_TEMPERATURE[8] = {0}; // 温度，摄氏度+40，（0度的值为40）

	// 读309的TR，顺便把AFE默认值配置传到AFE_ROM_PARAMETERS_Struction结构体(#define类型)。
	if (MTPRead(0x19, 1, &TR))
	{
		SH367309_Reg_Store.TR_ResRef = 680 + 5 * (TR & 0x7F);
		ucMTPBuffer[25] = TR & 0x7F;

/* 把默认的数据放在参数结构体里 */
#ifdef _SLEEP_WITH_CURRENT
		// 休眠带电暂且不需要预充功能
		// 宏定义少个括号，出事了，计算优先级问题
		ucMTPBuffer[1] = (BYTE_01H_SCONF2)&0xF3;
#endif
		memcpy((UINT8 *)&AFE_ROM_PARAMETERS_Struction, ucMTPBuffer, 26);
	}

	g_u32CS_Res_AFE = ((UINT32)OtherElement.u16Sys_CS_Res_Num * 1000) / OtherElement.u16Sys_CS_Res;

	/* 串数 */
	AFE_ROM_PARAMETERS_Struction.m00H_01H.CN = OtherElement.u16Sys_SeriesNum % 16;

	/* 充电过压 */
	AFE_ROM_PARAMETERS_Struction.m02H_03H.OVH = ((AFE_Parameters_RS485_Struction.u16VcellOvp.curValue / 5) >> 8) & 0x3;
	AFE_ROM_PARAMETERS_Struction.m02H_03H.OVL = (AFE_Parameters_RS485_Struction.u16VcellOvp.curValue / 5) & 0x00FF;
	/* 充电过压延时时间 */
	temp = AFE_Parameters_RS485_Struction.u16VcellOvp_Filter.curValue * 10;
	AFE_ROM_PARAMETERS_Struction.m02H_03H.OVT = Choose_Right_Value(temp, AFE_OVT_UVT);
	/* 充电过压恢复 */
	AFE_ROM_PARAMETERS_Struction.m04H_05H.OVRH = ((AFE_Parameters_RS485_Struction.u16VcellOvp_Rcv.curValue / 5) >> 8) & 0x3;
	AFE_ROM_PARAMETERS_Struction.m04H_05H.OVRL = (AFE_Parameters_RS485_Struction.u16VcellOvp_Rcv.curValue / 5) & 0x00FF;

	/*放电低压延时时间 */
	temp = AFE_Parameters_RS485_Struction.u16VcellUvp_Filter.curValue * 10;
	AFE_ROM_PARAMETERS_Struction.m04H_05H.UVT = Choose_Right_Value(temp, AFE_OVT_UVT);
	/* 放电低压 */
	AFE_ROM_PARAMETERS_Struction.m06H_07H.UV = (AFE_Parameters_RS485_Struction.u16VcellUvp.curValue / 20) & 0x00FF;
	/* 放电低压恢复 */
	AFE_ROM_PARAMETERS_Struction.m06H_07H.UVR = (AFE_Parameters_RS485_Struction.u16VcellUvp_Rcv.curValue / 20) & 0x00FF;

	/* 放电过流:     二级过流值（A*10）/         */
	temp = AFE_Parameters_RS485_Struction.u16IdsgOcp_Second.curValue * 100 / g_u32CS_Res_AFE; // 当前对应多少mv
	AFE_ROM_PARAMETERS_Struction.m0CH_0DH.OCD1V = Choose_Right_Value(temp, AFE_OCD1V_OCCV);
	/* 放电过流滤波时间 */
	temp = AFE_Parameters_RS485_Struction.u16IdsgOcp_Filter_Second.curValue * 10; // 当前对应多少ms
	AFE_ROM_PARAMETERS_Struction.m0CH_0DH.OCD1T = Choose_Right_Value(temp, AFE_OCD1T);

	/* 充电过流 */
	temp = AFE_Parameters_RS485_Struction.u16IchgOcp_Second.curValue * 100 / g_u32CS_Res_AFE; // 当前对应多少mv
	AFE_ROM_PARAMETERS_Struction.m0EH_0FH.OCCV = Choose_Right_Value(temp, AFE_OCD1V_OCCV);
	/* 充电过流滤波时间 */
	temp = AFE_Parameters_RS485_Struction.u16IchgOcp_Filter_Second.curValue * 10; // 当前对应多少ms
	AFE_ROM_PARAMETERS_Struction.m0EH_0FH.OCCT = Choose_Right_Value(temp, AFE_OCCT_OCD2T);

	/* 短路延时 */
	temp = AFE_Parameters_RS485_Struction.u16CBC_DelayT.curValue;
	AFE_ROM_PARAMETERS_Struction.m0EH_0FH.SCT = Choose_Right_Value(temp, AFE_SCT);
	/* 短路电压 */
	temp = AFE_Parameters_RS485_Struction.u16CBC_Cur_DSG.curValue * 1000 / g_u32CS_Res_AFE; // 当前对应多少mv
	AFE_ROM_PARAMETERS_Struction.m0EH_0FH.SCV = Choose_Right_Value(temp, AFE_SCV);

	/* 所有的温度保护 */
	AFE_TEMPERATURE[0] = AFE_Parameters_RS485_Struction.u16TChgOTp.curValue / 10;		 /* 充电高温保护 */
	AFE_TEMPERATURE[1] = AFE_Parameters_RS485_Struction.u16TChgOTp_Rcv.curValue / 10;	 /* 充电高温保护恢复 */
	AFE_TEMPERATURE[2] = AFE_Parameters_RS485_Struction.u16TchgUTp.curValue / 10;		 /* 充电低温保护 */
	AFE_TEMPERATURE[3] = AFE_Parameters_RS485_Struction.u16TchgUTp_Rcv.curValue / 10;	 /* 充电低温保护恢复 */
	AFE_TEMPERATURE[4] = AFE_Parameters_RS485_Struction.u16TdischgOTp.curValue / 10;	 /* 放电高温保护 */
	AFE_TEMPERATURE[5] = AFE_Parameters_RS485_Struction.u16TdischgOTp_Rcv.curValue / 10; /* 放电高温保护恢复 */
	AFE_TEMPERATURE[6] = AFE_Parameters_RS485_Struction.u16TdischgUTp.curValue / 10;	 /* 放电低温保护 */
	AFE_TEMPERATURE[7] = AFE_Parameters_RS485_Struction.u16TdischgUTp_Rcv.curValue / 10; /* 放电低温保护恢复 */

	for (i = 0; i < 8; i++)
	{
		temp = iSheldTemp_10K_NTC[AFE_TEMPERATURE[i]];
		*(((UINT8 *)&AFE_ROM_PARAMETERS_Struction.m11H_19H) + i) = (UINT8)(((UINT32)temp << 9) / ((UINT32)SH367309_Reg_Store.TR_ResRef + temp));
	}
}

/* 每次数据改变都读取rom参数比较一下，那个参数改变就写入那=哪个 */
void Write_Parameters(void)
{
	int i = 0;
	UINT8 temp[26] = {0};
	UINT8 *P = (UINT8 *)&AFE_ROM_PARAMETERS_Struction;

	if (MTPRead(0x00, 25, temp))
	{
		for (i = 0; i < 25; i++)
		{ // 最后一个TR不做对比
			if (temp[i] != P[i])
			{
				MTPWriteROM(i, 1, P + i); // 重写EEPROM的寄存器，两次
			}
		}
	}
}

// 开机的时候，AFE_PARAM_WRITE_Flag=1是默认值，所以开机的时候会执行一次。
void SH367309_UpdataAfeConfig(void)
{
	if (AFE_PARAM_WRITE_Flag)
	{
		AFE_PARAM_WRITE_Flag = 0;
		MCUO_AFE_VPRO = 1; // 进入烧写模式
		Delay1ms(20);
		Feed_WatchDog;

		Refresh_Parameters();
		Write_Parameters();

		Feed_WatchDog;
		MCUO_AFE_VPRO = 0; // 退出烧写模式
		Delay1ms(1);

		/* 每次写完如果不报错都要复位一下。这样写进去的参数才有效 */
		if (!System_ERROR_UserCallback(ERROR_STATUS_AFE1))
		{
			AFE_Reset(); // Reset IC
			Delay1ms(5);
			AFE_IsReady();
			AFE_ResetFlag = 1;
		}
	}
}

/*********************************** 以下是数据为了保存到EEPROM的设计 **********************************/

UINT8 Sci_WrRegs_0x10_AFE_Parameters(UINT16 u16Channel, struct RS485MSG *s)
{
	UINT16 u16WrRegNum;
	UINT16 u16SciRegStartAddr;

	int i = 0;
	UINT16 offset = 0;
	UINT16 *P = (UINT16 *)&AFE_Parameters_RS485_Struction;
	u16SciRegStartAddr = s->u16Buffer[3] + (s->u16Buffer[2] << 8);
	u16WrRegNum = s->u16Buffer[5] + (s->u16Buffer[4] << 8);

	/* 起始地址在AFE参数范围，且起始地址+写的寄存器数量在范围内 */
	if ((u16SciRegStartAddr >= RS485_CMD_ADDR_AFE_ROM_PARAMETERS_START) && (u16SciRegStartAddr <= RS485_CMD_ADDR_AFE_ROM_PARAMETERS_END) && (u16SciRegStartAddr + u16WrRegNum - 1 <= RS485_CMD_ADDR_AFE_ROM_PARAMETERS_END))
	{
		offset = u16SciRegStartAddr - RS485_CMD_ADDR_AFE_ROM_PARAMETERS_START;

		Feed_WatchDog;
		for (i = 0; i < u16WrRegNum; i++)
		{
			*(P + (i + offset) * 4) = s->u16Buffer[8 + i * 2] + (s->u16Buffer[7 + i * 2] << 8);

			/* 直接写道EEPROM中 */
			WriteEEPROM_Word_WithZone(E2P_ADDR_E2POS_AFE_Parameters + ((i + offset) << 1), *(P + (i + offset) * 4));
		}
		Feed_WatchDog;
		AFE_PARAM_WRITE_Flag = 1;
		return 1;
	}

	return 0;
}

void Sci_WrReg_0x06_Reset_AFE_Parameters(struct RS485MSG *s)
{
	UINT16 u16SciRegData = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (0x0001 == u16SciRegData)
	{
		EEPROM_ResetData_AFE_ParametersToDefault();
		AFE_PARAM_WRITE_Flag = 1;
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_DATA_INVALID;
	}
}

void Sci_ACK_0x03_RW_AFE_Parameters(struct RS485MSG *s, UINT8 t_u8BuffTemp[])
{
	UINT16 u16SciTemp;
	UINT16 i, j;
	UINT16 *P = (UINT16 *)&AFE_Parameters_RS485_Struction;
	i = 0;

	for (j = 0; j < AFE_PARAMETES_TOTAL_LENGTH; j++)
	{
		u16SciTemp = *(P + j * 4);
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	}
}

// AFE_Parameters  reset
void EEPROM_ResetData_AFE_ParametersToDefault(void)
{
	UINT8 i;
	UINT16 *P = (UINT16 *)&AFE_Parameters_RS485_Struction.u16VcellOvp.defaultValue;

	Feed_WatchDog;
	for (i = 0; i < AFE_PARAMETES_TOTAL_LENGTH; ++i)
	{
		*(P + i * 4 - 1) = *(P + i * 4); // 当前值变为默认值
		WriteEEPROM_Word_WithZone(E2P_ADDR_E2POS_AFE_Parameters + (i << 1), *(P + i * 4));
	}
	Feed_WatchDog;
}

void ReadEEPROM_AFE_Parameters(void)
{
	UINT16 i;

	AFE_Value_Typedef *P = &AFE_Parameters_RS485_Struction.u16VcellOvp;
	for (i = 0; i < AFE_PARAMETES_TOTAL_LENGTH; ++i)
	{
		(P + i)->curValue = ReadEEPROM_Word_WithZone(E2P_ADDR_E2POS_AFE_Parameters + (i << 1));

		if (((P + i)->curValue < (P + i)->minValue) || ((P + i)->curValue > (P + i)->maxValue))
		{
			(P + i)->curValue = (P + i)->defaultValue;
			if (0 == System_ErrFlag.u8ErrFlag_Com_EEPROM)
			{
				System_ERROR_UserCallback(ERROR_EEPROM_STORE);
			}
		}
	}
}
