#include "main.h"

struct RS485MSG g_stCurrentMsgPtr_SCI1;
UINT16 gu16_CommuErrCnt_SCI1 = 0; // SCI通信异常计数
UINT8 gu8_TxEnable_SCI1 = 0;
UINT8 gu8_TxFinishFlag_SCI1 = 0;

struct RS485MSG g_stCurrentMsgPtr_SCI2;
UINT16 gu16_CommuErrCnt_SCI2 = 0; // SCI通信异常计数
UINT8 gu8_TxEnable_SCI2 = 0;
UINT8 gu8_TxFinishFlag_SCI2 = 0;

struct RS485MSG g_stCurrentMsgPtr_SCI3;
UINT16 gu16_CommuErrCnt_SCI3 = 0; // SCI通信异常计数
UINT8 gu8_TxEnable_SCI3 = 0;
UINT8 gu8_TxFinishFlag_SCI3 = 0;

UINT8 g_u8SCITxBuff[SCI_TX_BUF_LEN];

struct stCell_Info g_stCellInfoReport;
UINT8 u8FlashUpdateFlag = 0;
UINT8 u8FlashUpdateE2PROM = 0;

void Sci_WrRegs_0x10_CalibCoef(UINT16 u16Channel, struct RS485MSG *s);
void Sci_WrRegs_0x10_Protect(UINT16 u16Channel, struct RS485MSG *s);
void Sci_WrRegs_0x10_SocTable(struct RS485MSG *s);
void Sci_WrRegs_0x10_CopperLoss(struct RS485MSG *s);
void Sci_WrRegs_0x10_RTC(struct RS485MSG *s);
void Sci_WrRegs_0x10_Balance(struct RS485MSG *s);
void Sci_WrRegs_0x10_SysOther(struct RS485MSG *s);
void Sci_WrRegs_0x10_SleepElement(struct RS485MSG *s);
void Sci_WrRegs_0x10_SocElement(struct RS485MSG *s);
void Sci_WrRegs_0x10_SystemElement(struct RS485MSG *s);
void Sci_WrRegs_0x10_HeatCoolElement(struct RS485MSG *s);
void Sci_WrRegs_0x10_FlashConnect(struct RS485MSG *s);
void Sci_WrRegs_0x10_SN_Version(UINT16 startADDR, struct RS485MSG *s);

void Sci_WrReg_0x06_Reset_CalibCoef(struct RS485MSG *s);
void Sci_WrReg_0x06_Reset_ProtectRecord(struct RS485MSG *s);
void Sci_WrReg_0x06_Reset_ProtectElement(struct RS485MSG *s);
void Sci_WrReg_0x06_Reset_OtherCanAdd(struct RS485MSG *s);
void Sci_WrReg_0x06_Reset_HeatCool(struct RS485MSG *s);
void Sci_WrReg_0x06_SwitchON(struct RS485MSG *s);
void Sci_WrReg_0x06_SwitchOFF(struct RS485MSG *s);
void Sci_WrReg_0x06_BMS_FunctionON(struct RS485MSG *s);
void Sci_WrReg_0x06_BMS_FunctionOFF(struct RS485MSG *s);
void Sci_WrReg_0x06_SetSocOnce(struct RS485MSG *s);

void Sci_DataInit(struct RS485MSG *s)
{
	UINT16 i;

	s->ptr_no = 0;
	s->csr = RS485_STA_IDLE;
	s->enRs485CmdType = RS485_CMD_READ_REGS;
	for (i = 0; i < RS485_MAX_BUFFER_SIZE; i++)
	{
		s->u16Buffer[i] = 0;
	}
	for (i = 0; i < SCI_TX_BUF_LEN; i++)
	{
		g_u8SCITxBuff[i] = 0;
	}
}

void CRC_verify(struct RS485MSG *s)
{
	UINT16 u16SciVerify;
	UINT16 t_u16FrameLenth;

	t_u16FrameLenth = s->ptr_no - 2;
	u16SciVerify = s->u16Buffer[t_u16FrameLenth] + (s->u16Buffer[t_u16FrameLenth + 1] << 8);
	if (u16SciVerify == Sci_CRC16RTU((UINT8 *)s->u16Buffer, t_u16FrameLenth))
	{
		s->AckType = RS485_ACK_POS;
	}
	else
	{
		s->u16RdRegByteNum = 0;
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CRC_ERROR;
	}
}

void Sci_Deal_ReadRegs_0x03(struct RS485MSG *s)
{
	UINT16 t_u16Temp;

	t_u16Temp = s->u16Buffer[3] + (s->u16Buffer[2] << 8);
	s->u16RdRegStartAddrActure = t_u16Temp;

	if (t_u16Temp >= RS485_ADDR_RO_START2)
	{ // 1个字
		t_u16Temp -= (RS485_ADDR_RO_START2 - 63 - 33);
	}

	else if (t_u16Temp >= RS485_ADDR_RO_START1)
	{ // 33个字
		t_u16Temp -= (RS485_ADDR_RO_START1 - 63);
	}

	else if (t_u16Temp >= RS485_ADDR_RO_START0)
	{ // 63个字
		t_u16Temp -= RS485_ADDR_RO_START0;
	}
	// 新加进来的
	else if (t_u16Temp >= RS485_ADDR_RO_LCD)
	{
		t_u16Temp -= RS485_ADDR_RO_LCD; // LCD，有一次顺序乱了，显示数据不对导致找不到原因
	}
	else if (t_u16Temp >= RS485_ADDR_RW_AFE_PARAMETER)
	{
		t_u16Temp -= RS485_ADDR_RW_AFE_PARAMETER; // AFE，耕耘代码添加，前面出问题是忘了这里要添加
	}
	else if (t_u16Temp >= RS485_ADDR_RW_OTHER_CANADD)
	{
		t_u16Temp -= RS485_ADDR_RW_OTHER_CANADD;
	}
	else if (t_u16Temp >= RS485_ADDR_RW_OTHER)
	{
		t_u16Temp -= RS485_ADDR_RW_OTHER;
	}
	else if (t_u16Temp >= RS485_ADDR_RW_PORTECT)
	{
		t_u16Temp -= RS485_ADDR_RW_PORTECT;
	}
	else if (t_u16Temp >= RS485_ADDR_RW_CALIB)
	{
		t_u16Temp -= RS485_ADDR_RW_CALIB;
	}

	s->u16RdRegStartAddr = t_u16Temp;
	s->u16RdRegByteNum = (s->u16Buffer[5] + (s->u16Buffer[4] << 8)) << 1;
}

void Sci_Deal_WrReg_0x06(struct RS485MSG *s)
{
	UINT16 u16SciRegAddr;
	u16SciRegAddr = s->u16Buffer[3] + (s->u16Buffer[2] << 8);
	switch (u16SciRegAddr)
	{
	case RS485_CMD_ADDR_RESET_CALIB_COEF:
		Sci_WrReg_0x06_Reset_CalibCoef(s);
		break;

	case RS485_CMD_ADDR_RESET_PROTECT_RECORD:
		Sci_WrReg_0x06_Reset_ProtectRecord(s);
		break;

	case RS485_CMD_ADDR_RESET_PROTECT_ELEMENT:
		Sci_WrReg_0x06_Reset_ProtectElement(s);
		break;

	case RS485_CMD_ADDR_RESET_OTHER_CANADD:
		Sci_WrReg_0x06_Reset_OtherCanAdd(s);
		break;

	case RS485_CMD_ADDR_RESET_HEAT_COOL:
		Sci_WrReg_0x06_Reset_HeatCool(s);
		break;

	case RS485_CMD_ADDR_SWITCH_ON:
		Sci_WrReg_0x06_SwitchON(s);
		break;

	case RS485_CMD_ADDR_SWITCH_OFF:
		Sci_WrReg_0x06_SwitchOFF(s);
		break;

	case RS485_CMD_ADDR_SYSTEM_FUNCTION_ON:
		Sci_WrReg_0x06_BMS_FunctionON(s);
		break;

	case RS485_CMD_ADDR_SYSTEM_FUNCTION_OFF:
		Sci_WrReg_0x06_BMS_FunctionOFF(s);
		break;

	case RS485_CMD_ADDR_SET_ONCE_SOC:
		Sci_WrReg_0x06_SetSocOnce(s);
		break;

	// 中颖AFE参数可读可写新增
	case RS485_CMD_ADDR_RESET_AFE_PARAMETERS:
		Sci_WrReg_0x06_Reset_AFE_Parameters(s);
		break;

	case RS485_CMD_ADDR_RESET_EVENT_RECORD:
		Sci_WrReg_0x06_Reset_EventRecord(s);
		break;

	default:
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_NO_PERMISSION;
		break;
	}
}

