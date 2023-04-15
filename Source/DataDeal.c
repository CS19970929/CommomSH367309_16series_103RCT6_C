#include "main.h"

UINT8 u8IICFaultcnt1 = 0;
UINT8 u8WakeCnt1 = 0;
UINT8 u8IICFaultcnt2 = 0;
UINT8 u8WakeCnt2 = 0;

UINT16 g_u16CalibCoefK[KB_NUM];
INT16 g_i16CalibCoefB[KB_NUM];

UINT16 CopperLoss[CompensateNUM]; // u��
UINT16 CopperLoss_Num[CompensateNUM];

UINT32 g_u32CS_Res_AFE = 0;

struct OTHER_ELEMENT OtherElement;

UINT32 u32_ChgCur_mA = 0;
UINT32 u32_DsgCur_mA = 0;

void Init_Registers(UINT8 num)
{
	UINT8 j;
	switch (num)
	{
	case 0:
		for (j = 0; j < 21; j++)
		{
			*(&(Registers_AFE1.Temp1) + j) = 0;
		}
		break;

	case 1:
		break;

	default:
		break;
	}
	// CHG_OFF;
	// DSG_OFF;
}

// UINT32 aaaaa1 = 0;
// UINT32 aaaaa2 = 0;

void DataLoad_CellVolt_Test(void)
{
	g_stCellInfoReport.u16VCell[23] = SH367309_Reg_Store.REG_BSTATUS1.all;
	g_stCellInfoReport.u16VCell[24] = SH367309_Reg_Store.REG_BSTATUS2.all;
	g_stCellInfoReport.u16VCell[25] = SH367309_Reg_Store.REG_BSTATUS3.all;

	g_stCellInfoReport.u16VCell[27] = aaaaaa1;
	g_stCellInfoReport.u16VCell[28] = aaaaaa2;
	g_stCellInfoReport.u16VCell[29] = aaaaaa3;
	g_stCellInfoReport.u16VCell[30] = aaaaaa4;
	g_stCellInfoReport.u16VCell[31] = aaa11;
}

// �������кþ��У�����Ҫ���λ��ӳ���>61000Ϊ����
// �������㣬AFE1У׼һ�Σ�Ȼ������У׼һ�ε����ǿ��Եġ�����Ҫȷ��ĳһ��KBֵ��������
// ������ȷ����AFE1���Ǳ����KB�Ļ�����������⡣���£�
// ������Ҫ����У׼���У�AFE1���У�Ȼ����ĳ���������⣬����ʹ�ñ���KBֵ��Ȼ����KBֵ��Ҫͬ��ǰ��AFE1��KBֵһ�������
// ����ֱ�ɵ���ʹ�ñ���KBֵУ׼�����ִ���
void DataLoad_CellVolt(void)
{
	UINT8 i;
	INT32 t_i32temp;

	for (i = 0; i < SeriesNum; ++i)
	{
		t_i32temp = (UINT32)SH367309_Read_AFE1.u16VCell[SeriesSelect_AFE1[SeriesNum - 1][i]];
		if (g_u16CalibCoefK[VOLT_AFE1] != 1024 || g_i16CalibCoefB[VOLT_AFE1] != 0)
		{
			t_i32temp = ((t_i32temp * g_u16CalibCoefK[VOLT_AFE1]) >> 10) + g_i16CalibCoefB[VOLT_AFE1];
		}
		t_i32temp = ((t_i32temp * g_u16CalibCoefK[i]) >> 10) + g_i16CalibCoefB[i];
		t_i32temp = t_i32temp > 0 ? t_i32temp : 0;
		g_stCellInfoReport.u16VCell[i] = (UINT16)t_i32temp;
	}

	if (SeriesNum < 32)
	{
		for (i = SeriesNum; i < 32; ++i)
		{
			g_stCellInfoReport.u16VCell[i] = 61001;
		}
	}

	if (g_stCellInfoReport.u16Ichg > 0)
	{
		for (i = 0; i < CompensateNUM; ++i)
		{
			if (CopperLoss_Num[i] == 0)
			{
				break;
			}
			t_i32temp = (UINT32)CopperLoss[i] * g_stCellInfoReport.u16Ichg;
			g_stCellInfoReport.u16VCell[CopperLoss_Num[i] - 1] -= (UINT16)(((t_i32temp >> 14) + (t_i32temp >> 15) + (t_i32temp >> 17)) & 0xFFFF);
		}
	}
	else if (g_stCellInfoReport.u16IDischg > 0)
	{
		for (i = 0; i < CompensateNUM; ++i)
		{
			if (CopperLoss_Num[i] == 0)
			{
				break;
			}
			t_i32temp = (UINT32)CopperLoss[i] * g_stCellInfoReport.u16IDischg;
			g_stCellInfoReport.u16VCell[CopperLoss_Num[i] - 1] += (UINT16)(((t_i32temp >> 14) + (t_i32temp >> 15) + (t_i32temp >> 17)) & 0xFFFF);
		}
	}

	// DataLoad_CellVolt_Test();
}

