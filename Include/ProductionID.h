#ifndef PRODUCTIONID_H
#define PRODUCTIONID_H

#define PRODUCT_ID_LENGTH_MAX 32

typedef struct {
	//��Ϊ��˹����
	UINT8 BMS_SerialNumber[PRODUCT_ID_LENGTH_MAX];			//BMS���к�
	UINT8 BMS_HardWareVersion[PRODUCT_ID_LENGTH_MAX];		//BMSӲ���汾��
	UINT8 BMS_SoftWareVersion[PRODUCT_ID_LENGTH_MAX];		//BMS����汾��

	UINT16 BMS_SerialNumberLength;			//BMS���кŵ�ַ				//���岻��ĩ��ȫ����0����˹����Ϊ��
	UINT16 BMS_HardWareVersionLength;		//BMSӲ���汾�ŵ�ַ			//���岻��ĩ��ȫ����0����˹����Ϊ��
	UINT16 BMS_SoftWareVersionLength;		//BMS����汾�ŵ�ַ			//���岻��ĩ��ȫ����0����˹����Ϊ��
	
	UINT16 BMS_SerialNumberHeadAdress;		//BMS���кŵ�ַ
	UINT16 BMS_HardWareVersionHeadAdress;	//BMSӲ���汾�ŵ�ַ
	UINT16 BMS_SoftWareVersionHeadAdress;	//BMS����汾�ŵ�ַ

	UINT8 BMS_SerialNumber_WriteFlag;
	UINT8 BMS_HardWareVersion_WriteFlag;
	UINT8 BMS_SoftWareVersion_WriteFlag;
}PRODUCTION_ID_INFO;

extern PRODUCTION_ID_INFO ProductionInfor;


void WriteProID_Default(void);
void App_ProID_Deal(void);


#endif	/* PRODUCTIONID_H */