// 主体OK
void Sci_Deal_WrRegs_0x10(struct RS485MSG *s)
{
	UINT16 u16SciRegStartAddr;
	u16SciRegStartAddr = s->u16Buffer[3] + (s->u16Buffer[2] << 8);

	if (Sci_WrRegs_0x10_AFE_Parameters(u16SciRegStartAddr, s))
	{
		return;
	}

	switch (u16SciRegStartAddr)
	{
	case RS485_CMD_ADDR_VC1CALIB_K:
	case RS485_CMD_ADDR_VC2CALIB_K:
	case RS485_CMD_ADDR_VC3CALIB_K:
	case RS485_CMD_ADDR_VC4CALIB_K:
	case RS485_CMD_ADDR_VC5CALIB_K:
	case RS485_CMD_ADDR_VC6CALIB_K:
	case RS485_CMD_ADDR_VC7CALIB_K:
	case RS485_CMD_ADDR_VC8CALIB_K:
	case RS485_CMD_ADDR_VC9CALIB_K:
	case RS485_CMD_ADDR_VC10CALIB_K:
	case RS485_CMD_ADDR_VC11CALIB_K:
	case RS485_CMD_ADDR_VC12CALIB_K:
	case RS485_CMD_ADDR_VC13CALIB_K:
	case RS485_CMD_ADDR_VC14CALIB_K:
	case RS485_CMD_ADDR_VC15CALIB_K:
	case RS485_CMD_ADDR_VC16CALIB_K:
	case RS485_CMD_ADDR_VC17CALIB_K:
	case RS485_CMD_ADDR_VC18CALIB_K:
	case RS485_CMD_ADDR_VC19CALIB_K:
	case RS485_CMD_ADDR_VC20CALIB_K:
	case RS485_CMD_ADDR_VC21CALIB_K:
	case RS485_CMD_ADDR_VC22CALIB_K:
	case RS485_CMD_ADDR_VC23CALIB_K:
	case RS485_CMD_ADDR_VC24CALIB_K:
	case RS485_CMD_ADDR_VC25CALIB_K:
	case RS485_CMD_ADDR_VC26CALIB_K:
	case RS485_CMD_ADDR_VC27CALIB_K:
	case RS485_CMD_ADDR_VC28CALIB_K:
	case RS485_CMD_ADDR_VC29CALIB_K:
	case RS485_CMD_ADDR_VC30CALIB_K:
	case RS485_CMD_ADDR_VC31CALIB_K:
	case RS485_CMD_ADDR_VC32CALIB_K:
	case RS485_CMD_ADDR_AFE1CALIB_K:
	case RS485_CMD_ADDR_AFE2CALIB_K:
	case RS485_CMD_ADDR_VBUSCALIB_K:
	case RS485_CMD_ADDR_ICHGCALIB_K:
	case RS485_CMD_ADDR_IDISCHGCALIB_K:
	case RS485_CMD_ADDR_TEMP1_CALIB_K:
	case RS485_CMD_ADDR_TEMP2_CALIB_K:
	case RS485_CMD_ADDR_TEMP3_CALIB_K:
	case RS485_CMD_ADDR_TEMP4_CALIB_K:
	case RS485_CMD_ADDR_TEMP5_CALIB_K:
	case RS485_CMD_ADDR_TEMP6_CALIB_K:
	case RS485_CMD_ADDR_TEMP_ENV1_CALIB_K:
	case RS485_CMD_ADDR_TEMP_ENV2_CALIB_K:
	case RS485_CMD_ADDR_TEMP_ENV3_CALIB_K:
	case RS485_CMD_ADDR_TEMP_MOS_CALIB_K:
		Sci_WrRegs_0x10_CalibCoef(u16SciRegStartAddr, s);
		break;

	case RS485_CMD_ADDR_VCELL_OVP_FIRST:
	case RS485_CMD_ADDR_VCELL_UVP_FIRST:
	case RS485_CMD_ADDR_VBUS_OVP_FIRST:
	case RS485_CMD_ADDR_VBUS_UVP_FIRST:
	case RS485_CMD_ADDR_ICHG_OCP_FIRST:
	case RS485_CMD_ADDR_IDSG_OCP_FIRST:
	case RS485_CMD_ADDR_TCHG_OTP_FIRST:
	case RS485_CMD_ADDR_TCHG_UTP_FIRST:
	case RS485_CMD_ADDR_TDSG_OTP_FIRST:
	case RS485_CMD_ADDR_TDSG_UTP_FIRST:
	case RS485_CMD_ADDR_TMOS_OTP_FIRST:
	case RS485_CMD_ADDR_VDELTA_OP_FIRST:
	case RS485_CMD_ADDR_SOC_UP_FIRST:
		Sci_WrRegs_0x10_Protect(u16SciRegStartAddr, s);
		break;

	case RS485_CMD_ADDR_SOC_VOLTAGE1:
		Sci_WrRegs_0x10_SocTable(s);
		break;

	case RS485_CMD_ADDR_COPPERLOSS1:
		Sci_WrRegs_0x10_CopperLoss(s);
		break;

	case RS485_CMD_ADDR_RTC_TIME_YEAR:
		Sci_WrRegs_0x10_RTC(s);
		break;

	case RS485_CMD_ADDR_BALANCE_OV:
		Sci_WrRegs_0x10_Balance(s);
		break;

	case RS485_CMD_ADDR_CS_CUR_CHGMAX:
		Sci_WrRegs_0x10_SysOther(s);
		break;

	case RS485_CMD_ADDR_SLEEP_V_NORMAL:
		Sci_WrRegs_0x10_SleepElement(s);
		break;

	case RS485_CMD_ADDR_SOC_AH:
		Sci_WrRegs_0x10_SocElement(s);
		break;

	case RS485_CMD_ADDR_SYS_SERIES_NUM:
		Sci_WrRegs_0x10_SystemElement(s);
		break;

	case RS485_CMD_ADDR_HEAT_DSG_HIGH:
		Sci_WrRegs_0x10_HeatCoolElement(s);
		break;

	case RS485_ADDR_SN_SERIAL_NUM:
	case RS485_ADDR_SN_HAEDWARE_VER:
	case RS485_ADDR_SN_SOFTWARE_VER:
		Sci_WrRegs_0x10_SN_Version(u16SciRegStartAddr, s);
		break;

	case RS485_CMD_ADDR_FLASH_CONNECT:
		Sci_WrRegs_0x10_FlashConnect(s);
		break; // 少了个BREAK导致OVER。

	default:
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CMD_INVALID;
		break;
	}
}

void Sci_ACK_0x03_ReadRegs_LCD(struct RS485MSG *s, UINT8 t_u8BuffTemp[])
{
	UINT16 u16SciTemp;
	UINT16 i, j;
	INT8 k, x;

	i = 0;
	switch (s->u16RdRegStartAddr)
	{
	case 0: // LCD
		u16SciTemp = 1;
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

		u16SciTemp = (g_stCellInfoReport.u16VCellTotle + 50) / 100;
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

		if (g_stCellInfoReport.u16Ichg > 0)
		{
			u16SciTemp = (g_stCellInfoReport.u16Ichg + 5005) / 10;
		}
		else
		{
			u16SciTemp = (5000 - g_stCellInfoReport.u16IDischg) / 10;
		}
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

		u16SciTemp = (g_stCellInfoReport.u16TempMax + 5) / 10;
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

		u16SciTemp = g_stCellInfoReport.SocElement.u16Soc;
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
		break;

	case 1: // 上位机第三级保护，60+10=70个
		for (j = 0; j < Record_len; j++)
		{
			k = FaultPoint_Third - 1 - j;
			if (k < 0)
			{
				k = Record_len + k;
			}
			u16SciTemp = Fault_record_Third[k];
			t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
			t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

			for (x = 0; x < 6; ++x)
			{
				u16SciTemp = RTC_Fault_record_Third[k][x];
				t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
				t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
			}
		}
		break;

	case 2: // 序列号，硬件版本号，软件版本号
		for (j = 0; j < PRODUCT_ID_LENGTH_MAX; j++)
		{
			t_u8BuffTemp[i++] = ProductionInfor.BMS_SerialNumber[j];
		}
		for (j = 0; j < PRODUCT_ID_LENGTH_MAX; j++)
		{
			t_u8BuffTemp[i++] = ProductionInfor.BMS_HardWareVersion[j];
		}
		for (j = 0; j < PRODUCT_ID_LENGTH_MAX; j++)
		{
			t_u8BuffTemp[i++] = ProductionInfor.BMS_SoftWareVersion[j];
		}
		break;

	case 8:
		Sci_ACK_0x03_ReadRegs_EventRecord(t_u8BuffTemp);
		break;

	default:
		s->u16RdRegStartAddr = 0;
		break;
	}
	s->u16RdRegStartAddr = 0;
}

void Sci_ACK_0x03_ReadRegs_Data(struct RS485MSG *s, UINT8 t_u8BuffTemp[])
{
	UINT16 u16SciTemp;
	UINT16 i = 0, j;
	INT8 k;
	UINT8 a[4];

	for (j = 0; j < 63; j++)
	{ // 0xD000_63
		u16SciTemp = *(&g_stCellInfoReport.u16VCell[0] + j);
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	}

	// 0xD100_33
	u16SciTemp = (UINT16)(RTC_time.RTC_Time_Month) | (RTC_time.RTC_Time_Year << 8);
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = (UINT16)(RTC_time.RTC_Time_Hour) | (RTC_time.RTC_Time_Day << 8);
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = (UINT16)(RTC_time.RTC_Time_Second) | (RTC_time.RTC_Time_Minute << 8);
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	for (j = 0; j < 4; j++)
	{
		k = FaultPoint_First2 - 1 - j;
		if (k < 0)
		{
			k = Record_len + k;
		}
		a[j] = k;
	}
	u16SciTemp = (Fault_record_First2[a[0]] << 8) | Fault_record_First2[a[1]];
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	u16SciTemp = (Fault_record_First2[a[2]] << 8) | Fault_record_First2[a[3]];
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	for (j = 0; j < 4; j++)
	{
		k = FaultPoint_Second2 - 1 - j;
		if (k < 0)
		{
			k = Record_len + k;
		}
		a[j] = k;
	}
	u16SciTemp = (Fault_record_Second2[a[0]] << 8) | Fault_record_Second2[a[1]];
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	u16SciTemp = (Fault_record_Second2[a[2]] << 8) | Fault_record_Second2[a[3]];
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	for (j = 0; j < 4; j++)
	{
		k = FaultPoint_Third2 - 1 - j;
		if (k < 0)
		{
			k = Record_len + k;
		}
		a[j] = k;
	}
	u16SciTemp = (Fault_record_Third2[a[0]] << 8) | Fault_record_Third2[a[1]];
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	u16SciTemp = (Fault_record_Third2[a[2]] << 8) | Fault_record_Third2[a[3]];
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	for (j = 0; j < 12; j++)
	{ // 0xD002到这里。
		u16SciTemp = ((*(&System_ErrFlag.u8ErrFlag_Com_AFE1 + 2 * j)) << 8) | (*(&System_ErrFlag.u8ErrFlag_Com_AFE1 + 2 * j + 1));
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	}

	switch (OPEN)
	{
	case 0:
		u16SciTemp = ((~((UINT16)(SystemStatus.all & 0x0000FFFF))) & 0x00FE) | (((UINT16)(SystemStatus.all & 0x0000FFFF)) & 0xFF01);
		break;
	case 1:
		u16SciTemp = (UINT16)(SystemStatus.all & 0x0000FFFF);
		break;
	default:
		u16SciTemp = (UINT16)(SystemStatus.all & 0x0000FFFF);
		break;
	}
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = (UINT16)(SystemStatus.all >> 16);
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = (UINT16)(System_OnOFF_Func.all & 0x0000FFFF);
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = (UINT16)(System_OnOFF_Func.all >> 16);
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = (UINT16)0;
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = (UINT16)0;
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = Heat_Cool_FaultFlag.all; // 可以加多一个
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = 0; // 可以加多一个
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = 0; // 可以加多一个
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = 0; // 可以加多一个
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = 0; // 可以加多一个
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = 0; // 可以加多一个
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	// 0xD200_1
	u16SciTemp = 0; // 可以加多一个
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
}