void DataLoad_CellVoltMaxMinFind(void)
{
	UINT8 i;
	UINT16 t_u16VcellTemp;
	UINT16 t_u16VcellMaxTemp;
	UINT16 t_u16VcellMinTemp;
	UINT8 t_u8VcellMaxPosition;
	UINT8 t_u8VcellMinPosition;
	UINT32 u32VCellTotle;

	t_u16VcellMaxTemp = 0;
	t_u16VcellMinTemp = 0x7FFF;
	t_u8VcellMaxPosition = 0;
	t_u8VcellMinPosition = 0;
	u32VCellTotle = 0;

	for (i = 0; i < SeriesNum; i++)
	{
		t_u16VcellTemp = g_stCellInfoReport.u16VCell[i];
		u32VCellTotle += g_stCellInfoReport.u16VCell[i];
		if (t_u16VcellMaxTemp < t_u16VcellTemp)
		{
			t_u16VcellMaxTemp = t_u16VcellTemp;
			t_u8VcellMaxPosition = i;
		}
		if (t_u16VcellMinTemp > t_u16VcellTemp)
		{
			t_u16VcellMinTemp = t_u16VcellTemp;
			t_u8VcellMinPosition = i;
		}
	}

	// ��Ƭ������ѹ
	// u32VCellTotle = ((g_i32ADCResult[ADC_VBC]*g_u16CalibCoefK[VOLT_VBUS])>>10) + (UINT32)g_i16CalibCoefB[VOLT_VBUS]*1000;
	// AFE����ѹ
	// u32VCellTotle = ((g_stBq769x0_Read_AFE1.u32VBat*g_u16CalibCoefK[VOLT_VBUS])>>10) + (UINT32)g_i16CalibCoefB[VOLT_VBUS]*1000;
	// ���е��ڵ�ص�ѹ������
	u32VCellTotle = ((u32VCellTotle * g_u16CalibCoefK[VOLT_VBUS]) >> 10) + (UINT32)g_i16CalibCoefB[VOLT_VBUS] * 1000;

	g_stCellInfoReport.u16VCellTotle = (UINT16)((u32VCellTotle * 1638 >> 14) & 0xFFFF); // ����10
	g_stCellInfoReport.u16VCellMax = t_u16VcellMaxTemp;									// max cell voltage
	g_stCellInfoReport.u16VCellMin = t_u16VcellMinTemp;									// min cell voltage
	g_stCellInfoReport.u16VCellDelta = t_u16VcellMaxTemp - t_u16VcellMinTemp;			// delta cell voltage
	g_stCellInfoReport.u16VCellMaxPosition = t_u8VcellMaxPosition + 1;					// max cell voltage
	g_stCellInfoReport.u16VCellMinPosition = t_u8VcellMinPosition + 1;					// min cell voltage
}

