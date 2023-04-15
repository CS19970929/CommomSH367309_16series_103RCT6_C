#include "main.h"

volatile union Can_Status Can_Status_Flag;
volatile union CanTxType_Status CanTxType_Flag;
CanTxMsg TxMessage; 
CanRxMsg RxMessage;

UINT16 g_u16BusOff_InitTestCnt = 0;             //CAN���߹رռ�ʱ
UINT16 g_u16BusOff_RecoverCnt = 0;          	//5s��ʱ��־λ


void CAN_TX_Test(void) { 
	UINT8 TXCounter = 0, TXStatus = 0;
	UINT8 u8MailBoxUsed;
	TxMessage.StdId = CANID_TX_Test; 	//��׼��ʶ��
	TxMessage.ExtId = 0; 				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD;			//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;  				//��Ϣ�����ݳ���Ϊ8���ֽ�
	TxMessage.Data[0] = 1; 				//����
	TxMessage.Data[1] = 2;
	TxMessage.Data[2] = 3;
	TxMessage.Data[3] = 4;
	TxMessage.Data[4] = 5;
	TxMessage.Data[5] = 6;
	TxMessage.Data[6] = 7;
	TxMessage.Data[7] = 8;
	
	/*
	while((CAN_TxStatus_Ok!=CAN_TransmitStatus(CAN1,CAN_Transmit(CAN1,&TxMessage)))&&(++i<0xFFF));		//��������
	if(i >= 0xFF) {
		ERROR_UserCallback(CAN_ERROR);
	}
	*/
	u8MailBoxUsed = CAN_Transmit(CAN1,&TxMessage);
	//CAN_Transmit�Ѽ���Can�������̴��룬�ǳ�����
	//���++i��Ӳ����BusOFF�ǲ���ͻ�ģ�����������ڵ�����յĴ�����CAN_TxStatus_Pending��BusOFF�ֿ�
	do {
		//����Can�����ԭ�������仰������ִ�е�������mailbox0�����ģ�CAN_Transmit()�ط����Ŵ��ˣ�
		//TXStatus = CAN_TransmitStatus(CAN1,CAN_Transmit(CAN1,&TxMessage));
		TXStatus = CAN_TransmitStatus(CAN1, u8MailBoxUsed);
		TXCounter++;
	}while((TXStatus != CAN_TxStatus_Ok)&&(TXCounter < 0xFF));		//Fail��OK���ù�

	if(TXCounter >= 0xFF) {
		System_ERROR_UserCallback(ERROR_CAN);	//����Ӧ����һ��Pending_Error����Canģ�鲻������Ҫ����ô�ðɡ�
	}
}


void CAN_Tx_Data(CanTxMsg* Msg) {
	UINT8 TXCounter = 0, TXStatus = 0;
	UINT8 u8MailBoxUsed;

	Msg->StdId += ((UINT32)CAN_ADRESS_STD_ID<<7);				//�������ַĬ��Ϊ0
	u8MailBoxUsed = CAN_Transmit(CAN1,Msg);
	do {
		TXStatus = CAN_TransmitStatus(CAN1, u8MailBoxUsed);
		TXCounter++;
	}while((TXStatus == CAN_TxStatus_Failed)&&(TXCounter < 0xFF)); 	//Fail��OK���ù�

	if(TXCounter >= 0xFF) {
		System_ERROR_UserCallback(ERROR_CAN);		//����Ӧ����һ��Pending_Error����Canģ�鲻������Ҫ����ô�ðɡ�
	}
}


void CAN_TX_0x00(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x00;	//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

    u16_tmp16a = g_stCellInfoReport.u16VCellTotle;			//��ѹ��10mV
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

	
    if(g_stCellInfoReport.u16IDischg > 0) {					//�ܵ����������ͣ����Ϊ�����ŵ�Ϊ��
		u16_tmp16a = g_stCellInfoReport.u16IDischg*10;		//10mA
		u16_tmp16a = (0x7FFF - u16_tmp16a + 1) | 0x8000;		
	}
	else {
		u16_tmp16a = g_stCellInfoReport.u16Ichg*10; 		// ����ܵ���
	}
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);


	u16_tmp16a = g_stCellInfoReport.SocElement.u16CapacityNow;//ʣ������10mAh
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);


	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x01(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x01;	//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

    u16_tmp16a = g_stCellInfoReport.SocElement.u16CapacityFull;	//��������10mAh
 	TxMessage.Data[0] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[1] = (UINT8)(u16_tmp16a & 0xFF);

    u16_tmp16a = g_stCellInfoReport.SocElement.u16Cycle_times;	//ѭ������
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

	TxMessage.StdId = CANID_CHECK_0x02;			//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

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

	TxMessage.StdId = CANID_CHECK_0x03;	//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

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

	u16_tmp16a = 0x1234;				//��������
 	TxMessage.Data[2] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[3] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = 0x1234;				//����汾
 	TxMessage.Data[4] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[5] = (UINT8)(u16_tmp16a & 0xFF);

	u16_tmp16a = Sci_CRC16RTU(TxMessage.Data, 6);
 	TxMessage.Data[6] = (UINT8)((u16_tmp16a>>8) & 0xFF);   
    TxMessage.Data[7] = (UINT8)(u16_tmp16a & 0xFF);

	CAN_Tx_Data(&TxMessage);
}


