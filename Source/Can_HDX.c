#include "main.h"

volatile union Can_Status Can_Status_Flag;
volatile union CanTxType_Status CanTxType_Flag;
CanTxMsg TxMessage; 
CanRxMsg RxMessage;

UINT16 g_u16BusOff_InitTestCnt = 0;             //CAN总线关闭计时
UINT16 g_u16BusOff_RecoverCnt = 0;          	//5s计时标志位


void CAN_TX_Test(void) { 
	UINT8 TXCounter = 0, TXStatus = 0;
	UINT8 u8MailBoxUsed;
	TxMessage.StdId = CANID_TX_Test; 	//标准标识符
	TxMessage.ExtId = 0; 				//扩展标识符
	TxMessage.IDE = CAN_ID_STD;			//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;  				//消息的数据长度为8个字节
	TxMessage.Data[0] = 1; 				//数据
	TxMessage.Data[1] = 2;
	TxMessage.Data[2] = 3;
	TxMessage.Data[3] = 4;
	TxMessage.Data[4] = 5;
	TxMessage.Data[5] = 6;
	TxMessage.Data[6] = 7;
	TxMessage.Data[7] = 8;
	
	/*
	while((CAN_TxStatus_Ok!=CAN_TransmitStatus(CAN1,CAN_Transmit(CAN1,&TxMessage)))&&(++i<0xFFF));		//发送数据
	if(i >= 0xFF) {
		ERROR_UserCallback(CAN_ERROR);
	}
	*/
	u8MailBoxUsed = CAN_Transmit(CAN1,&TxMessage);
	//CAN_Transmit已集成Can发送流程代码，非常方便
	//这个++i和硬件的BusOFF是不冲突的，这个可以是在等邮箱空的次数，CAN_TxStatus_Pending和BusOFF分开
	do {
		//出现Can错误的原因就是这句话！连续执行导致连续mailbox0是满的！CAN_Transmit()地方都放错了！
		//TXStatus = CAN_TransmitStatus(CAN1,CAN_Transmit(CAN1,&TxMessage));
		TXStatus = CAN_TransmitStatus(CAN1, u8MailBoxUsed);
		TXCounter++;
	}while((TXStatus != CAN_TxStatus_Ok)&&(TXCounter < 0xFF));		//Fail和OK不用管

	if(TXCounter >= 0xFF) {
		System_ERROR_UserCallback(ERROR_CAN);	//这里应该是一个Pending_Error但是Can模块不可能需要等这么久吧。
	}
}


void CAN_Tx_Data(CanTxMsg* Msg) {
	UINT8 TXCounter = 0, TXStatus = 0;
	UINT8 u8MailBoxUsed;

	Msg->StdId += ((UINT32)CAN_ADRESS_STD_ID<<7);				//单机版地址默认为0
	u8MailBoxUsed = CAN_Transmit(CAN1,Msg);
	do {
		TXStatus = CAN_TransmitStatus(CAN1, u8MailBoxUsed);
		TXCounter++;
	}while((TXStatus == CAN_TxStatus_Failed)&&(TXCounter < 0xFF)); 	//Fail和OK不用管

	if(TXCounter >= 0xFF) {
		System_ERROR_UserCallback(ERROR_CAN);		//这里应该是一个Pending_Error但是Can模块不可能需要等这么久吧。
	}
}


