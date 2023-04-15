#include "main.h"
#include "SH367309_DataDeal.h"
#include "string.h"

int AFE_PARAM_WRITE_Flag = 1;
int AFE_ResetFlag = 0;

const UINT16 AFE_OCD1V_OCCV[16] = {20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 160, 180, 200};					   // һ���ŵ�����ͳ���������λmv
const UINT16 AFE_SCV[16] = {50, 80, 110, 140, 170, 200, 230, 260, 290, 320, 350, 400, 500, 600, 800, 1000};					   // ��·������ѹ����λmv
const UINT16 AFE_OVT_UVT[16] = {100, 200, 300, 400, 600, 800, 1000, 2000, 3000, 4000, 6000, 8000, 10000, 20000, 30000, 40000}; // ��ѹ��ѹ��ʱʱ�䡣��λms
const UINT16 AFE_SCT[16] = {0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960};					   // ��·��ʱ,��λus��
const UINT16 AFE_OCD1T[16] = {50, 100, 200, 400, 600, 800, 1000, 2000, 4000, 6000, 8000, 10000, 15000, 20000, 30000, 40000};   // �ŵ����1��ʱ����λms
const UINT16 AFE_OCCT_OCD2T[16] = {10, 20, 40, 60, 80, 100, 200, 400, 600, 800, 1000, 2000, 4000, 8000, 10000, 20000};		   // �ŵ����2�ͳ�������ʱ����λms

AFE_ROM_PARAMETERS_TypeDef AFE_ROM_PARAMETERS_Struction = {0};
AFE_Parameters_RS485_Typedef AFE_Parameters_RS485_Struction = AFE_PARAMETERS_RS485_STRUCTION_DEFAULT;

/* �õ����ⲿ������ */
extern UINT8 ucMTPBuffer[26];
extern UINT16 iSheldTemp_10K_NTC[141];

#define DSG_CHG_OCP_DELAY_TIME 30 * 100 // ������ʱʱ�� (10ms��λ)
#define OFF 0
#define ON 1