/*�����������������⣬�����>>��������ȼ��ͱ�ķ������ȼ�������
  ��������ȼ�̫���ҵ����������������
   (UINT16)(t_i32temp/100) ��
	(UINT16)(t_i32temp)/100��һ��
*/
void DataLoad_Temperature(void)
{
	UINT8 i;
	INT32 t_i32temp;
	UINT8 Select;

	Select = 2;
	// û����ͳ�Ƶģ�Ĭ��ֵ����0��
	for (i = 0; i < Select; i++)
	{
		t_i32temp = (INT32)SH367309_Read_AFE1.u16TempBat[i] / 10 - 40;
		t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_TEMP1 + i]) + g_i16CalibCoefB[MDL_TEMP1 + i]) >> 10;
		g_stCellInfoReport.u16Temperature[i] = (UINT16)(t_i32temp * 10 + 400);
		Monitor_TempBreak(&g_stCellInfoReport.u16Temperature[i]);
	}

	g_stCellInfoReport.u16Temperature[2] = 0;

#if 0
	//�����¶�1
	t_i32temp = g_i32ADCResult[ADC_TEMP_EV1] / 10 - 40;		//�Ŵ�1000����Bֵ��Ӧ����˼
	//t_i32temp =  - 40;
	t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_TEMP_ENV1]) + g_i16CalibCoefB[MDL_TEMP_ENV1])>>10;
	g_stCellInfoReport.u16Temperature[ENV_TEMP1] = (UINT16)(t_i32temp*10 + 400);
	Monitor_TempBreak(&g_stCellInfoReport.u16Temperature[ENV_TEMP1]);
#endif

	// �����¶�2
	// ���û�У����Ĭ�Ͼ���0(ADC.c�������)
	t_i32temp = g_i32ADCResult[ADC_TEMP_EV2] / 10 - 40;
	t_i32temp = -40;
	t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_TEMP_ENV2]) + g_i16CalibCoefB[MDL_TEMP_ENV2]) >> 10;
	g_stCellInfoReport.u16Temperature[ENV_TEMP2] = (UINT16)(t_i32temp * 10 + 400);

	// �����¶�3
	t_i32temp = g_i32ADCResult[ADC_TEMP_EV3] / 10 - 40;
	t_i32temp = -40;
	t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_TEMP_ENV3]) + g_i16CalibCoefB[MDL_TEMP_ENV3]) >> 10;
	g_stCellInfoReport.u16Temperature[ENV_TEMP3] = (UINT16)(t_i32temp * 10 + 400);

#if 1
	// MOS�¶�Ϊɢ��Ƭ�¶�
	// ȡ�������ֵ
	// t_i32temp = (g_i32ADCResult[ADC_TEMP_MOS1] > g_i32ADCResult[ADC_TEMP_MOS2] ? g_i32ADCResult[ADC_TEMP_MOS1]:g_i32ADCResult[ADC_TEMP_MOS2]);
	t_i32temp = g_i32ADCResult[ADC_TEMP_MOS1];
	t_i32temp = t_i32temp / 10 - 40;
	t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_TEMP_MOS1]) + g_i16CalibCoefB[MDL_TEMP_MOS1]) >> 10;
	g_stCellInfoReport.u16Temperature[MOS_TEMP1] = (UINT16)(t_i32temp * 10 + 400);
	Monitor_TempBreak(&g_stCellInfoReport.u16Temperature[MOS_TEMP1]);
#endif
}

void DataLoad_TemperatureMaxMinFind(void)
{
	UINT8 i;
	UINT16 t_u16VcellTemp;
	UINT16 t_u16VcellMaxTemp;
	UINT16 t_u16VcellMinTemp;
	t_u16VcellMaxTemp = 0;
	t_u16VcellMinTemp = 0x7FFF;

	// ��������������¶ȣ����Ϊ8���
	for (i = 0; i < 7; i++)
	{ // Ĭ��ֻ��һ�������¶ȣ��������
		if (g_stCellInfoReport.u16Temperature[i] == 0)
		{			  // ��δ���ʲô��˼�����˾Ͳ��ж���
			continue; // �еģ���ض��ᱻ��ֵ��Ҫô-29���϶ȡ�
		}			  // �յģ������Ĭ�ϸ��ϵ��ֵ0
		t_u16VcellTemp = g_stCellInfoReport.u16Temperature[i];
		if (t_u16VcellMaxTemp < t_u16VcellTemp)
		{
			t_u16VcellMaxTemp = t_u16VcellTemp;
		}
		if (t_u16VcellMinTemp > t_u16VcellTemp)
		{
			t_u16VcellMinTemp = t_u16VcellTemp;
		}
	}

	g_stCellInfoReport.u16TempMax = t_u16VcellMaxTemp; // max temp
	g_stCellInfoReport.u16TempMin = t_u16VcellMinTemp; // min temp
}

