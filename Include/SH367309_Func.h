#ifndef SH367309_FUNC_H
#define SH367309_FUNC_H

typedef enum GPIO_TYPE {
	GPIO_PreCHG = 0,
	GPIO_CHG,
	GPIO_DSG,
	GPIO_MAIN,
}GPIO_Type;


#ifdef TERNARYLI
#define VAL_CELL_OVP			((UINT16)4205)	//单位mV	
#define VAL_CELL_OVP_REC		((UINT16)4010)	//单位mV，过充保护恢复
#define VAL_CELL_UVP			((UINT16)2800)	//单位mV	
#define VAL_CELL_UVP_REC		((UINT16)2900)	//单位mV，低压保护恢复
#define VAL_BAL_OPEN			((UINT16)5000)	//单位mV，均衡开启电压

#elif (defined(LIFEPO))
#define VAL_CELL_OVP			((UINT16)3750)	//单位mV	
#define VAL_CELL_OVP_REC		((UINT16)3650)	//单位mV，过充保护恢复
#define VAL_CELL_UVP			((UINT16)2500)	//单位mV	
#define VAL_CELL_UVP_REC		((UINT16)2600)	//单位mV，低压保护恢复
#define VAL_BAL_OPEN			((UINT16)5000)	//单位mV，均衡开启电压

#endif

/*
摄氏度
充电高温保护------->70
充电高温保护恢复--->65
充电低温保护------->-5
充电低温保护恢复--->0
放电高温保护------->70
放电高温保护恢复--->65
放电低温保护------->-5
放电低温保护恢复--->0
//因为没有TR的值，所以先换算为(X + 40)
//要求低温保护类型一定要小于25摄氏度，不然计算有问题.
//由于只有8位存储数据，其-256处理。如果大于25摄氏度，计算结果会负数(Rt/(Rs+Rr)-0.5)
*/
#define VAL_CHG_OTP				((UINT8)110)
#define VAL_CHG_OTP_RCV			((UINT8)105)
#define VAL_CHG_UTP				((UINT8)35)
#define VAL_CHG_UTP_RCV			((UINT8)40)
#define VAL_DSG_OTP				((UINT8)110)
#define VAL_DSG_OTP_RCV			((UINT8)105)
#define VAL_DSG_UTP				((UINT8)35)
#define VAL_DSG_UTP_RCV			((UINT8)40)


#define BIT_ENPCH	(0<<7)		//0:禁止预充电功能，1：启动预充功能
#define BIT_ENMOS	(1<<6)		//0:禁止充电MOS恢复，1:启动充电MOS恢复控制位。
								//当过充电/温度保护(温度实际关2个)关闭充电MOS后，如果检测到放电状态，则开启充电MOS
								//用这个吧，MOS应该会没那么热。
#define BIT_OCPM	(0<<5)		//0:充电过流只关闭充电MOS，放电过流只关放电MOS。1则同时关
#define BIT_BAL		(1<<4)		//0：平衡功能由内部SH367309控制，1:由外部MCU控
#define BIT0_3_CN	(0)			//5-15，对应串数，别的为16串
#define BYTE_00H_SCONF1			BIT_ENPCH|BIT_ENMOS|BIT_OCPM|BIT_BAL|BIT0_3_CN

#define BIT_E0VB	(0<<7)		//0：关闭。1：开启---“禁止低压电芯充电”功能
#define BIT_UV_OP	(0<<5)		//0：过放只关闭放电MOS。1：过放关闭充放电MOS
#define BIT_DIS_PF	(1<<4)		//0：开启。1：禁止---二次过充电保护，同时也是断线检测功能(奇怪了，怎么揉在一起了)
								//这个标志位为0则开启，会出现问题，重新断电上电容易出现全部电压为10000mV左右，所以禁止掉(出问题PF引脚会输出VSS电平)
#define BIT2_3_CTLC	(2<<2)		//MOS由内部逻辑控制，CTL引脚无效，具体看规格书
								//2，设置为控制放电MOS，预放功能
#define BIT_OCRA	(0<<1)		//0：不允许。1：允许---“电流保护定时恢复”功能，也即意味着只能负载释放才能解除电流保护，不能自动恢复
#define BIT_EUVR	(0)			//0：过放保护状态释放与负载释放无关，意味着负载释放只和电流保护有关了
								//过放保护还必须负载释放才能解除。
								//先改为0测试，后面改回1，如果不关充电管，P+永远检测到一个高电平，负载移除没法检测到
								//改回0，测试没问题
#define BYTE_01H_SCONF2			BIT_E0VB|BIT_UV_OP|BIT_DIS_PF|BIT2_3_CTLC|BIT_OCRA|BIT_EUVR