/* һ������������־ */
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
		/* ��ʱ30s����Ȩ����AFE */
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

	/* �������ж� */
	flag = g_stCellInfoReport.unMdlFault_First.bits.b1IchgOcp;
	Ocp_MosJudge(g_stCellInfoReport.u16Ichg,
				 AFE_Parameters_RS485_Struction.u16IchgOcp_First.curValue, AFE_Parameters_RS485_Struction.u16IchgOcp_Filter_First.curValue, &flag, 0);
	g_stCellInfoReport.unMdlFault_First.bits.b1IchgOcp = flag;

	/*�ŵ�����ж�*/
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
			/* ǿ�йرճ��� */
			SH367309_DriverMos_Ctrl(GPIO_CHG, OFF);
			IchgOcp_Flag = 1;
		}
		break;
	case 1:
		if (!g_stCellInfoReport.unMdlFault_First.bits.b1IchgOcp)
		{
			/* MCU�����Ƴ��ܣ���AFE���� */
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
			/* ǿ�йرշŵ�� */
			SH367309_DriverMos_Ctrl(GPIO_DSG, OFF);
			IdischgOcp_Flag = 1;
		}
		break;
	case 1:
		if (!g_stCellInfoReport.unMdlFault_First.bits.b1IdischgOcp)
		{
			/* MCU�����Ʒŵ�ܣ���AFE���� */
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
			/* MCU�����Ʒŵ�ܣ���AFE���� */

			if (g_stCellInfoReport.unMdlFault_First.bits.b1IdischgOcp)
			{										   // ���д����ʵ��̫�ã��ò����ж����
				SH367309_DriverMos_Ctrl(GPIO_CHG, ON); // �������������±������ܣ����ⲻ��
			}										   // ������ˣ���Ҫ˼�������ָ�������
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

/* �ҳ�Ҫд��AFE�Ĵ�����ֵ������1����ǰҪд��ֵ������2��AFE�����б�ĵ�ַ */
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

// ȫ������ˢ�µ�AFE_ROM_PARAMETERS_Struction�ṹ������
// һ������Ĭ�ϵ����ã���.h�ļ���#define������
// һ�����ǿ��޸ĵģ�AFE_Parameters_RS485_Struction������
void Refresh_Parameters(void)
{
	int i = 0;
	int temp = 0;
	UINT8 TR = 0;
	UINT16 AFE_TEMPERATURE[8] = {0}; // �¶ȣ����϶�+40����0�ȵ�ֵΪ40��

	// ��309��TR��˳���AFEĬ��ֵ���ô���AFE_ROM_PARAMETERS_Struction�ṹ��(#define����)��
	if (MTPRead(0x19, 1, &TR))
	{
		SH367309_Reg_Store.TR_ResRef = 680 + 5 * (TR & 0x7F);
		ucMTPBuffer[25] = TR & 0x7F;

/* ��Ĭ�ϵ����ݷ��ڲ����ṹ���� */
#ifdef _SLEEP_WITH_CURRENT
		// ���ߴ������Ҳ���ҪԤ�书��
		// �궨���ٸ����ţ������ˣ��������ȼ�����
		ucMTPBuffer[1] = (BYTE_01H_SCONF2)&0xF3;
#endif
		memcpy((UINT8 *)&AFE_ROM_PARAMETERS_Struction, ucMTPBuffer, 26);
	}

	g_u32CS_Res_AFE = ((UINT32)OtherElement.u16Sys_CS_Res_Num * 1000) / OtherElement.u16Sys_CS_Res;

	/* ���� */
	AFE_ROM_PARAMETERS_Struction.m00H_01H.CN = OtherElement.u16Sys_SeriesNum % 16;

	/* ����ѹ */
	AFE_ROM_PARAMETERS_Struction.m02H_03H.OVH = ((AFE_Parameters_RS485_Struction.u16VcellOvp.curValue / 5) >> 8) & 0x3;
	AFE_ROM_PARAMETERS_Struction.m02H_03H.OVL = (AFE_Parameters_RS485_Struction.u16VcellOvp.curValue / 5) & 0x00FF;
	/* ����ѹ��ʱʱ�� */
	temp = AFE_Parameters_RS485_Struction.u16VcellOvp_Filter.curValue * 10;
	AFE_ROM_PARAMETERS_Struction.m02H_03H.OVT = Choose_Right_Value(temp, AFE_OVT_UVT);
	/* ����ѹ�ָ� */
	AFE_ROM_PARAMETERS_Struction.m04H_05H.OVRH = ((AFE_Parameters_RS485_Struction.u16VcellOvp_Rcv.curValue / 5) >> 8) & 0x3;
	AFE_ROM_PARAMETERS_Struction.m04H_05H.OVRL = (AFE_Parameters_RS485_Struction.u16VcellOvp_Rcv.curValue / 5) & 0x00FF;

	/*�ŵ��ѹ��ʱʱ�� */
	temp = AFE_Parameters_RS485_Struction.u16VcellUvp_Filter.curValue * 10;
	AFE_ROM_PARAMETERS_Struction.m04H_05H.UVT = Choose_Right_Value(temp, AFE_OVT_UVT);
	/* �ŵ��ѹ */
	AFE_ROM_PARAMETERS_Struction.m06H_07H.UV = (AFE_Parameters_RS485_Struction.u16VcellUvp.curValue / 20) & 0x00FF;
	/* �ŵ��ѹ�ָ� */
	AFE_ROM_PARAMETERS_Struction.m06H_07H.UVR = (AFE_Parameters_RS485_Struction.u16VcellUvp_Rcv.curValue / 20) & 0x00FF;

	/* �ŵ����:     ��������ֵ��A*10��/         */
	temp = AFE_Parameters_RS485_Struction.u16IdsgOcp_Second.curValue * 100 / g_u32CS_Res_AFE; // ��ǰ��Ӧ����mv
	AFE_ROM_PARAMETERS_Struction.m0CH_0DH.OCD1V = Choose_Right_Value(temp, AFE_OCD1V_OCCV);
	/* �ŵ�����˲�ʱ�� */
	temp = AFE_Parameters_RS485_Struction.u16IdsgOcp_Filter_Second.curValue * 10; // ��ǰ��Ӧ����ms
	AFE_ROM_PARAMETERS_Struction.m0CH_0DH.OCD1T = Choose_Right_Value(temp, AFE_OCD1T);

	/* ������ */
	temp = AFE_Parameters_RS485_Struction.u16IchgOcp_Second.curValue * 100 / g_u32CS_Res_AFE; // ��ǰ��Ӧ����mv
	AFE_ROM_PARAMETERS_Struction.m0EH_0FH.OCCV = Choose_Right_Value(temp, AFE_OCD1V_OCCV);
	/* �������˲�ʱ�� */
	temp = AFE_Parameters_RS485_Struction.u16IchgOcp_Filter_Second.curValue * 10; // ��ǰ��Ӧ����ms
	AFE_ROM_PARAMETERS_Struction.m0EH_0FH.OCCT = Choose_Right_Value(temp, AFE_OCCT_OCD2T);

	/* ��·��ʱ */
	temp = AFE_Parameters_RS485_Struction.u16CBC_DelayT.curValue;
	AFE_ROM_PARAMETERS_Struction.m0EH_0FH.SCT = Choose_Right_Value(temp, AFE_SCT);
	/* ��·��ѹ */
	temp = AFE_Parameters_RS485_Struction.u16CBC_Cur_DSG.curValue * 1000 / g_u32CS_Res_AFE; // ��ǰ��Ӧ����mv
	AFE_ROM_PARAMETERS_Struction.m0EH_0FH.SCV = Choose_Right_Value(temp, AFE_SCV);

	/* ���е��¶ȱ��� */
	AFE_TEMPERATURE[0] = AFE_Parameters_RS485_Struction.u16TChgOTp.curValue / 10;		 /* �����±��� */
	AFE_TEMPERATURE[1] = AFE_Parameters_RS485_Struction.u16TChgOTp_Rcv.curValue / 10;	 /* �����±����ָ� */
	AFE_TEMPERATURE[2] = AFE_Parameters_RS485_Struction.u16TchgUTp.curValue / 10;		 /* �����±��� */
	AFE_TEMPERATURE[3] = AFE_Parameters_RS485_Struction.u16TchgUTp_Rcv.curValue / 10;	 /* �����±����ָ� */
	AFE_TEMPERATURE[4] = AFE_Parameters_RS485_Struction.u16TdischgOTp.curValue / 10;	 /* �ŵ���±��� */
	AFE_TEMPERATURE[5] = AFE_Parameters_RS485_Struction.u16TdischgOTp_Rcv.curValue / 10; /* �ŵ���±����ָ� */
	AFE_TEMPERATURE[6] = AFE_Parameters_RS485_Struction.u16TdischgUTp.curValue / 10;	 /* �ŵ���±��� */
	AFE_TEMPERATURE[7] = AFE_Parameters_RS485_Struction.u16TdischgUTp_Rcv.curValue / 10; /* �ŵ���±����ָ� */

	for (i = 0; i < 8; i++)
	{
		temp = iSheldTemp_10K_NTC[AFE_TEMPERATURE[i]];
		*(((UINT8 *)&AFE_ROM_PARAMETERS_Struction.m11H_19H) + i) = (UINT8)(((UINT32)temp << 9) / ((UINT32)SH367309_Reg_Store.TR_ResRef + temp));
	}
}

/* ÿ�����ݸı䶼��ȡrom�����Ƚ�һ�£��Ǹ������ı��д����=�ĸ� */
void Write_Parameters(void)
{
	int i = 0;
	UINT8 temp[26] = {0};
	UINT8 *P = (UINT8 *)&AFE_ROM_PARAMETERS_Struction;

	if (MTPRead(0x00, 25, temp))
	{
		for (i = 0; i < 25; i++)
		{ // ���һ��TR�����Ա�
			if (temp[i] != P[i])
			{
				MTPWriteROM(i, 1, P + i); // ��дEEPROM�ļĴ���������
			}
		}
	}
}

// ������ʱ��AFE_PARAM_WRITE_Flag=1��Ĭ��ֵ�����Կ�����ʱ���ִ��һ�Ρ�
void SH367309_UpdataAfeConfig(void)
{
	if (AFE_PARAM_WRITE_Flag)
	{
		AFE_PARAM_WRITE_Flag = 0;
		MCUO_AFE_VPRO = 1; // ������дģʽ
		Delay1ms(20);
		Feed_WatchDog;

		Refresh_Parameters();
		Write_Parameters();

		Feed_WatchDog;
		MCUO_AFE_VPRO = 0; // �˳���дģʽ
		Delay1ms(1);

		/* ÿ��д�����������Ҫ��λһ�¡�����д��ȥ�Ĳ�������Ч */
		if (!System_ERROR_UserCallback(ERROR_STATUS_AFE1))
		{
			AFE_Reset(); // Reset IC
			Delay1ms(5);
			AFE_IsReady();
			AFE_ResetFlag = 1;
		}
	}
}

/*********************************** ����������Ϊ�˱��浽EEPROM����� **********************************/

UINT8 Sci_WrRegs_0x10_AFE_Parameters(UINT16 u16Channel, struct RS485MSG *s)
{
	UINT16 u16WrRegNum;
	UINT16 u16SciRegStartAddr;

	int i = 0;
	UINT16 offset = 0;
	UINT16 *P = (UINT16 *)&AFE_Parameters_RS485_Struction;
	u16SciRegStartAddr = s->u16Buffer[3] + (s->u16Buffer[2] << 8);
	u16WrRegNum = s->u16Buffer[5] + (s->u16Buffer[4] << 8);

	/* ��ʼ��ַ��AFE������Χ������ʼ��ַ+д�ļĴ��������ڷ�Χ�� */
	if ((u16SciRegStartAddr >= RS485_CMD_ADDR_AFE_ROM_PARAMETERS_START) && (u16SciRegStartAddr <= RS485_CMD_ADDR_AFE_ROM_PARAMETERS_END) && (u16SciRegStartAddr + u16WrRegNum - 1 <= RS485_CMD_ADDR_AFE_ROM_PARAMETERS_END))
	{
		offset = u16SciRegStartAddr - RS485_CMD_ADDR_AFE_ROM_PARAMETERS_START;

		Feed_WatchDog;
		for (i = 0; i < u16WrRegNum; i++)
		{
			*(P + (i + offset) * 4) = s->u16Buffer[8 + i * 2] + (s->u16Buffer[7 + i * 2] << 8);

			/* ֱ��д��EEPROM�� */
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
		*(P + i * 4 - 1) = *(P + i * 4); // ��ǰֵ��ΪĬ��ֵ
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
