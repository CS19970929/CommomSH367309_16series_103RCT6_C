#include "main.h"

SH367309_REG_STORE SH367309_Reg_Store;

UINT8 gu8_DriverStartUpFlag = 0;

//-40��100����ֵ
UINT16 iSheldTemp_10K_NTC[141] = {20375, 19204, 18115, 17100, 16152, 15266, 14437, 13661, 12934, 12251,
								  11611, 11008, 10442, 9909, 9407, 8935, 8489, 8068, 7672, 7297,
								  6943, 6608, 6292, 5993, 5710, 5442, 5188, 4948, 4720, 4504,
								  4300, 4105, 3921, 3746, 3580, 3422, 3272, 3130, 2994, 2866,
								  2751, 2627, 2516, 2410, 2310, 2214, 2123, 2036, 1953, 1874,
								  1801, 1726, 1658, 1592, 1530, 1470, 1413, 1358, 1306, 1256,
								  1209, 1163, 1119, 1078, 1038, 1000, 963, 928, 894, 862,
								  831, 801, 773, 746, 719, 694, 670, 647, 625, 604,
								  583, 563, 544, 526, 509, 492, 476, 460, 445, 431,
								  416, 403, 390, 378, 366, 355, 343, 333, 322, 312,
								  303, 294, 285, 276, 268, 260, 252, 244, 237, 230,
								  224, 217, 211, 205, 199, 193, 188, 182, 177, 172,
								  167, 163, 158, 154, 150, 146, 142, 138, 134, 131,
								  127, 124, 120, 117, 114, 111, 108, 106, 103, 100,
								  98};

// ǰ26���Ĵ���Ĭ�ϲ���
UINT8 ucMTPBuffer[26] = {
	BYTE_00H_SCONF1, BYTE_01H_SCONF2, BYTE_02H_OVT_LDRT_OVH, BYTE_03H_OVL, BYTE_04H_UVT_OVRH,
	BYTE_05H_OVRL, BYTE_06H_UV, BYTE_07H_UVR, BYTE_08H_BALV, BYTE_09H_PREV,
	BYTE_0AH_L0V, BYTE_0BH_PFV, BYTE_0CH_OCD1V_OCD1T, BYTE_0DH_OCD2V_OCD2T, BYTE_0EH_SCV_SCT,
	BYTE_0FH_OCCV_OCCT, BYTE_10H_MOST_OCRT_PFT, BYTE_11H_OTC, BYTE_12H_OTCR, BYTE_13H_UTC,
	BYTE_14H_UTCR, BYTE_15H_OTD, BYTE_16H_OTDR, BYTE_17H_UTD, BYTE_18H_UTDR,
	BYTE_19H_TR};

UINT16 aaaaaa1 = 0;
UINT16 aaaaaa2 = 0;
UINT16 aaaaaa3 = 0;
UINT16 aaaaaa4 = 0;
UINT8 aaa11 = 0;

/*******************************************************************************
Function:ResetAFE()
Description:  Reset SH367309 IC, Send Data:0xEA, 0xC0, CRC
Input:	 NULL
Output: NULL
Others:
*******************************************************************************/
void AFE_Reset(void)
{
	UINT8 WrBuf[2];

	WrBuf[0] = 0xC0;
	WrBuf[1] = 0xA5;

	/*
	if(!System_ErrFlag.u8ErrFlag_Com_AFE1) {
		if(!MTPWrite(AFE_ID, 0xEA, 1, WrBuf)) {              //0xEA, 0xC0?A CRC
			MTPWrite(AFE_ID, 0xEA, 1, WrBuf);
		}
		//MTPWrite(0xEA, 1, WrBuf);
	}
	*/

	if (!System_ERROR_UserCallback(ERROR_STATUS_AFE1))
	{
		if (!MTPWrite(0xEA, 1, WrBuf))
		{ // 0xEA, 0xC0?A CRC
			MTPWrite(0xEA, 1, WrBuf);
		}
	}
}

// ��������ģʽ
void AFE_Sleep(void)
{
	SH367309_Reg_Store.REG_MTP_CONF.bits.SLEEP = 1;
	MTPWrite(MTP_CONF, 1, &SH367309_Reg_Store.REG_MTP_CONF.all);
}

// ����IDLEģʽ
// 1���д��󣬲��ܽ���
UINT8 AFE_IDLE_Old(void)
{
	UINT8 result = 0;

	if (MTPRead(MTP_BSTATUS1, 3, &SH367309_Reg_Store.REG_BSTATUS1.all))
	{
		if (SH367309_Reg_Store.REG_BSTATUS1.all || SH367309_Reg_Store.REG_BSTATUS2.all || SH367309_Reg_Store.REG_BSTATUS3.bits.L0V || SH367309_Reg_Store.REG_BSTATUS3.bits.PCHG_FET)
		{
			// ���ܽ���IDLE
			result = 1;
			System_ERROR_UserCallback(ERROR_AFE1);
		}
		else
		{
			SH367309_Reg_Store.REG_MTP_CONF.bits.IDLE = 1;
			MTPWrite(MTP_CONF, 1, &SH367309_Reg_Store.REG_MTP_CONF.all);
		}
	}
	else
	{
		result = 1;
		// System_ERROR_UserCallback(ERROR_AFE1);
	}

	return result;
}

void AFE_IDLE(void)
{
	SH367309_Reg_Store.REG_MTP_CONF.bits.IDLE = 1;
	MTPWrite(MTP_CONF, 1, &SH367309_Reg_Store.REG_MTP_CONF.all);
}

