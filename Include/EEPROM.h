#ifndef EEPROM_H
#define EEPROM_H

//Mini STM32开发板使用的是24c02
//#define AT24C02
#define DELAY_US_IIC_EEPROM		2	//4为100KHz，2为150KHz

#define sEEAddress   0xA0			//E2 = E1 = E0 = 0

#define AZONE				0x0000              // A区: 0x0000~0x0799		//2K一个区间
#define BZONE				0x0800				// B区: 0x0800~0x0999
#define CZONE				0x1000				// C区: 0x1000~0x1800

//IO方向设置
//改括号里面那个数X<<2
#define SDA_IN_SEE()  {GPIOB->CRH&=0xFFFF0FFF;GPIOB->CRH|=(UINT32)8<<(3<<2);}
#define SDA_OUT_SEE() {GPIOB->CRH&=0xFFFF0FFF;GPIOB->CRH|=(UINT32)3<<(3<<2);}

//IO操作函数	
#define IIC_SCL_SEE    PBout(10) //SCL
#define IIC_SDA_SEE    PBout(11) //SDA
#define READ_SDA_SEE   PBin(11)  //输入SDA


#define EEPROM_ADDR_PASS           			((UINT16)0x3FFC)
#define EEPROM_ADDR_SLEEP           		((UINT16)0x3FFA)
#define EEPROM_ADDR_FLASHUPDATE     		((UINT16)0x3FFE)

#define EEPROM_VALUE_BEGIN_FLAG				0x1133		//默认0x1133，如果自己需要刷一遍，则自己改完再改回0x1133
														//默认改为0x1688
#define EEPROM_VALUE_SLEEP    				((UINT16)0xABCD)
#define EEPROM_VALUE_SLEEP_RESET    		((UINT16)0xFFFF)
#define EEPROM_VALUE_FLASHUPDATE    		((UINT16)0xABCD)
#define EEPROM_VALUE_FLASHUPDATE_RESET    	((UINT16)0xFFFF)


//#define EEPROM_ADDR_SLEEPMODE     			2036	//取消，改为FLASH
#define EEPROM_ADDR_SWITCH_ONOFF     		2040
#define EEPROM_ADDR_SYS_FUNC_SELECT     	2044


#define E2P_PARA_NUM_VOLCUR_PROTECT 		30
#define E2P_PARA_NUM_TEM_PROTECT 			25
#define E2P_PARA_NUM_OTHER_PROTECT			10

#define E2P_PARA_ALL_VOLCUR_PROTECT 		0x3FFFFFFF		//30个
#define E2P_PARA_ALL_TEM_PROTECT 			0x01FFFFFF		//25个
#define E2P_PARA_ALL_OTHER_PROTECT			0x000003FF		//10个
#define E2P_PARA_ALL_RTC_ELEMENT 			0x00000FFF		//12个
#define E2P_PARA_ALL_OTHER_ELEMENT1 		0xFFFFFFFF		//32个
#define E2P_PARA_ALL_HEAT_COOL_ELE 			0x00FFFFFF		//24个，和RTC一样不需要以下的分开

//VolCur_Protect_WriteFlag
#define	EE_FLAG_VCELL_OVP_FIRST				0x00000001
#define	EE_FLAG_VCELL_OVP_SECOND			0x00000002
#define	EE_FLAG_VCELL_OVP_THIRD				0x00000004
#define	EE_FLAG_VCELL_OVP_RCV				0x00000008
#define	EE_FLAG_VCELL_OVP_FILTER			0x00000010

#define	EE_FLAG_VCELL_UVP_FIRST				0x00000020
#define	EE_FLAG_VCELL_UVP_SECOND			0x00000040
#define	EE_FLAG_VCELL_UVP_THIRD				0x00000080
#define	EE_FLAG_VCELL_UVP_RCV				0x00000100
#define	EE_FLAG_VCELL_UVP_FILTER			0x00000200

#define	EE_FLAG_VBUS_OVP_FIRST				0x00000400
#define	EE_FLAG_VBUS_OVP_SECOND				0x00000800
#define	EE_FLAG_VBUS_OVP_THIRD				0x00001000
#define	EE_FLAG_VBUS_OVP_RCV				0x00002000
#define	EE_FLAG_VBUS_OVP_FILTER				0x00004000

#define	EE_FLAG_VBUS_UVP_FIRST				0x00008000
#define	EE_FLAG_VBUS_UVP_SECOND				0x00010000
#define	EE_FLAG_VBUS_UVP_THIRD				0x00020000
#define	EE_FLAG_VBUS_UVP_RCV				0x00040000
#define	EE_FLAG_VBUS_UVP_FILTER				0x00080000

