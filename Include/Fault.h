#ifndef FAULT_H
#define FAULT_H

#define CurOverFaultDelay 3000		//30s��10msʱ��

enum FaultFlag {
	CellOvp_First = 1,
	CellUvp_First,
	BatOvp_First,
	BatUvp_First,
	IchgOcp_First,
	IdischgOcp_First,
	CellChgOTp_First,
	CellChgUTp_First,
	CellDsgOTp_First,
	CellDsgUTp_First,
	MosOTp_First,
	VdeltaOvp_First,
	CellSocUp_First,
	
	CellOvp_Second,
	CellUvp_Second,
	BatOvp_Second,
	BatUvp_Second,
	IchgOcp_Second,
	IdischgOcp_Second,
	CellChgOTp_Second,
	CellChgUTp_Second,
	CellDsgOTp_Second,
	CellDsgUTp_Second,
	MosOTp_Second,
	VdeltaOvp_Second,
	CellSocUp_Second,

	CellOvp_Third,
	CellUvp_Third,
	BatOvp_Third,
	BatUvp_Third,
	IchgOcp_Third,
	IdischgOcp_Third,
	CellChgOTp_Third,
	CellChgUTp_Third,
	CellDsgOTp_Third,
	CellDsgUTp_Third,
	MosOTp_Third,
	VdeltaOvp_Third,
	CellSocUp_Third
};


union FAULT_FLAG_FIRST {
    UINT16 all;
    struct Fault_Flag_First {
		UINT8 CellOvp_First     :1;
		UINT8 CellUvp_First     :1;
		UINT8 BatOvp_First      :1;
		UINT8 BatUvp_First      :1;
		
		UINT8 IchgOcp_First  	:1;
		UINT8 IdischgOcp_First  :1;
		UINT8 CellChgOTp_First  :1;
		UINT8 CellChgUTp_First  :1;
		
		UINT8 CellDsgOTp_First  :1;
		UINT8 CellDsgUTp_First  :1;
		UINT8 MosOTp_First		:1;
		UINT8 VdeltaOvp_First	:1;
		
		UINT8 CellSocUp_First	:1;
		UINT8 Rcv				:3;
     }bits;	
};

union FAULT_FLAG_SECOND {
    UINT16 all;
    struct Fault_Flag_Second {
		UINT8 CellOvp_Second    :1;
		UINT8 CellUvp_Second    :1;
		UINT8 BatOvp_Second     :1;
		UINT8 BatUvp_Second     :1;
		
		UINT8 IchgOcp_Second    :1;
		UINT8 IdischgOcp_Second :1;
		UINT8 CellChgOTp_Second :1;
		UINT8 CellDsgOTp_Second :1;
		
		UINT8 CellChgUTp_Second :1;
		UINT8 CellDsgUTp_Second :1;
		UINT8 MosOTp_Second     :1;
		UINT8 VdeltaOvp_Second	:1;
		
		UINT8 CellSocUp_Second	:1;
		UINT8 Rcv				:3;
     }bits;	
};

union FAULT_FLAG_THIRD {
    UINT16 all;
    struct Fault_Flag_Third {
		UINT8 CellOvp_Third     :1;
		UINT8 CellUvp_Third     :1;
		UINT8 BatOvp_Third      :1;
		UINT8 BatUvp_Third      :1;
		
		UINT8 IchgOcp_Third     :1;
		UINT8 IdischgOcp_Third  :1;
		UINT8 CellChgOTp_Third  :1;
		UINT8 CellChgUTp_Third  :1;
		
		UINT8 CellDsgOTp_Third  :1;
		UINT8 CellDsgUTp_Third  :1;
		UINT8 MosOTp_Third      :1;
		UINT8 VdeltaOvp_Third   :1;
		
		UINT8 CellSocUp_Third   :1;
		UINT8 Rcv				:3;
     }bits;
};


struct PRT_E2ROM_PARAS {
//--------------parameters store sequence and its address allocation-----------
	UINT16	u16VcellOvp_First;
	UINT16	u16VcellOvp_Second;
	UINT16	u16VcellOvp_Third;
	UINT16	u16VcellOvp_Rcv;
	UINT16	u16VcellOvp_Filter;

	UINT16	u16VcellUvp_First;
	UINT16	u16VcellUvp_Second;
	UINT16	u16VcellUvp_Third;
	UINT16	u16VcellUvp_Rcv;
	UINT16	u16VcellUvp_Filter;
	