// ��������ģʽ
void AFE_SHIP(void)
{
	MCUO_AFE_SHIP = 0;
}

// ����IDLEģʽ
// 0��IDLE��1��Sleep��2��SHIP(ûͨѶ��)
UINT8 AFE_SleepMode_Judge(void)
{
	UINT8 result = 0;

	if (MTPRead(MTP_BSTATUS1, 3, &SH367309_Reg_Store.REG_BSTATUS1.all))
	{
		if (SH367309_Reg_Store.REG_BSTATUS1.all || SH367309_Reg_Store.REG_BSTATUS2.all || SH367309_Reg_Store.REG_BSTATUS3.bits.L0V || SH367309_Reg_Store.REG_BSTATUS3.bits.PCHG_FET)
		{
			// ���ܽ���IDLE
			result = 1;
		}
		else
		{
			result = 0;
		}
	}
	else
	{
		result = 2;
	}

	return result;
}

// 1�������⣬ͬʱ�ϱ�ϵͳAFE����
// 0��û����
UINT8 AFE_IsReady(void)
{
	UINT8 TempCnt = 0, result = 0;
	UINT8 TempVar;

	while (1)
	{
		Feed_WatchDog;

		TempVar = 0;
		if (MTPRead(MTP_BFLAG2, 1, &TempVar))
		{ // ��ȡBLFG2���鿴VADC�Ƿ�ת�����
			if ((TempVar & 0x10) == 0x10)
			{
				break;
			}
		}

		Delay1ms(20);
		if (++TempCnt >= 50)
		{
			// System_ERROR_UserCallback(ERROR_AFE1);
			result = 1;
			break;
		}
	}
	return result;
}

UINT8 AFE_GetData(void)
{
	UINT8 result;
	result = MTPRead(MTP_TEMP1, sizeof(Registers_AFE1), (UINT8 *)&Registers_AFE1);
	return result;
}

// 1��״̬��ѯʧ�ܡ�0���ɹ�
UINT8 AFE_CheckStatus(void)
{
	UINT8 result = 0;

	if (!MTPRead(MTP_BSTATUS1, 3, &SH367309_Reg_Store.REG_BSTATUS1.all))
	{
		result = 1;
	}
	return result;
}

/*******************************************************************************
Function:EnableAFEWdtCadc()
Description:ʹ��CHG&DSG&PCHG�������ʹ��WDT��CADCģ��
Input:
Output:
Others:
*******************************************************************************/
void SH367309_Enable_AFE_Wdt_Cadc_Drivers(void)
{
	// ucMTP_CONF |= 0x04;						//�������Ź����������Ź��в���
	// ���ۣ����Բ����������Ź������������
	// 1���رճ�ŵ�MOS��Ԥ��MOS
	// 2���������
	// ���߶���Ŀǰʹ��������岻�����ߴ��粻������MCU������û����
	// 30x����Ҫ���ģ���Ϊ���Լ��ı�����ϵ�����309�õ������Լ�����ϵ�����Ծ��������
	// ���Ź����أ����Լ��ı�����ϵ�ж��Ƿ��MOS������Ҳ����
	SH367309_Reg_Store.REG_MTP_CONF.bits.CADCON = 1; // ����CADC
	SH367309_Reg_Store.REG_MTP_CONF.bits.CHGMOS = 1; // ���MOS��AFEӲ������
	SH367309_Reg_Store.REG_MTP_CONF.bits.DSGMOS = 1; // �ŵ�MOS��AFEӲ������
	MTPWrite(MTP_CONF, 1, &SH367309_Reg_Store.REG_MTP_CONF.all);
}

// 1���޸�ʧ�ܡ�0���޸ĳɹ�
UINT8 SH367309_SC_DelayT_Set(void)
{
	UINT8 result = 0;
	UINT8 u8temp_now = 0;
	UINT8 u8temp_need = 0;

	u8temp_now = SH367309_Reg_Store.u8_MTP_SCV_SCT & 0x0F;
	u8temp_need = OtherElement.u16CBC_DelayT >> 6; // ����64�ó��ȼ������������64usΪһ���ȼ���
	if (u8temp_need > 15)
		u8temp_need = 15;

	if (u8temp_now != u8temp_need)
	{
		if (MTPWriteROM(0x0E, 1, (UINT8 *)((SH367309_Reg_Store.u8_MTP_SCV_SCT & 0xF0) | u8temp_need)))
		{
			SH367309_Reg_Store.u8_MTP_SCV_SCT = (SH367309_Reg_Store.u8_MTP_SCV_SCT & 0xF0) | u8temp_need;
			OtherElement.u16CBC_DelayT = (UINT16)u8temp_need << 6;
		}
		else
		{
			// дʧ��
			OtherElement.u16CBC_DelayT = (UINT16)u8temp_now << 6;
			result = 1;
		}
	}
	else
	{
		// ��ͬ�����޸��ϴ��������
		OtherElement.u16CBC_DelayT = (UINT16)u8temp_now << 6;
	}

	return result;
}