void CAN_TX_0x00(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x00;	//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCellTotle;			//总压，10mV
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	
    if(g_stCellInfoReport.u16IDischg > 0) {					//总电流，符号型，充电为正，放电为负
		u16_tmp16a = g_stCellInfoReport.u16IDischg*10;		//10mA
		u16_tmp16a = (0x7FFF - u16_tmp16a + 1) | 0x8000;		
	}
	else {
		u16_tmp16a = g_stCellInfoReport.u16Ichg*10; 		// 电池总电流
	}
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);


	u16_tmp16a = g_stCellInfoReport.SocElement.u16CapacityNow;//剩余容量10mAh
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);


	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x01(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x01;	//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.SocElement.u16CapacityFull;	//满电容量10mAh
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

    u16_tmp16a = g_stCellInfoReport.SocElement.u16Cycle_times;	//循环次数
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.SocElement.u16Soc;
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x02(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x02;			//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16BalanceFlag1;
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16BalanceFlag2;
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.unMdlFault_Third.all;
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x03(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x03;	//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

	switch(OPEN) {
		case 0:
			u16_tmp16a = ((~((UINT16)(SystemStatus.all&0x000003FF)))&0x00FE)|(((UINT16)(SystemStatus.all&0x000003FF))&0xFF01);
			break;
		case 1:
			u16_tmp16a = (UINT16)(SystemStatus.all&0x000003FF);
			break;
		default:
			u16_tmp16a = (UINT16)(SystemStatus.all&0x000003FF);
			break;
	}
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 0x1234;				//生产日期
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 0x1234;				//软件版本
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x04(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x04;	//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    TxMessage.Data[0] = SeriesNum;			//电池串数
	TxMessage.Data[1] = System_ErrFlag.u8ErrFlag_CBC_DSG > 0 ? 1 : 0;

	u16_tmp16a = g_stCellInfoReport.u16TempMax;
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16TempMin;
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x05(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x05;	//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.SocElement.u16Soh;
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 0;
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 0;
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x06(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x06;	//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = 0;
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 0;
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 0;
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x07(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x07;			//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[0];
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[1];
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[2];
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x08(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x08;	//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[3];
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[4];
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[5];
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x09(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x09;	//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[6];
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[7];
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[8];
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x0A(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x0A;	//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[9];
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[10];
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[11];
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x0B(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x0B;	//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[12];
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[13];
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[14];
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x0C(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x0C;			//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[15];
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[16];
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[17];
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x0D(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x0D;	//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[18];
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[19];
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[20];
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x0E(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x0E;	//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[21];
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[22];
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[23];
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x0F(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x0F;	//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[24];
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[25];
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[26];
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x10(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x10;	//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[27];
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[28];
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[29];
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x11(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x11;	//标准标识符
	TxMessage.ExtId = 0;				//扩展标识符
	TxMessage.IDE = CAN_ID_STD; 		//使用标准标识符
	TxMessage.RTR = CAN_RTR_DATA;		//为数据帧
	TxMessage.DLC = 8;					//消息的数据长度为8个字节

    u16_tmp16a = g_stCellInfoReport.u16VCell[30];
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = g_stCellInfoReport.u16VCell[31];
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 0;
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void InitCan_GPIO(void) {
	GPIO_InitTypeDef GPIO_InitStructure;   
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA, ENABLE);	//复用功能和GPIOB端口时钟使能
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			//Configure CAN pin: RX    // PD0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  		// 上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;			//Configure CAN pin: TX    // PD1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 	// 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//GPIO_PinRemapConfig(GPIO_Remap2_CAN1, ENABLE);	 	//14:13位  //0：映射到PA11，PA12(默认)//1: 不用。
}

void InitCan_NVIC(void) {
    NVIC_InitTypeDef NVIC_InitStructure;

	/*不用设置了，systemInit()已经做了相应操作
	#ifdef  VECT_TAB_RAM  
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif
	*/
	/* enabling interrupt */
  	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}


void InitCan_Filter(void) {
	UINT16 u16CAN_FilterIdHigh;
	UINT16 u16CAN_FilterIdLow;
	UINT16 u16CAN_FilterMaskIdHigh;
	UINT16 u16CAN_FilterMaskIdLow;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;

	//这两句怎么来的，详细看截图——过滤器配置表
	//CAN_ID_STD之类的不需要移位，官方已经定好
	//CAN_FilterMode_IdList，列表模式FBMx = 1
	//CAN_FilterScale_16bit，过滤器组的位宽，FSCx = 0
	//结合这两个，高位的屏蔽位也作为标识符列表，可以搞4个CANID
	//别的情况是高位作为低位的屏蔽位。
	
	//stm32 can的屏蔽位模式：
	//一个是标识符寄存器(过滤器Filter)，一个是屏蔽位寄存器(Mask)。
	//凡是屏蔽位寄存器里为1的位所对应的标识符寄存器的位，这些位是必须匹配的
	//也就是说，你接受到的Message里面的标识符（ID）里面对应的位必须跟标识符寄存器里对应的位相同，才能被接受。

	//远程帧过滤器
	u16CAN_FilterIdHigh = (CANID_RX_COMMON_MSG_MASK<<5)|CAN_ID_STD|CAN_RTR_DATA;
	u16CAN_FilterIdLow = (CANID_RX_COMMON_MSG_FILTER<<5)|CAN_ID_STD|CAN_RTR_DATA;
	
	u16CAN_FilterMaskIdHigh = (CANID_RX_COMMON_MSG_MASK<<5)|CAN_ID_STD|CAN_RTR_DATA;	//设置成一样
	u16CAN_FilterMaskIdLow = (CANID_RX_COMMON_MSG_FILTER<<5)|CAN_ID_STD|CAN_RTR_DATA;

	CAN_FilterInitStructure.CAN_FilterNumber = 0;							//指定过滤器为0，如果想接收多几个，范围为0——13
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;			//指定过滤器为屏蔽模式
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;		//过滤器位宽为16位，也即2个带屏蔽位的标准帧
	CAN_FilterInitStructure.CAN_FilterIdHigh = u16CAN_FilterIdHigh;			//过滤器标识符的高16位值
	CAN_FilterInitStructure.CAN_FilterIdLow = u16CAN_FilterIdLow;			//过滤器标识符的低16位值
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = u16CAN_FilterMaskIdHigh;	//过滤器屏蔽标识符的高16位值
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = u16CAN_FilterMaskIdLow;	//过滤器屏蔽标识符的低16位值
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;	//设定了指向过滤器的FIFO为0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;					//使能过滤器
	CAN_FilterInit(&CAN_FilterInitStructure);								//按上面的参数初始化过滤器
}


void InitCan_CAN1(void) {
	CAN_InitTypeDef		CAN_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); 	//CAN1 模块时钟使能
															//APB1时钟最高36MHz

	CAN_DeInit(CAN1); 								//将外设CAN的全部寄存器重设为缺省值
	CAN_StructInit(&CAN_InitStructure);				//把CAN_InitStruct中的每一个参数按缺省值填入

	CAN_InitStructure.CAN_TTCM = DISABLE;			//没有使能时间触发模式
	CAN_InitStructure.CAN_ABOM = DISABLE;			//没有使能自动离线管理，BusOFF自动离线取消，需要手动处理
	CAN_InitStructure.CAN_AWUM = DISABLE;			//没有使能自动唤醒模式
	CAN_InitStructure.CAN_NART = DISABLE;			//没有使能非自动重传模式
	CAN_InitStructure.CAN_RFLM = DISABLE;			//没有使能接收FIFO锁定模式
	CAN_InitStructure.CAN_TXFP = DISABLE;			//没有使能发送FIFO优先级
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	//CAN设置为正常模式
	//CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;

	//关于以下的设置，sample=(1+CAN_BS1)/(1+CAN_BS1+CAN_BS2)，采样点设置在80%到87.5%之间比较好。
	//如果can采样点选取合适，can总线就能容纳更多的can节点。因此极其重要。
	//如果这个不行，就改为那个PDF里面的常用参考参数
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq; 		//重新同步跳跃宽度1个时间单位
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq; 		//时间段1为3个时间单位
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq; 		//时间段2为2个时间单位
	CAN_InitStructure.CAN_Prescaler = 12;  			//时间单位长度为60
	CAN_Init(CAN1,&CAN_InitStructure);				//波特率为：72M/2/6/(1+8+3)=0.5 即500K，非PDF范例
													//波特率为：72M/2/12/(1+3+2)=0.5 即500K，为DPF的范例
													//波特率为：72M/2/24/(1+3+2)=0.25 即250K，为DPF的范例

	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE); 		//使能FIFO0消息挂号中断	
}


void Can_BusOFF_FaultTimeCtrl(void) {
    if(TRUE == Can_Status_Flag.bits.b1Can_BusOFF) {
        g_u16BusOff_InitTestCnt++;			    //BUSOFF计时
    }
    if((FALSE == (CAN1->ESR&CAN_ESR_BOFF))&&(FALSE == Can_Status_Flag.bits.b1Can_BusOFF)) {
        g_u16BusOff_RecoverCnt++;		    	//BUSOFF清除计时
    }
}


void Can_BusOFF_FaultChk(void) {
    static UINT8 s_u8FlagBusOff = 0;
    if(FALSE == Can_Status_Flag.bits.b1Can_BusOFF) {
        if(CAN1->ESR&CAN_ESR_BOFF) {					//检测到BusOff，总线进入离线状态，找不到相关函数，自己写
            Can_Status_Flag.bits.b1Can_BusOFF = TRUE;	//找到了 --> CAN_GetFlagStatus(CAN1, CAN_FLAG_BOF)==SET
            CAN1->MCR |= CAN_MCR_INRQ;			//置位，从正常模式转为初始化模式(一旦当前的CAN活动(发送或接收)结束，CAN就进入初始化模式)
            s_u8FlagBusOff = 1;
            g_u16BusOff_RecoverCnt = 0;			//时序计算初始化
			g_u16BusOff_InitTestCnt = 0;
			System_ERROR_UserCallback(ERROR_CAN);
        }
    }

    if(1 == s_u8FlagBusOff) {							//下一轮过来置回环模式
        s_u8FlagBusOff = 0;
        //CAN1->BTR =	(UINT32)CAN_Mode_LoopBack<<30;    	//请求环回模式，不需要静默回环模式，收到需要发ACK到总线上。
        									//真的需要改为回环模式吗，打个问号？软件INRQ位处理可在初始化模式和正常模式切换
	}
}


void Can_BusOFF_Recover(void) {
	UINT16 u16BusOFF_InitCycleT;
	static UINT8 s_u8BusOFF_InitCnt = 0;
	if(TRUE == Can_Status_Flag.bits.b1Can_BusOFF) {
		if(s_u8BusOFF_InitCnt < 10) {               		//快恢复计时10次
	        u16BusOFF_InitCycleT = DELAYB10MS_100MS;        //快恢复计时10次100ms
	    }
	    else {
	        s_u8BusOFF_InitCnt = 10;
	        u16BusOFF_InitCycleT = DELAYB10MS_1S;           //1s周期
	    }

	    if(g_u16BusOff_InitTestCnt >= u16BusOFF_InitCycleT) {	//周期初始化CAN，前10次为100ms，后面为1s
	        s_u8BusOFF_InitCnt++;
	        g_u16BusOff_InitTestCnt = 0;
	        Can_Status_Flag.bits.b1Can_BusOFF_TestSd = 1;  	//时间到尝试发送Test报文
	        Can_Status_Flag.bits.b1Can_BusOFF = FALSE;
	        //CAN1->BTR =	(UINT32)CAN_Mode_Normal<<30;		//请求正常模式
	        CAN1->MCR &= ~CAN_MCR_INRQ;		//复位，从初始化模式转为正常模式(当CAN在接收引脚检测到连续的11个隐性位后，CAN就达到同步)
	    }
	}
	
    if(g_u16BusOff_RecoverCnt > DELAYB10MS_500MS) {      	//5S内未检测到BusOFF标志，则表示与外部通信恢复正常
        g_u16BusOff_RecoverCnt = DELAYB10MS_500MS;
        s_u8BusOFF_InitCnt = 0;
    }
}


void Can_BusOFF_Monitor(void) {
	Can_BusOFF_FaultChk();
	Can_BusOFF_FaultTimeCtrl();
	Can_BusOFF_Recover();
}


void Can_ReceiveDeal(void) {
	if(!Can_Status_Flag.bits.b1Can_Received) {
		return;
	}

	//ID已经过滤掉，不需要再判断ID是否为本机
	if((RxMessage.StdId>>7) != CAN_ADRESS_STD_ID) {
		return;
	}
	
	switch(RxMessage.StdId&0x007F) {	//按照逻辑，是这个函数处理接受数据，然后处理数据，然后赋值标志位返回才对
		case CANID_CHECK_0x00:
			CanTxType_Flag.bits.b1CanTx_0x00 = 1;
			break;
		case CANID_CHECK_0x01:
			CanTxType_Flag.bits.b1CanTx_0x01 = 1;
			break;
		case CANID_CHECK_0x02:
			CanTxType_Flag.bits.b1CanTx_0x02 = 1;
			break;
		case CANID_CHECK_0x03:
			CanTxType_Flag.bits.b1CanTx_0x03 = 1;
			break;
		case CANID_CHECK_0x04:
			CanTxType_Flag.bits.b1CanTx_0x04 = 1;
			break;
		case CANID_CHECK_0x05:
			CanTxType_Flag.bits.b1CanTx_0x05 = 1;
			break;
		case CANID_CHECK_0x06:
			CanTxType_Flag.bits.b1CanTx_0x06 = 1;
			break;
		case CANID_CHECK_0x07:
			CanTxType_Flag.bits.b1CanTx_0x07 = 1;
			break;
		case CANID_CHECK_0x08:
			CanTxType_Flag.bits.b1CanTx_0x08 = 1;
			break;
		case CANID_CHECK_0x09:
			CanTxType_Flag.bits.b1CanTx_0x09 = 1;
			break;
		case CANID_CHECK_0x0A:
			CanTxType_Flag.bits.b1CanTx_0x0A = 1;
			break;
		case CANID_CHECK_0x0B:
			CanTxType_Flag.bits.b1CanTx_0x0B = 1;
			break;
		case CANID_CHECK_0x0C:
			CanTxType_Flag.bits.b1CanTx_0x0C = 1;
			break;
		case CANID_CHECK_0x0D:
			CanTxType_Flag.bits.b1CanTx_0x0D = 1;
			break;
		case CANID_CHECK_0x0E:
			CanTxType_Flag.bits.b1CanTx_0x0E = 1;
			break;
		case CANID_CHECK_0x0F:
			CanTxType_Flag.bits.b1CanTx_0x0F = 1;
			break;
		case CANID_CHECK_0x10:
			CanTxType_Flag.bits.b1CanTx_0x10 = 1;
			break;
		case CANID_CHECK_0x11:
			CanTxType_Flag.bits.b1CanTx_0x11 = 1;
			break;

		default:
			break;
	}
	
	Can_Status_Flag.bits.b1Can_Received = 0;
}


void Can_TransmitDeal(void) {
	if(CanTxType_Flag.all) {
		RTC_ExtComCnt++;
	}

    if(CanTxType_Flag.bits.b1CanTx_0x00) {
        CanTxType_Flag.bits.b1CanTx_0x00 = 0;
		CAN_TX_0x00();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x01) {
        CanTxType_Flag.bits.b1CanTx_0x01 = 0;
		CAN_TX_0x01();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x02) {
       	CanTxType_Flag.bits.b1CanTx_0x02 = 0;
		CAN_TX_0x02();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x03) {
        CanTxType_Flag.bits.b1CanTx_0x03 = 0;
		CAN_TX_0x03();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x04) {
        CanTxType_Flag.bits.b1CanTx_0x04 = 0;
		CAN_TX_0x04();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x05) {
        CanTxType_Flag.bits.b1CanTx_0x05 = 0;
		CAN_TX_0x05();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x06) {
        CanTxType_Flag.bits.b1CanTx_0x06 = 0;
		CAN_TX_0x06();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x07) {
        CanTxType_Flag.bits.b1CanTx_0x07 = 0;
		CAN_TX_0x07();
    }
	else if(CanTxType_Flag.bits.b1CanTx_0x08) {
        CanTxType_Flag.bits.b1CanTx_0x08 = 0;
		CAN_TX_0x08();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x09) {
       	CanTxType_Flag.bits.b1CanTx_0x09 = 0;
		CAN_TX_0x09();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x0A) {
        CanTxType_Flag.bits.b1CanTx_0x0A = 0;
		CAN_TX_0x0A();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x0B) {
        CanTxType_Flag.bits.b1CanTx_0x0B = 0;
		CAN_TX_0x0B();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x0C) {
        CanTxType_Flag.bits.b1CanTx_0x0C = 0;
		CAN_TX_0x0C();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x0D) {
        CanTxType_Flag.bits.b1CanTx_0x0D = 0;
		CAN_TX_0x0D();
    }
	else if(CanTxType_Flag.bits.b1CanTx_0x0E) {
        CanTxType_Flag.bits.b1CanTx_0x0E = 0;
		CAN_TX_0x0E();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x0F) {
       	CanTxType_Flag.bits.b1CanTx_0x0F = 0;
		CAN_TX_0x0F();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x10) {
        CanTxType_Flag.bits.b1CanTx_0x10 = 0;
		CAN_TX_0x10();
    }
    else if(CanTxType_Flag.bits.b1CanTx_0x11) {
        CanTxType_Flag.bits.b1CanTx_0x11 = 0;
		CAN_TX_0x11();
    }
	else if(CanTxType_Flag.bits.b1CanTx_Test) {
        CanTxType_Flag.bits.b1CanTx_Test = 0;
        CAN_TX_Test();
    }
}


void InitCan(void) {
#if 0
	Can_Status_Flag.all = 0;
	CanTxType_Flag.all = 0;
	InitCan_GPIO();
	InitCan_NVIC();
	InitCan_Filter();
	InitCan_CAN1();			//目前是回环模式，要改回普通模式,Test
#endif
	Can_Status_Flag.all = 0;
	CanTxType_Flag.all = 0;
	InitCan_GPIO();
	InitCan_NVIC();
	InitCan_CAN1(); 		//目前是回环模式，要改回普通模式,Test
	InitCan_Filter();		//这个调到后面，RX也可以了
}


//这个函数不能用Switch架构来解决，因为这个都是并行任务，不是串行。
void App_Can(void) {
	if(STARTUP_CONT == System_FUNC_StartUp(SYSTEM_FUNC_STARTUP_CAN)) {
		//return;
	}

	if(0 == g_st_SysTimeFlag.bits.b1Sys10msFlag2) {
		return;
	}
	
	Can_BusOFF_Monitor();
	Can_ReceiveDeal();
	Can_TransmitDeal();
}


void App_CanTest(void) {
	if(0 == g_st_SysTimeFlag.bits.b1Sys200msFlag1) {
		return;
    }
	CAN_TX_Test();
}


void USB_LP_CAN1_RX0_IRQHandler(void) {
	//CanRxMsg RxMessage;
	RxMessage.StdId=0x00;
	RxMessage.ExtId=0x00;
	RxMessage.IDE=0;
	RxMessage.DLC=0;
	RxMessage.FMI=0;
	RxMessage.Data[0]=0x00;
	RxMessage.Data[1]=0x00;    

	CAN_Receive(CAN1,CAN_FIFO0, &RxMessage); //接收FIFO0中的数据
	Can_Status_Flag.bits.b1Can_Received = 1;
}


