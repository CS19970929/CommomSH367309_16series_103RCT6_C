#ifndef LOG_RECORD_H
#define LOG_RECORD_H

typedef enum _LogEventArray {
	BMS_EVENT_NULL1 = 0,
	BMS_START_UP,
	BMS_SLEEP,
	BALANCE_OPEN,
	HEAT_OPEN,
	COOL_OPEN,
	
	VCELL_OVP,
	VBUS_OVP,
	CHG_OCP,
	
	VCELL_UVP,
	VBUS_UVP,
	DSG_OCP,

	CHG_UTP,
	DSG_UTP,
	CHG_OTP,
	DSG_OTP,
	VDELTA_OP,
	CBC_ERR,
	AFE1_ERR,
	AFE2_ERR,
	EEPROM_ERR,

	EVENT_NUM
}LogEventArray;


typedef union __LOG_RECORD_FLAG {
    UINT8 all;
    struct _LOG_RECORD_FLAG {
		UINT8 Log_StartUp  		:1;
		UINT8 Log_Sleep     	:1;
		UINT8 BatOvp_Third      :1;
		UINT8 BatUvp_Third      :1;
		
		UINT8 Rcv				:4;
     }bits;
}LOG_RECORD_FLAG;


extern LOG_RECORD_FLAG LogRecord_Flag;
extern UINT8 gu8_Reset_EventRecord;

//”√ÕÍ…æ≥˝
//extern UINT8 BMS_LOG_POINT;
//extern UINT8 BMS_LOG_RECORD[100][2];


void App_LogRecord(void);
void Sci_ACK_0x03_ReadRegs_EventRecord(UINT8 t_u8BuffTemp[]);
void Sci_WrReg_0x06_Reset_EventRecord(struct RS485MSG *s);
void EEPROM_ResetData_EventRecord_ToDefault(void);
void ReadEEPROM_EventRecord_Parameters(void);

#endif	/* LOG_RECORD_H */