// ����������޸����̣��Ӳ���˵�����з����������㣬���볹�׸����ף�����һ����ָ��
void DataLoad_CurrentCali(void)
{
	static UINT8 su8_StartUpFlag = 0;
	static UINT8 su8_StartUp_CaliCnt = 0;
	static UINT8 su8_StartUpDelay_Tcnt1 = 0;
	static UINT8 su8_StartUpDelay_Tcnt2 = 0;

	static UINT16 su16_OffsetValue = 0;
	static UINT16 su16_OffsetValue_CHG = 0;
	static UINT16 su16_OffsetValue_DSG = 0;

	static UINT32 su32_OffsetValue_CHG = 0;
	static UINT32 su32_OffsetValue_DSG = 0;
	// static UINT8 su8_OffsetValue_CHG_Cnt = 0;
	// static UINT8 su8_OffsetValue_DSG_Cnt = 0;

	/* AFE��λ��Ҫ����У׼ */
	if (AFE_ResetFlag)
	{
		AFE_ResetFlag = 0;
		su8_StartUpFlag = 0;
		su8_StartUp_CaliCnt = 0;
		su8_StartUpDelay_Tcnt1 = 0;
		su8_StartUpDelay_Tcnt2 = 0;
		su16_OffsetValue = 0;
		su16_OffsetValue_CHG = 0;
		su16_OffsetValue_DSG = 0;
		su32_OffsetValue_CHG = 0;
		su32_OffsetValue_DSG = 0;
	}

	switch (su8_StartUpFlag)
	{
	case 0:
// ΪʲôҪ�����أ��������һ�����ߴ���Ĵ��룬Ȼ������һ�������ߴ���Ĵ��룬���п��ܻῨ���������У׼��
#ifdef _SLEEP_WITH_CURRENT
		// ��������ߴ���ģʽ��������ߣ�����Ҫ�ٴν���У׼��ֱ��ʹ�ñ����У׼ֵ��
		if (FLASH_309_RTC_RTC_VALUE == FlashReadOneHalfWord(FLASH_ADDR_SH367309_FLAG) || FLASH_309_RTC_NORMAL_VALUE == FlashReadOneHalfWord(FLASH_ADDR_SH367309_FLAG))
		{
			su16_OffsetValue = FlashReadOneHalfWord(FLASH_ADDR_SH367309_VALUE);
			su8_StartUpFlag = 3;
		}
		else
		{
			SystemStatus.bits.b1Status_BnCloseIO = 1; // ��ѭ��50ms���ڿ϶���û�򿪹��ӣ�����Ҫ��InitVar()����
			if (++su8_StartUpDelay_Tcnt1 >= 3)
			{ // ��Ϊ200ms
				su8_StartUpDelay_Tcnt1 = 0;
				su8_StartUpFlag = 1;
			}
		}

#else
		SystemStatus.bits.b1Status_BnCloseIO = 1; // ��ѭ��50ms���ڿ϶���û�򿪹��ӣ�����Ҫ��InitVar()����
		if (++su8_StartUpDelay_Tcnt1 >= 3)
		{ // ��Ϊ200ms
			su8_StartUpDelay_Tcnt1 = 0;
			su8_StartUpFlag = 1;
		}
#endif
		break;

	case 1:
		if (++su8_StartUp_CaliCnt <= 8)
		{ // ��2sΪ20*2�Σ�3sΪ20*3��
			/*
			if(SH367309_Read_AFE1.u16Current >= 0xFFFD || SH367309_Read_AFE1.u16Current <= 0x0003) {
				SH367309_Read_AFE1.u16Current = 0;
			}
			//su32_OffsetValue += SH367309_Read_AFE1.u16Current;
			//CopperLoss_Num[su8_StartUp_CaliCnt] = SH367309_Read_AFE1.u16Current;
			*/
			if ((SH367309_Read_AFE1.u16Current & 0x8000) == 0)
			{
				su32_OffsetValue_CHG += SH367309_Read_AFE1.u16Current;
				//++su8_OffsetValue_CHG_Cnt;
			}
			else
			{
				// su32_OffsetValue_DSG += SH367309_Read_AFE1.u16Current;
				su32_OffsetValue_DSG += 0xFFFF - SH367309_Read_AFE1.u16Current + 1;
				//++su8_OffsetValue_DSG_Cnt;
			}

			if (su32_OffsetValue_CHG >= su32_OffsetValue_DSG)
			{
				su16_OffsetValue = (UINT16)((su32_OffsetValue_CHG - su32_OffsetValue_DSG) >> 3);
			}
			else
			{
				su16_OffsetValue = (UINT16)(0xFFFF - ((su32_OffsetValue_DSG - su32_OffsetValue_CHG) >> 3) + 1);
			}
		}
		else
		{
			su8_StartUp_CaliCnt = 0;
			su8_StartUpFlag = 2;
		}
		break;

	case 2:
		if (++su8_StartUpDelay_Tcnt2 >= 3)
		{
			su8_StartUpDelay_Tcnt2 = 0;
			SystemStatus.bits.b1Status_BnCloseIO = 0;

			su8_StartUpFlag = 3;
			// �����normal�����ߺͻ��ѣ�����Ҫ�ٴα������µ�ֵ��
			FlashWriteOneHalfWord(FLASH_ADDR_SH367309_VALUE, su16_OffsetValue);
		}
		break;

	case 3:
		// ����mA
		// su32_OffsetValue = (su32_OffsetValue>>3);
		if ((su16_OffsetValue & 0x8000) == 0)
		{
			su16_OffsetValue_CHG = (UINT32)su16_OffsetValue * 200 * g_u32CS_Res_AFE / (21470);
		}
		else
		{
			su16_OffsetValue_DSG = (UINT32)((UINT16)(0xFFFF - su16_OffsetValue + 1)) * 200 * g_u32CS_Res_AFE / (21470); // mA
		}

		// ʹ��ƫ��ֵУ׼
		if (su16_OffsetValue_CHG)
		{
			su8_StartUpFlag = 4;
		}
		else
		{
			su8_StartUpFlag = 5;
		}
		gu8_DriverStartUpFlag = 1; // У׼��ϣ����Դ򿪷ŵ��
		break;

	// ���ƫ��
	case 4:
		if (u32_ChgCur_mA > su16_OffsetValue_CHG)
		{
			u32_ChgCur_mA = u32_ChgCur_mA - su16_OffsetValue_CHG;
		}
		else
		{
			// u32_ChgCur_mA = 0;	//��������0������Ȼ����
			u32_DsgCur_mA = u32_DsgCur_mA + su16_OffsetValue_CHG - u32_ChgCur_mA;
			u32_ChgCur_mA = 0;
		}
		break;

	case 5:
		if (u32_DsgCur_mA > su16_OffsetValue_DSG)
		{
			u32_DsgCur_mA = u32_DsgCur_mA - su16_OffsetValue_DSG;
		}
		else
		{
			// u32_DsgCur_mA = 0;
			u32_ChgCur_mA = u32_ChgCur_mA + su16_OffsetValue_DSG - u32_DsgCur_mA;
			u32_DsgCur_mA = 0;
		}
		break;

	default:
		break;
	}

	g_stCellInfoReport.u16Ichg = (UINT16)(u32_ChgCur_mA / 100);
	g_stCellInfoReport.u16IDischg = (UINT16)(u32_DsgCur_mA / 100);
	// aaaaa1 = su16_OffsetValue_CHG;
	// aaaaa2 = su16_OffsetValue_DSG;
}

