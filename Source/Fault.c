#include "main.h"

struct PRT_E2ROM_PARAS PRT_E2ROMParas;

union FAULT_FLAG_FIRST Fault_Flag_Fisrt;
union FAULT_FLAG_SECOND Fault_Flag_Second;
union FAULT_FLAG_THIRD Fault_Flag_Third;

UINT16 Fault_record_First[Record_len];
UINT16 Fault_record_Second[Record_len];
UINT16 Fault_record_Third[Record_len];
UINT16 RTC_Fault_record_Third[Record_len][6];

UINT16 Fault_record_First2[Record_len];
UINT16 Fault_record_Second2[Record_len];
UINT16 Fault_record_Third2[Record_len];

UINT8  FaultPoint_First;
UINT8  FaultPoint_Second;
UINT8  FaultPoint_Third;

UINT8  FaultPoint_First2;
UINT8  FaultPoint_Second2;
UINT8  FaultPoint_Third2;

UINT16 FaultCnt_StartUp_First = 0;
UINT16 FaultCnt_StartUp_Second = 0;
UINT16 FaultCnt_StartUp_Third = 0;

//ԭ���Ǻ��������·�ҹ��ģ����ڷֿ���OtherElement.u16Sleep_VirCur_Chg
//���ԭ������3A�Ļ����ͻ���֣�����С����������ܴ�
//����Ĭ�����������0.1A��Ҳ��0.2A����Ч
#define OTP_UTP_VirCur_Chg 1
#define OTP_UTP_VirCur_Dsg 1

void FaultWarnRecord(enum FaultFlag num);
void FaultWarnRecord2(enum FaultFlag num);
void PwrMag_Protect_Record(enum FaultFlag num);
void PwrMag_Protect_Record_StartUp(void);

void App_CellOvp_FirstCheck(void) {
    static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag1) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16VCellMax;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16VcellOvp_First;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16VcellOvp_First - 10;                //DELAYB10MS_500MS��Ϊfilter
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16VcellOvp_Filter;                  //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16VcellOvp_Filter;                  //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                          		//���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_First.bits.b1CellOvp; 	//���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_First.bits.b1CellOvp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Fisrt.bits.CellOvp_First) {
					FaultWarnRecord(CellOvp_First);
					FaultWarnRecord2(CellOvp_First);
					Fault_Flag_Fisrt.bits.CellOvp_First = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.CellOvp_First == 1) {
				Fault_Flag_Fisrt.bits.CellOvp_First = 0;
			}
        }
    }
}

void App_CellOvp_SecondCheck(void) {
    static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag1) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16VCellMax;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16VcellOvp_Second;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16VcellOvp_First;                //DELAYB10MS_500MS��Ϊfilter
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16VcellOvp_Filter;                           //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16VcellOvp_Filter;                           //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Second.bits.b1CellOvp; //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_Second.bits.b1CellOvp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Second.bits.CellOvp_Second) {
					FaultWarnRecord(CellOvp_Second);
					FaultWarnRecord2(CellOvp_Second);
					Fault_Flag_Second.bits.CellOvp_Second = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.CellOvp_Second == 1) {
				Fault_Flag_Second.bits.CellOvp_Second = 0;
			}
        }
    }
}

void App_CellOvp_ThirdCheck(void) {
    static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag1) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16VCellMax;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16VcellOvp_Third;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16VcellOvp_Rcv;                //DELAYB10MS_500MS��Ϊfilter
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16VcellOvp_Filter;                           //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16VcellOvp_Filter;                           //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Third.bits.b1CellOvp; //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_Third.bits.b1CellOvp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Third.bits.CellOvp_Third) {
					FaultWarnRecord(CellOvp_Third);
					FaultWarnRecord2(CellOvp_Third);
					Fault_Flag_Third.bits.CellOvp_Third = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.CellOvp_Third == 1) {
				Fault_Flag_Third.bits.CellOvp_Third = 0;
			}
        }
    }
}



void App_CellUvp_FirstCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag1) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16VCellMin;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16VcellUvp_First + 10;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16VcellUvp_First;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16VcellUvp_Filter;                           //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16VcellUvp_Filter;                           //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 0;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_First.bits.b1CellUvp; //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_First.bits.b1CellUvp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Fisrt.bits.CellUvp_First) {
					FaultWarnRecord(CellUvp_First);
					FaultWarnRecord2(CellUvp_First);
					Fault_Flag_Fisrt.bits.CellUvp_First = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.CellUvp_First == 1) {
				Fault_Flag_Fisrt.bits.CellUvp_First = 0;
			}
		}
    }
}

void App_CellUvp_SecondCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag1) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16VCellMin;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16VcellUvp_First;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16VcellUvp_Second;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16VcellUvp_Filter;                           //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16VcellUvp_Filter;                           //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 0;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Second.bits.b1CellUvp; //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_Second.bits.b1CellUvp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Second.bits.CellUvp_Second) {
					FaultWarnRecord(CellUvp_Second);
					FaultWarnRecord2(CellUvp_Second);
					Fault_Flag_Second.bits.CellUvp_Second = 1;
				}

			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.CellUvp_Second == 1) {
				Fault_Flag_Second.bits.CellUvp_Second = 0;

			}
		}
    }
}

void App_CellUvp_ThirdCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag1) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16VCellMin;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16VcellUvp_Rcv;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16VcellUvp_Third;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16VcellUvp_Filter;
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16VcellUvp_Filter;
        t_sPubOPUPChk.u8FlagLogic = 0;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Third.bits.b1CellUvp; //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_Third.bits.b1CellUvp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Third.bits.CellUvp_Third) {
					FaultWarnRecord(CellUvp_Third);
					FaultWarnRecord2(CellUvp_Third);
					Fault_Flag_Third.bits.CellUvp_Third = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.CellUvp_Third == 1) {
				Fault_Flag_Third.bits.CellUvp_Third = 0;
			}
		}
    }
}



void App_BatOvp_FirstCheck(void) {
    static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag2) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16VCellTotle;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16VbusOvp_First;          
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16VbusOvp_First - 10;			//Delta = 100mV
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16VbusOvp_Filter;            //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16VbusOvp_Filter;            //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_First.bits.b1BatOvp;  //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_First.bits.b1BatOvp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Fisrt.bits.BatOvp_First) {
					FaultWarnRecord(BatOvp_First);
					FaultWarnRecord2(BatOvp_First);
					Fault_Flag_Fisrt.bits.BatOvp_First = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.BatOvp_First == 1) {
				Fault_Flag_Fisrt.bits.BatOvp_First = 0;
			}
		}
    }
}


void App_BatOvp_SecondCheck(void) {
    static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag2) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16VCellTotle;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16VbusOvp_Second;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16VbusOvp_First;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16VbusOvp_Filter;            //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16VbusOvp_Filter;            //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Second.bits.b1BatOvp;  //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_Second.bits.b1BatOvp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Second.bits.BatOvp_Second) {
					FaultWarnRecord(BatOvp_Second);
					FaultWarnRecord2(BatOvp_Second);
					Fault_Flag_Second.bits.BatOvp_Second = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.BatOvp_Second == 1) {
				Fault_Flag_Second.bits.BatOvp_Second = 0;
			}
		}
    }
}

