#ifndef _SH367309_DATADEAL_H
#define _SH367309_DATADEAL_H


#define E2P_ADDR_E2POS_AFE_Parameters			950		//到974，AFE参数保存到eeprom的起始地址

#define RS485_CMD_ADDR_AFE_ROM_PARAMETERS_START 	0x2400  
#define RS485_CMD_ADDR_AFE_ROM_PARAMETERS_END 		0x2417
#define AFE_PARAMETES_TOTAL_LENGTH  				24    //单位，寄存器


#define RS485_ADDR_RW_AFE_PARAMETER  0x2400

//三元里
#ifdef TERNARYLI
/*curValue*/  /*defaultValue*/ /*maxValue*/ /*minValue*/
#define AFE_PARAMETERS_RS485_STRUCTION_DEFAULT  {\
	/*单节过压*/			4200,	4200,	5000,	1000,\
	/*单节过压恢复*/			4000,	4000,	5000,	1000,\
	/*单节过压延时*/			100,	100,	50000,	1,\
	/*单节低压*/			800,	2800,	5000,	1000,\
	/*单节低压恢复*/			2900,	2900,	5000,	1000,\
	/*单节低压延时*/			100,	100,	50000,	1,\
	/*一级充电过流*/			1200,	1200,	50000,	10,\
	/*一级充电过流延时*/100,			100,	50000,	1,\
	/*二级充电过流*/			1300,	1300,	50000,	10,\
	/*二级充电过流延时*/100,			100,	50000,	1,\
	/*一级放电过流*/			1200,	1200,	50000,	10,\
	/*一级放电过流延时*/100,			100,	50000,	1,\
	/*二级放电过流*/			1300,	1300,	50000,	10,\
	/*二级放电过流延时*/100,			100,	50000,	1,\
	/*充电高温*/			1200,	1200,	2000,	400,\
	/*充电高温恢复*/			1150,	1150,	50000,	1,\
	/*充电低温*/			410,	410,	800,	0,\
	/*充电低温恢复*/			450,	450,	50000,	1,\
	/*放电高温*/			1200,	1200,	2000,	400,\
	/*放电高温恢复*/			1150,	1150,	50000,	1,\
	/*放电低温*/			410,	410,	800,	0,\
	/*放电低温恢复*/			450,	450,	50000,	1,\
	/*短路电流*/			100,	100,	65000,	0,\
	/*短路延时*/			64,		64,		65000,	0,\
}
//磷酸铁锂
#elif (defined(LIFEPO))
/*curValue*/  /*defaultValue*/ /*maxValue*/ /*minValue*/
#define AFE_PARAMETERS_RS485_STRUCTION_DEFAULT  {\
	/*单节过压*/			3650,	3650,	5000,	1000,\
	/*单节过压恢复*/			3600,	3600,	5000,	1000,\
	/*单节过压延时*/			100,	100,	50000,	1,\
	/*单节低压*/			2500,	2500,	5000,	1000,\
	/*单节低压恢复*/			2600,	2600,	5000,	1000,\
	/*单节低压延时*/			100,	100,	50000,	1,\
	/*一级充电过流*/			1200,	1200,	50000,	10,\
	/*一级充电过流延时*/100,			100,	50000,	1,\
	/*二级充电过流*/			1300,	1300,	50000,	10,\
	/*二级充电过流延时*/100,			100,	50000,	1,\
	/*一级放电过流*/			1200,	1200,	50000,	10,\
	/*一级放电过流延时*/100,			100,	50000,	1,\
	/*二级放电过流*/			1300,	1300,	50000,	10,\
	/*二级放电过流延时*/100,			100,	50000,	1,\
	/*充电高温*/			1050,	1050,	2000,	400,\
	/*充电高温恢复*/			900,	900,	50000,	1,\
	/*充电低温*/			200,	200,	800,	0,\
	/*充电低温恢复*/			300,	300,	50000,	1,\
	/*放电高温*/			1050,	1050,	2000,	400,\
	/*放电高温恢复*/			900,	900,	50000,	1,\
	/*放电低温*/			200,	200,	800,	0,\
	/*放电低温恢复*/			300,	300,	50000,	1,\
	/*短路电流*/			100,	100,	65000,	0,\
	/*短路延时*/			64,		64,		65000,	0,\
}