void CAN_TX_0x04(void) {
	UINT16 u16_tmp16a;

	TxMessage.StdId = CANID_CHECK_0x04;	//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

    TxMessage.Data[0] = SeriesNum;			//��ش���
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

	TxMessage.StdId = CANID_CHECK_0x05;	//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

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

	TxMessage.StdId = CANID_CHECK_0x06;	//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

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

	TxMessage.StdId = CANID_CHECK_0x07;			//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

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

	TxMessage.StdId = CANID_CHECK_0x08;	//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

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

	TxMessage.StdId = CANID_CHECK_0x09;	//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

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

	TxMessage.StdId = CANID_CHECK_0x0A;	//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

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

	TxMessage.StdId = CANID_CHECK_0x0B;	//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

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

	TxMessage.StdId = CANID_CHECK_0x0C;			//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

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

	TxMessage.StdId = CANID_CHECK_0x0D;	//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

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

	TxMessage.StdId = CANID_CHECK_0x0E;	//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

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

	TxMessage.StdId = CANID_CHECK_0x0F;	//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

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

	TxMessage.StdId = CANID_CHECK_0x10;	//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

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

	TxMessage.StdId = CANID_CHECK_0x11;	//��׼��ʶ��
	TxMessage.ExtId = 0;				//��չ��ʶ��
	TxMessage.IDE = CAN_ID_STD; 		//ʹ�ñ�׼��ʶ��
	TxMessage.RTR = CAN_RTR_DATA;		//Ϊ����֡
	TxMessage.DLC = 8;					//��Ϣ�����ݳ���Ϊ8���ֽ�

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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA, ENABLE);	//���ù��ܺ�GPIOB�˿�ʱ��ʹ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			//Configure CAN pin: RX    // PD0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  		// ��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;			//Configure CAN pin: TX    // PD1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 	// �����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//GPIO_PinRemapConfig(GPIO_Remap2_CAN1, ENABLE);	 	//14:13λ  //0��ӳ�䵽PA11��PA12(Ĭ��)//1: ���á�
}