//以下电压的大小要求
//PFV->OV->OVR->UVR->UV->Vpd->PREV->L0V
#define BIT4_7_OVT				(6<<4)		//0110，过充电保护延时1s
#define BIT2_3_LDRT				(11<<2)		//11，负载释放延时2000ms，这个和短路保护释放延时有关系
#define BIT0_1_OVH				(UINT8)((VAL_CELL_OVP/5)>>8)			//过充保护前2位
#define BYTE_02H_OVT_LDRT_OVH	BIT4_7_OVT|BIT2_3_LDRT|BIT0_1_OVH

#define BIT0_7_OVL				(UINT8)((VAL_CELL_OVP/5)&0x00FF)		//过充保护后8位
#define BYTE_03H_OVL			BIT0_7_OVL

#define BIT4_7_UVT				(6<<4)									//0110，过放保护延时1s
#define BIT0_1_OVR				(UINT8)((VAL_CELL_OVP_REC/5)>>8)		//过充保护恢复前2位
#define BYTE_04H_UVT_OVRH		BIT4_7_UVT|BIT0_1_OVR

#define BIT0_7_OVR				(UINT8)((VAL_CELL_OVP_REC/5)&0x00FF)	//过充保护恢复后8位
#define BYTE_05H_OVRL			BIT0_7_OVR

#define BIT0_7_UV				(UINT8)((VAL_CELL_UVP/20)&0x00FF)		//低压保护
#define BYTE_06H_UV				BIT0_7_UV

#define BIT0_7_UVR				(UINT8)((VAL_CELL_UVP_REC/20)&0x00FF)	//低压保护恢复
#define BYTE_07H_UVR			BIT0_7_UVR

#define BIT0_7_BALV 			(UINT8)((VAL_BAL_OPEN/20)&0x00FF)		//均衡开启电压
#define BYTE_08H_BALV			BIT0_7_BALV

#define BIT0_7_PREV				(UINT8)((2000/20)&0x00FF)				//预充电压
#define BYTE_09H_PREV			BIT0_7_PREV

#define BIT0_7_L0V				(UINT8)(1000/20)				//低电压禁止充电电压
#define BYTE_0AH_L0V			BIT0_7_L0V

#define BIT0_7_PFV				(UINT8)(5000/20)				//二次过充电保护电压设置寄存器，放大一些，不用这个
#define BYTE_0BH_PFV			BIT0_7_PFV



#define BIT4_7_OCD1V			(0<<4)		//1000，放电过流保护1保护电压=100mV，0000为20mV
#define BIT0_3_OCD1T			(6)			//0110，放电过流1保护延时1s
#define BYTE_0CH_OCD1V_OCD1T	BIT4_7_OCD1V|BIT0_3_OCD1T

#define BIT4_7_OCD2V			(15<<4)		//1000，放电过流保护1保护电压=120mV，0000为30mV，弄成最大
#define BIT0_3_OCD2T			(6)			//0110，放电过流1保护延时200ms
#define BYTE_0DH_OCD2V_OCD2T	BIT4_7_OCD2V|BIT0_3_OCD2T

#define BIT4_7_SCV				(0<<4)		//1000，短路保护电压=290mV,0010为110mV
#define BIT0_3_SCT				(1)			//0001，短路保护延时64us
#define BYTE_0EH_SCV_SCT		BIT4_7_SCV|BIT0_3_SCT

#define BIT4_7_OCCV				(0<<4)		//1000，充电过流保护电压=100mV,0000为20mV
#define BIT0_3_OCCT				(10)		//1010，充电过流保护延时1s
#define BYTE_0FH_OCCV_OCCT		BIT4_7_OCCV|BIT0_3_OCCT

#define BIT6_7_CHS				(0<<6)		//充放电状态监测电压=200uV，就是BSTATUS3的CHGING和DSGING位的置位的界限
#define BIT4_5_MOST				(0<<4)		//充放电MOSFET开启延时=64us
#define BIT2_3_OCRT				(2<<2)		//充放电过流自动回复延时=32s
#define BIT0_1_PFT				(0)			//二次过充保护延时=9s
#define BYTE_10H_MOST_OCRT_PFT	BIT6_7_CHS|BIT4_5_MOST|BIT2_3_OCRT|BIT0_1_PFT


/*
摄氏度
充电高温保护------->
充电高温保护恢复--->
充电低温保护------->
充电低温保护恢复--->
放电高温保护------->
放电高温保护恢复--->
放电低温保护------->
放电低温保护恢复--->
//因为没有TR的值，所以先换算为(X + 40)
*/
#define BYTE_11H_OTC			VAL_CHG_OTP
#define BYTE_12H_OTCR			VAL_CHG_OTP_RCV
#define BYTE_13H_UTC			VAL_CHG_UTP
#define BYTE_14H_UTCR			VAL_CHG_UTP_RCV
#define BYTE_15H_OTD			VAL_DSG_OTP
#define BYTE_16H_OTDR			VAL_DSG_OTP_RCV
#define BYTE_17H_UTD			VAL_DSG_UTP
#define BYTE_18H_UTDR			VAL_DSG_UTP_RCV