// ���ߴ�����Ҫ�ģ�����ȨȫȨ����AFE
// MCU����������ȫ���ù��ڲ����ֱ�������ô����ֻ��Ҫ��Ӱ�����������ط����úþ���
// �¶ȱ������ÿ�һЩ
// ȡ�����ι��䱣��
// Ŀǰ��������������ϲ���AFE������MCU��������ֻҪ���¶���ֵ�滻���У����߶���ȡ�����ι��䱣����
// ����ǣ�ʹ��MCU����ʱ����΢Ӱ��һ��㿪��ʱ�䣬���ⲻ��
void SH367309_UpdataAfeConfig_Old(void)
{
	UINT8 bufferbak[26], mtpbufferbak[26];
	UINT8 i;
	UINT16 ResTemp[8];

	Feed_WatchDog;

	// ��EEPROM��ؼĴ����޸�
	ucMTPBuffer[0x0E] = (BYTE_0EH_SCV_SCT & 0xF0) | (OtherElement.u16CBC_DelayT >> 6);

	SH367309_Reg_Store.u8_MTP_SCONF2 = ucMTPBuffer[0x01];
	SH367309_Reg_Store.u8_MTP_SCV_SCT = ucMTPBuffer[0x0E];

	if (MTPRead(0x00, 26, bufferbak))
	{													// ��309����,����TR
		ucMTPBuffer[MTP_TR] = bufferbak[MTP_TR] & 0x7F; // �Ȼ�ȡTR[6~0]��ֵ
		MemoryCopy(ucMTPBuffer, mtpbufferbak, sizeof(ucMTPBuffer));
		SH367309_Reg_Store.TR_ResRef = 680 + 5 * ucMTPBuffer[MTP_TR];

		// ֻҪ��֤���±������͵���25���϶ȣ�(UINT8)ǿ��ת���൱��-256������
		for (i = 0; i < 8; ++i)
		{
			ResTemp[i] = iSheldTemp_10K_NTC[ucMTPBuffer[i + MTP_OTC]];
			mtpbufferbak[i + MTP_OTC] = (UINT8)(((UINT32)ResTemp[i] << 9) / ((UINT32)SH367309_Reg_Store.TR_ResRef + ResTemp[i]));
		}

		for (i = 0; i < 25; i++)
		{ // ���һ��TR�����Ա�
			if (bufferbak[i] != mtpbufferbak[i])
			{
				MCUO_AFE_VPRO = 1;
				Delay1ms(20);
				if (!MTPWriteROM(0x00, 25, mtpbufferbak))
				{ // ��дEEPROM�ļĴ���������
					// System_ERROR_UserCallback(ERROR_AFE1);
				}
				MCUO_AFE_VPRO = 0;
				Delay1ms(1);

				if (!System_ERROR_UserCallback(ERROR_STATUS_AFE1))
				{
					// EA = 0;								//ɶ��˼������仰TODO
					AFE_Reset(); // Reset IC
					Delay1ms(5);
					AFE_IsReady();
				}

				break;
			}
		}

		if (!System_ERROR_UserCallback(ERROR_STATUS_AFE1))
		{
			if (MTPRead(0x00, 26, bufferbak))
			{ // Read MTP value
				for (i = 0; i < 25; i++)
				{ // ���һ��TR�����Ա�
					if (bufferbak[i] != mtpbufferbak[i])
					{
						System_ERROR_UserCallback(ERROR_AFE1);
						// break;
					}
				}
			}
		}
	}
}

void SH367309_DriverMos_Ctrl(GPIO_Type Type, UINT8 OnOFF)
{
	switch (Type)
	{
	case GPIO_PreCHG:
		SH367309_Reg_Store.REG_MTP_CONF.bits.PCHMOS = OnOFF;
		break;

	case GPIO_CHG:
		SH367309_Reg_Store.REG_MTP_CONF.bits.CHGMOS = OnOFF;
		break;

	case GPIO_DSG:
		SH367309_Reg_Store.REG_MTP_CONF.bits.DSGMOS = OnOFF;
		break;

	default:
		break;
	}

	MTPWrite(MTP_CONF, 1, &SH367309_Reg_Store.REG_MTP_CONF.all);
}