/*=================================================================
 * FUNCTION: Sci_Tx_RW_Fun
 * PURPOSE : 将需要发送的数据进行更新
 * INPUT:    void
 *
 * RETURN:   void
 *
 * CALLS:    void
 *
 * CALLED BY:Sci2_Updata()
 *
 *=================================================================*/
void Sci_ACK_0x03_RW_Data_Pro(struct RS485MSG *s, UINT8 t_u8BuffTemp[])
{ // 65个
	UINT16 u16SciTemp;
	UINT16 i, j;
	i = 0;
	for (j = 0; j < E2P_PARA_NUM_PROTECT; j++)
	{
		u16SciTemp = *(&PRT_E2ROMParas.u16VcellOvp_First + j);
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	}
}

void Sci_ACK_0x03_RW_Data_Cali(struct RS485MSG *s, UINT8 t_u8BuffTemp[])
{ // 94个
	UINT16 u16SciTemp;
	UINT16 i, j;
	i = 0;
	for (j = 0; j < KB_NUM; j++)
	{
		u16SciTemp = g_u16CalibCoefK[j];
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
		u16SciTemp = g_i16CalibCoefB[j];
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	}
}

void Sci_ACK_0x03_RW_Data_Other(struct RS485MSG *s, UINT8 t_u8BuffTemp[])
{ // 86
	UINT16 u16SciTemp;
	UINT16 i, j;
	i = 0;
	for (j = 0; j < SOC_Size_TableCanSet; j++)
	{ // 由于GetEndValue()函数的问题，只能混在一起
		switch (OtherElement.u16Soc_TableSelect)
		{
		case SOC_TABLE_TEST:
			u16SciTemp = SOC_Table_Set[j];
			break;
		case SOC_TABLE_LIFEPO:
			u16SciTemp = SOC_Table_LiFePO[j];
			break;
		case SOC_TABLE_TERNARYLI:
			u16SciTemp = SocTable_TernaryLi[j];
			break;
		case SOC_TABLE_LIFEPO2:
			u16SciTemp = SocTable_LiFePO2[j];
			break;
		default:
			u16SciTemp = SOC_Table_Set[j];
			break;
		}
		// u16SciTemp = SOC_Table_Set[j];
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	}

	for (j = 0; j < CompensateNUM; j++)
	{
		u16SciTemp = CopperLoss[j];
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	}

	for (j = 0; j < CompensateNUM; j++)
	{
		u16SciTemp = CopperLoss_Num[j];
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	}

	for (j = 0; j < E2P_PARA_NUM_RTC; j++)
	{
		u16SciTemp = *(&RTC_time.RTC_Time_Year + j);
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	}
}

void Sci_ACK_0x03_RW_Data_OtherCanAdd(struct RS485MSG *s, UINT8 t_u8BuffTemp[])
{ // 32+24=56个
	UINT16 u16SciTemp;
	UINT16 i = 0, j;

	for (j = 0; j < E2P_PARA_NUM_OTHER_ELEMENT1; j++)
	{
		u16SciTemp = *(&OtherElement.u16Balance_OpenVoltage + j);
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	}

	for (j = 0; j < E2P_PARA_NUM_HEAT_COOL; j++)
	{
		u16SciTemp = *(&Heat_Cool_Element.u16Heat_OpenTemp + j);
		t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
	}
}

void Sci_ACK_0x03(struct RS485MSG *s)
{
	UINT8 i;
	UINT16 u16SciTemp;
	if (s->AckType == RS485_ACK_POS)
	{
		if (s->u16RdRegStartAddrActure >= RS485_ADDR_RW_CALIB)
		{
			if (s->u16RdRegStartAddrActure >= RS485_ADDR_RO_START0)
			{
				Sci_ACK_0x03_ReadRegs_Data(s, g_u8SCITxBuff);
			}
			else if (s->u16RdRegStartAddrActure >= RS485_ADDR_RO_LCD)
			{
				Sci_ACK_0x03_ReadRegs_LCD(s, g_u8SCITxBuff);
			}
			else if (s->u16RdRegStartAddrActure >= RS485_ADDR_RW_AFE_PARAMETER)
			{
				Sci_ACK_0x03_RW_AFE_Parameters(s, g_u8SCITxBuff);
			}
			else if (s->u16RdRegStartAddrActure >= RS485_ADDR_RW_OTHER_CANADD)
			{
				Sci_ACK_0x03_RW_Data_OtherCanAdd(s, g_u8SCITxBuff);
			}
			else if (s->u16RdRegStartAddrActure >= RS485_ADDR_RW_OTHER)
			{
				Sci_ACK_0x03_RW_Data_Other(s, g_u8SCITxBuff);
			}
			else if (s->u16RdRegStartAddrActure >= RS485_ADDR_RW_PORTECT)
			{
				Sci_ACK_0x03_RW_Data_Pro(s, g_u8SCITxBuff);
			}
			else
			{
				Sci_ACK_0x03_RW_Data_Cali(s, g_u8SCITxBuff);
			}
			// 头码，前三个字节保持不变
			s->u16Buffer[0] = (s->u16Buffer[0] != 0) ? RS485_SLAVE_ADDR : s->u16Buffer[0];
			s->u16Buffer[1] = s->enRs485CmdType;
			s->u16Buffer[2] = s->u16RdRegByteNum;
			// 数据
			for (i = 0; i < (s->u16RdRegByteNum); i++)
			{
				s->u16Buffer[i + 3] = g_u8SCITxBuff[i + ((s->u16RdRegStartAddr) << 1)];
			}
			i = s->u16RdRegByteNum + 3;
		}
	}
	else
	{
		i = 1;
		s->u16Buffer[i++] = s->enRs485CmdType | 0x80;
		s->u16Buffer[i++] = s->ErrorType;
	}
	u16SciTemp = Sci_CRC16RTU((UINT8 *)s->u16Buffer, i);
	s->u16Buffer[i++] = u16SciTemp & 0x00FF;
	s->u16Buffer[i++] = u16SciTemp >> 8;
	s->AckLenth = i;

	s->ptr_no = 0;
	s->csr = RS485_STA_TX_COMPLETE;
}

void Sci_ACK_0x06_0x10(struct RS485MSG *s)
{
	UINT8 i;
	UINT16 u16SciTemp;

	if (s->AckType == RS485_ACK_POS)
	{
		i = 6;
	}
	else
	{
		i = 1;
		s->u16Buffer[i++] = s->enRs485CmdType | 0x80;
		s->u16Buffer[i++] = s->ErrorType;
	}

	u16SciTemp = Sci_CRC16RTU((UINT8 *)s->u16Buffer, i);
	s->u16Buffer[i++] = u16SciTemp & 0x00FF;
	s->u16Buffer[i++] = u16SciTemp >> 8;
	s->AckLenth = i;

	s->ptr_no = 0;
	s->csr = RS485_STA_TX_COMPLETE;
}

#if (defined _COMMOM_UPPER_SCI1)
void Sci1_CommonUpper_FaultChk(void)
{
	UINT8 FaultCnt = 0;
	if (USART1->SR & 0x08)
	{ // 接收溢出错误，RXNEIE或EIE使能产生中断，开
		USART1->SR &= ~0x08;
		FaultCnt++;
	}

	if (USART1->SR & 0x04)
	{ // 检测到噪声，默认开，不开的话CR3的ONEBIT置1，不开
	  // USART_CR3的EIE使能中断
		USART1->SR &= ~0x04;
		FaultCnt++;
	}

	if (USART1->SR & 0x02)
	{						 // 帧错误，USART_CR3的EIE使能中断，开
		USART1->SR &= ~0x02; // 清除
		FaultCnt++;
	}

	if (USART1->SR & 0x01)
	{						 // 奇偶校验错误标志 USART_CR1的PEIE使能该中断，不开
		USART1->SR &= ~0x01; // 清除
		FaultCnt++;
	}

	if (FaultCnt)
	{
		gu16_CommuErrCnt_SCI1++;
	}
}

// 将接收数据解码，接收中断中调用
/*=================================================================
 * FUNCTION: Sci2_Rx_Deal
 * PURPOSE : 串口数据接收解码
 * INPUT:    void
 *
 * RETURN:   void
 *
 * CALLS:    void
 *
 * CALLED BY:ISR()
 *
 *=================================================================*/
void Sci1_CommonUpper_Rx_Deal(struct RS485MSG *s)
{
	// RC1IE = 0;// 禁止EUSART2 接收中断
	// s->u16Buffer[s->ptr_no] = RCREG1;                 //读RCREG寄存器来读取接收到的8位数据
	// NVIC_DisableIRQ(USART1_IRQn);
	USART1->CR1 &= ~(1 << 5);			  // 和上面那句话二选一

	s->u16Buffer[s->ptr_no] = USART1->DR; // 从RXFIFO 中读取接收到的数据
	
	if ((s->ptr_no == 0) && (s->u16Buffer[0] != RS485_SLAVE_ADDR) && (s->u16Buffer[0] != RS485_BROADCAST_ADDR))
	{
		s->ptr_no = 0;
		s->u16Buffer[0] = 0;
	}
	else
	{
		if (s->ptr_no == 1)
		{
			switch (s->u16Buffer[s->ptr_no])
			{
			case RS485_CMD_READ_REGS:
				s->enRs485CmdType = RS485_CMD_READ_REGS;
				break;
			case RS485_CMD_WRITE_REG:
				s->enRs485CmdType = RS485_CMD_WRITE_REG;
				break;
			case RS485_CMD_WRITE_REGS:
				s->enRs485CmdType = RS485_CMD_WRITE_REGS;
				break;
			default:
				s->ptr_no = RS485_MAX_BUFFER_SIZE;
				s->u16Buffer[0] = 0;
				s->u16Buffer[1] = 0;
				break;
			}
		}
		else if (s->ptr_no >= 2)
		{
			switch (s->enRs485CmdType)
			{
			case RS485_CMD_READ_REGS:
			case RS485_CMD_WRITE_REG:
				if (s->ptr_no == 7)
				{ //	receive complete
					s->csr = RS485_STA_RX_COMPLETE;
					// RCSTA1bits.CREN = 0;  //禁止接收
					// RC1IE = 0;			// 禁止EUSART2 接收中断
					USART1->CR1 &= ~(1 << 2);
					USART1->CR1 &= ~(1 << 5);
				}
				break;
			case RS485_CMD_WRITE_REGS:
				if ((s->ptr_no >= 7) && (s->ptr_no == (s->u16Buffer[6] + 8)))
				{
					s->csr = RS485_STA_RX_COMPLETE;
					// disable rx TODO
					// disable rx/tx interrupt TODO
					// RCSTA1bits.CREN = 0;    //禁止接收
					// RC1IE = 0;				// 禁止EUSART2 接收中断
					USART1->CR1 &= ~(1 << 2);
					USART1->CR1 &= ~(1 << 5);
				}
				break;
			default:
				s->ptr_no = RS485_MAX_BUFFER_SIZE;
				s->u16Buffer[0] = 0;
				break;
			}
		}
		s->ptr_no++;
		if (s->ptr_no >= RS485_MAX_BUFFER_SIZE)
		{
			s->ptr_no = 0;
			s->u16Buffer[0] = 0;
		}
	}
	USART1->CR1 |= (1 << 5);
}