// ��AFE1����AD����������˵
void DataLoad_Current(void)
{
	UINT8 temp_I;
	INT32 t_i32temp;

	if ((SH367309_Read_AFE1.u16Current & 0x8000) == 0)
	{
		t_i32temp = (UINT32)SH367309_Read_AFE1.u16Current * 200 * g_u32CS_Res_AFE / (21470);
		temp_I = CurCHG;
	}
	else
	{
		t_i32temp = (UINT32)(0xFFFF - SH367309_Read_AFE1.u16Current + 1) * 200 * g_u32CS_Res_AFE / (21470); // mA
		temp_I = CurDSG;
	}

	if (temp_I == CurDSG)
	{
		/* ��������2aУ׼��bֵ */
		if (t_i32temp > 2000)
		{
			t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_IDSG])) + (INT32)g_i16CalibCoefB[MDL_IDSG] * 1000; // Bֵ�ǻ���AΪ��λ���������
		}
		/* ����С��2aУ׼��bֵ */
		else
		{
			t_i32temp = ((t_i32temp * 1024));
		}
	}
	else
	{
		/* ��������2aУ׼��bֵ */
		if (t_i32temp > 2000)
		{
			t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_ICHG])) + (INT32)g_i16CalibCoefB[MDL_ICHG] * 1000;
		}
		/* ����С��2aУ׼��bֵ */
		else
		{
			t_i32temp = ((t_i32temp * 1024));
		}
	}

	t_i32temp = t_i32temp > 0 ? t_i32temp : 0;

	if (temp_I == CurCHG)
	{
		u32_ChgCur_mA = t_i32temp >> 10; // mA
		u32_DsgCur_mA = 0;
	}
	else
	{
		u32_ChgCur_mA = 0;
		u32_DsgCur_mA = t_i32temp >> 10; // mA
	}

	/*
	#ifdef _SLEEP_WITH_CURRENT
	//���ߴ��粻��У׼
	//1��������У׼Ҫ�ع��ӣ�����ع��ӻ������������MOS�����
	//2�������˺�˽�һ��С�������ܻ��ѵĸ��أ�����ͻȻ�ϵ磬�ͻ�����ܲ�
	g_stCellInfoReport.u16Ichg = (UINT16)(u32_ChgCur_mA/100);
	g_stCellInfoReport.u16IDischg = (UINT16)(u32_DsgCur_mA/100);

	#else
	DataLoad_CurrentCali();
	#endif
	*/

	// ���ߴ���ҲҪУ׼����ƫ��ֵ�����������У�ģ��ǰ��û�н������ߣ����Ӵ򿪣�����Ҫ�ٴ�У׼���ѡ�
	// ���IDLEģʽҲ���У�������Ҫ��У׼�Ļ�����Ҫ�ѿ��Ź��ص���Ȼ��ģ��ǰ�˲��������ߣ����š�
	// �����޸ĵ㣬�������ߺ������ﴦ��һ�±�ɡ�
	// �ȴ�������֤
	DataLoad_CurrentCali();
}