void SH367309_Driver_Supplement(void)
{
	static UINT8 su8_MOS_DSG_Status = 0;
	UINT8 DSG_Close_Flag = 0;
	static UINT32 su32_PreRelayOPEN_MODE_Cnt = 0;
	static UINT32 su32_Delay_Cnt = 0;
	static UINT8 su8_OpenT_Cnt = 0;

	if (0 == g_st_SysTimeFlag.bits.b1Sys10msFlag3)
	{
		return;
	}

	switch (su8_MOS_DSG_Status)
	{
	case 0:
		// �տ���Ҳ�ǹرգ��������ⲻ��
		// �����ڼ��鵽�رգ�ҲҪִ��һ��ѭ��
		if (!gu8_DriverStartUpFlag)
		{ // �ѵ���У׼����ǰ��
			return;
		}
		if (SystemStatus.bits.b1Status_MOS_DSG == CLOSE)
		{
			MCUO_AFE_CTLC = 0; // ��ǿ�ƹر���˵
			su8_MOS_DSG_Status = 1;
		}
		break;

	case 1:
		DSG_Close_Flag = 0; // Ĭ���ǲ�Close

		// ����ḻ��ӵı���ģʽ
		if (g_stCellInfoReport.unMdlFault_First.bits.b1IdischgOcp)
		{
			// ����չ�ķŵ籣���ص��ŵ�MOS
			// ���ֲ���
			DSG_Close_Flag = 1;
		}
		else if (SH367309_Reg_Store.REG_BSTATUS1.bits.UV)
		{
			DSG_Close_Flag = 1;
		}
		else if (SH367309_Reg_Store.REG_BSTATUS1.bits.OCD1)
		{
			DSG_Close_Flag = 1;
		}
		else if (SH367309_Reg_Store.REG_BSTATUS1.bits.OCD2)
		{
			DSG_Close_Flag = 1;
		}
		else if (SH367309_Reg_Store.REG_BSTATUS1.bits.SC)
		{
			DSG_Close_Flag = 1;
		}
		else if (SH367309_Reg_Store.REG_BSTATUS2.bits.UTD)
		{
			DSG_Close_Flag = 1;
		}
		else if (SH367309_Reg_Store.REG_BSTATUS2.bits.OTD)
		{
			DSG_Close_Flag = 1;
		}
		else if (SystemStatus.bits.b1Status_BnCloseIO)
		{ // ����б���ֶ��رյ��źţ�������ӵ�����
			DSG_Close_Flag = 1;
		}
		else
		{
			// 1�����Ź����--------����AFE������������֣����ֱ���Ĵ���
			// 2�����ι���籣��----��ֹ(DIS_PF=1)
			// 3�����߼��----------��ֹ(DIS_PF=1)
		}

		if (DSG_Close_Flag == 0)
		{
			su8_MOS_DSG_Status = 2;
		}
		break;

	case 2:
		// ��Ԥ��
		SystemStatus.bits.b1Status_MOS_PRE = !SystemStatus.bits.b1Status_MOS_PRE;
		if (SystemStatus.bits.b1Status_MOS_PRE == OPEN)
		{
			++su32_PreRelayOPEN_MODE_Cnt;
		}
		if (su32_PreRelayOPEN_MODE_Cnt >= (UINT32)OtherElement.u16Sys_PreChg_Time)
		{
			su32_PreRelayOPEN_MODE_Cnt = 0;
			su8_MOS_DSG_Status = 3;
		}
		break;

	case 3:
		MCUO_AFE_CTLC = 1; // Ԥ�����������򿪷ŵ�MOS
		if (++su32_Delay_Cnt >= 10)
		{ // ��ʱ100ms�ر�Ԥ��
			su32_Delay_Cnt = 0;
			SystemStatus.bits.b1Status_MOS_PRE = CLOSE;
			su8_MOS_DSG_Status = 4;
		}
		break;

	case 4:
		if (SystemStatus.bits.b1Status_MOS_DSG == OPEN)
		{ // ����Ƿ���Ĵ��ˣ��򿪲Żص�ԭ���ĵط���ء�
			su8_MOS_DSG_Status = 0;
			su8_OpenT_Cnt = 0;
		}

		if (++su8_OpenT_Cnt >= 200)
		{ // �����ʱ2s��û�򿪣��򱨴�
			su8_OpenT_Cnt = 0;
			System_ERROR_UserCallback(ERROR_AFE2); // ˵��û���򿪣�������
		}
		break;

	default:
		break;
	}

	MCUO_MOS_PRE = SystemStatus.bits.b1Status_MOS_PRE;
	// SystemStatus.bits.b1Status_Relay_PRE = MCUO_AFE_CTLC;
}