void Sci1_CommonUpper_Tx_Deal(struct RS485MSG *s)
{
	static int delayFlag = 0;

	if (0 == gu8_TxEnable_SCI1)
	{
		return;
	}

	if (gu16_CommuErrCnt_SCI1)
	{ // 出现错误也得把数据全部接收完，然后不回复
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_TxFinishFlag_SCI1 = 1;
		gu8_TxEnable_SCI1 = 0;
		gu16_CommuErrCnt_SCI1 = 0;
		return;
	}

	if (delayFlag)
	{
		if (g_st_SysTimeFlag.bits.b1Sys10msFlag1)
		{
			if (++delayFlag == 6)
			{
				delayFlag = 0;
			}
		}
		return;
	}

	while (!((USART1->SR) & (1 << 7)))
		; // 1<<6 也可以
	if (s->ptr_no < s->AckLenth)
	{
		USART1->DR = s->u16Buffer[s->ptr_no]; // load data
		s->ptr_no++;
		if ((s->ptr_no == 19) || (s->ptr_no == 39) || (s->ptr_no == 59))
		{
			delayFlag = 1;
		}
	}
	else
	{
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_TxFinishFlag_SCI1 = 1;
		gu8_TxEnable_SCI1 = 0;
		if (u8FlashUpdateE2PROM)
		{
			u8FlashUpdateE2PROM = 0;
			u8FlashUpdateFlag = 1;
		}
	}
}

// 串口初始化函数
void InitSCI1_CommonUpper(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE); // PB6,PB7

	// Enable the USART1 Interrupt(使能USART1中断)
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// USART1_TX   PA9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // PA9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出，按道理是开漏输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// USART1_RX	  PA10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;			  // PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入，做RX
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// 串口初始化
	USART_InitStructure.USART_BaudRate = 19200;										// 设置串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 设置数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 设置停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 设置效验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 设置流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 设置工作模式
	USART_Init(USART1, &USART_InitStructure);

	USART1->CR3 |= 1 << 0; // EIE，开帧错误中断，同时开启噪声中断

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 使能接收中断
	USART_Cmd(USART1, ENABLE);					   // 使能串口1

	Sci_DataInit(&g_stCurrentMsgPtr_SCI1);
}

void App_CommonUpperSCI1(struct RS485MSG *s)
{
	switch (s->csr)
	{
	// IDLE-空闲态，保持50ms后使能接收（物理层）receive set
	case RS485_STA_IDLE:
	{
		break;
	}
	// receive complete, to deal the receive data
	case RS485_STA_RX_COMPLETE:
	{
		USART1->CR1 &= ~(1 << 5); // 禁止产生中断
		CRC_verify(s);
		if (s->AckType == RS485_ACK_POS)
		{
			switch (s->enRs485CmdType)
			{
			case RS485_CMD_READ_REGS:
				Sci_Deal_ReadRegs_0x03(s);
				break;
			case RS485_CMD_WRITE_REG:
				Sci_Deal_WrReg_0x06(s);
				break;
			case RS485_CMD_WRITE_REGS:
				Sci_Deal_WrRegs_0x10(s);
				break;
			default:
				s->u16RdRegByteNum = 0;
				s->AckType = RS485_ACK_NEG;
				s->ErrorType = RS485_ERROR_NULL;
				break;
			}
		}
		s->csr = RS485_STA_RX_OK; // receive the correct data, switch to transmit wait 50ms
		break;					  // 下一轮再来
	}
	// receive ok, to transmit wait 50ms
	case RS485_STA_RX_OK:
	{
		switch (s->enRs485CmdType)
		{
		case RS485_CMD_READ_REGS:
			Sci_ACK_0x03(s);
			break;
		case RS485_CMD_WRITE_REG:
		case RS485_CMD_WRITE_REGS:
			Sci_ACK_0x06_0x10(s);
			break;
		default: // 这个defualt不用加错误操作
			break;
		}
		USART1->CR1 |= (1 << 3); // 使能发送
		gu8_TxEnable_SCI1 = 1;
	}
	// transmit complete, to switch receive wait 20ms
	case RS485_STA_TX_COMPLETE:
	{
		if (gu8_TxFinishFlag_SCI1)
		{
			s->csr = RS485_STA_IDLE;
			s->u16Buffer[0] = 0;
			s->u16Buffer[1] = 0;
			s->u16Buffer[2] = 0;
			s->u16Buffer[3] = 0;
			gu8_TxFinishFlag_SCI1 = 0;
			s->ptr_no = 0;
			USART1->CR1 |= (1 << 2); // 使能接收
			USART1->CR1 |= (1 << 5); // 使能接收中断
			gu8_TxEnable_SCI1 = 0;
		}
		break;
	}

	default:
	{
		s->csr = RS485_STA_IDLE;
		break;
	}
	}
	Sci1_CommonUpper_Tx_Deal(s);
	// Sci1_FaultChk();	//没必要在这加
}

#endif

#if (defined _COMMOM_UPPER_SCI2)

void Sci2_CommonUpper_FaultChk(void)
{
	UINT8 FaultCnt = 0;
	if (USART2->SR & 0x08)
	{ // 接收溢出错误，RXNEIE或EIE使能产生中断，开
		USART2->SR &= ~0x08;
		FaultCnt++;
	}

	if (USART2->SR & 0x04)
	{ // 检测到噪声，默认开，不开的话CR3的ONEBIT置1，不开
	  // USART_CR3的EIE使能中断
		USART2->SR &= ~0x04;
		FaultCnt++;
	}

	if (USART2->SR & 0x02)
	{						 // 帧错误，USART_CR3的EIE使能中断，开
		USART2->SR &= ~0x02; // 清除
		FaultCnt++;
	}

	if (USART2->SR & 0x01)
	{						 // 奇偶校验错误标志 USART_CR1的PEIE使能该中断，不开
		USART2->SR &= ~0x01; // 清除
		FaultCnt++;
	}

	if (FaultCnt)
	{
		gu16_CommuErrCnt_SCI2++;
	}
}

// 将接收数据解码，接收中断中调用
/*=================================================================
 * FUNCTION: Sci2_Rx_Deal
 * PURPOSE : 串口数据接收解码
 * INPUT:    void
 *
 * RETURN:   void
 *
 * CALLS:    void
 *
 * CALLED BY:ISR()
 *
 *=================================================================*/
void Sci2_CommonUpper_Rx_Deal(struct RS485MSG *s)
{
	// RC1IE = 0;// 禁止EUSART2 接收中断
	// s->u16Buffer[s->ptr_no] = RCREG1;                 //读RCREG寄存器来读取接收到的8位数据
	// NVIC_DisableIRQ(USART2_IRQn);
	USART2->CR1 &= ~(1 << 5);			  // 和上面那句话二选一
	s->u16Buffer[s->ptr_no] = USART2->DR; // 从RXFIFO 中读取接收到的数据
	if ((s->ptr_no == 0) && (s->u16Buffer[0] != RS485_SLAVE_ADDR) && (s->u16Buffer[0] != RS485_BROADCAST_ADDR))
	{
		s->ptr_no = 0;
		s->u16Buffer[0] = 0;
	}
	else
	{
		if (s->ptr_no == 1)
		{
			switch (s->u16Buffer[s->ptr_no])
			{
			case RS485_CMD_READ_REGS:
				s->enRs485CmdType = RS485_CMD_READ_REGS;
				break;
			case RS485_CMD_WRITE_REG:
				s->enRs485CmdType = RS485_CMD_WRITE_REG;
				break;
			case RS485_CMD_WRITE_REGS:
				s->enRs485CmdType = RS485_CMD_WRITE_REGS;
				break;
			default:
				s->ptr_no = RS485_MAX_BUFFER_SIZE;
				s->u16Buffer[0] = 0;
				s->u16Buffer[1] = 0;
				break;
			}
		}
		else if (s->ptr_no >= 2)
		{
			switch (s->enRs485CmdType)
			{
			case RS485_CMD_READ_REGS:
			case RS485_CMD_WRITE_REG:
				if (s->ptr_no == 7)
				{ //	receive complete
					s->csr = RS485_STA_RX_COMPLETE;
					// RCSTA1bits.CREN = 0;  //禁止接收
					// RC1IE = 0;			// 禁止EUSART2 接收中断
					USART2->CR1 &= ~(1 << 2);
					USART2->CR1 &= ~(1 << 5);
				}
				break;
			case RS485_CMD_WRITE_REGS:
				if ((s->ptr_no >= 7) && (s->ptr_no == (s->u16Buffer[6] + 8)))
				{
					s->csr = RS485_STA_RX_COMPLETE;
					// disable rx TODO
					// disable rx/tx interrupt TODO
					// RCSTA1bits.CREN = 0;    //禁止接收
					// RC1IE = 0;				// 禁止EUSART2 接收中断
					USART2->CR1 &= ~(1 << 2);
					USART2->CR1 &= ~(1 << 5);
				}
				break;
			default:
				s->ptr_no = RS485_MAX_BUFFER_SIZE;
				s->u16Buffer[0] = 0;
				break;
			}
		}
		s->ptr_no++;
		if (s->ptr_no >= RS485_MAX_BUFFER_SIZE)
		{
			s->ptr_no = 0;
			s->u16Buffer[0] = 0;
		}
	}
	USART2->CR1 |= (1 << 5);
}