#endif


//typedef  unsigned char UINT8;
//typedef  unsigned short UINT16;

/******************************* AFE寄存器结构体 *****************************/
typedef struct {
	UINT8 CN 		:4;  		//5-15，对应串数，别的为16串
	UINT8 BAL 		:1;			//0：平衡功能由内部SH367309控制，1:由外部MCU控
	UINT8 OCPM 		:1;			//0:充电过流只关闭充电MOS，放电过流只关放电MOS。1则同时关
	UINT8 ENMOS 	:1;			/*0:禁止充电MOS恢复控制位，1:启动充电MOS恢复控制位。
												(当过充电/温度保护(温度实际关2个)关闭充电MOS后，如果检测到放电状态，则开启充电MOS) */									
	UINT8 ENPCH 	:1;			//0:禁止预充电功能，1：启动预充功能
	
	UINT8 EUVR 		:1;			//0：过放保护状态释放与负载释放无关，意味着负载释放只和电流保护有关了
	UINT8 OCRA 		:1;			/*0：不允许。1：允许---“电流保护定时恢复”功能，
												也即意味着只能负载释放才能解除电流保护，不能自动恢复 */
	UINT8 CTLC 		:2;			//
	UINT8 DIS_PF 	:1;			//
	UINT8 UV_OP 	:1;			//
	UINT8 Reserve 	:1;			//
	UINT8 E0VB 		:1;			//
	
}BYTE_00H_01H_TypeDef;


typedef struct {
	UINT8 OVH 		:2;  		//过充保护前2位
	UINT8 LDRT 		:2;			//11，负载释放延时2000ms，这个和短路保护释放延时有关系
	UINT8 OVT		:4;			//过充电保护延时
	UINT8 OVL;					//过充保护后8位
}BYTE_02H_03H_TypeDef;


typedef struct {
	UINT8 OVRH 		:2;  		//过充保护恢复前2位
	UINT8 Reserve 	:2;		
	UINT8 UVT		:4;			//低压保护延时
	UINT8 OVRL;					//过充保护恢复后8位
}BYTE_04H_05H_TypeDef;

typedef struct {
	UINT8 UV;					//低压保护
	UINT8 UVR;					//低压保护恢复
}BYTE_06H_07H_TypeDef;


typedef struct {
	UINT8 BALV;					//均衡开启电压
	UINT8 PREV;					//预充电压
}BYTE_08H_09H_TypeDef;

typedef struct {
	UINT8 L0V;				//低电压禁止充电电压
	UINT8 PFV;				//二次过充电保护电压设置寄存器，放大一些，不用这个
}BYTE_0AH_0BH_TypeDef;


typedef struct {
	UINT8 OCD1T		:4;			//放电过流1保护延时
	UINT8 OCD1V		:4;			//放电过流保护1保护电压
	UINT8 OCD2T		:4;			//放电过流2保护延时
	UINT8 OCD2V		:4;			//放电过流保护2保护电压
}BYTE_0CH_0DH_TypeDef;

typedef struct {
	UINT8 SCT		:4;			//短路保护延时
	UINT8 SCV		:4;			//短路保护电压
	UINT8 OCCT		:4;			//充电过流保护延时
	UINT8 OCCV		:4;			//充电过流保护电压
}BYTE_0EH_0FH_TypeDef;

typedef struct {
	UINT8 PFT		:2;			//短路保护延时
	UINT8 OCRT		:2;			//短路保护电压
	UINT8 MOST		:2;			//充电过流保护延时
	UINT8 CHS		:2;			//充电过流保护电压
}BYTE_10H_TypeDef;

