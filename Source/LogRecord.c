#include "main.h"

#define EVENT_RECORD_LENGTH 	100

UINT8 BMS_LOG_POINT = 0;
UINT8 BMS_LOG_RECORD[EVENT_RECORD_LENGTH][2];		//0是事件编号，1是与上一个事件的时间间隔
LOG_RECORD_FLAG LogRecord_Flag;
UINT8 gu8_Reset_EventRecord = 0;


UINT8 LogTime_Map(UINT32* Time_S_Cnt) {
	UINT8 result = 0;

	if((*Time_S_Cnt) <= 60) {	//1min以内
		result = 171;
	}
	else if((*Time_S_Cnt) <= 3600 * 168) {	//7d以内，按小时算
		result = (*Time_S_Cnt)/3600 + ((*Time_S_Cnt)%3600)>0?1:0;
	}
	else {
		result = 170;			//大于7d，统一为大于7d
	}

	(*Time_S_Cnt) = 0;			//清零，统计下一个间隔

	return result;
}


void LogEvent_EEPROM(LogEventArray event, UINT32* Time_S_Cnt) {
	UINT16 temp = 0;
	
	if(BMS_LOG_POINT >= EVENT_RECORD_LENGTH)BMS_LOG_POINT = 0;
	BMS_LOG_RECORD[BMS_LOG_POINT][0] = event;
	BMS_LOG_RECORD[BMS_LOG_POINT][1] = LogTime_Map(Time_S_Cnt);
	if(event == BMS_START_UP)BMS_LOG_RECORD[BMS_LOG_POINT][1] = 0;
	++BMS_LOG_POINT;
	//if(++BMS_LOG_POINT >= 100)BMS_LOG_POINT = 0;		//这样写完蛋了，下面需要减1

	temp = BMS_LOG_RECORD[BMS_LOG_POINT-1][0] + (BMS_LOG_RECORD[BMS_LOG_POINT-1][1]<<8);

	WriteEEPROM_Word_WithZone(E2P_ADDR_START_EVENT_RECORD + ((BMS_LOG_POINT-1)<<1), temp);
	WriteEEPROM_Word_WithZone(E2P_ADDR_E2POS_EVENT_POINT, BMS_LOG_POINT);
}


void LogEvent_Record(UINT8 temp, LogEventArray event, UINT32* Time_S_Cnt) {
	static UINT8 su8_Event[EVENT_NUM] = {0};
	static UINT8 su8_CBC_Temp = 0;

	if(BMS_START_UP == event) {
		if(LogRecord_Flag.bits.Log_StartUp) {	//直接使用，直接修改，别的不能自己修改
			LogEvent_EEPROM(event, Time_S_Cnt);
			LogRecord_Flag.bits.Log_StartUp = 0;
		}
	}
	else if(BMS_SLEEP == event) {
		if(LogRecord_Flag.bits.Log_Sleep) {
			LogEvent_EEPROM(event, Time_S_Cnt);
			LogRecord_Flag.bits.Log_Sleep = 0;
			SleepElement.u8_ToSleepFlag = 0;	//释放，进入休眠
		}
	}	
	else if(CBC_ERR == event) {					//因为CBC不会被清除，只会一直叠加上去，所以用这个记录
		if(su8_CBC_Temp != temp) {				//有一些会瞬间触发好几次中断，导致CBC加几次，问题不大，因为这个是S级别响应。
			su8_CBC_Temp = temp;
			LogEvent_EEPROM(event, Time_S_Cnt);
		}		
	}
	else {
		switch(su8_Event[event]) {
			case 0:
				if(temp) {
					LogEvent_EEPROM(event, Time_S_Cnt);
					su8_Event[event] = 1;
					//这里后续会有EEPROM操作
				}
				break;
		
			case 1:
				if(!temp) {
					su8_Event[event] = 0;
				}
				break;
				
			default:
				break;
		}
	}
}