#define	EE_FLAG_ICHG_OCP_FIRST				0x00100000
#define	EE_FLAG_ICHG_OCP_SECOND				0x00200000
#define	EE_FLAG_ICHG_OCP_THIRD				0x00400000
#define	EE_FLAG_ICHG_OCP_RCV				0x00800000
#define	EE_FLAG_ICHG_OCP_FILTER				0x01000000

#define	EE_FLAG_IDSG_OCP_FIRST				0x02000000
#define	EE_FLAG_IDSG_OCP_SECOND				0x04000000
#define	EE_FLAG_IDSG_OCP_THIRD				0x08000000
#define	EE_FLAG_IDSG_OCP_RCV				0x10000000
#define	EE_FLAG_IDSG_OCP_FILTER				0x20000000


//Temp_Protect_WriteFlag
#define	EE_FLAG_TCHG_OTP_FIRST				0x00000001
#define	EE_FLAG_TCHG_OTP_SECOND				0x00000002
#define	EE_FLAG_TCHG_OTP_THIRD				0x00000004
#define	EE_FLAG_TCHG_OTP_RCV				0x00000008
#define	EE_FLAG_TCHG_OTP_FILTER				0x00000010

#define	EE_FLAG_TCHG_UTP_FIRST				0x00000020
#define	EE_FLAG_TCHG_UTP_SECOND				0x00000040
#define	EE_FLAG_TCHG_UTP_THIRD				0x00000080
#define	EE_FLAG_TCHG_UTP_RCV				0x00000100
#define	EE_FLAG_TCHG_UTP_FILTER				0x00000200

#define	EE_FLAG_TDSG_OTP_FIRST				0x00000400
#define	EE_FLAG_TDSG_OTP_SECOND				0x00000800
#define	EE_FLAG_TDSG_OTP_THIRD				0x00001000
#define	EE_FLAG_TDSG_OTP_RCV				0x00002000
#define	EE_FLAG_TDSG_OTP_FILTER				0x00004000

#define	EE_FLAG_TDSG_UTP_FIRST				0x00008000
#define	EE_FLAG_TDSG_UTP_SECOND				0x00010000
#define	EE_FLAG_TDSG_UTP_THIRD				0x00020000
#define	EE_FLAG_TDSG_UTP_RCV				0x00040000
#define	EE_FLAG_TDSG_UTP_FILTER				0x00080000

#define	EE_FLAG_TMOS_OTP_FIRST			    0x00100000
#define	EE_FLAG_TMOS_OTP_SECOND			    0x00200000
#define	EE_FLAG_TMOS_OTP_THIRD			    0x00400000
#define	EE_FLAG_TMOS_OTP_RCV				0x00800000
#define	EE_FLAG_TMOS_OTP_FILTER			    0x01000000

//Other_Protect_WriteFlag
#define	EE_FLAG_VDELTA_OVP_FIRST			0x00000001
#define	EE_FLAG_VDELTA_OVP_SECOND			0x00000002
#define	EE_FLAG_VDELTA_OVP_THIRD			0x00000004
#define	EE_FLAG_VDELTA_OVP_RCV				0x00000008
#define	EE_FLAG_VDELTA_OVP_FILTER			0x00000010

#define	EE_FLAG_SOC_UP_FIRST				0x00000020
#define	EE_FLAG_SOC_UP_SECOND				0x00000040
#define	EE_FLAG_SOC_UP_THIRD				0x00000080
#define	EE_FLAG_SOC_UP_RCV					0x00000100
#define	EE_FLAG_SOC_UP_FILTER				0x00000200

//Other1_WriteFlag
#define	EE_FLAG_OTHER1_BALANCE_OV			0x00000001
#define	EE_FLAG_OTHER1_BALANCE_OW			0x00000002
#define	EE_FLAG_OTHER1_BALANCE_CW1			0x00000004
#define	EE_FLAG_OTHER1_BALANCE_CW2			0x00000008

#define	EE_FLAG_OTHER1_OPENTIME_ODD			0x00000010
#define	EE_FLAG_OTHER1_OPENTIME_EVEN		0x00000020
#define	EE_FLAG_OTHER1_OPENTIME_MOS			0x00000040
#define	EE_FLAG_OTHER1_RES					0x00000080

#define	EE_FLAG_CS_CUR_CHGMAX				0x00000100
#define	EE_FLAG_CS_CUR_DSGMAX				0x00000200
#define	EE_FLAG_CBC_CUR_CHG					0x00000400
#define	EE_FLAG_CBC_CUR_DSG					0x00000800

#define	EE_FLAG_OTHER1_COOL_DSG_H			0x00001000
#define	EE_FLAG_OTHER1_COOL_DSG_L			0x00002000
#define	EE_FLAG_CUR_MODE_V_DELTA			0x00004000
#define	EE_FLAG_CUR_MODE_CUR_LIMIT			0x00008000