// DataDeal.h�Ĳ�����Ĭ�Ͻ���ڶ������ߣ�ʱ��Ϊ5�죬7200���ӡ�RTCΪ�ڼ������ж�
// �ڶ�������ΪMOSȫ������
// ��һ������Ϊ���ߴ�������
void SH367309_SleepMode_Ctrl(void)
{
	// UINT8 u8_BYTE_01H_SCONF2;

	static UINT8 su8_StartUp_Flag = 0;
	static UINT16 su16_Delay_100msTCnt = 0;

	static UINT8 su8_SleepExtComCnt = 0;

	static UINT16 su16_RTC1_100msTCnt = 0;
	static UINT16 su16_Normal1_100msTCnt = 0;
	static UINT16 su16_RTC2_100msTCnt = 0;
	static UINT16 su16_Normal2_100msTCnt = 0;

	static UINT16 su16_AFE_ErrSleep_100msTCnt = 0;

	if (0 == g_st_SysTimeFlag.bits.b1Sys100msFlag)
	{ // ���ʱ������������Ӱ��MOS��������ʼ������У׼
		return;
	}

	switch (su8_StartUp_Flag)
	{
	case 0:
		// ���ﳤһ�㣬�õ���У׼������ȷ���ꡣ
		if (++su16_Delay_100msTCnt >= 5)
		{
			su16_Delay_100msTCnt = 0; // ���RTC��ƽ���������⣬���������ʱ10s��RTC���10minƽ������Ҫ�ﵽҪ��
			su8_StartUp_Flag = 1;
		}
		break;

	case 1:
		if (g_stCellInfoReport.u16Ichg > 10 || g_stCellInfoReport.u16IDischg > 10)
		{
			// �е������ж�
			su16_RTC1_100msTCnt = 0;
			su16_RTC2_100msTCnt = 0;
			su16_Normal1_100msTCnt = 0;
			su16_Normal2_100msTCnt = 0;

			// ��Ҫ�ѽ�������ʱ���ӳ���
			// ����Ҫ��
			if (FLASH_309_RTC_RTC_VALUE == FlashReadOneHalfWord(FLASH_ADDR_SH367309_FLAG))
			{
				FlashWriteOneHalfWord(FLASH_ADDR_SH367309_FLAG, FLASH_309_RTC_NORMAL_VALUE);
			}
		}

#if 1
		else if (su8_SleepExtComCnt != RTC_ExtComCnt)
		{
			// ��ͨѶ���ж�
			su16_RTC1_100msTCnt = 0;
			su16_RTC2_100msTCnt = 0;
			su16_Normal1_100msTCnt = 0;
			su16_Normal2_100msTCnt = 0;
			su8_SleepExtComCnt = RTC_ExtComCnt;

			// ��Ҫ�ѽ�������ʱ���ӳ���
			// ����Ҫ��
			if (FLASH_309_RTC_RTC_VALUE == FlashReadOneHalfWord(FLASH_ADDR_SH367309_FLAG))
			{
				FlashWriteOneHalfWord(FLASH_ADDR_SH367309_FLAG, FLASH_309_RTC_NORMAL_VALUE);
			}
		}
#endif

		else
		{
			switch (FlashReadOneHalfWord(FLASH_ADDR_SH367309_FLAG))
			{
			case FLASH_309_RTC_RTC_VALUE:
				if (AFE_SleepMode_Judge())
				{
					// if(++su16_Normal1_100msTCnt > 16) {			//û�������ʱ�������ͨѶҲ���̽�������
					if (++su16_Normal1_100msTCnt > 100)
					{ // û�������ʱ�������ͨѶҲ���̽�������
						su16_Normal1_100msTCnt = 0;
						// �������дFLASH���ҷţ��տ�ʼ�������++su16_RTC1_100msTCnt����ζ��Ҫд16�Σ������þ��˵�Ƭ������
						// �������������ȵ���ûʲô���⣬�ǳ�����������ȷ��ֻдһ�Σ�Ȼ��������ĵط���
						SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1; // ������ͨ����
					}
					if (su16_RTC1_100msTCnt)
						su16_RTC1_100msTCnt = 0;
				}
				else
				{
					// if(++su16_RTC1_100msTCnt > 16) {			//û�������ʱ�������ͨѶҲ���̽�������
					if (++su16_RTC1_100msTCnt > 100)
					{ // û�������ʱ�������ͨѶҲ���̽�������
						su16_RTC1_100msTCnt = 0;
						SleepElement.Sleep_Mode.bits.b1ForceToSleep_L1 = 1; // ��������RTC
					}
					if (su16_Normal1_100msTCnt)
						su16_Normal1_100msTCnt = 0;
				}
				aaa11 = 1;
				break;

			case FLASH_309_RTC_NORMAL_VALUE:
			case FLASH_309_NORMAL_NORMAL_VALUE:
				if (AFE_SleepMode_Judge())
				{
					// Sleep_Mode.bits.b1ForceToSleep_L2 = 1;		//������ͨ����
					// ����б����Ļ����򽻸����ߺ�������������������
					// ���ǲ�Ҫ�ˣ���ɣ��Դ�������ϵ����ϡ�
					if (++su16_Normal2_100msTCnt >= 10 * 60 * 5)
					{ // �����б�����������ͨ����ģʽ��20min
						// if(++su16_Normal_100msTCnt >= 10*20) {
						su16_Normal2_100msTCnt = 0;
						SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1;
					}
					if (su16_RTC2_100msTCnt)
						su16_RTC2_100msTCnt = 0;
				}
				else
				{
					if (++su16_RTC2_100msTCnt >= 10 * 60 * 5)
					{ // �������ѣ�û���⣬����RTCģʽ�����10min
						// if(++su16_RTC_100msTCnt >= 10*20) {
						su16_RTC2_100msTCnt = 0;
						SleepElement.Sleep_Mode.bits.b1ForceToSleep_L1 = 1; // û���⣬��������RTC����
					}
					if (su16_Normal2_100msTCnt)
						su16_Normal2_100msTCnt = 0;
				}
				aaa11 = 2;
				break;

			default:
				// �����ֵ�һ���ã�������ǰ�ù���Ȼ��õط���ֵ��д�����֣���ֹ���ֵ�һ���ϵ����ҡ�
				// ������ǣ�SOC�漰EEPROM���֣��ǲ���ҲҪ�����default�أ�ѭ����������Թ�����
				FlashWriteOneHalfWord(FLASH_ADDR_SH367309_FLAG, FLASH_309_NORMAL_NORMAL_VALUE);
				break;
			}
		}
		break;

	default:
		break;
	}

	// ���׼���������ߴ��磬�ȼ��һЩMOS
	// ���û�򿪹��ӣ��򲻽������ߴ���״̬
	if (SleepElement.Sleep_Mode.bits.b1ForceToSleep_L1)
	{
		if (!SH367309_Reg_Store.REG_BSTATUS3.bits.CHG_FET || !SH367309_Reg_Store.REG_BSTATUS3.bits.DSG_FET)
		{
			SleepElement.Sleep_Mode.bits.b1ForceToSleep_L1 = 0;
			// TODO���ϴ���λ����������
			System_ERROR_UserCallback(ERROR_UPPER);

			if (++su16_AFE_ErrSleep_100msTCnt >= 10 * 60 * 10)
			{
				su16_AFE_ErrSleep_100msTCnt = 0;
				SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1;
			}
		}
		else
		{
			if (su16_AFE_ErrSleep_100msTCnt)
				su16_AFE_ErrSleep_100msTCnt = 0;
		}
	}

	// ��Ϊ������RTC���Ѻͱ�Ļ�����ʽ����Ļ�����ʽ��Ҫ��10min�ż����������ߣ������RTC���ѣ�������ٴν�������
	// RTC����������޸ı�־λ��
	// �����FLASH_COMPLETE���жϻ���Ҫ����Ϊ���д̫�ؼ��ˡ�
	if (SleepElement.Sleep_Mode.bits.b1ForceToSleep_L1)
	{
		if (FLASH_COMPLETE == FlashWriteOneHalfWord(FLASH_ADDR_SH367309_FLAG, FLASH_309_RTC_NORMAL_VALUE))
		{
			// �ɹ�����������
		}
		else
		{
			// FLASH���󣬵���System_ERROR_UserCallback()����ȡ����
			SleepElement.Sleep_Mode.bits.b1ForceToSleep_L1 = 0;
		}

		// ��CTLCӰ�쿳��
		// �������룬Ĭ�ϲ�ҪCTLCӰ�죬���ҪԤ�䣬�ż��ϣ�����������һЩ��
		// ���У��㲻������ΪҪ��������CTLC��Ե�ʣ����úûƻ��˶�����MOS����ǰ�򿪡�
		// ��Ѱ취�������������ǰ���úá�
		// ������ߴ��磬ҲҪԤ����뷨���Ѿ�����ˣ������Ȳ�д��
		// ���ߴ���������InitAFE()��ؼĴ�����Ҫ��flash���߱�־λ�ֿ�������
		// ��������ߴ����ڼ��ֶ��Ͽ���Դ����������Ч�������е�С覴á�
		// ����뷨�Ȳ�Ҫ�ɣ����ߴ���Ĭ�ϲ���ҪԤ�䣬��Ϊ���ڴ򿪹��ӵ����⣬���������������һ�ζ�·��ɡ�
		// u8_BYTE_01H_SCONF2 = BYTE_01H_SCONF2&0xF3;
		// MTPWrite(MTP_SCONF2, 1, &u8_BYTE_01H_SCONF2);
	}
	else if (SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2)
	{
		if (FLASH_COMPLETE == FlashWriteOneHalfWord(FLASH_ADDR_SH367309_FLAG, FLASH_309_NORMAL_NORMAL_VALUE))
		{
			// �ɹ�����������
		}
		else
		{
			// FLASH���󣬵���System_ERROR_UserCallback()����ȡ����
			SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 0;
		}

		// �����Ҫ����normal_normalģʽ������ǰ��CTLC���úã���ɡ�
		// u8_BYTE_01H_SCONF2 = BYTE_01H_SCONF2;
		// MTPWrite(MTP_SCONF2, 1, &u8_BYTE_01H_SCONF2);
	}

	aaaaaa1 = su16_RTC1_100msTCnt;
	aaaaaa2 = su16_Normal1_100msTCnt;
	aaaaaa3 = su16_RTC2_100msTCnt;
	aaaaaa4 = su16_Normal2_100msTCnt;
}

