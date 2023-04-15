#ifndef CHARGERLOADFUNC_H
#define CHARGERLOADFUNC_H

typedef union __CHARGERLOAD_FUNC {
    UINT16 all;
    struct _CHARGERLOAD_FUNC {
		UINT8 b1OFFDriver_ChgOcp	:1;		//���������⣬�ر��������ܣ�����������
		UINT8 b1OFFDriver_DsgOcp	:1;		//���������⣬�ر��������ܣ��ŵ��������
		UINT8 b1OFFDriver_Ovp		:1;		//�������ι�ѹ�������¹ر�����
		UINT8 b1OFFDriver_Uvp		:1;		//�������ε�ѹ����
		
		UINT8 b1OFFDriver_Vdelta	:1;		//ѹ����󱣻�
		UINT8 b1OFFDriver_AFE_ERR	:1;		//AFE�����¹ر�����
		UINT8 b1OFFDriver_EEPROM_ERR:1;		//EEPROM��ȡ�������ͨѶ����
		UINT8 b1OFFDriver_CBC		:1; 	//CBC���£�CBCֻ�ڷŵ���Ч��
		
		UINT8 b1OFFDriver_UpperCom	:1;		//��λ��ǿ�ƹر�ָ��
		UINT8 b1ON_Charger			:1;		//��ǹ����
		UINT8 b1ON_Load				:1;		//���ز���
		UINT8 b1ON_Charger_AllSeries:1;		//��ǹ���룬ȫϵ�ж��У�PA0��IN_WK_MCU
		
		UINT8 bRcved1				:4;		//res
     }bits;
}CHARGERLOAD_FUNC;


//#define _CHARGER_LOAD

extern CHARGERLOAD_FUNC ChargerLoad_Func;

void Init_ChargerLoad_Det(void);
void App_ChargerLoad_Det(void);

#endif	/* CHARGERLOADFUNC_H */