	UINT16	u16VbusOvp_First;
	UINT16	u16VbusOvp_Second;
	UINT16	u16VbusOvp_Third;
	UINT16	u16VbusOvp_Rcv;
	UINT16	u16VbusOvp_Filter;

	UINT16	u16VbusUvp_First;
	UINT16	u16VbusUvp_Second;
	UINT16	u16VbusUvp_Third;
	UINT16	u16VbusUvp_Rcv;	
	UINT16	u16VbusUvp_Filter;

	UINT16	u16IchgOcp_First;
	UINT16	u16IchgOcp_Second;
	UINT16	u16IchgOcp_Third;
	UINT16	u16IchgOcp_Rcv;
	UINT16	u16IchgOcp_Filter;

	UINT16	u16IdsgOcp_First;
	UINT16	u16IdsgOcp_Second;
	UINT16	u16IdsgOcp_Third;
	UINT16	u16IdsgOcp_Rcv;
	UINT16	u16IdsgOcp_Filter;
	
	UINT16	u16TChgOTp_First;
	UINT16	u16TChgOTp_Second;
	UINT16	u16TChgOTp_Third;
	UINT16	u16TChgOTp_Rcv;
	UINT16	u16TChgOTp_Filter;

	UINT16	u16TchgUTp_First;
	UINT16	u16TchgUTp_Second;
	UINT16	u16TchgUTp_Third;
	UINT16	u16TchgUTp_Rcv;
	UINT16	u16TchgUTp_Filter;

	UINT16	u16TdischgOTp_First;
	UINT16	u16TdischgOTp_Second;
	UINT16	u16TdischgOTp_Third;
	UINT16	u16TdischgOTp_Rcv;
	UINT16	u16TdischgOTp_Filter;

	UINT16	u16TdischgUTp_First;
	UINT16	u16TdischgUTp_Second;
	UINT16	u16TdischgUTp_Third;
	UINT16	u16TdischgUTp_Rcv;
	UINT16  u16TdischgUTp_Filter;

	UINT16	u16TmosOTp_First;
	UINT16	u16TmosOTp_Second;
	UINT16	u16TmosOTp_Third;
	UINT16	u16TmosOTp_Rcv;
	UINT16	u16TmosOTp_Filter;

	UINT16	u16VdeltaOvp_First;
	UINT16	u16VdeltaOvp_Second;
	UINT16	u16VdeltaOvp_Third;
	UINT16	u16VdeltaOvp_Rcv;
	UINT16	u16VdeltaOvp_Filter;

	UINT16	u16SocUp_First;
	UINT16	u16SocUp_Second;
	UINT16	u16SocUp_Third;
	UINT16	u16SocUp_Rcv;
	UINT16	u16SocUp_Filter;
};


#define E2P_PROTECT_MIN_PRT		{/*���ڹ�ѹ*/1000,	1000,	1000,	1000,	1,\
								 /*���ڵ�ѹ*/1000,	1000,	1000,	1000,	1,\
								 /*��ѹ��ѹ*/300,	300,	300,	300,	1,\
								 /*��ѹ��ѹ*/300,	300,	300,	300,	1,\
		        				 /*������*/10,	10,		10,		10,		1,\
		        				 /*�ŵ����*/10,	10,		10,		10,		1,\
								 /*������*/400,	400,	400,	400,	1,\
								 /*������*/0,		0,		0,		0,		1,\
								 /*�ŵ����*/400,	400,	400,	400,	1,\
								 /*�ŵ����*/0,		0,		0,		0,		1,\
		        				 /*��������*/400,	400,	400,	400,	1,\
		        				 /*ѹ�����*/10,	10,		10,		10,		1,\
		        				 /*��������*/0,		0,		0,		0,		1}