#define BYTE_19H_TR				10			//内部默认应该是10kΩ，实际是多少读出来便可，这个数值无效

typedef union __MTP_REG_BSTATUS1 {
    UINT8 all;
    struct _MTP_REG_BSTATUS1 {
		UINT8 OV     			:1;		//单节过压
		UINT8 UV     			:1;		//单节低压
		UINT8 OCD1      		:1;		//放电过流1保护状态
		UINT8 OCD2      		:1;		//放电过流2保护状态
		
		UINT8 OCC     			:1;		//充电过流保护状态
		UINT8 SC  				:1;		//短路保护状态
		UINT8 PF  				:1;		//二次过充电保护状态位
		UINT8 WDT  				:1;		//看门狗溢出位
     }bits;
}MTP_REG_BSTATUS1;


typedef union __MTP_REG_BSTATUS2 {
    UINT8 all;
    struct _MTP_REG_BSTATUS2 {
		UINT8 UTC  				:1;		//充电低温保护状态位
		UINT8 OTC  				:1;		//充电高温保护状态位
		UINT8 UTD      			:1;		//放电低温保护状态位
		UINT8 OTD   			:1;		//放电高温保护状态位
		
		UINT8 Rcv				:4;		//保留位
		//UINT8 Rcv2				:8;		//保留位
     }bits;
}MTP_REG_BSTATUS2;


typedef union __MTP_REG_BSTATUS3 {
    UINT8 all;
    struct _MTP_REG_BSTATUS3 {
		UINT8 DSG_FET     		:1;		//放电管状态
		UINT8 CHG_FET     		:1;		//充电管状态
		UINT8 PCHG_FET      	:1;		//预充管状态
		UINT8 L0V      			:1;		//低电压禁止充电状态位
		
		UINT8 EEPR_WR     		:1;		//EEPROM写操作状态位
		UINT8 RCV  				:1;		//保留位
		UINT8 DSGING  			:1;		//放电状态
		UINT8 CHGING  			:1;		//充电状态
     }bits;
}MTP_REG_BSTATUS3;


typedef union __MTP_REG_CONF {
    UINT8 all;
    struct _MTP_REG_CONF {
		UINT8 IDLE     		:1;		//放电管状态
		UINT8 SLEEP     	:1;		//充电管状态
		UINT8 ENWDT      	:1;		//预充管状态
		UINT8 CADCON      	:1;		//低电压禁止充电状态位
		
		UINT8 CHGMOS     	:1;		//EEPROM写操作状态位
		UINT8 DSGMOS  		:1;		//保留位
		UINT8 PCHMOS  		:1;		//放电状态
		UINT8 OCRC  		:1;		//充电状态
     }bits;
}MTP_REG_CONF;


typedef struct _AFE_REG_STORE {
	UINT8 u8_MTP_SCONF2;				//01H
	UINT8 u8_MTP_SCV_SCT;				//0EH
	
	MTP_REG_CONF REG_MTP_CONF;			//40H
	UINT8 u8_MTP_BALANCEH;				//41H
	UINT8 u8_MTP_BALANCEL;				//42H
	MTP_REG_BSTATUS1 REG_BSTATUS1;		//43H
	MTP_REG_BSTATUS2 REG_BSTATUS2;		//44H
	MTP_REG_BSTATUS3 REG_BSTATUS3;		//45H
	UINT8 TR;					//71H
	UINT16 TR_ResRef;			//680 + 5*TR，单位是kΩ*100
}SH367309_REG_STORE;


typedef enum _FAULT_AFE_TO_MCU {
	CellOvp,
	CellUvp,
	IdischgOcp1,
	IdischgOcp2,
	IchgOcp,
	CellChgUTp,
	CellChgOTp,
	CellDsgUTp,
	CellDsgOTp,
}FAULT_AFE_TO_MCU;


extern SH367309_REG_STORE SH367309_Reg_Store;

extern UINT16 aaaaaa1;
extern UINT16 aaaaaa2;
extern UINT16 aaaaaa3;
extern UINT16 aaaaaa4;
extern UINT8 aaa11;
extern UINT8 gu8_DriverStartUpFlag;


UINT8 AFE_CheckStatus(void);
UINT8 AFE_IsReady(void);
void AFE_Reset(void);

void AFE_Sleep(void);
void AFE_IDLE(void);
void AFE_SHIP(void);
UINT32 AFE_CalcuVbat(void);

UINT8 SH367309_SC_DelayT_Set(void);
void SH367309_DriverMos_Ctrl(GPIO_Type Type, UINT8 OnOFF);
void SH367309_UpdataAfeConfig(void);
void SH367309_Enable_AFE_Wdt_Cadc_Drivers(void);


void App_SH367309(void);

#endif	/* SH367309_FUNC_H */