void InitCan_NVIC(void) {
    NVIC_InitTypeDef NVIC_InitStructure;

	/*���������ˣ�systemInit()�Ѿ�������Ӧ����
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

	//��������ô���ģ���ϸ����ͼ�������������ñ�
	//CAN_ID_STD֮��Ĳ���Ҫ��λ���ٷ��Ѿ�����
	//CAN_FilterMode_IdList���б�ģʽFBMx = 1
	//CAN_FilterScale_16bit�����������λ��FSCx = 0
	//�������������λ������λҲ��Ϊ��ʶ���б����Ը�4��CANID
	//�������Ǹ�λ��Ϊ��λ������λ��
	
	//stm32 can������λģʽ��
	//һ���Ǳ�ʶ���Ĵ���(������Filter)��һ��������λ�Ĵ���(Mask)��
	//��������λ�Ĵ�����Ϊ1��λ����Ӧ�ı�ʶ���Ĵ�����λ����Щλ�Ǳ���ƥ���
	//Ҳ����˵������ܵ���Message����ı�ʶ����ID�������Ӧ��λ�������ʶ���Ĵ������Ӧ��λ��ͬ�����ܱ����ܡ�

	//Զ��֡������
	u16CAN_FilterIdHigh = (CANID_RX_COMMON_MSG_MASK<<5)|CAN_ID_STD|CAN_RTR_DATA;
	u16CAN_FilterIdLow = (CANID_RX_COMMON_MSG_FILTER<<5)|CAN_ID_STD|CAN_RTR_DATA;
	
	u16CAN_FilterMaskIdHigh = (CANID_RX_COMMON_MSG_MASK<<5)|CAN_ID_STD|CAN_RTR_DATA;	//���ó�һ��
	u16CAN_FilterMaskIdLow = (CANID_RX_COMMON_MSG_FILTER<<5)|CAN_ID_STD|CAN_RTR_DATA;

	CAN_FilterInitStructure.CAN_FilterNumber = 0;							//ָ��������Ϊ0���������ն༸������ΧΪ0����13
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;			//ָ��������Ϊ����ģʽ
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;		//������λ��Ϊ16λ��Ҳ��2��������λ�ı�׼֡
	CAN_FilterInitStructure.CAN_FilterIdHigh = u16CAN_FilterIdHigh;			//��������ʶ���ĸ�16λֵ
	CAN_FilterInitStructure.CAN_FilterIdLow = u16CAN_FilterIdLow;			//��������ʶ���ĵ�16λֵ
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = u16CAN_FilterMaskIdHigh;	//���������α�ʶ���ĸ�16λֵ
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = u16CAN_FilterMaskIdLow;	//���������α�ʶ���ĵ�16λֵ
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;	//�趨��ָ���������FIFOΪ0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;					//ʹ�ܹ�����
	CAN_FilterInit(&CAN_FilterInitStructure);								//������Ĳ�����ʼ��������
}


void InitCan_CAN1(void) {
	CAN_InitTypeDef		CAN_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); 	//CAN1 ģ��ʱ��ʹ��
															//APB1ʱ�����36MHz

	CAN_DeInit(CAN1); 								//������CAN��ȫ���Ĵ�������Ϊȱʡֵ
	CAN_StructInit(&CAN_InitStructure);				//��CAN_InitStruct�е�ÿһ��������ȱʡֵ����

	CAN_InitStructure.CAN_TTCM = DISABLE;			//û��ʹ��ʱ�䴥��ģʽ
	CAN_InitStructure.CAN_ABOM = DISABLE;			//û��ʹ���Զ����߹���BusOFF�Զ�����ȡ������Ҫ�ֶ�����
	CAN_InitStructure.CAN_AWUM = DISABLE;			//û��ʹ���Զ�����ģʽ
	CAN_InitStructure.CAN_NART = DISABLE;			//û��ʹ�ܷ��Զ��ش�ģʽ
	CAN_InitStructure.CAN_RFLM = DISABLE;			//û��ʹ�ܽ���FIFO����ģʽ
	CAN_InitStructure.CAN_TXFP = DISABLE;			//û��ʹ�ܷ���FIFO���ȼ�
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	//CAN����Ϊ����ģʽ
	//CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;

	//�������µ����ã�sample=(1+CAN_BS1)/(1+CAN_BS1+CAN_BS2)��������������80%��87.5%֮��ȽϺá�
	//���can������ѡȡ���ʣ�can���߾������ɸ����can�ڵ㡣��˼�����Ҫ��
	//���������У��͸�Ϊ�Ǹ�PDF����ĳ��òο�����
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq; 		//����ͬ����Ծ���1��ʱ�䵥λ
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq; 		//ʱ���1Ϊ3��ʱ�䵥λ
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq; 		//ʱ���2Ϊ2��ʱ�䵥λ
	CAN_InitStructure.CAN_Prescaler = 12;  			//ʱ�䵥λ����Ϊ60
	CAN_Init(CAN1,&CAN_InitStructure);				//������Ϊ��72M/2/6/(1+8+3)=0.5 ��500K����PDF����
													//������Ϊ��72M/2/12/(1+3+2)=0.5 ��500K��ΪDPF�ķ���
													//������Ϊ��72M/2/24/(1+3+2)=0.25 ��250K��ΪDPF�ķ���

	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE); 		//ʹ��FIFO0��Ϣ�Һ��ж�	
}


void Can_BusOFF_FaultTimeCtrl(void) {
    if(TRUE == Can_Status_Flag.bits.b1Can_BusOFF) {
        g_u16BusOff_InitTestCnt++;			    //BUSOFF��ʱ
    }
    if((FALSE == (CAN1->ESR&CAN_ESR_BOFF))&&(FALSE == Can_Status_Flag.bits.b1Can_BusOFF)) {
        g_u16BusOff_RecoverCnt++;		    	//BUSOFF�����ʱ
    }
}


void Can_BusOFF_FaultChk(void) {
    static UINT8 s_u8FlagBusOff = 0;
    if(FALSE == Can_Status_Flag.bits.b1Can_BusOFF) {
        if(CAN1->ESR&CAN_ESR_BOFF) {					//��⵽BusOff�����߽�������״̬���Ҳ�����غ������Լ�д
            Can_Status_Flag.bits.b1Can_BusOFF = TRUE;	//�ҵ��� --> CAN_GetFlagStatus(CAN1, CAN_FLAG_BOF)==SET
            CAN1->MCR |= CAN_MCR_INRQ;			//��λ��������ģʽתΪ��ʼ��ģʽ(һ����ǰ��CAN�(���ͻ����)������CAN�ͽ����ʼ��ģʽ)
            s_u8FlagBusOff = 1;
            g_u16BusOff_RecoverCnt = 0;			//ʱ������ʼ��
			g_u16BusOff_InitTestCnt = 0;
			System_ERROR_UserCallback(ERROR_CAN);
        }
    }

    if(1 == s_u8FlagBusOff) {							//��һ�ֹ����ûػ�ģʽ
        s_u8FlagBusOff = 0;
        //CAN1->BTR =	(UINT32)CAN_Mode_LoopBack<<30;    	//���󻷻�ģʽ������Ҫ��Ĭ�ػ�ģʽ���յ���Ҫ��ACK�������ϡ�
        									//�����Ҫ��Ϊ�ػ�ģʽ�𣬴���ʺţ����INRQλ������ڳ�ʼ��ģʽ������ģʽ�л�
	}
}


void Can_BusOFF_Recover(void) {
	UINT16 u16BusOFF_InitCycleT;
	static UINT8 s_u8BusOFF_InitCnt = 0;
	if(TRUE == Can_Status_Flag.bits.b1Can_BusOFF) {
		if(s_u8BusOFF_InitCnt < 10) {               		//��ָ���ʱ10��
	        u16BusOFF_InitCycleT = DELAYB10MS_100MS;        //��ָ���ʱ10��100ms
	    }
	    else {
	        s_u8BusOFF_InitCnt = 10;
	        u16BusOFF_InitCycleT = DELAYB10MS_1S;           //1s����
	    }

	    if(g_u16BusOff_InitTestCnt >= u16BusOFF_InitCycleT) {	//���ڳ�ʼ��CAN��ǰ10��Ϊ100ms������Ϊ1s
	        s_u8BusOFF_InitCnt++;
	        g_u16BusOff_InitTestCnt = 0;
	        Can_Status_Flag.bits.b1Can_BusOFF_TestSd = 1;  	//ʱ�䵽���Է���Test����
	        Can_Status_Flag.bits.b1Can_BusOFF = FALSE;
	        //CAN1->BTR =	(UINT32)CAN_Mode_Normal<<30;		//��������ģʽ
	        CAN1->MCR &= ~CAN_MCR_INRQ;		//��λ���ӳ�ʼ��ģʽתΪ����ģʽ(��CAN�ڽ������ż�⵽������11������λ��CAN�ʹﵽͬ��)
	    }
	}
	
    if(g_u16BusOff_RecoverCnt > DELAYB10MS_500MS) {      	//5S��δ��⵽BusOFF��־�����ʾ���ⲿͨ�Żָ�����
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

	//ID�Ѿ����˵�������Ҫ���ж�ID�Ƿ�Ϊ����
	if((RxMessage.StdId>>7) != CAN_ADRESS_STD_ID) {
		return;
	}
	
	switch(RxMessage.StdId&0x007F) {	//�����߼����������������������ݣ�Ȼ�������ݣ�Ȼ��ֵ��־λ���زŶ�
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
	InitCan_CAN1();			//Ŀǰ�ǻػ�ģʽ��Ҫ�Ļ���ͨģʽ,Test
#endif
	Can_Status_Flag.all = 0;
	CanTxType_Flag.all = 0;
	InitCan_GPIO();
	InitCan_NVIC();
	InitCan_CAN1(); 		//Ŀǰ�ǻػ�ģʽ��Ҫ�Ļ���ͨģʽ,Test
	InitCan_Filter();		//����������棬RXҲ������
}


//�������������Switch�ܹ����������Ϊ������ǲ������񣬲��Ǵ��С�
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

	CAN_Receive(CAN1,CAN_FIFO0, &RxMessage); //����FIFO0�е�����
	Can_Status_Flag.bits.b1Can_Received = 1;
}