void Fault_ChangeToMCU(void)
{
	static UINT8 su8_CellOvp_Flag = 0;
	static UINT8 su8_CellUvp_Flag = 0;
	static UINT8 su8_IdischgOcp1_Flag = 0;
	static UINT8 su8_IdischgOcp2_Flag = 0;
	static UINT8 su8_IchgOcp_Flag = 0;
	static UINT8 su8_CellChgUtp_Flag = 0;
	static UINT8 su8_CellChgOtp_Flag = 0;
	static UINT8 su8_CellDsgUtp_Flag = 0;
	static UINT8 su8_CellDsgOtp_Flag = 0;

	g_stCellInfoReport.unMdlFault_Third.bits.b1CellOvp = SH367309_Reg_Store.REG_BSTATUS1.bits.OV;
	switch (su8_CellOvp_Flag)
	{
	case 0:
		if (g_stCellInfoReport.unMdlFault_Third.bits.b1CellOvp)
		{
			FaultWarnRecord2(CellOvp_Third);
			su8_CellOvp_Flag = 1;
		}
		break;

	case 1:
		if (!g_stCellInfoReport.unMdlFault_Third.bits.b1CellOvp)
		{
			su8_CellOvp_Flag = 0;
		}
		break;

	default:
		break;
	}

	g_stCellInfoReport.unMdlFault_Third.bits.b1CellUvp = SH367309_Reg_Store.REG_BSTATUS1.bits.UV;
	switch (su8_CellUvp_Flag)
	{
	case 0:
		if (g_stCellInfoReport.unMdlFault_Third.bits.b1CellUvp)
		{
			FaultWarnRecord2(CellUvp_Third);
			su8_CellUvp_Flag = 1;
		}
		break;

	case 1:
		if (!g_stCellInfoReport.unMdlFault_Third.bits.b1CellUvp)
		{
			su8_CellUvp_Flag = 0;
		}
		break;

	default:
		break;
	}

	g_stCellInfoReport.unMdlFault_Second.bits.b1IdischgOcp = SH367309_Reg_Store.REG_BSTATUS1.bits.OCD1;
	switch (su8_IdischgOcp1_Flag)
	{
	case 0:
		if (g_stCellInfoReport.unMdlFault_Second.bits.b1IdischgOcp)
		{
			FaultWarnRecord2(IdischgOcp_Second);
			su8_IdischgOcp1_Flag = 1;
		}
		break;

	case 1:
		if (!g_stCellInfoReport.unMdlFault_Second.bits.b1IdischgOcp)
		{
			su8_IdischgOcp1_Flag = 0;
		}
		break;

	default:
		break;
	}

	g_stCellInfoReport.unMdlFault_Third.bits.b1IdischgOcp = SH367309_Reg_Store.REG_BSTATUS1.bits.OCD2;
	switch (su8_IdischgOcp2_Flag)
	{
	case 0:
		if (g_stCellInfoReport.unMdlFault_Third.bits.b1IdischgOcp)
		{
			FaultWarnRecord2(IdischgOcp_Third);
			su8_IdischgOcp2_Flag = 1;
		}
		break;

	case 1:
		if (!g_stCellInfoReport.unMdlFault_Third.bits.b1IdischgOcp)
		{
			su8_IdischgOcp2_Flag = 0;
		}
		break;

	default:
		break;
	}

	g_stCellInfoReport.unMdlFault_Third.bits.b1IchgOcp = SH367309_Reg_Store.REG_BSTATUS1.bits.OCC;
	switch (su8_IchgOcp_Flag)
	{
	case 0:
		if (g_stCellInfoReport.unMdlFault_Third.bits.b1IchgOcp)
		{
			FaultWarnRecord2(IchgOcp_Third);
			su8_IchgOcp_Flag = 1;
		}
		break;

	case 1:
		if (!g_stCellInfoReport.unMdlFault_Third.bits.b1IchgOcp)
		{
			su8_IchgOcp_Flag = 0;
		}
		break;

	default:
		break;
	}

	g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgUtp = SH367309_Reg_Store.REG_BSTATUS2.bits.UTC;
	switch (su8_CellChgUtp_Flag)
	{
	case 0:
		if (g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgUtp)
		{
			FaultWarnRecord2(CellChgUTp_Third);
			su8_CellChgUtp_Flag = 1;
		}
		break;

	case 1:
		if (!g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgUtp)
		{
			su8_CellChgUtp_Flag = 0;
		}
		break;

	default:
		break;
	}

	g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgOtp = SH367309_Reg_Store.REG_BSTATUS2.bits.OTC;
	switch (su8_CellChgOtp_Flag)
	{
	case 0:
		if (g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgOtp)
		{
			FaultWarnRecord2(CellChgOTp_Third);
			su8_CellChgOtp_Flag = 1;
		}
		break;

	case 1:
		if (!g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgOtp)
		{
			su8_CellChgOtp_Flag = 0;
		}
		break;

	default:
		break;
	}

	g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp = SH367309_Reg_Store.REG_BSTATUS2.bits.UTD;
	switch (su8_CellDsgUtp_Flag)
	{
	case 0:
		if (g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp)
		{
			FaultWarnRecord2(CellDsgUTp_Third);
			su8_CellDsgUtp_Flag = 1;
		}
		break;

	case 1:
		if (!g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp)
		{
			su8_CellDsgUtp_Flag = 0;
		}
		break;

	default:
		break;
	}

	g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgOtp = SH367309_Reg_Store.REG_BSTATUS2.bits.OTD;
	switch (su8_CellDsgOtp_Flag)
	{
	case 0:
		if (g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgOtp)
		{
			FaultWarnRecord2(CellDsgOTp_Third);
			su8_CellDsgOtp_Flag = 1;
		}
		break;

	case 1:
		if (!g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgOtp)
		{
			su8_CellDsgOtp_Flag = 0;
		}
		break;

	default:
		break;
	}
}

