#include "main.h"

volatile struct SYSTEM_ERROR System_ErrFlag;
volatile union System_OnOFF_Function System_OnOFF_Func;
volatile union System_OnOFF_Function System_OnOFF_Func_StartUpRec;
volatile union System_Status SystemStatus;
volatile union System_Function_StartUp System_Func_StartUp;


void InitSystemMonitorData_EEPROM(void) {
	//ϵͳ���ܿ������ͣ�ԭ���ϣ��رչ��ܲ��ù�����ģ�ֻ��Ҫ�������ͺ�
	System_OnOFF_Func.all = 0;
    System_OnOFF_Func.bits.b1OnOFF_Balance = 1;		//��Ϊ���⻹û����ã�ȫϵ������
    System_OnOFF_Func.bits.b1OnOFF_BMS_Source = 1;
    System_OnOFF_Func.bits.b1OnOFF_MOS_Relay = 1;	//���Ĺ���
	//System_OnOFF_Func.bits.b1OnOFF_Relay_Rec = 1;
	System_OnOFF_Func.bits.b1OnOFF_AFE1 = 1;
	//System_OnOFF_Func.bits.b1OnOFF_AFE2 = 1;
    System_OnOFF_Func.bits.b1OnOFF_Sleep = 1;
	System_OnOFF_Func.bits.b1OnOFF_Heat = 0;
	System_OnOFF_Func.bits.b1OnOFF_Cool = 0;
	System_OnOFF_Func.bits.b1OnOFF_SOC_Fixed = 0;


	//ϵͳ����ʱ���������
	System_Func_StartUp.all = 0;		//��1��ԭ������Ҫ��ʼ������˼����˼�����������Ҫ��ʼ�������Բ���0
	System_Func_StartUp.bits.b1StartUpFlag_SOC = 1;
	System_Func_StartUp.bits.b1StartUpFlag_Balance = 1;
	System_Func_StartUp.bits.b1StartUpFlag_Protect = 1;
	System_Func_StartUp.bits.b1StartUpFlag_Relay = 1;		//û�ü̵�����MOS�����ֵ�Ͳ��ܾͺã���Ϊ��ʼ��������||������һ���������
	System_Func_StartUp.bits.b1StartUpFlag_MOS = 1;
	System_Func_StartUp.bits.b1StartUpFlag_ADC = 1;
	System_Func_StartUp.bits.b1StartUpFlag_CAN = 1;
	System_Func_StartUp.bits.b1StartUpFlag_Cool = 1;
	System_Func_StartUp.bits.b1StartUpFlag_Heat = 1;		//ԭ���ϲ��ã�������
	System_Func_StartUp.bits.b1StartUpFlag_BlueT = 1;		//

	//ϵͳ״̬��������
	SystemStatus.all = 0;
	//SystemStatus.bits.b1Status_SysLimits = 0;		//Ĭ�������룬������
	SystemStatus.bits.b1StartUpBMS = 1;				//MOS���߽Ӵ����ܴ���ζ�ų�ʼ�����
	SystemStatus.bits.b1Status_Relay_PRE = CLOSE;	//Ĭ���ǲ��򿪣�����CLOSE��һ���ǵ͵�ƽ
	SystemStatus.bits.b1Status_Relay_CHG = CLOSE;	//ȫ��д�������ϴ�����λ��������Ҫѡͨ�ٸ�
	SystemStatus.bits.b1Status_Relay_DSG = CLOSE;
	SystemStatus.bits.b1Status_Relay_MAIN = CLOSE;
	SystemStatus.bits.b1Status_MOS_PRE = CLOSE;
	SystemStatus.bits.b1Status_MOS_CHG = CLOSE;
	SystemStatus.bits.b1Status_MOS_DSG = CLOSE;

	//�ܸо���������ں��鷳�����������ǰ���˴��룬������رռ�������������û������
	//���������ˣ��������ص����ٴ������ִ򿪺��鷳(���ǻ�������Ҫ������)
	//���������������ι۲�һ��
	//System_OnOFF_Func.all = (UINT32)ReadEEPROM_Word_WithZone(EEPROM_ADDR_SYS_FUNC_SELECT);
	//System_OnOFF_Func.all |= ((UINT32)ReadEEPROM_Word_WithZone(EEPROM_ADDR_SYS_FUNC_SELECT + 2)<<16);		//������Ϊ32λ����λ
	
	System_OnOFF_Func_StartUpRec.all = System_OnOFF_Func.all;	//���ĳ���ܿ������򿪣�����������;�򿪣�����Ҫ��ʼ������λΪ��¼λ
	
	if(!System_OnOFF_Func.bits.b1OnOFF_Balance)		System_Func_StartUp.bits.b1StartUpFlag_Balance = 0;
	if(!System_OnOFF_Func.bits.b1OnOFF_Heat)		System_Func_StartUp.bits.b1StartUpFlag_Heat = 0;
	if(!System_OnOFF_Func.bits.b1OnOFF_Cool)		System_Func_StartUp.bits.b1StartUpFlag_Cool = 0;	
	
	if(!System_OnOFF_Func.bits.b1OnOFF_MOS_Relay) {			//û��MOS���ܣ��򲻻���ɳ�ʼ�����򿪲��ܰ�SystemStatus.bits.b1StartUpBMS�ص�ͬʱ��MOS
		System_Func_StartUp.bits.b1StartUpFlag_MOS = 0;
		System_Func_StartUp.bits.b1StartUpFlag_Relay = 0;
	}

	//ϵͳ�����������
	//���û��
}