void App_BatOvp_ThirdCheck(void) {
    static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag2) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16VCellTotle;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16VbusOvp_Third;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16VbusOvp_Rcv;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16VbusOvp_Filter;            //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16VbusOvp_Filter;            //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Third.bits.b1BatOvp;  //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_Third.bits.b1BatOvp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Third.bits.BatOvp_Third) {
					FaultWarnRecord(BatOvp_Third);
					FaultWarnRecord2(BatOvp_Third);
					Fault_Flag_Third.bits.BatOvp_Third = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.BatOvp_Third == 1) {
				Fault_Flag_Third.bits.BatOvp_Third = 0;
			}
		}
    }
}


void App_BatUvp_FirstCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag2) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16VCellTotle;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16VbusUvp_First + 10;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16VbusUvp_First;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16VbusUvp_Filter;                           //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16VbusUvp_Filter;                           //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 0;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_First.bits.b1BatUvp;  //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_First.bits.b1BatUvp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Fisrt.bits.BatUvp_First) {
					FaultWarnRecord(BatUvp_First);
					FaultWarnRecord2(BatUvp_First);
					Fault_Flag_Fisrt.bits.BatUvp_First = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.BatUvp_First == 1) {
				Fault_Flag_Fisrt.bits.BatUvp_First = 0;
			}
		}
    }
}


void App_BatUvp_SecondCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag2) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16VCellTotle;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16VbusUvp_First;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16VbusUvp_Second;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16VbusUvp_Filter;                           //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16VbusUvp_Filter;                           //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 0;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Second.bits.b1BatUvp;  //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_Second.bits.b1BatUvp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Second.bits.BatUvp_Second) {
					FaultWarnRecord(BatUvp_Second);
					FaultWarnRecord2(BatUvp_Second);
					Fault_Flag_Second.bits.BatUvp_Second = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.BatUvp_Second == 1) {
				Fault_Flag_Second.bits.BatUvp_Second = 0;
			}
		}
    }
}


void App_BatUvp_ThirdCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag2) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16VCellTotle;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16VbusUvp_Rcv;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16VbusUvp_Third;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16VbusUvp_Filter;                           //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16VbusUvp_Filter;                           //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 0;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Third.bits.b1BatUvp;  //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_Third.bits.b1BatUvp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Third.bits.BatUvp_Third) {
					FaultWarnRecord(BatUvp_Third);
					FaultWarnRecord2(BatUvp_Third);
					Fault_Flag_Third.bits.BatUvp_Third = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.BatUvp_Third == 1) {
				Fault_Flag_Third.bits.BatUvp_Third = 0;
			}
		}
    }
}


void App_IchgOcp_FirstCheck(void) {
    static UINT16 s_i16TimeCnt = 0;
    //static UINT16 s_i16TimeCntClr = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag3) {
        //if(0 == g_stCellInfoReport.unMdlFault_First.bits.b1IchgOcp) {
            t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16Ichg;
            t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16IchgOcp_First;       // �����ж�
            t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16IchgOcp_First - 10;       // û�лָ��ж�
            t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
            t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16IchgOcp_Filter;     // �����ж�ʱ��500ms
            t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16IchgOcp_Filter + CurOverFaultDelay;  // û�лָ��ж�-->�лָ��ж�
            t_sPubOPUPChk.u8FlagLogic = 1;                                      // ���߼�
            t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_First.bits.b1IchgOcp;       // ���ϱ�־����ֵ

            if(App_PubOPUPChk(&t_sPubOPUPChk)) {
                g_stCellInfoReport.unMdlFault_First.bits.b1IchgOcp = t_sPubOPUPChk.u8FlagBit;   // ���ع����жϽ��
				if(t_sPubOPUPChk.u8FlagBit == 1) {
					if(0 == Fault_Flag_Fisrt.bits.IchgOcp_First) {
						FaultWarnRecord(IchgOcp_First);
						FaultWarnRecord2(IchgOcp_First);
						Fault_Flag_Fisrt.bits.IchgOcp_First = 1;
					}
				}
				if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.IchgOcp_First == 1) {
					Fault_Flag_Fisrt.bits.IchgOcp_First = 0;
				}
            }
        //}
		#ifdef _hiccup_mode
        else {
            if((++s_i16TimeCntClr) > DELAYB10MS_5S) {                             // 5S�����־λ�����ϻָ��������ô���
                s_i16TimeCntClr = 0;
                g_stCellInfoReport.unMdlFault_First.bits.b1IchgOcp = 0;
            }
        }
		#endif
    }
}


void App_IchgOcp_SecondCheck(void) {
    static UINT16 s_i16TimeCnt = 0;
    //static UINT16 s_i16TimeCntClr = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag3) {
        //if(0 == g_stCellInfoReport.unMdlFault_Second.bits.b1IchgOcp) {
            t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16Ichg;
            t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16IchgOcp_Second;		// �����ж�
            t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16IchgOcp_First;      // û�лָ��ж�
            t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
            t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16IchgOcp_Filter;   // �����ж�ʱ��500ms
            t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16IchgOcp_Filter + CurOverFaultDelay;  	// û�лָ��ж�-->�лָ��ж�
            t_sPubOPUPChk.u8FlagLogic = 1;                                  // ���߼�
            t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Second.bits.b1IchgOcp;       // ���ϱ�־����ֵ

            if(App_PubOPUPChk(&t_sPubOPUPChk)) {
                g_stCellInfoReport.unMdlFault_Second.bits.b1IchgOcp = t_sPubOPUPChk.u8FlagBit;   // ���ع����жϽ��
				if(t_sPubOPUPChk.u8FlagBit == 1) {
					if(0 == Fault_Flag_Second.bits.IchgOcp_Second) {
						FaultWarnRecord(IchgOcp_Second);
						FaultWarnRecord2(IchgOcp_Second);
						Fault_Flag_Second.bits.IchgOcp_Second = 1;
					}
				}
				if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.IchgOcp_Second == 1) {
					Fault_Flag_Second.bits.IchgOcp_Second = 0;
				}
            }
        //}
		#ifdef _hiccup_mode
        else {
            if((++s_i16TimeCntClr) > DELAYB10MS_5S) {                        // 5S�����־λ�����ϻָ��������ô���
                s_i16TimeCntClr = 0;
                g_stCellInfoReport.unMdlFault_Second.bits.b1IchgOcp = 0;
            }
        }
		#endif
    }
}


void App_IchgOcp_ThirdCheck(void) {
    static UINT16 s_i16TimeCnt = 0;
    //static UINT16 s_i16TimeCntClr = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag3) {
        //if(0 == g_stCellInfoReport.unMdlFault_Third.bits.b1IchgOcp) {
            t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16Ichg;
            t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16IchgOcp_Third;      // �����ж�
            t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16IchgOcp_Rcv;        // û�лָ��ж�
            t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
            t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16IchgOcp_Filter;   // �����ж�ʱ��500ms
            t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16IchgOcp_Filter + CurOverFaultDelay;  	// û�лָ��ж�-->�лָ��ж�
            t_sPubOPUPChk.u8FlagLogic = 1;                                  // ���߼�
            t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Third.bits.b1IchgOcp;       // ���ϱ�־����ֵ

            if(App_PubOPUPChk(&t_sPubOPUPChk)) {
                g_stCellInfoReport.unMdlFault_Third.bits.b1IchgOcp = t_sPubOPUPChk.u8FlagBit;   // ���ع����жϽ��
				if(t_sPubOPUPChk.u8FlagBit == 1) {
					if(0 == Fault_Flag_Third.bits.IchgOcp_Third) {
						FaultWarnRecord(IchgOcp_Third);
						FaultWarnRecord2(IchgOcp_Third);
						Fault_Flag_Third.bits.IchgOcp_Third = 1;
					}
				}
				if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.IchgOcp_Third == 1) {
					Fault_Flag_Third.bits.IchgOcp_Third = 0;
				}
            }
        //}
		#ifdef _hiccup_mode
        else {
            if((++s_i16TimeCntClr) > DELAYB10MS_5S) {                            // 5S�����־λ�����ϻָ��������ô���
                s_i16TimeCntClr = 0;
                g_stCellInfoReport.unMdlFault_Third.bits.b1IchgOcp = 0;
            }
        }
		#endif
    }
}


