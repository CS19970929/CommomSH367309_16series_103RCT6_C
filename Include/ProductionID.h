#ifndef PRODUCTIONID_H
#define PRODUCTIONID_H

#define PRODUCT_ID_LENGTH_MAX 32

typedef struct {
	//均为阿斯克码
	UINT8 BMS_SerialNumber[PRODUCT_ID_LENGTH_MAX];			//BMS序列号
	UINT8 BMS_HardWareVersion[PRODUCT_ID_LENGTH_MAX];		//BMS硬件版本号
	UINT8 BMS_SoftWareVersion[PRODUCT_ID_LENGTH_MAX];		//BMS软件版本号

	UINT16 BMS_SerialNumberLength;			//BMS序列号地址				//意义不大，末端全部填0，阿斯克码为空
	UINT16 BMS_HardWareVersionLength;		//BMS硬件版本号地址			//意义不大，末端全部填0，阿斯克码为空
	UINT16 BMS_SoftWareVersionLength;		//BMS软件版本号地址			//意义不大，末端全部填0，阿斯克码为空
	
	UINT16 BMS_SerialNumberHeadAdress;		//BMS序列号地址
	UINT16 BMS_HardWareVersionHeadAdress;	//BMS硬件版本号地址
	UINT16 BMS_SoftWareVersionHeadAdress;	//BMS软件版本号地址

	UINT8 BMS_SerialNumber_WriteFlag;
	UINT8 BMS_HardWareVersion_WriteFlag;
	UINT8 BMS_SoftWareVersion_WriteFlag;
}PRODUCTION_ID_INFO;

extern PRODUCTION_ID_INFO ProductionInfor;


void WriteProID_Default(void);
void App_ProID_Deal(void);


#endif	/* PRODUCTIONID_H */

