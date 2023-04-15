#ifndef SOCENHANCE_H
#define SOCENHANCE_H

#include "stm32f10x.h"


#define SOC_Size_TableCanSet 	(UINT16)42
#define SOC_Size_LiFePO 		(UINT16)42
#define SOC_Size_TernaryLi 		(UINT16)42
#define SOC_Size_LiFePO2 		(UINT16)42

enum SOC_TABLE_SELECT {
	SOC_TABLE_TEST = 0,
	SOC_TABLE_LIFEPO,
	SOC_TABLE_TERNARYLI,
	SOC_TABLE_LIFEPO2
};

extern const UINT16 SOC_Table_LiFePO[SOC_Size_LiFePO];
extern const UINT16 SocTable_TernaryLi[SOC_Size_TernaryLi];
extern const UINT16 SocTable_LiFePO2[SOC_Size_LiFePO2];


#define E2P_AdressNum 			(UINT16)16

struct SOC_ENHANCE_ELEMENT {
	//只需要赋值一次的参数
	UINT16 u16_SOC_Ah;				//Ah*10
	UINT16 u16_SOC_CycleT_Ever;		//已经循环次数
	UINT16 u16_SOC_CycleT_Limit;	//电池可循环次数
    UINT16 u16_SOC_TableSelect;		//0:可调整数组。1:磷酸铁锂(古瑞瓦特)。2:三元里。3:磷酸铁锂2(不常用)。均为42个数值。
    UINT16 u16_SOC_DsgVcell_Limit;	//mV，允许放电达到的单串最小电压值。该值+200复原
    UINT16 u16_SOC_0_Vol;			//mV，SOC为0时的电压
    UINT16 u16_SOC_100_Vol;			//mV，SOC为100时的电压    
	UINT16 SOC_E2P_Adress[E2P_AdressNum];			//需要16个EEPROM地址存相关数据
	UINT16 SOC_Table_CanSet[SOC_Size_TableCanSet];	//外部控制可设的OCV表格
	UINT8 u8_SetSocOnce;			//外界要修改SOC的值。
	UINT8 u8_LargeCurFlag_Chg;		//外部使用大电流，末端恒流充的标志位，如果末端大电流充电，0.5C以上，可置1处理，默认为0。
									//不一定要置1，如果末端实在没法进入100%，可以思考置1。
	UINT8 u8_LargeCurFlag_Dsg;		//外部使用大电流，末端恒流放的标志位，如果末端大电流放电，0.5C以上，可置1处理，默认为0。
									//不一定要置1，如果末端实在没法进入0%，可以思考置1。
									//如果循环多次还是不能归零或者100%，则可调用

	//需要不间断赋值的参数
	UINT16 u16_VCellMax;			//mV
	UINT16 u16_VCellMin;        	//mV，第6串(只有6串)和第16串不建议纳入计算
	UINT16 u16_Ichg;				//A*10
	UINT16 u16_Idsg;				//A*10

	//能获取结果信息的参数
	UINT16 u16_SOC_InitOver;		//SOC开机初始化完毕标志位，1:初始化完成。0:还没初始化完
	UINT16 u16_SOC_CailFaultCnt;	//出现过调现象，需要OCV再次校准次数。
	UINT8 u8_SOC;					//Soc的值
	UINT8 u8_SOH;					//电池健康状态
	UINT16 u16_CapacityNow;			//Ah*100，当前剩余容量
	UINT16 u16_CapacityFull; 		//Ah*100，满电容量
	UINT16 u16_CapacityFactory;		//Ah*100，出厂容量，不需要
	UINT16 u16_Cycle_times;			//循环次数
	//以下三个为测试获取信息参数
	UINT8 u8_SOC_OCV_Cali;			//Soc校准值
	UINT8 u8_n_CoulombicEff;		//库伦效率
	UINT8 u8_n_InnerCorrect;		//内环校准参数

	//信息交换区
	UINT16 u16_RefreshData_Flag;	//刷新数据标志位。X:需要刷新，0:不需要，刷新后自行清0
									//1，SOC根据表格刷新。
									//2，SOC归零功能(改为循环次数刷新为0)。
									//3，SOC值外部修正。
									//均能保存。
};

extern struct SOC_ENHANCE_ELEMENT SOC_Enhance_Element;

extern UINT16 ChgValue;
extern UINT16 DsgValue;

UINT16 InverterChgCurve(void);
UINT16 InverterDsgCurve(void);

void SOC_OCV_Ctrl(UINT8 TimeBase_200ms);
void SOC_IntEnhance_Ctrl(UINT8 TimeBase_200ms);


extern UINT16 ReadEEPROM_Word_WithZone(UINT16 addr);						//原则上不返回
extern void WriteEEPROM_Word_WithZone(UINT16 addr, UINT16 data);

#endif	/* SOCENHANCE_H */