void App_IdischgOcp_FirstCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    //static UINT16 s_i16TimeCntClr = 0;
    SPUBOPUPCHK t_sPubOPUPChk;

    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag3) {
        //if(0 == g_stCellInfoReport.unMdlFault_First.bits.b1IdischgOcp) {
            t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16IDischg;
            t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16IdsgOcp_First;      // �����ж�
            t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16IdsgOcp_First - 10;	// û�лָ��ж�
            t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
            t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16IdsgOcp_Filter;  	// �����ж�ʱ��500ms
            t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16IdsgOcp_Filter + CurOverFaultDelay;  	// û�лָ��ж�-->�лָ��ж�
            t_sPubOPUPChk.u8FlagLogic = 1;                                  // ���߼�
            t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_First.bits.b1IdischgOcp;       // ���ϱ�־����ֵ

            if(App_PubOPUPChk(&t_sPubOPUPChk)) {
                g_stCellInfoReport.unMdlFault_First.bits.b1IdischgOcp = t_sPubOPUPChk.u8FlagBit;   // ���ع����жϽ��
				if(t_sPubOPUPChk.u8FlagBit == 1) {
					if(0 == Fault_Flag_Fisrt.bits.IdischgOcp_First) {
						FaultWarnRecord(IdischgOcp_First);
						FaultWarnRecord2(IdischgOcp_First);
						Fault_Flag_Fisrt.bits.IdischgOcp_First = 1;
					}
				}
				if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.IdischgOcp_First == 1) {
					Fault_Flag_Fisrt.bits.IdischgOcp_First = 0;
				}
			}
        //}
		#ifdef _hiccup_mode
        else {
            if((++s_i16TimeCntClr) > DELAYB10MS_5S) {                            // 5S�����־λ�����ϻָ��������ô���
                s_i16TimeCntClr = 0;
                g_stCellInfoReport.unMdlFault_First.bits.b1IdischgOcp = 0;
            }
        }
		#endif
    }
}

void App_IdischgOcp_SecondCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    //static UINT16 s_i16TimeCntClr = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag3) {
        //if(0 == g_stCellInfoReport.unMdlFault_Second.bits.b1IdischgOcp) {
            t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16IDischg;
            t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16IdsgOcp_Second;     // �����ж�
            t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16IdsgOcp_First;      // û�лָ��ж�
            t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
            t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16IdsgOcp_Filter;  	// �����ж�ʱ��500ms
            t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16IdsgOcp_Filter + CurOverFaultDelay;  	// û�лָ��ж�-->�лָ��ж�
            t_sPubOPUPChk.u8FlagLogic = 1;                                 	// ���߼�
            t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Second.bits.b1IdischgOcp;       // ���ϱ�־����ֵ

            if(App_PubOPUPChk(&t_sPubOPUPChk)) {
                g_stCellInfoReport.unMdlFault_Second.bits.b1IdischgOcp = t_sPubOPUPChk.u8FlagBit;   // ���ع����жϽ��
				if(t_sPubOPUPChk.u8FlagBit == 1) {
					if(0 == Fault_Flag_Second.bits.IdischgOcp_Second) {
						FaultWarnRecord(IdischgOcp_Second);
						FaultWarnRecord2(IdischgOcp_Second);
						Fault_Flag_Second.bits.IdischgOcp_Second = 1;
					}
				}
				if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.IdischgOcp_Second == 1) {
					Fault_Flag_Second.bits.IdischgOcp_Second = 0;
				}
			}
        //}
		#ifdef _hiccup_mode
        else {
            if((++s_i16TimeCntClr) > DELAYB10MS_5S) {                        // 5S�����־λ�����ϻָ��������ô���
                s_i16TimeCntClr = 0;
                g_stCellInfoReport.unMdlFault_Second.bits.b1IdischgOcp = 0;
            }
        }
		#endif
    }
}


void App_IdischgOcp_ThirdCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    //static UINT16 s_i16TimeCntClr = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag3) {
        //if(0 == g_stCellInfoReport.unMdlFault_Third.bits.b1IdischgOcp) {
            t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16IDischg;
            t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16IdsgOcp_Third;		// �����ж�
            t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16IdsgOcp_Rcv;		// û�лָ��ж�
            t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
            t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16IdsgOcp_Filter;  	//�����ж�ʱ��500ms
            t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16IdsgOcp_Filter + CurOverFaultDelay;  	//û�лָ��ж�-->�лָ��ж�
            t_sPubOPUPChk.u8FlagLogic = 1;                                  // ���߼�
            t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Third.bits.b1IdischgOcp;       // ���ϱ�־����ֵ

            if(App_PubOPUPChk(&t_sPubOPUPChk)) {
                g_stCellInfoReport.unMdlFault_Third.bits.b1IdischgOcp = t_sPubOPUPChk.u8FlagBit;   // ���ع����жϽ��
				if(t_sPubOPUPChk.u8FlagBit == 1) {
					if(0 == Fault_Flag_Third.bits.IdischgOcp_Third) {
						FaultWarnRecord(IdischgOcp_Third);
						FaultWarnRecord2(IdischgOcp_Third);
						Fault_Flag_Third.bits.IdischgOcp_Third = 1;
					}
				}
				if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.IdischgOcp_Third == 1) {
					Fault_Flag_Third.bits.IdischgOcp_Third = 0;
				}
			}
        //}
		#ifdef _hiccup_mode
        else {
            if((++s_i16TimeCntClr) > DELAYB10MS_5S) {                            // 5S�����־λ�����ϻָ��������ô���
                s_i16TimeCntClr = 0;
                g_stCellInfoReport.unMdlFault_Third.bits.b1IdischgOcp = 0;
            }
        }
		#endif
    }
}


