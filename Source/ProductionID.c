#include "main.h"

PRODUCTION_ID_INFO ProductionInfor;


void InitProID(void) {
	UINT8 i;

	ProductionInfor.BMS_SerialNumberHeadAdress = E2P_ADDR_E2POS_SERIAL_NUM;
	ProductionInfor.BMS_HardWareVersionHeadAdress = E2P_ADDR_E2POS_HAEDWARE_VER;
	ProductionInfor.BMS_SoftWareVersionHeadAdress = E2P_ADDR_E2POS_SOFTWARE_VER;

	ProductionInfor.BMS_SerialNumberLength = ReadEEPROM_Word_WithZone(ProductionInfor.BMS_SerialNumberHeadAdress);
	ProductionInfor.BMS_HardWareVersionLength = ReadEEPROM_Word_WithZone(ProductionInfor.BMS_HardWareVersionHeadAdress);
	ProductionInfor.BMS_SoftWareVersionLength = ReadEEPROM_Word_WithZone(ProductionInfor.BMS_SoftWareVersionHeadAdress);

	//这样写可以吗？
	for(i = 0; i < (PRODUCT_ID_LENGTH_MAX>>1); i++) {
		*((UINT16*)(&ProductionInfor.BMS_SerialNumber[i*2])) = \
			ReadEEPROM_Word_WithZone(ProductionInfor.BMS_SerialNumberHeadAdress + 2 + i*2);		//往后挪一位

		*((UINT16*)(&ProductionInfor.BMS_HardWareVersion[i*2])) = \
			ReadEEPROM_Word_WithZone(ProductionInfor.BMS_HardWareVersionHeadAdress + 2 + i*2);
		
		*((UINT16*)(&ProductionInfor.BMS_SoftWareVersion[i*2])) = \
			ReadEEPROM_Word_WithZone(ProductionInfor.BMS_SoftWareVersionHeadAdress + 2 + i*2);
	}
}


void WriteProID(void) {
	UINT8 i;

	if(ProductionInfor.BMS_SerialNumber_WriteFlag) {
		WriteEEPROM_Word_WithZone(ProductionInfor.BMS_SerialNumberHeadAdress, ProductionInfor.BMS_SerialNumberLength);
		for(i = 0; i < (PRODUCT_ID_LENGTH_MAX>>1); i++) {
			 WriteEEPROM_Word_WithZone(ProductionInfor.BMS_SerialNumberHeadAdress + 2 + i*2,\
			 	*((UINT16*)(&ProductionInfor.BMS_SerialNumber[i*2])));
		}
		ProductionInfor.BMS_SerialNumber_WriteFlag  = 0;
	}

	if(ProductionInfor.BMS_HardWareVersion_WriteFlag) {
		WriteEEPROM_Word_WithZone(ProductionInfor.BMS_HardWareVersionHeadAdress, ProductionInfor.BMS_HardWareVersionLength);
		for(i = 0; i < (PRODUCT_ID_LENGTH_MAX>>1); i++) {
			WriteEEPROM_Word_WithZone(ProductionInfor.BMS_HardWareVersionHeadAdress + 2 + i*2,\
			   *((UINT16*)(&ProductionInfor.BMS_HardWareVersion[i*2])));
		}
		ProductionInfor.BMS_HardWareVersion_WriteFlag  = 0;
	}

	if(ProductionInfor.BMS_SoftWareVersion_WriteFlag) {
		WriteEEPROM_Word_WithZone(ProductionInfor.BMS_SoftWareVersionHeadAdress, ProductionInfor.BMS_SoftWareVersionLength);
		for(i = 0; i < (PRODUCT_ID_LENGTH_MAX>>1); i++) {
			WriteEEPROM_Word_WithZone(ProductionInfor.BMS_SoftWareVersionHeadAdress + 2 + i*2,\
			   *((UINT16*)(&ProductionInfor.BMS_SoftWareVersion[i*2])));
		}
		ProductionInfor.BMS_SoftWareVersion_WriteFlag = 0;
	}
}


void WriteProID_Default(void) {
	UINT8 harewareCount =sizeof(BMS_HARDWARE_VERDION_DEFAULT) >32 ? 32 : sizeof(BMS_HARDWARE_VERDION_DEFAULT);
	UINT8 softwareCount = sizeof(BMS_SOFTWARE_VERDION_DEFAULT) >32 ? 32 : sizeof(BMS_SOFTWARE_VERDION_DEFAULT);
	UINT8 serialNumberCount = sizeof(BMS_SERIAL_NUMBER_DEFAULT) >32 ? 32 : sizeof(BMS_SERIAL_NUMBER_DEFAULT);
	
	
	memset(&ProductionInfor,0,sizeof(PRODUCTION_ID_INFO));
	
	ProductionInfor.BMS_SerialNumberHeadAdress = E2P_ADDR_E2POS_SERIAL_NUM;
	ProductionInfor.BMS_HardWareVersionHeadAdress = E2P_ADDR_E2POS_HAEDWARE_VER;
	ProductionInfor.BMS_SoftWareVersionHeadAdress = E2P_ADDR_E2POS_SOFTWARE_VER;
	
	memcpy(&ProductionInfor.BMS_HardWareVersion[0], BMS_HARDWARE_VERDION_DEFAULT,harewareCount);
	memcpy(&ProductionInfor.BMS_SoftWareVersion[0], BMS_SOFTWARE_VERDION_DEFAULT, softwareCount);
	memcpy(&ProductionInfor.BMS_SerialNumber[0], BMS_SERIAL_NUMBER_DEFAULT, serialNumberCount);

	ProductionInfor.BMS_SerialNumber_WriteFlag = 1;
	ProductionInfor.BMS_HardWareVersion_WriteFlag = 1;
	ProductionInfor.BMS_SoftWareVersion_WriteFlag  = 1;
	WriteProID();
}


void App_ProID_Deal(void) {
	static UINT8 su8_StartUpFlag = 0;

	switch(su8_StartUpFlag) {
		case 0:
			InitProID();
			su8_StartUpFlag = 1;
			break;
	
		case 1:
			WriteProID();
			break;

		default:
			su8_StartUpFlag = 0;
			break;
	}
}

