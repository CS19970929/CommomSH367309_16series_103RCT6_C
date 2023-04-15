#include "main.h"

UINT8 u8IICFaultcnt1 = 0;
UINT8 u8WakeCnt1 = 0;
UINT8 u8IICFaultcnt2 = 0;
UINT8 u8WakeCnt2 = 0;

UINT16 g_u16CalibCoefK[KB_NUM];
INT16 g_i16CalibCoefB[KB_NUM];

UINT16 CopperLoss[CompensateNUM]; // uΩ
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

// 这里排列好就行，不需要电池位号映射表。>61000为不用
// 经过验算，AFE1校准一次，然后本身再校准一次叠加是可以的。不需要确定某一个KB值的做法。
// 假设先确定用AFE1还是本身的KB的话，会出现问题。如下：
// 假设需要整体校准，行，AFE1先行，然后发现某几串出问题，继续使用本身KB值，然后本身KB值需要同步前面AFE1的KB值一起算才行
// 如果又变成单独使用本身KB值校准，出现错误。
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

	// 单片机读总压
	// u32VCellTotle = ((g_i32ADCResult[ADC_VBC]*g_u16CalibCoefK[VOLT_VBUS])>>10) + (UINT32)g_i16CalibCoefB[VOLT_VBUS]*1000;
	// AFE读总压
	// u32VCellTotle = ((g_stBq769x0_Read_AFE1.u32VBat*g_u16CalibCoefK[VOLT_VBUS])>>10) + (UINT32)g_i16CalibCoefB[VOLT_VBUS]*1000;
	// 所有单节电池电压加起来
	u32VCellTotle = ((u32VCellTotle * g_u16CalibCoefK[VOLT_VBUS]) >> 10) + (UINT32)g_i16CalibCoefB[VOLT_VBUS] * 1000;

	g_stCellInfoReport.u16VCellTotle = (UINT16)((u32VCellTotle * 1638 >> 14) & 0xFFFF); // 除以10
	g_stCellInfoReport.u16VCellMax = t_u16VcellMaxTemp;									// max cell voltage
	g_stCellInfoReport.u16VCellMin = t_u16VcellMinTemp;									// min cell voltage
	g_stCellInfoReport.u16VCellDelta = t_u16VcellMaxTemp - t_u16VcellMinTemp;			// delta cell voltage
	g_stCellInfoReport.u16VCellMaxPosition = t_u8VcellMaxPosition + 1;					// max cell voltage
	g_stCellInfoReport.u16VCellMinPosition = t_u8VcellMinPosition + 1;					// min cell voltage
}

/*这个是数据溢出的问题，其次是>>这个的优先级和别的符号优先级的问题
  运算符优先级太混乱导致数据溢出的问题
   (UINT16)(t_i32temp/100) 和
	(UINT16)(t_i32temp)/100不一样
*/
void DataLoad_Temperature(void)
{
	UINT8 i;
	INT32 t_i32temp;
	UINT8 Select;

	Select = 2;
	// 没纳入统计的，默认值就是0了
	for (i = 0; i < Select; i++)
	{
		t_i32temp = (INT32)SH367309_Read_AFE1.u16TempBat[i] / 10 - 40;
		t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_TEMP1 + i]) + g_i16CalibCoefB[MDL_TEMP1 + i]) >> 10;
		g_stCellInfoReport.u16Temperature[i] = (UINT16)(t_i32temp * 10 + 400);
		Monitor_TempBreak(&g_stCellInfoReport.u16Temperature[i]);
	}

	g_stCellInfoReport.u16Temperature[2] = 0;

#if 0
	//环境温度1
	t_i32temp = g_i32ADCResult[ADC_TEMP_EV1] / 10 - 40;		//放大1000倍和B值对应的意思
	//t_i32temp =  - 40;
	t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_TEMP_ENV1]) + g_i16CalibCoefB[MDL_TEMP_ENV1])>>10;
	g_stCellInfoReport.u16Temperature[ENV_TEMP1] = (UINT16)(t_i32temp*10 + 400);
	Monitor_TempBreak(&g_stCellInfoReport.u16Temperature[ENV_TEMP1]);
#endif

	// 环境温度2
	// 如果没有，这个默认就是0(ADC.c不会调用)
	t_i32temp = g_i32ADCResult[ADC_TEMP_EV2] / 10 - 40;
	t_i32temp = -40;
	t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_TEMP_ENV2]) + g_i16CalibCoefB[MDL_TEMP_ENV2]) >> 10;
	g_stCellInfoReport.u16Temperature[ENV_TEMP2] = (UINT16)(t_i32temp * 10 + 400);

	// 环境温度3
	t_i32temp = g_i32ADCResult[ADC_TEMP_EV3] / 10 - 40;
	t_i32temp = -40;
	t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_TEMP_ENV3]) + g_i16CalibCoefB[MDL_TEMP_ENV3]) >> 10;
	g_stCellInfoReport.u16Temperature[ENV_TEMP3] = (UINT16)(t_i32temp * 10 + 400);