void App_CellChgOtp_FirstCheck(void) {
    static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag4) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16TempMax;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16TChgOTp_First;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16TChgOTp_First - 10;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16TChgOTp_Filter;       //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16TChgOTp_Filter;       //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                               		//���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_First.bits.b1CellChgOtp;  //���ϱ�־����ֵ

		switch(t_sPubOPUPChk.u8FlagBit) {
			case 0:
				if(g_stCellInfoReport.u16Ichg > OTP_UTP_VirCur_Chg) {
					if(App_PubOPUPChk(&t_sPubOPUPChk)) {
						g_stCellInfoReport.unMdlFault_First.bits.b1CellChgOtp = t_sPubOPUPChk.u8FlagBit;	   //���ع����жϽ��
						if(t_sPubOPUPChk.u8FlagBit == 1) {
							if(0 == Fault_Flag_Fisrt.bits.CellChgOTp_First) {
								FaultWarnRecord(CellChgOTp_First);
								FaultWarnRecord2(CellChgOTp_First);
								Fault_Flag_Fisrt.bits.CellChgOTp_First = 1;
							}
						}
						if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.CellChgOTp_First == 1) {
							Fault_Flag_Fisrt.bits.CellChgOTp_First = 0;
						}
					}
				}
				break;

			case 1:
				if(App_PubOPUPChk(&t_sPubOPUPChk)) {
					g_stCellInfoReport.unMdlFault_First.bits.b1CellChgOtp = t_sPubOPUPChk.u8FlagBit;	   //���ع����жϽ��
					if(t_sPubOPUPChk.u8FlagBit == 1) {
						if(0 == Fault_Flag_Fisrt.bits.CellChgOTp_First) {
							FaultWarnRecord(CellChgOTp_First);
							FaultWarnRecord2(CellChgOTp_First);
							Fault_Flag_Fisrt.bits.CellChgOTp_First = 1;
						}
					}
					if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.CellChgOTp_First == 1) {
						Fault_Flag_Fisrt.bits.CellChgOTp_First = 0;
					}
				}
				break;

			default:
				break;
		}
    }
}



void App_CellChgOtp_SecondCheck(void) {
    static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag4) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16TempMax;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16TChgOTp_Second;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16TChgOTp_First;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16TChgOTp_Filter;    	//�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16TChgOTp_Filter;       //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                      //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Second.bits.b1CellChgOtp;  //���ϱ�־����ֵ


		switch(t_sPubOPUPChk.u8FlagBit) {
			case 0:
				if(g_stCellInfoReport.u16Ichg > OTP_UTP_VirCur_Chg) {
					if(App_PubOPUPChk(&t_sPubOPUPChk)) {
			            g_stCellInfoReport.unMdlFault_Second.bits.b1CellChgOtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
						if(t_sPubOPUPChk.u8FlagBit == 1) {
							if(0 == Fault_Flag_Second.bits.CellChgOTp_Second) {
								FaultWarnRecord(CellChgOTp_Second);
								FaultWarnRecord2(CellChgOTp_Second);
								Fault_Flag_Second.bits.CellChgOTp_Second = 1;
							}
						}
						if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.CellChgOTp_Second == 1) {
							Fault_Flag_Second.bits.CellChgOTp_Second = 0;
						}
					}
				}
				break;
		
			case 1:
				if(App_PubOPUPChk(&t_sPubOPUPChk)) {
		            g_stCellInfoReport.unMdlFault_Second.bits.b1CellChgOtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
					if(t_sPubOPUPChk.u8FlagBit == 1) {
						if(0 == Fault_Flag_Second.bits.CellChgOTp_Second) {
							FaultWarnRecord(CellChgOTp_Second);
							FaultWarnRecord2(CellChgOTp_Second);
							Fault_Flag_Second.bits.CellChgOTp_Second = 1;
						}
					}
					if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.CellChgOTp_Second == 1) {
						Fault_Flag_Second.bits.CellChgOTp_Second = 0;
					}
				}
				break;
		
			default:
				break;
		}
    }
}


void App_CellChgOtp_ThirdCheck(void) {
    static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag4) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16TempMax;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16TChgOTp_Third;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16TChgOTp_Rcv;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16TChgOTp_Filter;       //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16TChgOTp_Filter;       //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                  	//���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgOtp;  //���ϱ�־����ֵ

		switch(t_sPubOPUPChk.u8FlagBit) {
			case 0:
				if(g_stCellInfoReport.u16Ichg > OTP_UTP_VirCur_Chg) {
					if(App_PubOPUPChk(&t_sPubOPUPChk)) {
			            g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgOtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
						if(t_sPubOPUPChk.u8FlagBit == 1) {
							if(0 == Fault_Flag_Third.bits.CellChgOTp_Third) {
								FaultWarnRecord(CellChgOTp_Third);
								FaultWarnRecord2(CellChgOTp_Third);
								Fault_Flag_Third.bits.CellChgOTp_Third = 1;
							}
						}
						if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.CellChgOTp_Third == 1) {
							Fault_Flag_Third.bits.CellChgOTp_Third = 0;
						}
					}
				}
				break;
		
			case 1:
				if(App_PubOPUPChk(&t_sPubOPUPChk)) {
		            g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgOtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
					if(t_sPubOPUPChk.u8FlagBit == 1) {
						if(0 == Fault_Flag_Third.bits.CellChgOTp_Third) {
							FaultWarnRecord(CellChgOTp_Third);
							FaultWarnRecord2(CellChgOTp_Third);
							Fault_Flag_Third.bits.CellChgOTp_Third = 1;
						}
					}
					if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.CellChgOTp_Third == 1) {
						Fault_Flag_Third.bits.CellChgOTp_Third = 0;
					}
				}
				break;
		
			default:
				break;
		}
    }
}


void App_CellDisChgOtp_FirstCheck(void) {
    static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag4) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16TempMax;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16TdischgOTp_First;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16TdischgOTp_First - 10;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16TdischgOTp_Filter;        //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16TdischgOTp_Filter;        //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_First.bits.b1CellDischgOtp;  //���ϱ�־����ֵ

		switch(t_sPubOPUPChk.u8FlagBit) {
			case 0:
				if(g_stCellInfoReport.u16IDischg > OTP_UTP_VirCur_Dsg) {
					if(App_PubOPUPChk(&t_sPubOPUPChk)) {
			            g_stCellInfoReport.unMdlFault_First.bits.b1CellDischgOtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
						if(t_sPubOPUPChk.u8FlagBit == 1) {
							if(0 == Fault_Flag_Fisrt.bits.CellDsgOTp_First) {
								FaultWarnRecord(CellDsgOTp_First);
								FaultWarnRecord2(CellDsgOTp_First);
								Fault_Flag_Fisrt.bits.CellDsgOTp_First = 1;
							}
						}
						if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.CellDsgOTp_First == 1) {
							Fault_Flag_Fisrt.bits.CellDsgOTp_First = 0;
						}
					}
				}
				break;
		
			case 1:
				if(App_PubOPUPChk(&t_sPubOPUPChk)) {
		            g_stCellInfoReport.unMdlFault_First.bits.b1CellDischgOtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
					if(t_sPubOPUPChk.u8FlagBit == 1) {
						if(0 == Fault_Flag_Fisrt.bits.CellDsgOTp_First) {
							FaultWarnRecord(CellDsgOTp_First);
							FaultWarnRecord2(CellDsgOTp_First);
							Fault_Flag_Fisrt.bits.CellDsgOTp_First = 1;
						}
					}
					if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.CellDsgOTp_First == 1) {
						Fault_Flag_Fisrt.bits.CellDsgOTp_First = 0;
					}
				}
				break;
		
			default:
				break;
		}
    }
}