typedef struct {
	UINT8 OTC;					//充电高温保护
	UINT8 OTCR;					//充电高温保护恢复
	UINT8 UTC;					//充电低温保护
	UINT8 UTCR;					//充电低温保护恢复
	UINT8 OTD;					//放电高温保护
	UINT8 OTDR;					//放电高温保护恢复
	UINT8 UTD;					//放电低温保护
	UINT8 UTDR;					//放电低温保护恢复
	UINT8 TR;
}BYTE_11H_19H_TypeDef;


typedef	struct {
	BYTE_00H_01H_TypeDef 	m00H_01H;
	BYTE_02H_03H_TypeDef	m02H_03H;
	BYTE_04H_05H_TypeDef	m04H_05H;
	BYTE_06H_07H_TypeDef 	m06H_07H;
	BYTE_08H_09H_TypeDef 	m08H_09H;
	BYTE_0AH_0BH_TypeDef 	m0AH_0BH;
	BYTE_0CH_0DH_TypeDef	m0CH_0DH;
	BYTE_0EH_0FH_TypeDef 	m0EH_0FH;
	BYTE_10H_TypeDef 			m10H;
	BYTE_11H_19H_TypeDef	m11H_19H;
}AFE_ROM_PARAMETERS_TypeDef;



/******************************* AFE保护参数结构体 *****************************/
typedef struct {
	UINT16 curValue;			//当前值
	UINT16 defaultValue;		//默认值
	UINT16 maxValue;			//最大值
	UINT16 minValue;			//最小值
}AFE_Value_Typedef;

typedef struct{
	AFE_Value_Typedef	u16VcellOvp;  		//单节过压 mv
	AFE_Value_Typedef	u16VcellOvp_Rcv;	//过压恢复 mv
	AFE_Value_Typedef	u16VcellOvp_Filter;	//过压延时 10ms
	
	AFE_Value_Typedef	u16VcellUvp;		//单节低压
	AFE_Value_Typedef	u16VcellUvp_Rcv;
	AFE_Value_Typedef	u16VcellUvp_Filter;
	
	AFE_Value_Typedef	u16IchgOcp_First;	//一级充电过流 A*10
	AFE_Value_Typedef	u16IchgOcp_Filter_First;
	
	AFE_Value_Typedef	u16IchgOcp_Second;	//二级充电过流
	AFE_Value_Typedef	u16IchgOcp_Filter_Second;
	
	AFE_Value_Typedef	u16IdsgOcp_First;	//一级放电过流
	AFE_Value_Typedef	u16IdsgOcp_Filter_First;
	
	AFE_Value_Typedef	u16IdsgOcp_Second;	//二级放电过流
	AFE_Value_Typedef	u16IdsgOcp_Filter_Second;
	
	AFE_Value_Typedef	u16TChgOTp;			//充电高温 (℃*10+400)
	AFE_Value_Typedef	u16TChgOTp_Rcv;
	AFE_Value_Typedef	u16TchgUTp;			//充电低温
	AFE_Value_Typedef	u16TchgUTp_Rcv;
	AFE_Value_Typedef	u16TdischgOTp;		//放电高温
	AFE_Value_Typedef	u16TdischgOTp_Rcv;
	AFE_Value_Typedef	u16TdischgUTp;		//放电低温
	AFE_Value_Typedef	u16TdischgUTp_Rcv;
	AFE_Value_Typedef 	u16CBC_Cur_DSG;
	AFE_Value_Typedef 	u16CBC_DelayT;
}AFE_Parameters_RS485_Typedef; 



void App_SH367309_Supplement(void);

void Sci_ACK_0x03_RW_AFE_Parameters(struct RS485MSG *s,UINT8 t_u8BuffTemp[]);
UINT8 Sci_WrRegs_0x10_AFE_Parameters(UINT16 u16Channel,struct RS485MSG *s);
void Sci_WrReg_0x06_Reset_AFE_Parameters(struct RS485MSG *s);

void EEPROM_ResetData_AFE_ParametersToDefault(void);
void ReadEEPROM_AFE_Parameters(void);


extern int AFE_PARAM_WRITE_Flag;
extern int AFE_ResetFlag;
extern AFE_Parameters_RS485_Typedef  AFE_Parameters_RS485_Struction;

#endif