//��������޸��ˣ�Ҫ�޸�EEPROM���ϵ��־λ
void SystemMonitorResetData_EEPROM(void) {
	//ϵͳ���ܿ�������
	System_OnOFF_Func.all = 0;
    System_OnOFF_Func.bits.b1OnOFF_Balance = 1;
    System_OnOFF_Func.bits.b1OnOFF_BMS_Source = 1;
    System_OnOFF_Func.bits.b1OnOFF_MOS_Relay = 1;	//���Ĺ���
	//System_OnOFF_Func.bits.b1OnOFF_Relay_Rec = 1;
	System_OnOFF_Func.bits.b1OnOFF_AFE1 = 1;
	//System_OnOFF_Func.bits.b1OnOFF_AFE2 = 1;
    System_OnOFF_Func.bits.b1OnOFF_Sleep = 1;
	System_OnOFF_Func.bits.b1OnOFF_Heat = 1;
	System_OnOFF_Func.bits.b1OnOFF_Cool = 1;
	System_OnOFF_Func.bits.b1OnOFF_SOC_Fixed = 0;

	WriteEEPROM_Word_WithZone(EEPROM_ADDR_SYS_FUNC_SELECT, (UINT16)(System_OnOFF_Func.all&0x0000FFFF));
	WriteEEPROM_Word_WithZone(EEPROM_ADDR_SYS_FUNC_SELECT + 2, (UINT16)(System_OnOFF_Func.all>>16));
}


UINT16 Sys_FindProtectFilterMax(void) {
	UINT8 i;
	static UINT16 s_u16ProtectFilterMax = 0;
	if(0 != s_u16ProtectFilterMax) {
		return s_u16ProtectFilterMax + 2;		//�Ƴ�20ms
	}
	for(i = 0; i < 13; ++i) {
		if(*(&PRT_E2ROMParas.u16VcellOvp_Filter + 5*i) > s_u16ProtectFilterMax) {
			s_u16ProtectFilterMax = *(&PRT_E2ROMParas.u16VcellOvp_Filter + 5*i);
		}
	}
	s_u16ProtectFilterMax = 0;
	return s_u16ProtectFilterMax + 2;			//�Ƴ�20ms
}