void TemperatureCheck(void)
{
	// SH367309_Reg_Store.REG_BSTATUS2.bits.OTC;
	// SH367309_Reg_Store.REG_BSTATUS2.bits.UTC;
	// SH367309_Reg_Store.REG_BSTATUS2.bits.OTD;
	// SH367309_Reg_Store.REG_BSTATUS2.bits.UTD;

	if (SH367309_Reg_Store.REG_BSTATUS2.bits.OTC || SH367309_Reg_Store.REG_BSTATUS2.bits.UTC)
	{
		if (g_stCellInfoReport.u16Ichg == 0)
		{
			SH367309_Reg_Store.REG_BSTATUS2.bits.OTC = 0;
			SH367309_Reg_Store.REG_BSTATUS2.bits.UTC = 0;
		}
	}

	if (SH367309_Reg_Store.REG_BSTATUS2.bits.OTD || SH367309_Reg_Store.REG_BSTATUS2.bits.UTD)
	{
		if (g_stCellInfoReport.u16IDischg == 0)
		{
			SH367309_Reg_Store.REG_BSTATUS2.bits.OTD = 0;
			SH367309_Reg_Store.REG_BSTATUS2.bits.UTD = 0;
		}
	}
}

// mos���ƻ��ܣ���ʷ������¼������ϵ
void App_SH367309_Monitor(void)
{
	static UINT8 su8_SC_Flag = 0;
	static UINT8 su8_EEPR_WR_Flag = 0;

	static UINT8 su8_CtrlMos_Flag = 0;

	if (0 == g_st_SysTimeFlag.bits.b1Sys100msFlag)
	{ // ���ʱ������������Ӱ��MOS��������ʼ������У׼
		return;
	}

	// if(MTPRead(MTP_BSTATUS1, 3, &SH367309_Reg_Store.REG_BSTATUS1.all)) {
	if (MTPRead(MTP_BALANCEH, 5, &SH367309_Reg_Store.u8_MTP_BALANCEH))
	{
		// g_stCellInfoReport.u16BalanceFlag1 = SH367309_Reg_Store.u8_MTP_BALANCEL;
		// g_stCellInfoReport.u16BalanceFlag2 = SH367309_Reg_Store.u8_MTP_BALANCEH;
		// SystemStatus.bits.b1Status_MOS_PRE = SH367309_Reg_Store.REG_BSTATUS3.bits.PCHG_FET;
		SystemStatus.bits.b1Status_MOS_CHG = SH367309_Reg_Store.REG_BSTATUS3.bits.CHG_FET;
		SystemStatus.bits.b1Status_MOS_DSG = SH367309_Reg_Store.REG_BSTATUS3.bits.DSG_FET;

		TemperatureCheck();
		// 9��������
		Fault_ChangeToMCU();

		switch (su8_SC_Flag)
		{
		case 0:
			if (SH367309_Reg_Store.REG_BSTATUS1.bits.SC)
			{
				System_ERROR_UserCallback(ERROR_CBC_DSG);
				su8_SC_Flag = 1;
			}
			break;

		case 1:
			// ���ضϿ�(DSGD�̵ܽ�ƽ����VDSGD)������ʱ�䳬�������ͷ���ʱtD1����������Ϊ2s
			if (!SH367309_Reg_Store.REG_BSTATUS1.bits.SC)
			{
				su8_SC_Flag = 0;
			}
			break;

		default:
			break;
		}

		// �۲����ͣ����ܱ���λ����λ˵�������⣬���ò���
		// SH367309_Reg_Store.REG_BSTATUS1.bits.PF;
		// SH367309_Reg_Store.REG_BSTATUS1.bits.WDT;
		// SH367309_Reg_Store.REG_BSTATUS3.bits.L0V;
		// SH367309_Reg_Store.REG_BSTATUS3.bits.EEPR_WR;
		switch (su8_EEPR_WR_Flag)
		{
		case 0:
			if (SH367309_Reg_Store.REG_BSTATUS3.bits.EEPR_WR)
			{
				System_ERROR_UserCallback(ERROR_EEPROM_STORE);
				su8_EEPR_WR_Flag = 1;
			}
			break;

		case 1:
			// ���ضϿ�(DSGD�̵ܽ�ƽ����VDSGD)������ʱ�䳬�������ͷ���ʱtD1����������Ϊ2s
			if (!SH367309_Reg_Store.REG_BSTATUS3.bits.EEPR_WR)
			{
				su8_EEPR_WR_Flag = 0;
			}
			break;

		default:
			break;
		}

		if (SH367309_Reg_Store.REG_BSTATUS1.bits.PF)
		{
			System_ERROR_UserCallback(ERROR_SPI);
		}

		if (SH367309_Reg_Store.REG_BSTATUS1.bits.WDT)
		{
			System_ERROR_UserCallback(ERROR_UPPER);
		}

		if (SH367309_Reg_Store.REG_BSTATUS3.bits.L0V)
		{
			System_ERROR_UserCallback(ERROR_CLIENT);
		}

		// ԭ����ִ��һ�α�ɣ����ܸ���MOS״̬����ִ�������������ȻЧ�ʵ���
		switch (su8_CtrlMos_Flag)
		{
		case 0:
			if (SystemStatus.bits.b1Status_BnCloseIO)
			{
				SH367309_DriverMos_Ctrl(GPIO_CHG, 0);
				SH367309_DriverMos_Ctrl(GPIO_DSG, 0);
				su8_CtrlMos_Flag = 1;
			}
			break;

		case 1:
			if (!SystemStatus.bits.b1Status_BnCloseIO)
			{
				SH367309_DriverMos_Ctrl(GPIO_CHG, 1);
				SH367309_DriverMos_Ctrl(GPIO_DSG, 1);
				su8_CtrlMos_Flag = 0;
			}
			break;

		default:
			break;
		}

#ifdef _SLEEP_WITH_CURRENT
		// ���ߴ������
		SH367309_SleepMode_Ctrl();
#endif
	}
	else
	{
		// ��ȡʧ�ܣ�Ҫ����ʲô���飿
		// ����������߿���
		// Sleep_Mode.bits.b1ForceToSleep_L3 = 1;
	}
}