void Sci2_CommonUpper_Tx_Deal(struct RS485MSG *s)
{
	static int delayFlag = 0;

	if (0 == gu8_TxEnable_SCI2)
	{
		return;
	}

	if (gu16_CommuErrCnt_SCI2)
	{ // 出现错误也得把数据全部接收完，然后不回复
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_TxFinishFlag_SCI2 = 1;
		gu8_TxEnable_SCI2 = 0;
		gu16_CommuErrCnt_SCI2 = 0;
		return;
	}

	if (delayFlag)
	{
		if (g_st_SysTimeFlag.bits.b1Sys10msFlag1)
		{
			if (++delayFlag == 6)
			{
				delayFlag = 0;
			}
		}
		return;
	}

	while (!((USART2->SR) & (1 << 7)))
		; // 1<<6 也可以
	if (s->ptr_no < s->AckLenth)
	{
		USART2->DR = s->u16Buffer[s->ptr_no]; // load data
		s->ptr_no++;
		if ((s->ptr_no == 19) || (s->ptr_no == 39) || (s->ptr_no == 59))
		{
			delayFlag = 1;
		}
	}
	else
	{
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_TxFinishFlag_SCI2 = 1;
		gu8_TxEnable_SCI2 = 0;
		if (u8FlashUpdateE2PROM)
		{
			u8FlashUpdateE2PROM = 0;
			u8FlashUpdateFlag = 1;
		}
	}
}

// 串口初始化函数
void InitSCI2_CommonUpper(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// Enable the USART1 Interrupt(使能USART1中断)
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// USART2_TX   PA2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出，按道理是开漏输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART2_RX	  PA3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入，做RX
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// 串口初始化
	USART_InitStructure.USART_BaudRate = 19200;										// 设置串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 设置数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 设置停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 设置效验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 设置流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 设置工作模式
	USART_Init(USART2, &USART_InitStructure);

	USART2->CR3 |= 1 << 0; // EIE，开帧错误中断，同时开启噪声中断

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 使能接收中断
	USART_Cmd(USART2, ENABLE);					   // 使能串口2

	Sci_DataInit(&g_stCurrentMsgPtr_SCI2);
}

void App_CommonUpperSCI2(struct RS485MSG *s)
{
	switch (s->csr)
	{
	// IDLE-空闲态，保持50ms后使能接收（物理层）receive set
	case RS485_STA_IDLE:
	{
		break;
	}
	// receive complete, to deal the receive data
	case RS485_STA_RX_COMPLETE:
	{
		USART2->CR1 &= ~(1 << 5); // 禁止产生中断
		CRC_verify(s);
		if (s->AckType == RS485_ACK_POS)
		{
			switch (s->enRs485CmdType)
			{
			case RS485_CMD_READ_REGS:
				Sci_Deal_ReadRegs_0x03(s);
				break;
			case RS485_CMD_WRITE_REG:
				Sci_Deal_WrReg_0x06(s);
				break;
			case RS485_CMD_WRITE_REGS:
				Sci_Deal_WrRegs_0x10(s);
				break;
			default:
				s->u16RdRegByteNum = 0;
				s->AckType = RS485_ACK_NEG;
				s->ErrorType = RS485_ERROR_NULL;
				break;
			}
		}
		s->csr = RS485_STA_RX_OK; // receive the correct data, switch to transmit wait 50ms
		break;					  // 下一轮再来
	}
	// receive ok, to transmit wait 50ms
	case RS485_STA_RX_OK:
	{
		switch (s->enRs485CmdType)
		{
		case RS485_CMD_READ_REGS:
			Sci_ACK_0x03(s);
			break;
		case RS485_CMD_WRITE_REG:
		case RS485_CMD_WRITE_REGS:
			Sci_ACK_0x06_0x10(s);
			break;
		default: // 这个defualt不用加错误操作
			break;
		}
		USART2->CR1 |= (1 << 3); // 使能发送
		gu8_TxEnable_SCI2 = 1;
	}
	// transmit complete, to switch receive wait 20ms
	case RS485_STA_TX_COMPLETE:
	{
		if (gu8_TxFinishFlag_SCI2)
		{
			s->csr = RS485_STA_IDLE;
			s->u16Buffer[0] = 0;
			s->u16Buffer[1] = 0;
			s->u16Buffer[2] = 0;
			s->u16Buffer[3] = 0;
			gu8_TxFinishFlag_SCI2 = 0;
			s->ptr_no = 0;
			USART2->CR1 |= (1 << 2); // 使能接收
			USART2->CR1 |= (1 << 5); // 使能接收中断
			gu8_TxEnable_SCI2 = 0;
		}
		break;
	}

	default:
	{
		s->csr = RS485_STA_IDLE;
		break;
	}
	}
	Sci2_CommonUpper_Tx_Deal(s);
	// Sci1_FaultChk();	//没必要在这加
}

#endif

#if (defined _COMMOM_UPPER_SCI3)
void Sci3_CommonUpper_FaultChk(void)
{
	UINT8 FaultCnt = 0;
	if (USART3->SR & 0x08)
	{ // 接收溢出错误，RXNEIE或EIE使能产生中断，开
		USART3->SR &= ~0x08;
		FaultCnt++;
	}

	if (USART3->SR & 0x04)
	{ // 检测到噪声，默认开，不开的话CR3的ONEBIT置1，不开
	  // USART_CR3的EIE使能中断
		USART3->SR &= ~0x04;
		FaultCnt++;
	}

	if (USART3->SR & 0x02)
	{						 // 帧错误，USART_CR3的EIE使能中断，开
		USART3->SR &= ~0x02; // 清除
		FaultCnt++;
	}

	if (USART3->SR & 0x01)
	{						 // 奇偶校验错误标志 USART_CR1的PEIE使能该中断，不开
		USART3->SR &= ~0x01; // 清除
		FaultCnt++;
	}

	if (FaultCnt)
	{
		gu16_CommuErrCnt_SCI3++;
	}
}

// 将接收数据解码，接收中断中调用
/*=================================================================
 * FUNCTION: Sci2_Rx_Deal
 * PURPOSE : 串口数据接收解码
 * INPUT:    void
 *
 * RETURN:   void
 *
 * CALLS:    void
 *
 * CALLED BY:ISR()
 *
 *=================================================================*/
void Sci3_CommonUpper_Rx_Deal(struct RS485MSG *s)
{
	// RC1IE = 0;// 禁止EUSART2 接收中断
	// s->u16Buffer[s->ptr_no] = RCREG1;                 //读RCREG寄存器来读取接收到的8位数据
	// NVIC_DisableIRQ(USART3_IRQn);
	USART3->CR1 &= ~(1 << 5);			  // 和上面那句话二选一
	s->u16Buffer[s->ptr_no] = USART3->DR; // 从RXFIFO 中读取接收到的数据
	if ((s->ptr_no == 0) && (s->u16Buffer[0] != RS485_SLAVE_ADDR) && (s->u16Buffer[0] != RS485_BROADCAST_ADDR))
	{
		s->ptr_no = 0;
		s->u16Buffer[0] = 0;
	}
	else
	{
		if (s->ptr_no == 1)
		{
			switch (s->u16Buffer[s->ptr_no])
			{
			case RS485_CMD_READ_REGS:
				s->enRs485CmdType = RS485_CMD_READ_REGS;
				break;
			case RS485_CMD_WRITE_REG:
				s->enRs485CmdType = RS485_CMD_WRITE_REG;
				break;
			case RS485_CMD_WRITE_REGS:
				s->enRs485CmdType = RS485_CMD_WRITE_REGS;
				break;
			default:
				s->ptr_no = RS485_MAX_BUFFER_SIZE;
				s->u16Buffer[0] = 0;
				s->u16Buffer[1] = 0;
				break;
			}
		}
		else if (s->ptr_no >= 2)
		{
			switch (s->enRs485CmdType)
			{
			case RS485_CMD_READ_REGS:
			case RS485_CMD_WRITE_REG:
				if (s->ptr_no == 7)
				{ //	receive complete
					s->csr = RS485_STA_RX_COMPLETE;
					// RCSTA1bits.CREN = 0;  //禁止接收
					// RC1IE = 0;			// 禁止EUSART2 接收中断
					USART3->CR1 &= ~(1 << 2);
					USART3->CR1 &= ~(1 << 5);
				}
				break;
			case RS485_CMD_WRITE_REGS:
				if ((s->ptr_no >= 7) && (s->ptr_no == (s->u16Buffer[6] + 8)))
				{
					s->csr = RS485_STA_RX_COMPLETE;
					// disable rx TODO
					// disable rx/tx interrupt TODO
					// RCSTA1bits.CREN = 0;    //禁止接收
					// RC1IE = 0;				// 禁止EUSART2 接收中断
					USART3->CR1 &= ~(1 << 2);
					USART3->CR1 &= ~(1 << 5);
				}
				break;
			default:
				s->ptr_no = RS485_MAX_BUFFER_SIZE;
				s->u16Buffer[0] = 0;
				break;
			}
		}
		s->ptr_no++;
		if (s->ptr_no >= RS485_MAX_BUFFER_SIZE)
		{
			s->ptr_no = 0;
			s->u16Buffer[0] = 0;
		}
	}
	USART3->CR1 |= (1 << 5);
}

void Sci3_CommonUpper_Tx_Deal(struct RS485MSG *s)
{
	static int delayFlag = 0;

	if (0 == gu8_TxEnable_SCI3)
	{
		return;
	}

	if (gu16_CommuErrCnt_SCI3)
	{ // 出现错误也得把数据全部接收完，然后不回复
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_TxFinishFlag_SCI3 = 1;
		gu8_TxEnable_SCI3 = 0;
		gu16_CommuErrCnt_SCI3 = 0;
		return;
	}

	if (delayFlag)
	{
		if (g_st_SysTimeFlag.bits.b1Sys10msFlag1)
		{
			if (++delayFlag == 6)
			{
				delayFlag = 0;
			}
		}
		return;
	}

	while (!((USART3->SR) & (1 << 7)))
		; // 1<<6 也可以
	if (s->ptr_no < s->AckLenth)
	{
		USART3->DR = s->u16Buffer[s->ptr_no]; // load data
		s->ptr_no++;
		if ((s->ptr_no == 19) || (s->ptr_no == 39) || (s->ptr_no == 59))
		{
			delayFlag = 1;
		}
	}
	else
	{
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_TxFinishFlag_SCI3 = 1;
		gu8_TxEnable_SCI3 = 0;
		if (u8FlashUpdateE2PROM)
		{
			u8FlashUpdateE2PROM = 0;
			u8FlashUpdateFlag = 1;
		}
	}
}

