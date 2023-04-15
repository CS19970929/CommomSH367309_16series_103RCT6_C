#ifndef DATADEAL_SYS_H
#define DATADEAL_SYS_H

#define CompensateNUM 			16

#define CopperLoss_Min 			(UINT16)0
#define CopperLoss_Default 		(UINT16)0
#define CopperLoss_Max 			(UINT16)10000

#define CopperLossNum_Min 		(UINT16)0
#define CopperLossNum_Default 	(UINT16)0
#define CopperLossNum_Max 		(UINT16)32

typedef enum _CUR {
CurCHG = 0, CurDSG
}_Cur;


enum TempArray {
	AFE1_TEMP1 = 0,
	AFE1_TEMP2,
	AFE1_TEMP3,
	AFE2_TEMP1,
	AFE2_TEMP2,
	AFE2_TEMP3,
	ENV_TEMP1,
	ENV_TEMP2,
	ENV_TEMP3,
	MOS_TEMP1,
	
	TEMP_NUM
};


enum tagInfoForKBArray {
	VOLT_C1 = 0,
	VOLT_C2,
	VOLT_C3,
	VOLT_C4,
	VOLT_C5,
	VOLT_C6,
	VOLT_C7,
	VOLT_C8,
	VOLT_C9,
	VOLT_C10,
	VOLT_C11,
	VOLT_C12,
	VOLT_C13,
	VOLT_C14,
	VOLT_C15,
	VOLT_C16,
	VOLT_C17,
	VOLT_C18,
	VOLT_C19,
	VOLT_C20,
	VOLT_C21,
	VOLT_C22,
	VOLT_C23,
	VOLT_C24,
	VOLT_C25,
	VOLT_C26,
	VOLT_C27,
	VOLT_C28,
	VOLT_C29,
	VOLT_C30,
	VOLT_C31,
	VOLT_C32,
	VOLT_AFE1,
	VOLT_AFE2,
	VOLT_VBUS,

	MDL_ICHG,
	MDL_IDSG,
	MDL_TEMP1,
	MDL_TEMP2,
	MDL_TEMP3,
	MDL_TEMP4,
	MDL_TEMP5,
	MDL_TEMP6,
	MDL_TEMP_ENV1,
	MDL_TEMP_ENV2,
	MDL_TEMP_ENV3,
	MDL_TEMP_MOS1,
    KB_NUM		  // KB number_47
};


#define SYSKMAX   		((UINT16)1536)      // 1.5
#define SYSKDEFAULT		((UINT16)1024)      // 1
#define SYSKMIN   		((UINT16)512)       // 0.5

#define SYSBMAX   		((INT16)30000)      // 30
#define SYSBDEFAULT		((INT16)0)      	// 0
#define SYSBMIN   		((INT16)-30000)     // -30


struct OTHER_ELEMENT {
    UINT16 u16Balance_OpenVoltage;	//mV，均衡开启电压
    UINT16 u16Balance_OpenWindow;	//mV，均衡开启压差
    UINT16 u16Balance_CloseWindow;	//mV，均衡关闭压差
    UINT16 u16Balance_Res1;			//保留位
    UINT16 u16Balance_Res2;			//保留位
    UINT16 u16Balance_Res3;			//保留位
    UINT16 u16Balance_Res4;			//保留位
    UINT16 u16Balance_Res5;			//保留位

	UINT16 u16CS_Cur_CHGmax;		//A*10
	UINT16 u16CS_Cur_DSGmax;		//A*10
	UINT16 u16CBC_DelayT;			//us*10
	UINT16 u16CBC_Cur_DSG;			//A*10
	
	UINT16 u16Soc_TableSelect;		//原来是u16Password_Once
	UINT16 u16Password_Always;		//没用
	UINT16 u16CurLimit_Vdelta;		//mV
	UINT16 u16CurLimit_Cur;			//A*10