void MonitorAFE(UINT8 num, UINT8 Result)
{
	static UINT16 su16_Sleep_DelayT1 = 0;
	static UINT16 su16_Sleep_DelayT2 = 0;
	static UINT16 su16_Sleep_DelayT3 = 0;

	switch (num)
	{
	case 0:
		if (Result != 0)
		{
			++u8IICFaultcnt1;
			if (u8IICFaultcnt1 > 50)
			{ // 20��1s
				Init_Registers(num);
				u8IICFaultcnt1 = 0;
				System_ERROR_UserCallback(ERROR_AFE1); // ������ñ��
			}
			if (u8IICFaultcnt1 == 30 && u8WakeCnt1 <= 20)
			{
				SH367309_Enable_AFE_Wdt_Cadc_Drivers();
				++u8WakeCnt1;
			}
			SystemStatus.bits.b1Status_AFE1 = 0;
		}
		else
		{
			if (u8IICFaultcnt1 > 0)
			{
				u8IICFaultcnt1--;
			}
			if (u8WakeCnt1 > 0)
			{
				u8WakeCnt1--;
			}
			SystemStatus.bits.b1Status_AFE1 = 1;
			// System_ERROR_UserCallback(ERROR_REMOVE_AFE1);
		}
		break;

	case 1:
		if (Result != 0)
		{
			++u8IICFaultcnt2;
			if (u8IICFaultcnt2 > 50)
			{
				Init_Registers(num);
				u8IICFaultcnt2 = 0;
				System_ERROR_UserCallback(ERROR_AFE2); // ������ñ��
			}
			if (u8IICFaultcnt2 == 30 && u8WakeCnt2 <= 20)
			{
				SH367309_Enable_AFE_Wdt_Cadc_Drivers();
				++u8WakeCnt2;
			}
			SystemStatus.bits.b1Status_AFE2 = 0;
		}
		else
		{
			if (u8IICFaultcnt2 > 0)
			{
				u8IICFaultcnt2--;
			}
			if (u8WakeCnt2 > 0)
			{
				u8WakeCnt2--;
			}
			SystemStatus.bits.b1Status_AFE2 = 1;
			// System_ERROR_UserCallback(ERROR_REMOVE_AFE2);
		}
		break;
	default:
		break;
	}

	if (System_ERROR_UserCallback(ERROR_STATUS_AFE1))
	{
		if (++su16_Sleep_DelayT1 >= 5 * 60 * 5)
		{ // �ȴ�5min���������
			su16_Sleep_DelayT1 = 0;
			SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1;
		}
	}
	else
	{
		su16_Sleep_DelayT1 = 0;
	}

	if (System_ERROR_UserCallback(ERROR_STATUS_AFE2))
	{
		if (++su16_Sleep_DelayT2 >= 5 * 60 * 5)
		{ // �ȴ�5min���������
			su16_Sleep_DelayT2 = 0;
			SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1;
		}
	}
	else
	{
		su16_Sleep_DelayT2 = 0;
	}

	// ��ʱ�Ĵ�����
	if (System_ERROR_UserCallback(ERROR_STATUS_EEPROM_COM) || System_ERROR_UserCallback(ERROR_STATUS_EEPROM_STORE))
	{
		if (++su16_Sleep_DelayT3 >= 5 * 60 * 5)
		{ // �ȴ�5min���������
			su16_Sleep_DelayT3 = 0;
			SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1;
		}
	}
	else
	{
		su16_Sleep_DelayT3 = 0;
	}
}

// 030��Ƭ����8M��Ƶֻ�ܸ�Ϊ200ms����Ȼʱ�������⡣72M������50ms��
void App_AFEGet(void)
{
	if (0 == g_st_SysTimeFlag.bits.b1Sys200msFlag3 || 1 == gu8_TxEnable_SCI1 || 1 == gu8_TxEnable_SCI2 || 1 == gu8_TxEnable_SCI3)
	{
		return;
	}

	if (u32E2P_Pro_VolCur_WriteFlag != 0 || u32E2P_Pro_Temp_WriteFlag != 0 || u32E2P_Pro_Other_WriteFlag != 0 || u32E2P_OtherElement1_WriteFlag != 0 || u32E2P_RTC_Element_WriteFlag != 0 || u8E2P_SocTable_WriteFlag != 0 || u8E2P_CopperLoss_WriteFlag != 0 || u8E2P_KB_WriteFlag != 0)
	{
		return;
	}

	MonitorAFE(0, UpdateVoltageFromBqMaximo());

	DataLoad_CellVolt();
	// DataLoad_CellVolt_Test();
	DataLoad_CellVoltMaxMinFind();
	DataLoad_Temperature();
	DataLoad_TemperatureMaxMinFind();
	DataLoad_Current();
}