void App_CellDisChgOtp_SecondCheck(void) {
    static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag4) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16TempMax;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16TdischgOTp_Second;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16TdischgOTp_First;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16TdischgOTp_Filter;        //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16TdischgOTp_Filter;        //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Second.bits.b1CellDischgOtp;  //���ϱ�־����ֵ

		switch(t_sPubOPUPChk.u8FlagBit) {
			case 0:
				if(g_stCellInfoReport.u16IDischg > OTP_UTP_VirCur_Dsg) {
					if(App_PubOPUPChk(&t_sPubOPUPChk)) {
			            g_stCellInfoReport.unMdlFault_Second.bits.b1CellDischgOtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
						if(t_sPubOPUPChk.u8FlagBit == 1) {
							if(0 == Fault_Flag_Second.bits.CellDsgOTp_Second) {
								FaultWarnRecord(CellDsgOTp_Second);
								FaultWarnRecord2(CellDsgOTp_Second);
								Fault_Flag_Second.bits.CellDsgOTp_Second = 1;
							}
						}
						if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.CellDsgOTp_Second == 1) {
							Fault_Flag_Second.bits.CellDsgOTp_Second = 0;
						}
					}
				}
				break;
		
			case 1:
				if(App_PubOPUPChk(&t_sPubOPUPChk)) {
		            g_stCellInfoReport.unMdlFault_Second.bits.b1CellDischgOtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
					if(t_sPubOPUPChk.u8FlagBit == 1) {
						if(0 == Fault_Flag_Second.bits.CellDsgOTp_Second) {
							FaultWarnRecord(CellDsgOTp_Second);
							FaultWarnRecord2(CellDsgOTp_Second);
							Fault_Flag_Second.bits.CellDsgOTp_Second = 1;
						}
					}
					if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.CellDsgOTp_Second == 1) {
						Fault_Flag_Second.bits.CellDsgOTp_Second = 0;
					}
				}
				break;
		
			default:
				break;
		}
    }
}


void App_CellDisChgOtp_ThirdCheck(void) {
    static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag4) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16TempMax;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16TdischgOTp_Third;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16TdischgOTp_Rcv;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16TdischgOTp_Filter;        //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16TdischgOTp_Filter;        //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgOtp;  //���ϱ�־����ֵ

		switch(t_sPubOPUPChk.u8FlagBit) {
			case 0:
				if(g_stCellInfoReport.u16IDischg > OTP_UTP_VirCur_Dsg) {
					if(App_PubOPUPChk(&t_sPubOPUPChk)) {
			            g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgOtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
						if(t_sPubOPUPChk.u8FlagBit == 1) {
							if(0 == Fault_Flag_Third.bits.CellDsgOTp_Third) {
								FaultWarnRecord(CellDsgOTp_Third);
								FaultWarnRecord2(CellDsgOTp_Third);
								Fault_Flag_Third.bits.CellDsgOTp_Third = 1;
							}
						}
						if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.CellDsgOTp_Third == 1) {
							Fault_Flag_Third.bits.CellDsgOTp_Third = 0;
						}
					}
				}
				break;
		
			case 1:
				if(App_PubOPUPChk(&t_sPubOPUPChk)) {
		            g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgOtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
					if(t_sPubOPUPChk.u8FlagBit == 1) {
						if(0 == Fault_Flag_Third.bits.CellDsgOTp_Third) {
							FaultWarnRecord(CellDsgOTp_Third);
							FaultWarnRecord2(CellDsgOTp_Third);
							Fault_Flag_Third.bits.CellDsgOTp_Third = 1;
						}
					}
					if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.CellDsgOTp_Third == 1) {
						Fault_Flag_Third.bits.CellDsgOTp_Third = 0;
					}
				}
				break;
		
			default:
				break;
		}
    }
}


void App_MosOtp_FirstCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag5) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16Temperature[MOS_TEMP1];
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16TmosOTp_First;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16TmosOTp_First - 10;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16TmosOTp_Filter;           //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16TmosOTp_Filter;           //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_First.bits.b1TmosOtp; //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_First.bits.b1TmosOtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Fisrt.bits.MosOTp_First) {
					FaultWarnRecord(MosOTp_First);
					FaultWarnRecord2(MosOTp_First);
					Fault_Flag_Fisrt.bits.MosOTp_First = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.MosOTp_First == 1) {
				Fault_Flag_Fisrt.bits.MosOTp_First = 0;
			}
        }
    }
}


void App_MosOtp_SecondCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag5) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16Temperature[MOS_TEMP1];
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16TmosOTp_Second;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16TmosOTp_First;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16TmosOTp_Filter;           //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16TmosOTp_Filter;           //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Second.bits.b1TmosOtp; //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_Second.bits.b1TmosOtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Second.bits.MosOTp_Second) {
					FaultWarnRecord(MosOTp_Second);
					FaultWarnRecord2(MosOTp_Second);
					Fault_Flag_Second.bits.MosOTp_Second = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.MosOTp_Second == 1) {
				Fault_Flag_Second.bits.MosOTp_Second = 0;
			}
        }
    }
}

void App_MosOtp_ThirdCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag5) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16Temperature[MOS_TEMP1];
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16TmosOTp_Third;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16TmosOTp_Rcv;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16TmosOTp_Filter;           //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16TmosOTp_Filter;           //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Third.bits.b1TmosOtp; //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_Third.bits.b1TmosOtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Third.bits.MosOTp_Third) {
					FaultWarnRecord(MosOTp_Third);
					FaultWarnRecord2(MosOTp_Third);
					Fault_Flag_Third.bits.MosOTp_Third = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.MosOTp_Third == 1) {
				Fault_Flag_Third.bits.MosOTp_Third = 0;
			}
        }
    }
}


void App_CellChgUtp_FirstCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag5) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16TempMin;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16TchgUTp_First + 10;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16TchgUTp_First;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16TchgUTp_Filter;           //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16TchgUTp_Filter;       	//���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 0;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_First.bits.b1CellChgUtp; //���ϱ�־����ֵ

		switch(t_sPubOPUPChk.u8FlagBit) {
			case 0:
				if(g_stCellInfoReport.u16Ichg > OTP_UTP_VirCur_Chg) {
					if(App_PubOPUPChk(&t_sPubOPUPChk)) {
			            g_stCellInfoReport.unMdlFault_First.bits.b1CellChgUtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
						if(t_sPubOPUPChk.u8FlagBit == 1) {
							if(0 == Fault_Flag_Fisrt.bits.CellChgUTp_First) {
								FaultWarnRecord(CellChgUTp_First);
								FaultWarnRecord2(CellChgUTp_First);
								Fault_Flag_Fisrt.bits.CellChgUTp_First = 1;
							}

						}
						if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.CellChgUTp_First == 1) {
							Fault_Flag_Fisrt.bits.CellChgUTp_First = 0;
						}
					}
				}
				break;
		
			case 1:
				if(App_PubOPUPChk(&t_sPubOPUPChk)) {
		            g_stCellInfoReport.unMdlFault_First.bits.b1CellChgUtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
					if(t_sPubOPUPChk.u8FlagBit == 1) {
						if(0 == Fault_Flag_Fisrt.bits.CellChgUTp_First) {
							FaultWarnRecord(CellChgUTp_First);
							FaultWarnRecord2(CellChgUTp_First);
							Fault_Flag_Fisrt.bits.CellChgUTp_First = 1;
						}

					}
					if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.CellChgUTp_First == 1) {
						Fault_Flag_Fisrt.bits.CellChgUTp_First = 0;
					}
				}
				break;
		
			default:
				break;
		}
    }
}