// 串口初始化函数
void InitSCI3_CommonUpper(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
	// Enable the USART3 Interrupt(使能USART3中断)
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// USART3_TX   PD8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出，按道理是开漏输出
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// USART3_RX	  PD9初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入，做RX
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// 串口初始化
	USART_InitStructure.USART_BaudRate = 19200;										// 设置串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 设置数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 设置停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 设置效验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 设置流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 设置工作模式
	USART_Init(USART3, &USART_InitStructure);

	USART3->CR3 |= 1 << 0; // EIE，开帧错误中断，同时开启噪声中断

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 使能接收中断
	USART_Cmd(USART3, ENABLE);					   // 使能串口3

	Sci_DataInit(&g_stCurrentMsgPtr_SCI3);
}

void App_CommonUpperSCI3(struct RS485MSG *s)
{
	switch (s->csr)
	{
	// IDLE-空闲态，保持50ms后使能接收（物理层）receive set
	case RS485_STA_IDLE:
	{
		break;
	}
	// receive complete, to deal the receive data
	case RS485_STA_RX_COMPLETE:
	{
		USART3->CR1 &= ~(1 << 5); // 禁止产生中断
		CRC_verify(s);
		if (s->AckType == RS485_ACK_POS)
		{
			switch (s->enRs485CmdType)
			{
			case RS485_CMD_READ_REGS:
				Sci_Deal_ReadRegs_0x03(s);
				break;
			case RS485_CMD_WRITE_REG:
				Sci_Deal_WrReg_0x06(s);
				break;
			case RS485_CMD_WRITE_REGS:
				Sci_Deal_WrRegs_0x10(s);
				break;
			default:
				s->u16RdRegByteNum = 0;
				s->AckType = RS485_ACK_NEG;
				s->ErrorType = RS485_ERROR_NULL;
				break;
			}
		}
		s->csr = RS485_STA_RX_OK; // receive the correct data, switch to transmit wait 50ms
		break;					  // 下一轮再来
	}
	// receive ok, to transmit wait 50ms
	case RS485_STA_RX_OK:
	{
		switch (s->enRs485CmdType)
		{
		case RS485_CMD_READ_REGS:
			Sci_ACK_0x03(s);
			break;
		case RS485_CMD_WRITE_REG:
		case RS485_CMD_WRITE_REGS:
			Sci_ACK_0x06_0x10(s);
			break;
		default: // 这个defualt不用加错误操作
			break;
		}
		USART3->CR1 |= (1 << 3); // 使能发送
		gu8_TxEnable_SCI3 = 1;
	}
	// transmit complete, to switch receive wait 20ms
	case RS485_STA_TX_COMPLETE:
	{
		if (gu8_TxFinishFlag_SCI3)
		{
			s->csr = RS485_STA_IDLE;
			s->u16Buffer[0] = 0;
			s->u16Buffer[1] = 0;
			s->u16Buffer[2] = 0;
			s->u16Buffer[3] = 0;
			gu8_TxFinishFlag_SCI3 = 0;
			s->ptr_no = 0;
			USART3->CR1 |= (1 << 2); // 使能接收
			USART3->CR1 |= (1 << 5); // 使能接收中断
			gu8_TxEnable_SCI3 = 0;
		}
		break;
	}

	default:
	{
		s->csr = RS485_STA_IDLE;
		break;
	}
	}
	Sci3_CommonUpper_Tx_Deal(s);
	// Sci1_FaultChk();	//没必要在这加
}

#endif

void Sci_WrRegs_0x10_CalibCoef(UINT16 u16Channel, struct RS485MSG *s)
{
	UINT16 t_u16K, t_u16B, t_u16Temp;
	INT16 t_i16B;
	UINT16 u16WrRegNum;
	u16WrRegNum = s->u16Buffer[5] + (s->u16Buffer[4] << 8);

	if (u16WrRegNum == 2)
	{
		t_u16K = s->u16Buffer[8] + (s->u16Buffer[7] << 8);
		t_u16B = s->u16Buffer[10] + (s->u16Buffer[9] << 8);

		t_u16Temp = t_u16B & 0x8000;
		if (t_u16Temp == 0)
		{
			t_i16B = t_u16B & 0x7FFF;
		}
		else
		{
			t_i16B = -(t_u16B & 0x7FFF);
		}

		if ((t_u16K < SYSKMIN) || (t_u16K > SYSKMAX))
		{
			s->AckType = RS485_ACK_NEG;
			s->ErrorType = RS485_ERROR_DATA_INVALID;
			return;
		}

		if ((t_i16B < SYSBMIN) || (t_i16B > SYSBMAX))
		{
			s->AckType = RS485_ACK_NEG;
			s->ErrorType = RS485_ERROR_DATA_INVALID;
			return;
		}

		t_u16Temp = (u16Channel - RS485_CMD_ADDR_VC1CALIB_K) >> 1;
		g_u16CalibCoefK[t_u16Temp] = t_u16K;
		g_i16CalibCoefB[t_u16Temp] = t_i16B;
		u8E2P_KB_WriteFlag = 1;
		u8E2P_KB_WritePos = t_u16Temp;
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CMD_INVALID;
	}
}

// 节省了很多代码量吧？
void Sci_WrRegs_0x10_Protect(UINT16 u16Channel, struct RS485MSG *s)
{
	UINT16 t_u16Temp, i;
	UINT16 u16WrRegNum;
	u16WrRegNum = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16WrRegNum == 5)
	{
		t_u16Temp = u16Channel - RS485_CMD_ADDR_VCELL_OVP_FIRST;
		for (i = 0; i < 5; ++i)
		{
			*(&PRT_E2ROMParas.u16VcellOvp_First + i + t_u16Temp) = (UINT16)(s->u16Buffer[2 * i + 8] + (s->u16Buffer[2 * i + 7] << 8));
		}

		if (u16Channel >= RS485_CMD_ADDR_VDELTA_OP_FIRST)
		{
			u32E2P_Pro_Other_WriteFlag = (EE_FLAG_VCELL_OVP_FIRST | EE_FLAG_VCELL_OVP_SECOND | EE_FLAG_VCELL_OVP_THIRD | EE_FLAG_VCELL_OVP_RCV | EE_FLAG_VCELL_OVP_FILTER)
										 << (t_u16Temp - E2P_PARA_NUM_VOLCUR_PROTECT - E2P_PARA_NUM_TEM_PROTECT);
		}
		else if (u16Channel >= RS485_CMD_ADDR_TCHG_OTP_FIRST)
		{
			u32E2P_Pro_Temp_WriteFlag = (EE_FLAG_VCELL_OVP_FIRST | EE_FLAG_VCELL_OVP_SECOND | EE_FLAG_VCELL_OVP_THIRD | EE_FLAG_VCELL_OVP_RCV | EE_FLAG_VCELL_OVP_FILTER)
										<< (t_u16Temp - E2P_PARA_NUM_VOLCUR_PROTECT);
		}
		else
		{
			u32E2P_Pro_VolCur_WriteFlag = (EE_FLAG_VCELL_OVP_FIRST | EE_FLAG_VCELL_OVP_SECOND | EE_FLAG_VCELL_OVP_THIRD | EE_FLAG_VCELL_OVP_RCV | EE_FLAG_VCELL_OVP_FILTER) << (t_u16Temp);
			InitData_SOC();
		}
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CMD_INVALID;
	}
}

// 这种写法其实也有问题，主要是，倘若写失败，但是上传上位机是修改成功，就尴尬
// 但是上位机会有EEPROM写失败标志位弥补
void Sci_WrRegs_0x10_SocTable(struct RS485MSG *s)
{
	UINT8 i;
	UINT16 u16WrRegNum;
	u16WrRegNum = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16WrRegNum == E2P_PARA_NUM_SOC_TABLE)
	{
		for (i = 0; i < E2P_PARA_NUM_SOC_TABLE; ++i)
		{
			SOC_Table_Set[i] = (UINT16)(s->u16Buffer[2 * i + 8] + (s->u16Buffer[2 * i + 7] << 8));
		}
		u8E2P_SocTable_WriteFlag = E2P_PARA_NUM_SOC_TABLE;

		InitData_SOC();
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CMD_INVALID;
	}
}

void Sci_WrRegs_0x10_CopperLoss(struct RS485MSG *s)
{
	UINT8 i;
	UINT16 u16WrRegNum;
	u16WrRegNum = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16WrRegNum == E2P_PARA_NUM_COPPERLOSS * 2)
	{
		for (i = 0; i < E2P_PARA_NUM_COPPERLOSS; ++i)
		{
			CopperLoss[i] = (UINT16)(s->u16Buffer[2 * i + 8] + (s->u16Buffer[2 * i + 7] << 8));
			CopperLoss_Num[i] = (UINT16)(s->u16Buffer[2 * (i + 16) + 8] + (s->u16Buffer[2 * (i + 16) + 7] << 8));
		}
		u8E2P_CopperLoss_WriteFlag = E2P_PARA_NUM_COPPERLOSS;
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CMD_INVALID;
	}
}

void Sci_WrRegs_0x10_RTC(struct RS485MSG *s)
{
	UINT8 i;
	UINT16 u16WrRegNum;
	u16WrRegNum = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16WrRegNum == E2P_PARA_NUM_RTC)
	{
		for (i = 0; i < E2P_PARA_NUM_RTC; ++i)
		{
			*(&RTC_time.RTC_Time_Year + i) = (UINT16)(s->u16Buffer[2 * i + 8] + (s->u16Buffer[2 * i + 7] << 8));
		}
		u32E2P_RTC_Element_WriteFlag = E2P_PARA_ALL_RTC_ELEMENT;
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CMD_INVALID;
	}
}