void App_DI1_Switch(void)
{
	if (0 == g_st_SysTimeFlag.bits.b1Sys200msFlag3)
	{
		return;
	}

#ifdef _DI_SWITCH_DSG_ONOFF
	static uint8_t su8_OnOFF_Flag = 0;
	static uint8_t su8_Repeat_Tcnt = 0;

	switch (su8_OnOFF_Flag)
	{
	case 0:
		if (MCUI_ENI_DI1)
		{
			SH367309_DriverMos_Ctrl(GPIO_DSG, 0);
			if (++su8_Repeat_Tcnt >= 5)
			{
				su8_Repeat_Tcnt = 0;
				su8_OnOFF_Flag = 1;
			}
		}
		else
		{
			su8_Repeat_Tcnt = 0;
			su8_OnOFF_Flag = 1;
		}
		break;

	case 1:
		if (!MCUI_ENI_DI1)
		{
			SH367309_DriverMos_Ctrl(GPIO_DSG, 1);
			if (++su8_Repeat_Tcnt >= 5)
			{
				su8_Repeat_Tcnt = 0;
				su8_OnOFF_Flag = 0;
			}
		}
		else
		{
			su8_Repeat_Tcnt = 0;
			su8_OnOFF_Flag = 0;
		}
		break;

	default:
		break;
	}
#endif

#ifdef _DI_SWITCH_SYS_ONOFF
	static UINT16 su16_AntiShake_Cnt1 = 0;

	if (1 == MCUI_ENI_DI1)
	{
		if (++su16_AntiShake_Cnt1 >= 2)
		{
			su16_AntiShake_Cnt1 = 2;
			SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1;
		}
	}
	else
	{
		if (su16_AntiShake_Cnt1)
		{
			--su16_AntiShake_Cnt1;
			return;
		}
		// SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 0;
	}
#endif
}

void App_SH367309(void)
{
	App_SH367309_Monitor();
	App_SH367309_Supplement();
	SH367309_UpdataAfeConfig();
	App_DI1_Switch();

#ifdef _SLEEP_WITH_CURRENT
	SH367309_SleepMode_Ctrl();
#else
	SH367309_Driver_Supplement(); // 10msʱ������ҪApp_SH367309_Monitor()������������			   //���ߴ���Ŀǰ����ҪԤ�䡣
#endif
}