void App_CellChgUtp_SecondCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag5) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16TempMin;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16TchgUTp_First;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16TchgUTp_Second;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16TchgUTp_Filter;           //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16TchgUTp_Filter;           //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 0;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Second.bits.b1CellChgUtp; //���ϱ�־����ֵ

		switch(t_sPubOPUPChk.u8FlagBit) {
			case 0:
				if(g_stCellInfoReport.u16Ichg > OTP_UTP_VirCur_Chg) {
					if(App_PubOPUPChk(&t_sPubOPUPChk)) {
				        g_stCellInfoReport.unMdlFault_Second.bits.b1CellChgUtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
						if(t_sPubOPUPChk.u8FlagBit == 1) {
							if(0 == Fault_Flag_Second.bits.CellChgUTp_Second) {
								FaultWarnRecord(CellChgUTp_Second);
								FaultWarnRecord2(CellChgUTp_Second);
								Fault_Flag_Second.bits.CellChgUTp_Second = 1;
							}
						}
						if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.CellChgUTp_Second == 1) {
							Fault_Flag_Second.bits.CellChgUTp_Second = 0;
						}
					}
				}
				break;
		
			case 1:
				if(App_PubOPUPChk(&t_sPubOPUPChk)) {
		            g_stCellInfoReport.unMdlFault_Second.bits.b1CellChgUtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
					if(t_sPubOPUPChk.u8FlagBit == 1) {
						if(0 == Fault_Flag_Second.bits.CellChgUTp_Second) {
							FaultWarnRecord(CellChgUTp_Second);
							FaultWarnRecord2(CellChgUTp_Second);
							Fault_Flag_Second.bits.CellChgUTp_Second = 1;
						}
					}
					if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.CellChgUTp_Second == 1) {
						Fault_Flag_Second.bits.CellChgUTp_Second = 0;
					}
				}
				break;
		
			default:
				break;
		}
    }
}


void App_CellChgUtp_ThirdCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag5) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16TempMin;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16TchgUTp_Rcv;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16TchgUTp_Third;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16TchgUTp_Filter;            //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16TchgUTp_Filter;            //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 0;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgUtp; //���ϱ�־����ֵ

		switch(t_sPubOPUPChk.u8FlagBit) {
			case 0:
				if(g_stCellInfoReport.u16Ichg > OTP_UTP_VirCur_Chg) {
					if(App_PubOPUPChk(&t_sPubOPUPChk)) {
			            g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgUtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
						if(t_sPubOPUPChk.u8FlagBit == 1) {
							if(0 == Fault_Flag_Third.bits.CellChgUTp_Third) {
								FaultWarnRecord(CellChgUTp_Third);
								FaultWarnRecord2(CellChgUTp_Third);
								Fault_Flag_Third.bits.CellChgUTp_Third = 1;
							}
						}
						if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.CellChgUTp_Third == 1) {
							Fault_Flag_Third.bits.CellChgUTp_Third = 0;
						}
					}
				}
				break;
		
			case 1:
				if(App_PubOPUPChk(&t_sPubOPUPChk)) {
		            g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgUtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
					if(t_sPubOPUPChk.u8FlagBit == 1) {
						if(0 == Fault_Flag_Third.bits.CellChgUTp_Third) {
							FaultWarnRecord(CellChgUTp_Third);
							FaultWarnRecord2(CellChgUTp_Third);
							Fault_Flag_Third.bits.CellChgUTp_Third = 1;
						}
					}
					if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.CellChgUTp_Third == 1) {
						Fault_Flag_Third.bits.CellChgUTp_Third = 0;
					}
				}
				break;
		
			default:
				break;
		}
    }
}


void App_CellDischgUtp_FirstCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;

    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag5) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16TempMin;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16TdischgUTp_First + 10;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16TdischgUTp_First;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16TdischgUTp_Filter;        //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16TdischgUTp_Filter;         //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 0;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_First.bits.b1CellDischgUtp; //���ϱ�־����ֵ
        
		switch(t_sPubOPUPChk.u8FlagBit) {
			case 0:
				if(g_stCellInfoReport.u16IDischg > OTP_UTP_VirCur_Dsg) {
					if(App_PubOPUPChk(&t_sPubOPUPChk)) {
			            g_stCellInfoReport.unMdlFault_First.bits.b1CellDischgUtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
						if(t_sPubOPUPChk.u8FlagBit == 1) {
							if(0 == Fault_Flag_Fisrt.bits.CellDsgUTp_First) {
								FaultWarnRecord(CellDsgUTp_First);
								FaultWarnRecord2(CellDsgUTp_First);
								Fault_Flag_Fisrt.bits.CellDsgUTp_First = 1;
							}
						}
						if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.CellDsgUTp_First == 1) {
							Fault_Flag_Fisrt.bits.CellDsgUTp_First = 0;
						}
			        }
				}
				break;
		
			case 1:
				if(App_PubOPUPChk(&t_sPubOPUPChk)) {
		            g_stCellInfoReport.unMdlFault_First.bits.b1CellDischgUtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
					if(t_sPubOPUPChk.u8FlagBit == 1) {
						if(0 == Fault_Flag_Fisrt.bits.CellDsgUTp_First) {
							FaultWarnRecord(CellDsgUTp_First);
							FaultWarnRecord2(CellDsgUTp_First);
							Fault_Flag_Fisrt.bits.CellDsgUTp_First = 1;
						}
					}
					if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.CellDsgUTp_First == 1) {
						Fault_Flag_Fisrt.bits.CellDsgUTp_First = 0;
					}
		        }
				break;
		
			default:
				break;
		}
    }
}


void App_CellDischgUtp_SecondCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag5) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16TempMin;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16TdischgUTp_First;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16TdischgUTp_Second;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16TdischgUTp_Filter;            //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16TdischgUTp_Filter;            //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 0;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Second.bits.b1CellDischgUtp; //���ϱ�־����ֵ

		switch(t_sPubOPUPChk.u8FlagBit) {
			case 0:
				if(g_stCellInfoReport.u16IDischg > OTP_UTP_VirCur_Dsg) {
					if(App_PubOPUPChk(&t_sPubOPUPChk)) {
			            g_stCellInfoReport.unMdlFault_Second.bits.b1CellDischgUtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
						if(t_sPubOPUPChk.u8FlagBit == 1) {
							if(0 == Fault_Flag_Second.bits.CellDsgUTp_Second) {
								FaultWarnRecord(CellDsgUTp_Second);
								FaultWarnRecord2(CellDsgUTp_Second);
								Fault_Flag_Second.bits.CellDsgUTp_Second = 1;
							}
						}
						if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.CellDsgUTp_Second == 1) {
							Fault_Flag_Second.bits.CellDsgUTp_Second = 0;
						}
			        }
				}
				break;
		
			case 1:
				 if(App_PubOPUPChk(&t_sPubOPUPChk)) {
		            g_stCellInfoReport.unMdlFault_Second.bits.b1CellDischgUtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
					if(t_sPubOPUPChk.u8FlagBit == 1) {
						if(0 == Fault_Flag_Second.bits.CellDsgUTp_Second) {
							FaultWarnRecord(CellDsgUTp_Second);
							FaultWarnRecord2(CellDsgUTp_Second);
							Fault_Flag_Second.bits.CellDsgUTp_Second = 1;
						}
					}
					if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.CellDsgUTp_Second == 1) {
						Fault_Flag_Second.bits.CellDsgUTp_Second = 0;
					}
		        }
				break;
		
			default:
				break;
		}
    }
}