#define	EE_FLAG_OTHER1_SLEEP_V_NORMAL		0x00010000
#define	EE_FLAG_OTHER1_SLEEP_TIME_NORMAL	0x00020000
#define	EE_FLAG_OTHER1_SLEEP_V_LOW			0x00040000
#define	EE_FLAG_OTHER1_SLEEP_TIME_LOW		0x00080000

#define	EE_FLAG_OTHER1_SLEEP_I_CHG			0x00100000
#define	EE_FLAG_OTHER1_SLEEP_I_DSG			0x00200000
#define	EE_FLAG_OTHER1_SLEEP_RES1			0x00400000
#define	EE_FLAG_OTHER1_SLEEP_RES2			0x00800000

#define	EE_FLAG_OTHER1_SOC_AH				0x01000000
#define	EE_FLAG_OTHER1_SOC_CYCLE_TIME		0x02000000
#define	EE_FLAG_OTHER1_SOC_RES1				0x04000000
#define	EE_FLAG_OTHER1_SOC_RES2				0x08000000

#define	EE_FLAG_OTHER1_SYS_SERIES_NUM		0x10000000
#define	EE_FLAG_OTHER1_SYS_CS_RESIS			0x20000000
#define	EE_FLAG_OTHER1_SYS_CS_NUM			0x40000000
#define	EE_FLAG_OTHER1_SYS_PRECHG_TIME		0x80000000


#if 0	//这个不用，因为是一次性写的
//HeatCool_WriteFlag
#define	EE_FLAG_HEAT_DSG_HIGH				0x00000001
#define	EE_FLAG_HEAT_DSG_LOW				0x00000002
#define	EE_FLAG_HEAT_CHG_HIGH				0x00000004
#define	EE_FLAG_HEAT_CHG_LOW				0x00000008

#define	EE_FLAG_HEAT_CUR_MAX				0x00000010
#define	EE_FLAG_HEAT_CUR_MIN				0x00000020
#define	EE_FLAG_HEAT_TIME_MAX				0x00000040
#define	EE_FLAG_HEAT_RES1					0x00000080

#define	EE_FLAG_HEAT_RES2					0x00000100
#define	EE_FLAG_HEAT_RES3					0x00000200
#define	EE_FLAG_HEAT_RES4					0x00000400
#define	EE_FLAG_HEAT_RES5					0x00000800

#define	EE_FLAG_HEAT_RES6					0x00001000
#define	EE_FLAG_COOL_DSG_HIGH				0x00002000
#define	EE_FLAG_COOL_DSG_LOW				0x00004000
#define	EE_FLAG_COOL_CHG_HIGH				0x00008000

#define	EE_FLAG_COOL_CHG_LOW				0x00010000
#define	EE_FLAG_COOL_CUR_MAX				0x00020000
#define	EE_FLAG_COOL_CUR_MIN				0x00040000
#define	EE_FLAG_COOL_TIME_MAX				0x00080000

#define	EE_FLAG_COOL_RES1					0x00100000
#define	EE_FLAG_COOL_RES2					0x00200000
#define	EE_FLAG_COOL_RES3					0x00400000
#define	EE_FLAG_COOL_RES4					0x00800000
#endif


//以下为EEPROM保存数据的顺序和个数一览表
#define E2P_PARA_NUM_PROTECT 		 		65				//为以上三个相加，为何要分开，因为最高32位的问题，看函数怎么实现
#define E2P_PARA_NUM_RTC		 			12
#define E2P_PARA_NUM_CALIB_K 		 		KB_NUM			//47
#define E2P_PARA_NUM_CALIB_B 		 		KB_NUM
#define E2P_PARA_NUM_SOC_TABLE 		 		SOC_TABLE_SIZE	//42，因GetEndValue的缘故只能合在一起
#define E2P_PARA_NUM_COPPERLOSS 		 	CompensateNUM	//16
#define E2P_PARA_NUM_COPPERLOSS_NUM 		CompensateNUM
#define E2P_PARA_NUM_FAULT_RECORD 		 	(3*Record_len+ 3 +Record_len*6)//为什么define中有运算符合时要加括号呢，因为如果外面有运算符比define的优先级高就会出错
#define E2P_PARA_NUM_OTHER_ELEMENT1 		32				//可添加，再次添加备注——不可添加
#define E2P_PARA_NUM_HEAT_COOL				24