void App_LogRecord(void) {
	UINT8 temp;
	static UINT32 su32_Interval_S_Tcnt = 0;

	if(0 == g_st_SysTimeFlag.bits.b1Sys1000msFlag3) {
		return;
	}

	if(gu8_Reset_EventRecord) {
		return;
	}
	
	++su32_Interval_S_Tcnt;

	LogEvent_Record(LogRecord_Flag.bits.Log_StartUp, BMS_START_UP, &su32_Interval_S_Tcnt);
	LogEvent_Record(LogRecord_Flag.bits.Log_Sleep, BMS_SLEEP, &su32_Interval_S_Tcnt);
	LogEvent_Record(g_stCellInfoReport.u16BalanceFlag1, BALANCE_OPEN, &su32_Interval_S_Tcnt);
	
	LogEvent_Record(SystemStatus.bits.b1Status_Heat, HEAT_OPEN, &su32_Interval_S_Tcnt);
	LogEvent_Record(SystemStatus.bits.b1Status_Cool, COOL_OPEN, &su32_Interval_S_Tcnt);

	LogEvent_Record(g_stCellInfoReport.unMdlFault_Third.bits.b1CellOvp, VCELL_OVP, &su32_Interval_S_Tcnt);
	LogEvent_Record(g_stCellInfoReport.unMdlFault_Third.bits.b1BatOvp, VBUS_OVP, &su32_Interval_S_Tcnt);
	LogEvent_Record(g_stCellInfoReport.unMdlFault_Third.bits.b1IchgOcp, CHG_OCP, &su32_Interval_S_Tcnt);

	LogEvent_Record(g_stCellInfoReport.unMdlFault_Third.bits.b1CellUvp, VCELL_UVP, &su32_Interval_S_Tcnt);
	LogEvent_Record(g_stCellInfoReport.unMdlFault_Third.bits.b1BatUvp, VBUS_UVP, &su32_Interval_S_Tcnt);
	LogEvent_Record(g_stCellInfoReport.unMdlFault_Third.bits.b1IdischgOcp, DSG_OCP, &su32_Interval_S_Tcnt);
	
	LogEvent_Record(g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgUtp, CHG_UTP, &su32_Interval_S_Tcnt);
	LogEvent_Record(g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp, DSG_UTP, &su32_Interval_S_Tcnt);
	LogEvent_Record(g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgOtp, CHG_OTP, &su32_Interval_S_Tcnt);
	LogEvent_Record(g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgOtp, DSG_OTP, &su32_Interval_S_Tcnt);
	LogEvent_Record(g_stCellInfoReport.unMdlFault_Third.bits.b1VcellDeltaBig, VDELTA_OP, &su32_Interval_S_Tcnt);

	//以下这3个选项都不会清零，所以只会运行一次。
	//AFE和EEPROM(无复原机制)，出现过一次之后，5min会进入休眠，AFE1因为是++所以可能会有小BUG，但是影响不大。
	LogEvent_Record(System_ERROR_UserCallback(ERROR_STATUS_AFE1), AFE1_ERR, &su32_Interval_S_Tcnt);
	LogEvent_Record(System_ERROR_UserCallback(ERROR_STATUS_AFE2), AFE2_ERR, &su32_Interval_S_Tcnt);
	temp = System_ERROR_UserCallback(ERROR_STATUS_EEPROM_STORE)+System_ERROR_UserCallback(ERROR_STATUS_EEPROM_COM);
	LogEvent_Record(temp, EEPROM_ERR, &su32_Interval_S_Tcnt);

	//其中如果CBC连续触发三次，关掉驱动功能，但是不会进入休眠，如果没人管，会直到深度休眠
	LogEvent_Record(System_ERROR_UserCallback(ERROR_STATUS_CBC_DSG), CBC_ERR, &su32_Interval_S_Tcnt);

	/*
	switch(su8_VcellOVP) {
		case 0:
			if(g_stCellInfoReport.unMdlFault_Third.bits.b1CellOvp) {
				BMS_LOG_RECORD[BMS_LOG_POINT][0] = VCELL_OVP;
				BMS_LOG_RECORD[BMS_LOG_POINT][1] = LogTime_Map(&su32_Interval_S_Tcnt);
				BMS_LOG_POINT++;
				su8_VcellOVP = 1;
			}
			break;
	
		case 1:
			if(!g_stCellInfoReport.unMdlFault_Third.bits.b1CellOvp) {
				su8_VcellOVP = 0;
			}
			break;
			
		default:
			break;
	}
	*/
}


void Sci_ACK_0x03_ReadRegs_EventRecord(UINT8 t_u8BuffTemp[]) {
	UINT16	i,j;
	INT8 k;

	i = 0;		//少了这句话导致上传数值错误，意味着函数内的局部参数初始化不为0

	for(j = 0;j < EVENT_RECORD_LENGTH;j++) {
		k = BMS_LOG_POINT - 1 - j;
		if(k < 0) {
			k = EVENT_RECORD_LENGTH + k;
		}
		t_u8BuffTemp[i++] = BMS_LOG_RECORD[k][0];
		t_u8BuffTemp[i++] = BMS_LOG_RECORD[k][1];
	}
}