//��Ԫ��
#ifdef TERNARYLI
#define E2P_PROTECT_DEFAULT_PRT	{/*���ڹ�ѹ*/4200,	4200,	4200,	4000,	100,\
								 /*���ڵ�ѹ*/2800,	2800,	2800,	2900,	100,\
								 /*��ѹ��ѹ*/420*SNum, 420*SNum, 420*SNum, 400*SNum, 100,\
								 /*��ѹ��ѹ*/280*SNum, 280*SNum, 280*SNum, 290*SNum, 100,\
		        				 /*������*/1200,	1200,	1200,	1000,	100,\
		        				 /*�ŵ����*/1200,	1200,	1200,	1000,	100,\
								 /*������*/1200,	1200,	1200,	1150,	100,\
								 /*������*/410,	410,	410,	450,	100,\
								 /*�ŵ����*/1200,	1200,	1200,	1150,	100,\
								 /*�ŵ����*/410,	410,	410,	450,	100,\
		        				 /*��������*/1200,	1200,	1200,	1150,	100,\
		        				 /*ѹ�����*/1000,	1000,	1000,	900,	100,\
		        				 /*��������*/3,		2,		1,		2,		100}

//�������
#elif (defined(LIFEPO))
#define E2P_PROTECT_DEFAULT_PRT	{/*���ڹ�ѹ*/3550,	3550,	3650,	3500,	100,\
								 /*���ڵ�ѹ*/2500,	2500,	2500,	2600,	100,\
								 /*��ѹ��ѹ*/355*SNum, 355*SNum,	365*SNum, 350*SNum, 100,\
								 /*��ѹ��ѹ*/250*SNum, 250*SNum,	250*SNum, 260*SNum, 100,\
		        				 /*������*/1200,	1200,	1200,	1000,	100,\
		        				 /*�ŵ����*/1200,	1200,	1200,	1000,	100,\
								 /*������*/900,	1000,	1050,	900,	100,\
								 /*������*/500,	450,	400,	450,	100,\
								 /*�ŵ����*/900,	1000,	1050,	900,	100,\
								 /*�ŵ����*/300,	250,	200,	300,	100,\
		        				 /*��������*/1200,	1200,	1200,	1000,	100,\
		        				 /*ѹ�����*/1000,	1000,	1000,	900,	100,\
		        				 /*��������*/3,		2,		1,		2,		100}
#endif




#define E2P_PROTECT_MAX_PRT		{/*���ڹ�ѹ*/5000,	5000,	5000,	5000,	50000,\
								 /*���ڵ�ѹ*/5000,	5000,	5000,	5000,	50000,\
								 /*��ѹ��ѹ*/20000,	20000,	20000,	20000,	50000,\
								 /*��ѹ��ѹ*/20000,	20000,	20000,	20000,	50000,\
		        				 /*������*/50000,	50000,	50000,	50000,	50000,\
		        				 /*�ŵ����*/50000,	50000,	50000,	50000,	50000,\
								 /*������*/2000,	2000,	2000,	2000,	50000,\
								 /*������*/800,	800,	800,	800,	50000,\
								 /*�ŵ����*/2000,	2000,	2000,	2000,	50000,\
								 /*�ŵ����*/800,	800,	800,	800,	50000,\
		        				 /*��������*/2000,	2000,	2000,	2000,	50000,\
		        				 /*ѹ�����*/2000,	2000,	2000,	2000,	50000,\
		        				 /*��������*/50,	50,		50,		50,		50000}




#define Record_len 10

extern struct PRT_E2ROM_PARAS PRT_E2ROMParas;
extern union FAULT_FLAG_FIRST Fault_Flag_Fisrt;
extern union FAULT_FLAG_SECOND Fault_Flag_Second;
extern union FAULT_FLAG_THIRD Fault_Flag_Third;

extern UINT16 Fault_record_First[Record_len];
extern UINT16 Fault_record_Second[Record_len];
extern UINT16 Fault_record_Third[Record_len];
extern UINT16 RTC_Fault_record_Third[Record_len][6];

extern UINT16 Fault_record_First2[Record_len];
extern UINT16 Fault_record_Second2[Record_len];
extern UINT16 Fault_record_Third2[Record_len];

extern UINT8  FaultPoint_First;
extern UINT8  FaultPoint_Second;
extern UINT8  FaultPoint_Third;
extern UINT8  FaultPoint_First2;
extern UINT8  FaultPoint_Second2;
extern UINT8  FaultPoint_Third2;

extern UINT16 FaultCnt_StartUp_First;
extern UINT16 FaultCnt_StartUp_Second;
extern UINT16 FaultCnt_StartUp_Third;

void App_WarnCtrl(void);
void FaultWarnRecord2(enum FaultFlag num);

#endif	/* FAULT_H */