#if 1
	// MOS温度为散热片温度
	// 取两者最大值
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

	// 如果是两个环境温度，则改为8便可
	for (i = 0; i < 7; i++)
	{ // 默认只有一个环境温度，纳入计算
		if (g_stCellInfoReport.u16Temperature[i] == 0)
		{			  // 这段代码什么意思，断了就不判断吗？
			continue; // 有的，则必定会被赋值，要么-29摄氏度。
		}			  // 空的，则就是默认刚上电的值0
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

// 这个函数的修改历程，从侧面说明，有符号数的运算，必须彻底搞明白，还有一个是指针
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

	/* AFE复位后要重新校准 */
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
// 为什么要隔开呢，如果先烧一个休眠带电的代码，然后再烧一个非休眠带电的代码，则有可能会卡在这里。永不校准。
#ifdef _SLEEP_WITH_CURRENT
		// 如果是休眠带电模式进入的休眠，不需要再次进行校准，直接使用保存的校准值。
		if (FLASH_309_RTC_RTC_VALUE == FlashReadOneHalfWord(FLASH_ADDR_SH367309_FLAG) || FLASH_309_RTC_NORMAL_VALUE == FlashReadOneHalfWord(FLASH_ADDR_SH367309_FLAG))
		{
			su16_OffsetValue = FlashReadOneHalfWord(FLASH_ADDR_SH367309_VALUE);
			su8_StartUpFlag = 3;
		}
		else
		{
			SystemStatus.bits.b1Status_BnCloseIO = 1; // 主循环50ms以内肯定还没打开管子，不需要再InitVar()处理
			if (++su8_StartUpDelay_Tcnt1 >= 3)
			{ // 改为200ms
				su8_StartUpDelay_Tcnt1 = 0;
				su8_StartUpFlag = 1;
			}
		}

#else
		SystemStatus.bits.b1Status_BnCloseIO = 1; // 主循环50ms以内肯定还没打开管子，不需要再InitVar()处理
		if (++su8_StartUpDelay_Tcnt1 >= 3)
		{ // 改为200ms
			su8_StartUpDelay_Tcnt1 = 0;
			su8_StartUpFlag = 1;
		}
#endif
		break;

	case 1:
		if (++su8_StartUp_CaliCnt <= 8)
		{ // 先2s为20*2次，3s为20*3次
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
			// 如果是normal的休眠和唤醒，则需要再次保存最新的值。
			FlashWriteOneHalfWord(FLASH_ADDR_SH367309_VALUE, su16_OffsetValue);
		}
		break;

	case 3:
		// 计算mA
		// su32_OffsetValue = (su32_OffsetValue>>3);
		if ((su16_OffsetValue & 0x8000) == 0)
		{
			su16_OffsetValue_CHG = (UINT32)su16_OffsetValue * 200 * g_u32CS_Res_AFE / (21470);
		}
		else
		{
			su16_OffsetValue_DSG = (UINT32)((UINT16)(0xFFFF - su16_OffsetValue + 1)) * 200 * g_u32CS_Res_AFE / (21470); // mA
		}

		// 使用偏置值校准
		if (su16_OffsetValue_CHG)
		{
			su8_StartUpFlag = 4;
		}
		else
		{
			su8_StartUpFlag = 5;
		}
		gu8_DriverStartUpFlag = 1; // 校准完毕，可以打开放电管
		break;

	// 充电偏置
	case 4:
		if (u32_ChgCur_mA > su16_OffsetValue_CHG)
		{
			u32_ChgCur_mA = u32_ChgCur_mA - su16_OffsetValue_CHG;
		}
		else
		{
			// u32_ChgCur_mA = 0;	//不能先置0啊，不然错了
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

// 用AFE1还是AD采样后面再说
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
		/* 电流大于2a校准有b值 */
		if (t_i32temp > 2000)
		{
			t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_IDSG])) + (INT32)g_i16CalibCoefB[MDL_IDSG] * 1000; // B值是基于A为单位计算出来的
		}
		/* 电流小于2a校准无b值 */
		else
		{
			t_i32temp = ((t_i32temp * 1024));
		}
	}
	else
	{
		/* 电流大于2a校准有b值 */
		if (t_i32temp > 2000)
		{
			t_i32temp = ((t_i32temp * g_u16CalibCoefK[MDL_ICHG])) + (INT32)g_i16CalibCoefB[MDL_ICHG] * 1000;
		}
		/* 电流小于2a校准无b值 */
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
	//休眠带电不能校准
	//1，开机，校准要关管子，带电关管子会出现咔嚓咔嚓MOS疯狂动作
	//2，如果后端后端接一个小电流不能唤醒的负载，开机突然断电，客户体验很差
	g_stCellInfoReport.u16Ichg = (UINT16)(u32_ChgCur_mA/100);
	g_stCellInfoReport.u16IDischg = (UINT16)(u32_DsgCur_mA/100);

	#else
	DataLoad_CurrentCali();
	#endif
	*/

	// 休眠带电也要校准，把偏置值保存起来就行，模拟前端没有进入休眠，管子打开，不需要再次校准而已。
	// 如果IDLE模式也不行，起来需要再校准的话，需要把看门狗关掉，然后模拟前端不进入休眠，挂着。
	// 具体修改点，就是休眠函数那里处理一下便可。
	// 等待后续验证
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
			{ // 20次1s
				Init_Registers(num);
				u8IICFaultcnt1 = 0;
				System_ERROR_UserCallback(ERROR_AFE1); // 这里调用便可
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
				System_ERROR_UserCallback(ERROR_AFE2); // 这里调用便可
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
		{ // 等待5min后进入休眠
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
		{ // 等待5min后进入休眠
			su16_Sleep_DelayT2 = 0;
			SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1;
		}
	}
	else
	{
		su16_Sleep_DelayT2 = 0;
	}

	// 暂时寄存这里
	if (System_ERROR_UserCallback(ERROR_STATUS_EEPROM_COM) || System_ERROR_UserCallback(ERROR_STATUS_EEPROM_STORE))
	{
		if (++su16_Sleep_DelayT3 >= 5 * 60 * 5)
		{ // 等待5min后进入休眠
			su16_Sleep_DelayT3 = 0;
			SleepElement.Sleep_Mode.bits.b1ForceToSleep_L2 = 1;
		}
	}
	else
	{
		su16_Sleep_DelayT3 = 0;
	}
}

// 030单片机的8M主频只能改为200ms，不然时基出问题。72M可以用50ms。
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