//感觉在通讯之间reset会很慢呀，先看看这样有没有问题，没问题就算了
//这样写最方便，不需要搞这么多WriteEEPROM_ByteData_Circle()
void Sci_WrReg_0x06_Reset_EventRecord(struct RS485MSG *s) {
	UINT8 i;

	UINT16 u16SciRegData = s->u16Buffer[5] + (s->u16Buffer[4] << 8);
	if(0x0001 == u16SciRegData) {
		//EEPROM_ResetData_EventRecord_ToDefault();
		
		for(i = 0; i < EVENT_RECORD_LENGTH; ++i) {
			BMS_LOG_RECORD[i][0] = 0;
			BMS_LOG_RECORD[i][1] = 0;
		}
		BMS_LOG_POINT = 0;
		
		gu8_Reset_EventRecord = EVENT_RECORD_LENGTH;
		
	}
	else {
		s ->AckType = RS485_ACK_NEG;
		s ->ErrorType = RS485_ERROR_DATA_INVALID;
	}
}


void EEPROM_ResetData_EventRecord_ToDefault(void) {
	UINT8 i;

	for(i = 0; i < EVENT_RECORD_LENGTH; ++i) {
		BMS_LOG_RECORD[i][0] = 0;
		BMS_LOG_RECORD[i][1] = 0;
	}
	BMS_LOG_POINT = 0;

	for(i = 0; i < EVENT_RECORD_LENGTH; ++i) {
		WriteEEPROM_Word_WithZone(E2P_ADDR_START_EVENT_RECORD + (i<<1), 0);
	}
	WriteEEPROM_Word_WithZone(E2P_ADDR_E2POS_EVENT_POINT, BMS_LOG_POINT);
}


/*
论证一番，如果出现EEPROM存储错误
1，如果是该次上电出现干扰导致，5min后唤醒起来，消失，问题不大。
2，如果是真的数据错误，无法消除，这显然是一个不好的情况。这个展开分析，首先必须知道哪个数据出问题：
A，如果是日志记录出问题，然后后面自己偷偷修改，改为NULL，则会出现数据信息有误的问题，中间一条NULL？
B，如果是保护点出问题，自己偷偷修改默认值，则会出现大问题，如果数据不对产生隐患呢(例如三元锂的默认保护点弄到磷酸铁锂)？
C，基于A和B，就是出现EEPROM错误，不能偷偷修改，要人为去操作。如果是保护点出问题，则刷新保护点，如果是日志出问题，则刷新日志。
D，怎么找是哪里出错，观察保护点可修改那一页便可，还有日志记录，会找到问题点。
E，实际上BMS调教之后出去，不会出现这种状况，就算出现了，重启应该会消失，不然就算硬件坏了。

3，再想想，如果是日志出问题(新代码写到旧板子上)，为了方便，貌似偷偷修改为NULL问题也不大？
*/
void ReadEEPROM_EventRecord_Parameters(void) {
	UINT8 i;
	UINT16 t_u16RdTemp;

	BMS_LOG_POINT = ReadEEPROM_Word_WithZone(E2P_ADDR_E2POS_EVENT_POINT);
	if(BMS_LOG_POINT >= 101) {			//如果指针出问题，全部Reset
		System_ERROR_UserCallback(ERROR_EEPROM_STORE);
		EEPROM_ResetData_EventRecord_ToDefault();
	}

	for(i = 0; i < EVENT_RECORD_LENGTH; ++i) {
    	t_u16RdTemp = ReadEEPROM_Word_WithZone(E2P_ADDR_START_EVENT_RECORD + (i<<1));
		if(((t_u16RdTemp&0x00FF) <= EVENT_NUM) && (t_u16RdTemp>>8) <= 171) {	//NULL的值有两个，0和255，255是为了别的板子不需要再刷一次EEPROM
        	BMS_LOG_RECORD[i][0]  =  (UINT8)(t_u16RdTemp&0x00FF);				//基于上面那个全部reset，255位NULL值取消
			BMS_LOG_RECORD[i][1]  =  (UINT8)(t_u16RdTemp>>8);					//就算旧板子也没问题。
        }
        else {
			if(0 == System_ErrFlag.u8ErrFlag_Com_EEPROM) {
				System_ERROR_UserCallback(ERROR_EEPROM_STORE);
			}
        	BMS_LOG_RECORD[i][0]  =  0;
			BMS_LOG_RECORD[i][1]  =  0;
			WriteEEPROM_Word_WithZone(E2P_ADDR_START_EVENT_RECORD + (i<<1), 0);	//出现错误，日志可以偷偷尝试修改为默认值。别的绝对不可以。
        }
	}
}