    UINT16 u16Sleep_VNormal;      	//mV
	UINT16 u16Sleep_TimeNormal;		//min
    UINT16 u16Sleep_Vlow;     		//mV
	UINT16 u16Sleep_TimeVlow;		//min
	UINT16 u16Sleep_VirCur_Chg;     //A *10
    UINT16 u16Sleep_VirCur_Dsg;    	//A *10
	UINT16 u16Sleep_RTC_WakeUpTime;	//min，RTC唤醒时间
	UINT16 u16Sleep_TimeRTC;		//min，进入RTC休眠时间

	UINT16 u16Soc_Ah;               //10*Ah
	UINT16 u16Soc_Cycle_times;		//循环次数*1
	UINT16 u16Soc_V_100;			//SOC为100的电压点
	UINT16 u16Soc_V_0;				//SOC为0的电压点

	UINT16 u16Sys_SeriesNum;		//N
	UINT16 u16Sys_CS_Res;			//mΩ
	UINT16 u16Sys_CS_Res_Num;		//N
	UINT16 u16Sys_PreChg_Time;		//s，预充时间
};


#define  BMS_HARDWARE_VERDION_DEFAULT   "hanstar"
#define  BMS_SOFTWARE_VERDION_DEFAULT   "a009-220326-hs037v1p0-16s"  //32
#define  BMS_SERIAL_NUMBER_DEFAULT  	"hanstar"

#define SNum 16

//#define TERNARYLI		//三元锂电池，二选一
#define LIFEPO			//磷酸铁锂电池，二选一


#define CS_Res			1
#define CS_Res_Num		4

#define CS_Cur_CHGmax	((INT32)CS_Res_Num*1250/CS_Res-10)
#define CS_Cur_DSGmax	CS_Cur_CHGmax
#define CBC_DelayT		0
#define CBC_Cur_DSG		((CS_Cur_CHGmax<<2)/5)


#define OtherElement_min		{1000,	1,		0,		0,		0,	0,	0,	0,\
	                           	 0,		0,		0,		0,\
	                           	 0,		0,		0,		0,\
	                             1000,	1,		1000,	1,		0, 	0, 	0, 	0,\
	                             1, 	1, 		0, 		0,\
	                             3,		1,		1,		0}


#ifdef TERNARYLI
#define OtherElement_default 	{3200,	30,	20,		0,	0,		0,	0,	0,\
	                             CS_Cur_CHGmax,	CS_Cur_DSGmax,CBC_DelayT,CBC_Cur_DSG,\
	                             SOC_TABLE_TERNARYLI,	0,		1000,	30,\
	                             3200,	7200,	2800,	10,		10,		10,	240,0,\
	                             2100,	3,		4200,	2900,\
	                             SNum,CS_Res,CS_Res_Num,10}
#elif (defined(LIFEPO))
#define OtherElement_default 	{3000,	30,	20,	0,	0,	0,	0,	0,\
	                             CS_Cur_CHGmax,	CS_Cur_DSGmax,CBC_DelayT,CBC_Cur_DSG,\
	                             SOC_TABLE_LIFEPO,0,1000,30,\
	                             3200,	7200,	2800,	10,		10,	10,	240,0,\
	                             2100,	3,		3650,	2600,\
	                             SNum,CS_Res,CS_Res_Num,10}
#endif


#define OtherElement_max 		{5000,2000,2000,65000,65000,65000,65000,65000,\
	                             65000,65000,65000,65000,\
	                             65000,65000,65000,65000,\
	                             5000,65000,5000,65000,50000,50000,50000,50000,\
	                             65000,50000,50000,50000,\
	                             32,65000,10000,50000}



extern UINT16 g_u16CalibCoefK[KB_NUM];
extern INT16  g_i16CalibCoefB[KB_NUM];
extern UINT16 CopperLoss[CompensateNUM];
extern UINT16 CopperLoss_Num[CompensateNUM];
extern struct OTHER_ELEMENT OtherElement;
extern UINT32 g_u32CS_Res_AFE;

void App_AFEGet(  void);

#endif	/* DATADEAL_SYS_H */