void Sci_WrRegs_0x10_Balance(struct RS485MSG *s)
{
	UINT8 i;
	UINT16 u16WrRegNum;
	u16WrRegNum = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16WrRegNum == 8)
	{
		for (i = 0; i < 8; ++i)
		{
			*(&OtherElement.u16Balance_OpenVoltage + i) = (UINT16)(s->u16Buffer[2 * i + 8] + (s->u16Buffer[2 * i + 7] << 8));
		}
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_BALANCE_OV;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_BALANCE_OW;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_BALANCE_CW1;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_BALANCE_CW2;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_OPENTIME_ODD;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_OPENTIME_EVEN;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_OPENTIME_MOS;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_RES;
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CMD_INVALID;
	}
}

void Sci_WrRegs_0x10_SysOther(struct RS485MSG *s)
{
	UINT8 i;
	UINT16 u16WrRegNum;

	u16WrRegNum = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16WrRegNum == 8)
	{
		for (i = 0; i < 8; ++i)
		{
			*(&OtherElement.u16CS_Cur_CHGmax + i) = (UINT16)(s->u16Buffer[2 * i + 8] + (s->u16Buffer[2 * i + 7] << 8));
		}
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_CS_CUR_CHGMAX;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_CS_CUR_DSGMAX;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_CBC_CUR_CHG;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_CBC_CUR_DSG;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_COOL_DSG_H; // 不保存
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_COOL_DSG_L;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_CUR_MODE_V_DELTA;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_CUR_MODE_CUR_LIMIT;
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CMD_INVALID;
	}
}

void Sci_WrRegs_0x10_SleepElement(struct RS485MSG *s)
{
	UINT8 i;
	UINT16 u16WrRegNum;
	u16WrRegNum = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16WrRegNum == 8)
	{
		for (i = 0; i < 8; ++i)
		{
			*(&OtherElement.u16Sleep_VNormal + i) = (UINT16)(s->u16Buffer[2 * i + 8] + (s->u16Buffer[2 * i + 7] << 8));
		}
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SLEEP_V_NORMAL;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SLEEP_TIME_NORMAL;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SLEEP_V_LOW;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SLEEP_TIME_LOW;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SLEEP_I_CHG;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SLEEP_I_DSG;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SLEEP_RES1;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SLEEP_RES2;
		InitData_SleepFunc();
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CMD_INVALID;
	}
}

void Sci_WrRegs_0x10_SocElement(struct RS485MSG *s)
{
	UINT8 i;
	UINT16 u16WrRegNum;
	u16WrRegNum = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16WrRegNum == 4)
	{
		for (i = 0; i < 4; ++i)
		{
			*(&OtherElement.u16Soc_Ah + i) = (UINT16)(s->u16Buffer[2 * i + 8] + (s->u16Buffer[2 * i + 7] << 8));
		}
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SOC_AH;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SOC_CYCLE_TIME;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SOC_RES1;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SOC_RES2;
		InitData_SOC();
		// 同步更新安时数，循环次数等
		SOC_Enhance_Element.u16_RefreshData_Flag = 2;
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CMD_INVALID;
	}
}

void Sci_WrRegs_0x10_SystemElement(struct RS485MSG *s)
{
	UINT8 i;
	UINT16 u16WrRegNum;
	u16WrRegNum = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16WrRegNum == 4)
	{
		for (i = 0; i < 4; ++i)
		{
			*(&OtherElement.u16Sys_SeriesNum + i) = (UINT16)(s->u16Buffer[2 * i + 8] + (s->u16Buffer[2 * i + 7] << 8));
		}
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SYS_SERIES_NUM;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SYS_CS_RESIS;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SYS_CS_NUM;
		u32E2P_OtherElement1_WriteFlag |= EE_FLAG_OTHER1_SYS_PRECHG_TIME;
		SeriesNum = OtherElement.u16Sys_SeriesNum;
		// CS，直接使用不需要再赋值，TODO
		// 还是赋值吧，提高效率
		g_u32CS_Res_AFE = ((UINT32)OtherElement.u16Sys_CS_Res_Num * 1000) / OtherElement.u16Sys_CS_Res;
		AFE_PARAM_WRITE_Flag = 1; // CS检流电阻修改，则过流保护等要跟着修改。
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CMD_INVALID;
	}
}

void Sci_WrRegs_0x10_HeatCoolElement(struct RS485MSG *s)
{
	UINT8 i;
	UINT16 u16WrRegNum;
	u16WrRegNum = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16WrRegNum == E2P_PARA_NUM_HEAT_COOL)
	{
		for (i = 0; i < E2P_PARA_NUM_HEAT_COOL; ++i)
		{
			*(&Heat_Cool_Element.u16Heat_OpenTemp + i) = (UINT16)(s->u16Buffer[2 * i + 8] + (s->u16Buffer[2 * i + 7] << 8));
		}
		u32E2P_HeatCool_WriteFlag |= E2P_PARA_ALL_HEAT_COOL_ELE;
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CMD_INVALID;
	}
}

void Sci_WrRegs_0x10_FlashConnect(struct RS485MSG *s)
{
	UINT16 u16WrRegNum;
	u16WrRegNum = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16WrRegNum == 1)
	{
		if (FLASH_COMPLETE != FlashWriteOneHalfWord(FLASH_ADDR_UPDATE_FLAG, FLASH_TO_IAP_VALUE))
		{
			// System_ERROR_UserCallback(ERROR_FLASH);
			s->AckType = RS485_ACK_NEG;
			s->ErrorType = RS485_ERROR_CMD_INVALID;
		}
		else
		{
			u8FlashUpdateE2PROM = 1;
		}
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CMD_INVALID;
	}
}

/* 把BMS序列号，硬件版本号， 软件版本号写入 ohterInfor结构体
 * 并把写入到EEPROM标志置位
 * startADDR  如起始地址
 */
void Sci_WrRegs_0x10_SN_Version(UINT16 startADDR, struct RS485MSG *s)
{
	UINT8 i;
	UINT16 u16WrSNlength;

	u16WrSNlength = (UINT16)((UINT16)s->u16Buffer[5] + ((UINT16)s->u16Buffer[4] << 8)) << 1;

	switch (startADDR - RS485_ADDR_SN_SERIAL_NUM)
	{
	case 0:
		for (i = 0; i < PRODUCT_ID_LENGTH_MAX; ++i)
		{
			if (i < u16WrSNlength)
			{
				ProductionInfor.BMS_SerialNumber[i] = s->u16Buffer[7 + i];
			}
			else
			{
				ProductionInfor.BMS_SerialNumber[i] = '\0';
			}
		}
		ProductionInfor.BMS_SerialNumberLength = u16WrSNlength;
		ProductionInfor.BMS_SerialNumber_WriteFlag = 1;
		break;

	case 1:
		for (i = 0; i < PRODUCT_ID_LENGTH_MAX; ++i)
		{
			if (i < u16WrSNlength)
			{
				ProductionInfor.BMS_HardWareVersion[i] = s->u16Buffer[7 + i];
			}
			else
			{
				ProductionInfor.BMS_HardWareVersion[i] = '\0';
			}
		}
		ProductionInfor.BMS_HardWareVersionLength = u16WrSNlength;
		ProductionInfor.BMS_HardWareVersion_WriteFlag = 1;
		break;

	case 2:
		for (i = 0; i < PRODUCT_ID_LENGTH_MAX; ++i)
		{
			if (i < u16WrSNlength)
			{
				ProductionInfor.BMS_SoftWareVersion[i] = s->u16Buffer[7 + i];
			}
			else
			{
				ProductionInfor.BMS_SoftWareVersion[i] = '\0';
			}
		}
		ProductionInfor.BMS_SoftWareVersionLength = u16WrSNlength;
		ProductionInfor.BMS_SoftWareVersion_WriteFlag = 1;
		break;

	default:
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CMD_INVALID;
		break;
	}
}

void Sci_WrReg_0x06_Reset_CalibCoef(struct RS485MSG *s)
{
	UINT8 i;
	switch (s->u16Buffer[5] + (s->u16Buffer[4] << 8))
	{
	case 0x55AA:
		for (i = 0; i < 32; i++)
		{
			g_u16CalibCoefK[i] = SYSKDEFAULT;
			g_i16CalibCoefB[i] = SYSBDEFAULT;
			WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_K + (i << 1)), g_u16CalibCoefK[i]);
			WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_B + (i << 1)), g_i16CalibCoefB[i]);
		}
		break;
	case 0x55AB:
		g_u16CalibCoefK[VOLT_AFE1] = SYSKDEFAULT;
		g_i16CalibCoefB[VOLT_AFE1] = SYSBDEFAULT;
		WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_K + (VOLT_AFE1 << 1)), g_u16CalibCoefK[VOLT_AFE1]);
		WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_B + (VOLT_AFE1 << 1)), g_i16CalibCoefB[VOLT_AFE1]);
		break;
	case 0x55AC:
		g_u16CalibCoefK[VOLT_AFE2] = SYSKDEFAULT;
		g_i16CalibCoefB[VOLT_AFE2] = SYSBDEFAULT;
		WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_K + (VOLT_AFE2 << 1)), g_u16CalibCoefK[VOLT_AFE2]);
		WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_B + (VOLT_AFE2 << 1)), g_i16CalibCoefB[VOLT_AFE2]);
		break;
	case 0x55AD:
		g_u16CalibCoefK[VOLT_VBUS] = SYSKDEFAULT;
		g_i16CalibCoefB[VOLT_VBUS] = SYSBDEFAULT;
		WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_K + (VOLT_VBUS << 1)), g_u16CalibCoefK[VOLT_VBUS]);
		WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_B + (VOLT_VBUS << 1)), g_i16CalibCoefB[VOLT_VBUS]);
		break;
	case 0x55AE:
		for (i = 0; i < 10; i++)
		{
			g_u16CalibCoefK[MDL_TEMP1 + i] = SYSKDEFAULT;
			g_i16CalibCoefB[MDL_TEMP1 + i] = SYSBDEFAULT;
			WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_K + ((MDL_TEMP1 + i) << 1)), g_u16CalibCoefK[i]);
			WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_B + ((MDL_TEMP1 + i) << 1)), g_i16CalibCoefB[i]);
		}
		break;
	case 0x55AF:
		g_u16CalibCoefK[MDL_IDSG] = SYSKDEFAULT;
		g_i16CalibCoefB[MDL_IDSG] = SYSBDEFAULT;
		WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_K + (MDL_IDSG << 1)), g_u16CalibCoefK[MDL_IDSG]);
		WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_B + (MDL_IDSG << 1)), g_i16CalibCoefB[MDL_IDSG]);
		break;
	case 0x55B0:
		g_u16CalibCoefK[MDL_ICHG] = SYSKDEFAULT;
		g_i16CalibCoefB[MDL_ICHG] = SYSBDEFAULT;
		WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_K + (MDL_ICHG << 1)), g_u16CalibCoefK[MDL_ICHG]);
		WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_B + (MDL_ICHG << 1)), g_i16CalibCoefB[MDL_ICHG]);
		break;
	default:
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_DATA_INVALID;
		break;
	}
}