//ǰ���ʼ����Ӳ��3s����EEPROMҪ1s
//����ʱ����ܣ�AD�ȶ�500ms��SOC��ʼ��400ms���������ж�1s(���ֵ)���򿪹���30ms
//���������6s
StartUp_Status System_FUNC_StartUp(enum SYSTEM_FUNC_STARTUP_COMMAND SystemFunction) {
	static UINT16 s_u16SysStartUpCnt_ADC = 0,s_u16SysStartUpCnt_Protect = 0,s_u16SysStartUpCnt_Mos = 0;
	static UINT16 s_u16SysStartUpCnt_Relay = 0;
	StartUp_Status result = STARTUP_CONT;		//ֱ��һ���У�ȫ�����ش���ʼ��ֵ�������Լ��ж��Ƿ����Ѿ���ʼ��
	
	switch(SystemFunction) {					//Delay����������ʼ���Ļ����ⲿ��������Ҫͬʱ���У�����return����
		case SYSTEM_FUNC_STARTUP_ADC:			//���û��ʼ�������return���ؽ�ֹ����
			if(System_Func_StartUp.bits.b1StartUpFlag_ADC) {			//AD������AFE(�¶�Ϊ��)��һ�µģ�ͬʱ����3s���ȶ���������һ���ͺ�
				if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag1) {			//ʱ��������Ƕ����Ȼ���ⲿʱ���ۺ���һ�������
					if(++s_u16SysStartUpCnt_ADC >= DELAYB10MS_500MS) { 	//�����Ҫ�۲���˲��㷨
						s_u16SysStartUpCnt_ADC = 0;						//��Ϊ1msʱ�����˲�����Ҳ��Ϊ5��
						System_Func_StartUp.bits.b1StartUpFlag_ADC = 0;
						//MCUO_DEBUG_LED2 = 1;
					}
				}
			}
			else {								//����д�������Ч��Ҳ�ߵ�һƥ
				result = STARTUP_OVER;
			}
			break;
			
		case SYSTEM_FUNC_STARTUP_SOC:
			if(System_Func_StartUp.bits.b1StartUpFlag_SOC) {
				if(!System_Func_StartUp.bits.b1StartUpFlag_ADC) {
					//System_Func_StartUp.bits.b1StartUpFlag_SOC = 0;	//���ﲻ��д��ʼ����ϣ������ִ�����ʼ����������
					result = STARTUP_OVER;								//�ǿɽ��������ǳ�ʼ���꣬����д�Ͷ���
				}
			}
			else {
				result = STARTUP_OVER;
			}
			break;
			
		case SYSTEM_FUNC_STARTUP_BALANCE:
			if(System_Func_StartUp.bits.b1StartUpFlag_Balance) {
				if(!System_Func_StartUp.bits.b1StartUpFlag_ADC) {
					System_Func_StartUp.bits.b1StartUpFlag_Balance = 0;	//�������Ҳ������ôд������ҪSOC����
				}
			}
			else {
				result = STARTUP_OVER;
			}
			break;
			
		case SYSTEM_FUNC_STARTUP_PROTECT:
			if(System_Func_StartUp.bits.b1StartUpFlag_Protect) {
				//MCUO_DEBUG_LED2 = 0;
				if(0 == System_Func_StartUp.bits.b1StartUpFlag_ADC 
					&& 0 == System_Func_StartUp.bits.b1StartUpFlag_SOC) {
					//MCUO_DEBUG_LED2 = 1;
					if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag1) {
						if(++s_u16SysStartUpCnt_Protect >= Sys_FindProtectFilterMax()) {	//�Ա�������ӳ�Ϊ��׼������ȫ��С��3s
							s_u16SysStartUpCnt_Protect = 0;									//>>���޸�Ϊֱ��Ѱ�����ֵ
							System_Func_StartUp.bits.b1StartUpFlag_Protect = 0;
						}
					}
					result = STARTUP_OVER;		//����е��ص㣬����ǲ�����ǰ���ʼ������������Ҫ��ʱ��ʼ���ۺ���һ��
				}
			}
			else {
				result = STARTUP_OVER;
			}
			break;
			
		case SYSTEM_FUNC_STARTUP_MOS:
			if(System_Func_StartUp.bits.b1StartUpFlag_MOS) {
				if(!System_Func_StartUp.bits.b1StartUpFlag_Protect) {			//����������ϣ����ܲ���MOS
					if(!System_Func_StartUp.bits.b1StartUpFlag_Cool) {			//�����Լ����
						if(!System_Func_StartUp.bits.b1StartUpFlag_Heat) {		//�����Լ����
							if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag2) {
								if(++s_u16SysStartUpCnt_Mos >= 3) {					//��ʱһ��
									s_u16SysStartUpCnt_Mos = 0;
									System_Func_StartUp.bits.b1StartUpFlag_MOS = 0;

									SystemStatus.bits.b1StartUpBMS = 0;
									
									//MCUO_DEBUG_LED2 = 0;
								}
							}
						}
					}
				}
			}
			else {
				result = STARTUP_OVER;
			}
			break;
			
		case SYSTEM_FUNC_STARTUP_RELAY:
			if(System_Func_StartUp.bits.b1StartUpFlag_Relay) {
				if(!System_Func_StartUp.bits.b1StartUpFlag_Protect) {			//����������ϣ����ܲ���MOS
					if(!System_Func_StartUp.bits.b1StartUpFlag_Cool) {			//�����Լ����
						if(!System_Func_StartUp.bits.b1StartUpFlag_Heat) {		//�����Լ����
							if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag3) {
								if(++s_u16SysStartUpCnt_Relay >= 3) {			//��ʱһ��
									s_u16SysStartUpCnt_Relay = 0;
									System_Func_StartUp.bits.b1StartUpFlag_Relay = 0;

									SystemStatus.bits.b1StartUpBMS = 0;

									//MCUO_DEBUG_LED2 = 0;
								}
							}
						}
					}
				}
			}
			else {
				result = STARTUP_OVER;
			}
			break;

		case SYSTEM_FUNC_STARTUP_CAN:
			if(System_Func_StartUp.bits.b1StartUpFlag_CAN) {
				if(0 == System_Func_StartUp.bits.b1StartUpFlag_MOS\
					|| 0 == System_Func_StartUp.bits.b1StartUpFlag_Relay) {		//���||����
					System_Func_StartUp.bits.b1StartUpFlag_CAN = 0;
				}
			}
			else {
				result = STARTUP_OVER;
			}
			break;

		case SYSTEM_FUNC_STARTUP_COOL:			//���ﲻ��д��ʼ����ϣ������ִ�����ʼ���������ܣ���SOCһ��
			if(System_Func_StartUp.bits.b1StartUpFlag_Cool) {
				result = STARTUP_OVER;
			}
			else {
				result = STARTUP_OVER;
			}
			break;

		case SYSTEM_FUNC_STARTUP_HEAT:			//���ﲻ��д��ʼ����ϣ������ִ�����ʼ���������ܣ���SOCһ��
			if(System_Func_StartUp.bits.b1StartUpFlag_Heat) {
				result = STARTUP_OVER;
			}
			else {
				result = STARTUP_OVER;
			}
			break;


		default:
			result = STARTUP_CONT;
			break;
	}
	
	return result;
}