void App_CellDischgUtp_ThirdCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag5) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16TempMin;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16TdischgUTp_Rcv;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16TdischgUTp_Third;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16TdischgUTp_Filter;        //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16TdischgUTp_Filter;        //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 0;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp; //���ϱ�־����ֵ

		switch(t_sPubOPUPChk.u8FlagBit) {
			case 0:
				if(g_stCellInfoReport.u16IDischg > OTP_UTP_VirCur_Dsg) {
					if(App_PubOPUPChk(&t_sPubOPUPChk)) {
			            g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
						if(t_sPubOPUPChk.u8FlagBit == 1) {
							if(0 == Fault_Flag_Third.bits.CellDsgUTp_Third) {
								FaultWarnRecord(CellDsgUTp_Third);
								FaultWarnRecord2(CellDsgUTp_Third);
								Fault_Flag_Third.bits.CellDsgUTp_Third = 1;
							}
						}
						if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.CellDsgUTp_Third == 1) {
							Fault_Flag_Third.bits.CellDsgUTp_Third = 0;
						}
			        }
				}
				break;
		
			case 1:
				if(App_PubOPUPChk(&t_sPubOPUPChk)) {
		            g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
					if(t_sPubOPUPChk.u8FlagBit == 1) {
						if(0 == Fault_Flag_Third.bits.CellDsgUTp_Third) {
							FaultWarnRecord(CellDsgUTp_Third);
							FaultWarnRecord2(CellDsgUTp_Third);
							Fault_Flag_Third.bits.CellDsgUTp_Third = 1;
						}
					}
					if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.CellDsgUTp_Third == 1) {
						Fault_Flag_Third.bits.CellDsgUTp_Third = 0;
					}
		        }
				break;
		
			default:
				break;
		}
    }
}


void App_CellSocUp_FirstCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag4) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.SocElement.u16Soc;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16SocUp_First + 2;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16SocUp_First;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16SocUp_Filter;        //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16SocUp_Filter;        //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 0;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_First.bits.b1SocLow; //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_First.bits.b1SocLow = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Fisrt.bits.CellSocUp_First) {
					FaultWarnRecord(CellSocUp_First);
					FaultWarnRecord2(CellSocUp_First);
					Fault_Flag_Fisrt.bits.CellSocUp_First = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.CellSocUp_First == 1) {
				Fault_Flag_Fisrt.bits.CellSocUp_First = 0;
			}
		}
    }
}


void App_CellSocUp_SecondCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag4) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.SocElement.u16Soc;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16SocUp_First;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16SocUp_Second;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16SocUp_Filter;        //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16SocUp_Filter;        //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 0;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Second.bits.b1SocLow; //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_Second.bits.b1SocLow = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Second.bits.CellSocUp_Second) {
					FaultWarnRecord(CellSocUp_Second);
					FaultWarnRecord2(CellSocUp_Second);
					Fault_Flag_Second.bits.CellSocUp_Second = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.CellSocUp_Second == 1) {
				Fault_Flag_Second.bits.CellSocUp_Second = 0;
			}
		}
    }
}


void App_CellSocUp_ThirdCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag4) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.SocElement.u16Soc;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16SocUp_Rcv;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16SocUp_Third;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16SocUp_Filter;        //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16SocUp_Filter;        //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 0;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Third.bits.b1SocLow; //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_Third.bits.b1SocLow = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Third.bits.CellSocUp_Third) {
					FaultWarnRecord(CellSocUp_Third);
					FaultWarnRecord2(CellSocUp_Third);
					Fault_Flag_Third.bits.CellSocUp_Third = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.CellSocUp_Third == 1) {
				Fault_Flag_Third.bits.CellSocUp_Third = 0;
			}
		}
    }
}


void App_VdeltaOp_FirstCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag5) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16VCellDelta;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16VdeltaOvp_First;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16VdeltaOvp_First - 10;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16VdeltaOvp_Filter;        //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16VdeltaOvp_Filter;        //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_First.bits.b1VcellDeltaBig; //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_First.bits.b1VcellDeltaBig = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Fisrt.bits.VdeltaOvp_First) {
					FaultWarnRecord(VdeltaOvp_First);
					FaultWarnRecord2(VdeltaOvp_First);
					Fault_Flag_Fisrt.bits.VdeltaOvp_First = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Fisrt.bits.VdeltaOvp_First == 1) {
				Fault_Flag_Fisrt.bits.VdeltaOvp_First = 0;
			}
		}
    }
}


void App_VdeltaOp_SecondCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag5) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16VCellDelta;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16VdeltaOvp_Second;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16VdeltaOvp_First;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16VdeltaOvp_Filter;        //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16VdeltaOvp_Filter;        //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Second.bits.b1VcellDeltaBig; //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_Second.bits.b1VcellDeltaBig = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Second.bits.VdeltaOvp_Second) {
					FaultWarnRecord(VdeltaOvp_Second);
					FaultWarnRecord2(VdeltaOvp_Second);
					Fault_Flag_Second.bits.VdeltaOvp_Second = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Second.bits.VdeltaOvp_Second == 1) {
				Fault_Flag_Second.bits.VdeltaOvp_Second = 0;
			}
		}
    }
}


void App_VdeltaOp_ThirdCheck(void) {
	static UINT16 s_i16TimeCnt = 0;
    SPUBOPUPCHK t_sPubOPUPChk;
    
    if(1 == g_st_SysTimeFlag.bits.b1Sys10msFlag5) {
        t_sPubOPUPChk.u16ChkVal = g_stCellInfoReport.u16VCellDelta;
        t_sPubOPUPChk.u16OPValB = PRT_E2ROMParas.u16VdeltaOvp_Third;
        t_sPubOPUPChk.u16OPValS = PRT_E2ROMParas.u16VdeltaOvp_Rcv;
        t_sPubOPUPChk.i16ChkCnt = &s_i16TimeCnt;
        t_sPubOPUPChk.u16TimeCntB = PRT_E2ROMParas.u16VdeltaOvp_Filter;        //�����ж�ʱ��500ms
        t_sPubOPUPChk.u16TimeCntS = PRT_E2ROMParas.u16VdeltaOvp_Filter;        //���ϻָ��ж�ʱ��500ms
        t_sPubOPUPChk.u8FlagLogic = 1;                                          //���߼�
        t_sPubOPUPChk.u8FlagBit = g_stCellInfoReport.unMdlFault_Third.bits.b1VcellDeltaBig; //���ϱ�־����ֵ
        
        if(App_PubOPUPChk(&t_sPubOPUPChk)) {
            g_stCellInfoReport.unMdlFault_Third.bits.b1VcellDeltaBig = t_sPubOPUPChk.u8FlagBit;       //���ع����жϽ��
			if(t_sPubOPUPChk.u8FlagBit == 1) {
				if(0 == Fault_Flag_Third.bits.VdeltaOvp_Third) {
					FaultWarnRecord(VdeltaOvp_Third);
					FaultWarnRecord2(VdeltaOvp_Third);
					System_ERROR_UserCallback(ERROR_VDEATLE_OVER);
					Fault_Flag_Third.bits.VdeltaOvp_Third = 1;
				}
			}
			if(t_sPubOPUPChk.u8FlagBit == 0 && Fault_Flag_Third.bits.VdeltaOvp_Third == 1) {
				Fault_Flag_Third.bits.VdeltaOvp_Third = 0;
			}
		}
    }
}