void Sci_WrReg_0x06_Reset_ProtectRecord(struct RS485MSG *s)
{
	UINT16 u16SciRegData;
	UINT8 i;
	u16SciRegData = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (0x0001 == u16SciRegData)
	{
		for (i = 0; i < Record_len; ++i)
		{
			Fault_record_First2[i] = 0;
			Fault_record_Second2[i] = 0;
			Fault_record_Third2[i] = 0;
		}
		FaultPoint_First2 = 0;
		FaultPoint_Second2 = 0;
		FaultPoint_Third2 = 0;
		Fault_Flag_Fisrt.all = 0;
		Fault_Flag_Second.all = 0;
		Fault_Flag_Third.all = 0;
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_DATA_INVALID;
	}
}

void Sci_WrReg_0x06_Reset_ProtectElement(struct RS485MSG *s)
{
	UINT16 u16SciRegData;
	UINT8 i;
	const struct PRT_E2ROM_PARAS PrtE2PARAS_Default = E2P_PROTECT_DEFAULT_PRT;
	u16SciRegData = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (0x0001 == u16SciRegData)
	{
		for (i = 0; i < E2P_PARA_NUM_PROTECT; ++i)
		{
			*(&PRT_E2ROMParas.u16VcellOvp_First + i) = *(&PrtE2PARAS_Default.u16VcellOvp_First + i);
		}
		u32E2P_Pro_VolCur_WriteFlag = E2P_PARA_ALL_VOLCUR_PROTECT;
		u32E2P_Pro_Temp_WriteFlag = E2P_PARA_ALL_TEM_PROTECT;
		u32E2P_Pro_Other_WriteFlag = E2P_PARA_ALL_OTHER_PROTECT;
		InitData_SOC();
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_DATA_INVALID;
	}
}

void Sci_WrReg_0x06_Reset_OtherCanAdd(struct RS485MSG *s)
{
	UINT16 u16SciRegData;
	UINT8 i;
	const struct OTHER_ELEMENT OtherElement_Default = OtherElement_default;
	u16SciRegData = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (0x0001 == u16SciRegData)
	{
		for (i = 0; i < E2P_PARA_NUM_OTHER_ELEMENT1; ++i)
		{
			*(&OtherElement.u16Balance_OpenVoltage + i) = *(&OtherElement_Default.u16Balance_OpenVoltage + i);
		}
		u32E2P_OtherElement1_WriteFlag = E2P_PARA_ALL_OTHER_ELEMENT1;
		SeriesNum = OtherElement.u16Sys_SeriesNum;
		g_u32CS_Res_AFE = ((UINT32)OtherElement.u16Sys_CS_Res_Num * 1000) / OtherElement.u16Sys_CS_Res;
		AFE_PARAM_WRITE_Flag = 1; // CS检流电阻修改，则过流保护等要跟着修改。

		InitData_SOC();
		// 同步更新安时数，循环次数等
		SOC_Enhance_Element.u16_RefreshData_Flag = 2;
		InitData_SleepFunc();
		// CS，直接使用不需要再赋值，TODO
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_DATA_INVALID;
	}
}

void Sci_WrReg_0x06_Reset_HeatCool(struct RS485MSG *s)
{
	UINT16 u16SciRegData;
	UINT8 i;
	const struct HEAT_COOL_ELEMENT HeatCoolEle_Default = HeatCoolElement_Default;

	u16SciRegData = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (0x0001 == u16SciRegData)
	{
		for (i = 0; i < E2P_PARA_NUM_HEAT_COOL; ++i)
		{
			*(&Heat_Cool_Element.u16Heat_OpenTemp + i) = *(&HeatCoolEle_Default.u16Heat_OpenTemp + i);
		}
		u32E2P_HeatCool_WriteFlag = E2P_PARA_ALL_HEAT_COOL_ELE;
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_DATA_INVALID;
	}
}

void Sci_WrReg_0x06_SwitchON(struct RS485MSG *s)
{
	UINT16 u16SciRegData;
	u16SciRegData = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16SciRegData >= 1 && u16SciRegData <= 32)
	{
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_DATA_INVALID;
	}
}

void Sci_WrReg_0x06_SwitchOFF(struct RS485MSG *s)
{
	UINT16 u16SciRegData;
	u16SciRegData = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16SciRegData >= 1 && u16SciRegData <= 32)
	{
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_DATA_INVALID;
	}
}

// 关于这个函数
// A:第一次打开这个功能，以前从来没打开过，则因为各种标志位变量都没变过(switch结构里面的)，所以会进行初始化验证
// B:其中关闭了，又打开，则已经初始化过一次，这次打开就继续按照上一次的进度继续下去
void Sci_WrReg_0x06_BMS_FunctionON(struct RS485MSG *s)
{
	UINT16 u16SciRegData;
	u16SciRegData = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16SciRegData >= 1 && u16SciRegData <= 32)
	{
		switch (u16SciRegData)
		{		// 如果是以下功能被打开，则需要初始化验证，别的功能直接关就好
		case 1: // 均衡
			if (!System_OnOFF_Func_StartUpRec.bits.b1OnOFF_Balance)
			{
				System_OnOFF_Func_StartUpRec.bits.b1OnOFF_Balance = 1;
				System_Func_StartUp.bits.b1StartUpFlag_Balance = 1;
			}
			break;

		case 3: // MOS或者接触器功能
			if (!System_OnOFF_Func_StartUpRec.bits.b1OnOFF_MOS_Relay)
			{
				System_OnOFF_Func_StartUpRec.bits.b1OnOFF_MOS_Relay = 1;
				System_Func_StartUp.bits.b1StartUpFlag_MOS = 1;
				System_Func_StartUp.bits.b1StartUpFlag_Relay = 1;
			}
			break;

		case 6: // 加热功能
			if (!System_OnOFF_Func_StartUpRec.bits.b1OnOFF_Heat)
			{
				System_OnOFF_Func_StartUpRec.bits.b1OnOFF_Heat = 1;
				System_Func_StartUp.bits.b1StartUpFlag_Heat = 1;
			}
			break;

		case 7: // 冷凝功能
			if (!System_OnOFF_Func_StartUpRec.bits.b1OnOFF_Cool)
			{
				System_OnOFF_Func_StartUpRec.bits.b1OnOFF_Cool = 1;
				System_Func_StartUp.bits.b1StartUpFlag_Cool = 1;
			}
			break;

		case 8: // 激活模拟前端AFE1
			break;

		case 0x0A: // 立刻进入休眠
			SleepElement.Sleep_Mode.bits.b1ForceToSleep_L3 = 1;
			break;

		default:
			break;
		}

		System_OnOFF_Func.all |= ((UINT32)1 << (u16SciRegData - 1));
		if (u16SciRegData == 0x0B)
		{
			// System_OnOFF_Func.bits.b1OnOFF_SOC_Zero
			// 默认为0，不需要保存
		}
		else
		{
			WriteEEPROM_Word_WithZone(EEPROM_ADDR_SYS_FUNC_SELECT, (UINT16)(System_OnOFF_Func.all & 0x0000FFFF));
			WriteEEPROM_Word_WithZone(EEPROM_ADDR_SYS_FUNC_SELECT + 2, (UINT16)(System_OnOFF_Func.all >> 16));
		}

		if (System_OnOFF_Func.bits.b1OnOFF_SOC_Fixed)
		{
			SOC_Enhance_Element.u16_RefreshData_Flag = 1;
		}
		if (System_OnOFF_Func.bits.b1OnOFF_SOC_Zero)
		{
			SOC_Enhance_Element.u16_RefreshData_Flag = 2;
		}
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_DATA_INVALID;
	}
}

void Sci_WrReg_0x06_BMS_FunctionOFF(struct RS485MSG *s)
{
	UINT16 u16SciRegData;
	u16SciRegData = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16SciRegData >= 1 && u16SciRegData <= 32)
	{
		//*(&System_OnOFF_Func.bits.b1OnOFF_Balance+(u16SciRegData-1)) = 0;
		System_OnOFF_Func.all &= ~((UINT32)1 << (u16SciRegData - 1)); // 功能途中关闭不需要初始化验证

		if (u16SciRegData == 0x0B)
		{
			// System_OnOFF_Func.bits.b1OnOFF_SOC_Zero
			// 默认为0，不需要保存
		}
		else
		{
			WriteEEPROM_Word_WithZone(EEPROM_ADDR_SYS_FUNC_SELECT, (UINT16)(System_OnOFF_Func.all & 0x0000FFFF));
			WriteEEPROM_Word_WithZone(EEPROM_ADDR_SYS_FUNC_SELECT + 2, (UINT16)(System_OnOFF_Func.all >> 16));
		}
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_DATA_INVALID;
	}
}

void Sci_WrReg_0x06_SetSocOnce(struct RS485MSG *s)
{
	UINT16 u16SciRegData;
	u16SciRegData = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if (u16SciRegData <= 100)
	{
		SOC_Enhance_Element.u16_RefreshData_Flag = 3;
		SOC_Enhance_Element.u8_SetSocOnce = u16SciRegData;
	}
	else
	{
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_DATA_INVALID;
	}
}

void InitUSART_CommonUpper(void)
{
#ifdef _COMMOM_UPPER_SCI1
	InitSCI1_CommonUpper();
#endif

#ifdef _COMMOM_UPPER_SCI2
	InitSCI2_CommonUpper();
#endif

#ifdef _COMMOM_UPPER_SCI3
	InitSCI3_CommonUpper();
#endif
}

void App_CommonUpper(void)
{
#ifdef _COMMOM_UPPER_SCI1
	App_CommonUpperSCI1(&g_stCurrentMsgPtr_SCI1);
#endif

#ifdef _COMMOM_UPPER_SCI2
	App_CommonUpperSCI2(&g_stCurrentMsgPtr_SCI2);
#endif

#ifdef _COMMOM_UPPER_SCI3
	App_CommonUpperSCI3(&g_stCurrentMsgPtr_SCI3);
#endif
}