//结构体类型保存放这里
#define E2P_ADDR_E2POS_PROTECT 			{0,  2,  4,  6,  8,  10, 12, 14, 16, 18,\
						 				 20, 22, 24, 26, 28, 30, 32, 34, 36, 38,\
						 				 40, 42, 44, 46, 48, 50, 52, 54, 56, 58,\
						 				 \
						 				 60, 62, 64, 66, 68, 70, 72, 74, 76, 78,\
						 				 80, 82, 84, 86, 88, 90, 92, 94, 96, 98,\
						 				 100,102,104,106,108,\
						 				 \
						 				 110,112,114,116,118,120,122,124,126,128}

#define E2P_ADDR_E2POS_RTC 				{130,132,134,136,138,140,\
						   				 142,144,146,148,150,152}


#define	E2P_ADDR_START_CALIB_K		    154    	//47个字
#define	E2P_ADDR_START_CALIB_B		    248		//47个字
#define	E2P_ADDR_START_SOC_TABLE		342    	//42个字
#define	E2P_ADDR_START_COPPERLOSS		426    	//16个字
#define	E2P_ADDR_START_COPPERLOSS_NUM	458    	//16个字

#define E2P_ADDR_START_FAULT_RECORD 	490
#define	E2P_ADDR_START_FR_FIRST 		E2P_ADDR_START_FAULT_RECORD				//10个字
#define	E2P_ADDR_START_FR_SECOND  		(E2P_ADDR_START_FAULT_RECORD+20)		//10个字
#define	E2P_ADDR_START_FR_THIRD  		(E2P_ADDR_START_FAULT_RECORD+40)		//10个字
//加上2个字节的指针保存
#define E2P_ADDR_E2POS_FR_TEMP_FIRST	(E2P_ADDR_START_FAULT_RECORD+60)		//2个字
#define E2P_ADDR_E2POS_FR_TEMP_SECOND  	(E2P_ADDR_START_FAULT_RECORD+62)		//2个字
#define E2P_ADDR_E2POS_FR_TEMP_THIRD	(E2P_ADDR_START_FAULT_RECORD+64)		//2个字
#define E2P_ADDR_START_FR_THIRD_RTC 	(E2P_ADDR_START_FAULT_RECORD+66)		//黑盒记录，60个字

#define E2P_ADDR_START_OTHER_ELEMENT1	676		//E2P_ADDR_START_FR_THIRD_RTC + 120 = E2P_ADDR_START_FAULT_RECORD+66+120

#define E2P_ADDR_E2POS_OTHER_ELEMENT1 	{676,678,680,682,684,686,688,690,\
									 	 692,694,696,698,700,702,704,706,\
									 	 708,710,712,714,716,718,720,722,\
									 	 724,726,728,730,\
									 	 732,734,736,738}

#define E2P_ADDR_E2POS_HEAT_COOL 		{740,742,744,746,748,750,752,754,756,758,760,762,764,\
										 766,768,770,772,774,776,778,780,782,784,788}
#if 0
#define E2P_ADDR_E2POS_ENHANCE_SOC 		{790,792,794,796,798,800,802,804,\
										 806,808,810,812,814,816,818,820} 		//这个是不能在上位机改的
#endif

#define E2P_ADDR_E2POS_ENHANCE_SOC		790		//到828

#define E2P_ADDR_E2POS_SERIAL_NUM		830		//到868
#define E2P_ADDR_E2POS_HAEDWARE_VER		870		//到908
#define E2P_ADDR_E2POS_SOFTWARE_VER		910		//到948

#define E2P_ADDR_START_EVENT_RECORD 	1000	//到1198		//100个字
#define E2P_ADDR_E2POS_EVENT_POINT		1200	//下一个1202

extern UINT32 u32E2P_Pro_VolCur_WriteFlag;
extern UINT32 u32E2P_Pro_Temp_WriteFlag;
extern UINT32 u32E2P_Pro_Other_WriteFlag;
extern UINT32 u32E2P_OtherElement1_WriteFlag;
extern UINT32 u32E2P_HeatCool_WriteFlag;

extern UINT32 u32E2P_RTC_Element_WriteFlag;
extern UINT8 u8E2P_SocTable_WriteFlag;
extern UINT8 u8E2P_CopperLoss_WriteFlag;
extern UINT8 u8E2P_KB_WriteFlag;
extern UINT8 u8E2P_KB_WritePos;

UINT8 ReadEEPROM_Byte(UINT16 addr);
UINT8 WriteEEPROM_Byte(UINT16 addr, UINT8 val);
UINT16 ReadEEPROM_Word_NoZone(UINT16 addr);
UINT8 WriteEEPROM_Word_NoZone(UINT16 addr, UINT16 data);
UINT16 ReadEEPROM_Word_WithZone(UINT16 addr);						//原则上不返回
void WriteEEPROM_Word_WithZone(UINT16 addr, UINT16 data);

void InitE2PROM(void);
void App_E2promDeal(void);

void EEPROM_test(void);

#endif	/* EEPROM_H */

