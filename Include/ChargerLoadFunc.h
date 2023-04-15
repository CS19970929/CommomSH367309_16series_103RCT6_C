#ifndef CHARGERLOADFUNC_H
#define CHARGERLOADFUNC_H

typedef union __CHARGERLOAD_FUNC {
    UINT16 all;
    struct _CHARGERLOAD_FUNC {
		UINT8 b1OFFDriver_ChgOcp	:1;		//驱动出问题，关闭驱动功能，充电过流保护
		UINT8 b1OFFDriver_DsgOcp	:1;		//驱动出问题，关闭驱动功能，放电过流保护
		UINT8 b1OFFDriver_Ovp		:1;		//连续三次过压保护导致关闭驱动
		UINT8 b1OFFDriver_Uvp		:1;		//连续三次低压保护
		
		UINT8 b1OFFDriver_Vdelta	:1;		//压差过大保护
		UINT8 b1OFFDriver_AFE_ERR	:1;		//AFE错误导致关闭驱动
		UINT8 b1OFFDriver_EEPROM_ERR:1;		//EEPROM读取错误或者通讯错误
		UINT8 b1OFFDriver_CBC		:1; 	//CBC导致，CBC只在放电有效果
		
		UINT8 b1OFFDriver_UpperCom	:1;		//上位机强制关闭指令
		UINT8 b1ON_Charger			:1;		//电枪插入
		UINT8 b1ON_Load				:1;		//负载插入
		UINT8 b1ON_Charger_AllSeries:1;		//电枪插入，全系列都有，PA0，IN_WK_MCU
		
		UINT8 bRcved1				:4;		//res
     }bits;
}CHARGERLOAD_FUNC;


//#define _CHARGER_LOAD

extern CHARGERLOAD_FUNC ChargerLoad_Func;

void Init_ChargerLoad_Det(void);
void App_ChargerLoad_Det(void);

#endif	/* CHARGERLOADFUNC_H */