/*******************************************************************************
 *Function name: App_WarnCtrl()  
 *Description :  IO port state sample, filter, warning judge and treatment                                                    
 *input:         void                                 
 *global vars:   g_u16RunFlag.bit.WARN: run flag, 10ms once               
 *output:        void 
 *CALLED BY:     main()
 ******************************************************************************/
void App_WarnCtrl(void) {
	App_MosOtp_SecondCheck();
	App_MosOtp_ThirdCheck();
}


//��¼�ǰ�˳���¼��ȥ���ϴ��������µ��ڶ���
void FaultWarnRecord(enum FaultFlag num) {
	if(num >= 1 && num <= 13) {
		if(FaultPoint_First >= Record_len) {
            FaultPoint_First = 0;
		}
		Fault_record_First[FaultPoint_First++] = num;
	}
	else if(num >= 14 && num <= 26) {
		if(FaultPoint_Second >= Record_len) {
            FaultPoint_Second = 0;
		}
		Fault_record_Second[FaultPoint_Second++] = num;
	}
	else {
	    if(FaultPoint_Third >= Record_len) {
            FaultPoint_Third = 0;
		}
		RTC_Fault_record_Third[FaultPoint_Third][0] = RTC_time.RTC_Time_Year;
		RTC_Fault_record_Third[FaultPoint_Third][1] = RTC_time.RTC_Time_Month;
		RTC_Fault_record_Third[FaultPoint_Third][2] = RTC_time.RTC_Time_Day;
		RTC_Fault_record_Third[FaultPoint_Third][3] = RTC_time.RTC_Time_Hour;
		RTC_Fault_record_Third[FaultPoint_Third][4] = RTC_time.RTC_Time_Minute;
		RTC_Fault_record_Third[FaultPoint_Third][5] = RTC_time.RTC_Time_Second;

		Fault_record_Third[FaultPoint_Third++] = num;
	}
	//#ifdef  _FAULT_RECORD
	PwrMag_Protect_Record(num);
    //#endif	
}


void FaultWarnRecord2(enum FaultFlag num) {
	if(num >= 1 && num <= 13) {
		if(FaultPoint_First2 >= Record_len) {
            FaultPoint_First2 = 0;
		}
		Fault_record_First2[FaultPoint_First2++] = num;
	}
	else if(num >= 14 && num <= 26) {
		if(FaultPoint_Second2 >= Record_len) {
            FaultPoint_Second2 = 0;
		}
		Fault_record_Second2[FaultPoint_Second2++] = num;
	}
	else {
	    if(FaultPoint_Third2 >= Record_len) {
            FaultPoint_Third2 = 0;
		}
		Fault_record_Third2[FaultPoint_Third2++] = num;
	}
}


void PwrMag_Protect_Record(enum FaultFlag num) {
	UINT8 j;

	if(SystemStatus.bits.b1StartUpBMS) {				//��������ٽ��룬����ǰ�ļ����������д
		if(num >= 1 && num <= 13) {
			++FaultCnt_StartUp_First;
		}
		else if(num >= 14 && num <= 26) {
			++FaultCnt_StartUp_Second;
		}
		else {
			++FaultCnt_StartUp_Third;
		}
	}
	else {
	    if(num >= 1 && num <= 13) {
			WriteEEPROM_Word_WithZone(E2P_ADDR_START_FR_FIRST + ((FaultPoint_First-1)<<1),Fault_record_First[FaultPoint_First -1]);
			WriteEEPROM_Word_WithZone(E2P_ADDR_E2POS_FR_TEMP_FIRST, FaultPoint_First);
		}
		else if(num >= 14 && num <= 26) {
			WriteEEPROM_Word_WithZone(E2P_ADDR_START_FR_SECOND + ((FaultPoint_Second-1)<<1),Fault_record_Second[FaultPoint_Second -1]);
			WriteEEPROM_Word_WithZone(E2P_ADDR_E2POS_FR_TEMP_SECOND, FaultPoint_Second);
		}
		else {
			WriteEEPROM_Word_WithZone(E2P_ADDR_START_FR_THIRD + ((FaultPoint_Third-1)<<1),Fault_record_Third[FaultPoint_Third -1]);
			WriteEEPROM_Word_WithZone(E2P_ADDR_E2POS_FR_TEMP_THIRD, FaultPoint_Third);
			for(j = 0; j <6; ++j) {
				WriteEEPROM_Word_WithZone(E2P_ADDR_START_FR_THIRD_RTC + (((FaultPoint_Third-1)*6+j)<<1), RTC_Fault_record_Third[FaultPoint_Third-1][j]);
			}
		}
	}
}


//�ú����ı���������տ�����ʱ�����2-3����������Ῠס��ʹʱ����ס1.5s���ң����¿���ʱ���Ӻ�
void PwrMag_Protect_Record_StartUp(void) {
	UINT8 j;
	static UINT8 su8_StartUpRecord = 0;
	if(SystemStatus.bits.b1StartUpBMS) {				//��������ٽ���
		return;
	}

	/*
	if(0 == g_st_SysTimeFlag.bits.b1Sys10msFlag3) {
		return;
	}
	*/
	
	switch(su8_StartUpRecord) {
		case 0:
			if(FaultCnt_StartUp_First) {
				//MCUO_DEBUG_LED2 = 0;
				WriteEEPROM_Word_WithZone(E2P_ADDR_START_FR_FIRST + ((FaultCnt_StartUp_First-1)<<1),Fault_record_First[FaultCnt_StartUp_First -1]);
				//MCUO_DEBUG_LED2 = 1;

				WriteEEPROM_Word_WithZone(E2P_ADDR_E2POS_FR_TEMP_FIRST, FaultCnt_StartUp_First);
				--FaultCnt_StartUp_First;
			}
			else if(FaultCnt_StartUp_Second) {
				WriteEEPROM_Word_WithZone(E2P_ADDR_START_FR_SECOND + ((FaultCnt_StartUp_Second-1)<<1),Fault_record_Second[FaultCnt_StartUp_Second -1]);
				WriteEEPROM_Word_WithZone(E2P_ADDR_E2POS_FR_TEMP_SECOND, FaultCnt_StartUp_Second);
				--FaultCnt_StartUp_Second;
			}
			else if(FaultCnt_StartUp_Third) {
				//MCUO_DEBUG_LED2 = 0;
				WriteEEPROM_Word_WithZone(E2P_ADDR_START_FR_THIRD + ((FaultCnt_StartUp_Third-1)<<1),Fault_record_Third[FaultCnt_StartUp_Third -1]);
				WriteEEPROM_Word_WithZone(E2P_ADDR_E2POS_FR_TEMP_THIRD, FaultCnt_StartUp_Third);
				for(j = 0; j <6; ++j) {
					WriteEEPROM_Word_WithZone(E2P_ADDR_START_FR_THIRD_RTC + (((FaultCnt_StartUp_Third-1)*6+j)<<1), RTC_Fault_record_Third[FaultCnt_StartUp_Third-1][j]);
				}
				//MCUO_DEBUG_LED2 = 1;
				--FaultCnt_StartUp_Third;
			}
			else {
				su8_StartUpRecord = 1;
			}
			break;
	
		case 1:
			break;
			
		default:
			break;
	}
}