//0:û����X���д���
UINT8 System_ERROR_UserCallback(enum SYSTEM_ERROR_COMMAND errorCode) {
	UINT8 result = 0;

	switch(errorCode) {
		case ERROR_AFE1:
			System_ErrFlag.u8ErrFlag_Com_AFE1++;
			break;
		case ERROR_AFE2:
			System_ErrFlag.u8ErrFlag_Com_AFE2++;
			break;
		case ERROR_CAN:
			System_ErrFlag.u8ErrFlag_Com_Can++;
			break;	
		case ERROR_EEPROM_COM:
			System_ErrFlag.u8ErrFlag_Com_EEPROM++;
			break;
		case ERROR_SPI:
			System_ErrFlag.u8ErrFlag_Com_SPI++;
			break;
		case ERROR_UPPER:
			System_ErrFlag.u8ErrFlag_Com_Upper++;
			break;
		case ERROR_CLIENT:
			System_ErrFlag.u8ErrFlag_Com_Client++;
			break;
		case ERROR_SCREEN:
			System_ErrFlag.u8ErrFlag_Com_Screen++;
			break;
		case ERROR_WIFI:
			System_ErrFlag.u8ErrFlag_Com_Wifi++;
			break;
		case ERROR_BLUETOOTH:
			System_ErrFlag.u8ErrFlag_Com_BlueTooth++;
			break;
		case ERROR_APP:
			System_ErrFlag.u8ErrFlag_Com_App++;
			break;
		case ERROR_CBC_CHG:
			System_ErrFlag.u8ErrFlag_CBC_CHG++;
			break;
		case ERROR_CBC_DSG:
			System_ErrFlag.u8ErrFlag_CBC_DSG++;
			break;
		case ERROR_EEPROM_STORE:
			System_ErrFlag.u8ErrFlag_Store_EEPROM++;
			break;
		case ERROR_HSE:
			System_ErrFlag.u8ErrFlag_HSE++;
			break;
		case ERROR_LSE:
			System_ErrFlag.u8ErrFlag_LSE++;
			break;
		case ERROR_VDEATLE_OVER:
			System_ErrFlag.u8ErrFlag_Vdelta_OVER++;
			break;
		case ERROR_BALANCED:
			System_ErrFlag.u8ErrFlag_Balanced++;
			break;
		case ERROR_ADC:
			System_ErrFlag.u8ErrFlag_ADC++;
			break;
		case ERROR_SOC_CAIL:
			System_ErrFlag.u8ErrFlag_SOC_Cail++;
			break;
		case ERROR_HEAT:
			System_ErrFlag.u8ErrFlag_Heat++;
			break;
		case ERROR_COOL:
			System_ErrFlag.u8ErrFlag_Cool++;
			break;
		case ERROR_TEMP_BREAK:
			System_ErrFlag.u8ErrFlag_TempBreak = 1;
			break;


		case ERROR_REMOVE_AFE1:
			System_ErrFlag.u8ErrFlag_Com_AFE1 = 0;
			break;
		case ERROR_REMOVE_AFE2:
			System_ErrFlag.u8ErrFlag_Com_AFE2 = 0;
			break;
		case ERROR_REMOVE_CAN:
			System_ErrFlag.u8ErrFlag_Com_Can = 0;
			break;	
		case ERROR_REMOVE_EEPROM_COM:
			System_ErrFlag.u8ErrFlag_Com_EEPROM = 0;
			break;
		case ERROR_REMOVE_SPI:
			System_ErrFlag.u8ErrFlag_Com_SPI = 0;
			break;
		case ERROR_REMOVE_UPPER:
			System_ErrFlag.u8ErrFlag_Com_Upper = 0;
			break;
		case ERROR_REMOVE_CLIENT:
			System_ErrFlag.u8ErrFlag_Com_Client = 0;
			break;
		case ERROR_REMOVE_SCREEN:
			System_ErrFlag.u8ErrFlag_Com_Screen = 0;
			break;
		case ERROR_REMOVE_WIFI:
			System_ErrFlag.u8ErrFlag_Com_Wifi = 0;
			break;
		case ERROR_REMOVE_BLUETOOTH:
			System_ErrFlag.u8ErrFlag_Com_BlueTooth = 0;
			break;
		case ERROR_REMOVE_APP:
			System_ErrFlag.u8ErrFlag_Com_App = 0;
			break;
		case ERROR_REMOVE_CBC_CHG:
			System_ErrFlag.u8ErrFlag_CBC_CHG = 0;
			break;
		case ERROR_REMOVE_CBC_DSG:
			System_ErrFlag.u8ErrFlag_CBC_DSG = 0;
			break;
		case ERROR_REMOVE_EEPROM_STORE:
			System_ErrFlag.u8ErrFlag_Store_EEPROM = 0;
			break;
		case ERROR_REMOVE_HSE:
			System_ErrFlag.u8ErrFlag_HSE = 0;
			break;
		case ERROR_REMOVE_LSE:
			System_ErrFlag.u8ErrFlag_LSE = 0;
			break;
		case ERROR_REMOVE_VDEATLE_OVER:
			System_ErrFlag.u8ErrFlag_Vdelta_OVER = 0;
			break;
		case ERROR_REMOVE_BALANCED:
			System_ErrFlag.u8ErrFlag_Balanced = 0;
			break;
		case ERROR_REMOVE_ADC:
			System_ErrFlag.u8ErrFlag_ADC = 0;
			break;
		case ERROR_REMOVE_HEAT:
			System_ErrFlag.u8ErrFlag_Heat = 0;
			break;
		case ERROR_REMOVE_COOL:
			System_ErrFlag.u8ErrFlag_Cool = 0;
			break;
		case ERROR_REMOVE_SOC_CAIL:
			System_ErrFlag.u8ErrFlag_SOC_Cail = 0;
			break;
		case ERROR_REMOVE_TEMP_BREAK:
			System_ErrFlag.u8ErrFlag_TempBreak = 0;
			break;

		case ERROR_STATUS_AFE1:
			result = System_ErrFlag.u8ErrFlag_Com_AFE1;
			break;
		case ERROR_STATUS_AFE2:
			result = System_ErrFlag.u8ErrFlag_Com_AFE2;
			break;
		case ERROR_STATUS_CAN:
			result = System_ErrFlag.u8ErrFlag_Com_Can;
			break;	
		case ERROR_STATUS_EEPROM_COM:
			result = System_ErrFlag.u8ErrFlag_Com_EEPROM;
			break;
		case ERROR_STATUS_SPI:
			result = System_ErrFlag.u8ErrFlag_Com_SPI;
			break;
		case ERROR_STATUS_UPPER:
			result = System_ErrFlag.u8ErrFlag_Com_Upper;
			break;
		case ERROR_STATUS_CLIENT:
			result = System_ErrFlag.u8ErrFlag_Com_Client;
			break;
		case ERROR_STATUS_SCREEN:
			result = System_ErrFlag.u8ErrFlag_Com_Screen;
			break;
		case ERROR_STATUS_WIFI:
			result = System_ErrFlag.u8ErrFlag_Com_Wifi;
			break;
		case ERROR_STATUS_BLUETOOTH:
			result = System_ErrFlag.u8ErrFlag_Com_BlueTooth;
			break;
		case ERROR_STATUS_APP:
			result = System_ErrFlag.u8ErrFlag_Com_App;
			break;
		case ERROR_STATUS_CBC_CHG:
			result = System_ErrFlag.u8ErrFlag_CBC_CHG;
			break;
		case ERROR_STATUS_CBC_DSG:
			result = System_ErrFlag.u8ErrFlag_CBC_DSG;
			break;
		case ERROR_STATUS_EEPROM_STORE:
			result = System_ErrFlag.u8ErrFlag_Store_EEPROM;
			break;
		case ERROR_STATUS_HSE:
			result = System_ErrFlag.u8ErrFlag_HSE;
			break;
		case ERROR_STATUS_LSE:
			result = System_ErrFlag.u8ErrFlag_LSE;
			break;
		case ERROR_STATUS_VDEATLE_OVER:
			result = System_ErrFlag.u8ErrFlag_Vdelta_OVER;
			break;
		case ERROR_STATUS_BALANCED:
			result = System_ErrFlag.u8ErrFlag_Balanced;
			break;
		case ERROR_STATUS_ADC:
			result = System_ErrFlag.u8ErrFlag_ADC;
			break;
		case ERROR_STATUS_SOC_CAIL:
			result = System_ErrFlag.u8ErrFlag_SOC_Cail;
			break;
		case ERROR_STATUS_HEAT:
			result = System_ErrFlag.u8ErrFlag_Heat;
			break;
		case ERROR_STATUS_COOL:
			result = System_ErrFlag.u8ErrFlag_Cool;
			break;
		case ERROR_STATUS_TEMP_BREAK:
			result = System_ErrFlag.u8ErrFlag_TempBreak;
			break;
		
		default:
			break;
	}
	
	return result;
}

