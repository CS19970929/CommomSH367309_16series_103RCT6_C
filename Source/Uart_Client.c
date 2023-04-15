#include "main.h"

struct RS485MSG g_UartClientMsgPtr_SCI1;
UINT16 gu16_UartClientCommuErrCnt_SCI1 = 0;         //SCIͨ���쳣����
UINT8  gu8_UartClientTxEnable_SCI1 = 0;
UINT8  gu8_UartClientTxFinishFlag_SCI1 = 0;

struct RS485MSG g_UartClientMsgPtr_SCI2;
UINT16 gu16_UartClientCommuErrCnt_SCI2 = 0;         //SCIͨ���쳣����
UINT8  gu8_UartClientTxEnable_SCI2 = 0;
UINT8  gu8_UartClientTxFinishFlag_SCI2 = 0;

struct RS485MSG g_UartClientMsgPtr_SCI3;
UINT16 gu16_UartClientCommuErrCnt_SCI3 = 0;         //SCIͨ���쳣����
UINT8  gu8_UartClientTxEnable_SCI3 = 0;
UINT8  gu8_UartClientTxFinishFlag_SCI3 = 0;

UINT8 g_u8UartClientTxBuff[USART_TX_BUF_LEN];


void UartClient_DataInit(struct RS485MSG *s) {
	UINT16 i;

	s->ptr_no = 0;
	s->csr = RS485_STA_IDLE;
	s->enRs485CmdType = RS485_CMD_READ_REGS;
	for(i = 0; i < RS485_MAX_BUFFER_SIZE; i++) {
		s->u16Buffer[i] = 0;
	}
	for(i = 0; i < SCI_TX_BUF_LEN; i++) {
		g_u8UartClientTxBuff[i] = 0;
	}
}


void CRC8_Verify(struct RS485MSG *s) {
	UINT8 u8SciVerify;
	u8SciVerify = s->u16Buffer[s->ptr_no-1];
	if(u8SciVerify == CRC8((UINT8*)s->u16Buffer, s->ptr_no-1, CRC_KEY)) {		
		s ->AckType = RS485_ACK_POS;
	}
	else {
		s ->u16RdRegByteNum = 0;
		s ->AckType = RS485_ACK_NEG;
		s ->ErrorType = RS485_ERROR_CRC_ERROR;
	}
}


void Verify_Client(struct RS485MSG *s) {
	UINT8 u8SciVerify1,u8SciVerify2;
	u8SciVerify1 = s->u16Buffer[s->ptr_no-1];
	u8SciVerify2 = s->u16Buffer[2] + s->u16Buffer[3] + s->u16Buffer[9] + 1;
	if(u8SciVerify1 == u8SciVerify2) {		
		s ->AckType = RS485_ACK_POS;
	}
	else {
		s ->u16RdRegByteNum = 0;
		s ->AckType = RS485_ACK_NEG;
		s ->ErrorType = RS485_ERROR_CRC_ERROR;
	}
}


void UartClient_CRC_Verify(struct RS485MSG *s) {
#if 0
	switch(s->enRs485CmdType) {
		case UART_CLIENT_CMD_0x01:
			CRC8_Verify(s);
			break;
		case UART_CLIENT_CMD_0x02:
			CRC8_Verify(s);			//����в�ͬ�ķ�ʽ����ֿ���������Ը�
			break;
		default:
			s ->u16RdRegByteNum = 0;
			s ->AckType = RS485_ACK_NEG;
			s ->ErrorType = RS485_ERROR_NULL;
			break;
	}
#endif
	UINT16	u16SciVerify;
	UINT16	t_u16FrameLenth;

	t_u16FrameLenth = s->ptr_no - 2;
	u16SciVerify = s->u16Buffer[t_u16FrameLenth] + (s->u16Buffer[t_u16FrameLenth+1] << 8);
	if(u16SciVerify == Sci_CRC16RTU((UINT8 * )s->u16Buffer, t_u16FrameLenth)) { 	
		s ->AckType = RS485_ACK_POS;
	}
	else {
		s ->u16RdRegByteNum = 0;
		s ->AckType = RS485_ACK_NEG;
		s ->ErrorType = RS485_ERROR_CRC_ERROR;
	}
}


//��ʱ��д
void UartClient_RxDataDeal_0x01(struct RS485MSG *s,UINT8 t_u8BuffTemp[]) {
	#if 0
	UINT8 test;
	test = s->u16Buffer[5];
	#endif
}


//��ʱ��д
void UartClient_RxDataDeal_0x02(struct RS485MSG *s,UINT8 t_u8BuffTemp[]) {
	#if 0
	UINT8 test;
	test = s->u16Buffer[5];
	#endif
}


void UartClient_AckDataDeal_0x01(struct RS485MSG *s,UINT8 t_u8BuffTemp[]) {
	UINT16 i = 0, j = 0;
	UINT16 u16SciTemp;
	INT16 i16Temp;

	if(s->u16RdRegStartAddrActure >= 0x0081) {		//����ʱ�ڶ����ص����ѹ��Ϣ
		u16SciTemp = 0;
		for(j = 0; j < 16; j++){
			t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
			t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
		}
	//			u16ByteCount = 32;
	}
	else if(s->u16RdRegStartAddrActure >= 0x0071) {
		for(j = 0; j < 16; j++){
			u16SciTemp = g_stCellInfoReport.u16VCell[j];
			t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
			t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
		}
	//			u16ByteCount = 32;
	}
	else if(s->u16RdRegStartAddrActure >= 0x0031) { 	//����ʱ�ϱ���2����Spec and Status Query��Ϣ
		u16SciTemp = 0;
		for(j = 0; j < 64; j++){
			t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
			t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
		}
	//			u16ByteCount = 108;
	}
	else if(s->u16RdRegStartAddrActure >= 0x0010) {
		u16SciTemp = (g_stCellInfoReport.u16Ichg > 0) ? g_stCellInfoReport.u16Ichg : g_stCellInfoReport.u16IDischg;
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

		u16SciTemp = 0x0000;		//date & time: 2019.08.13 00:00:00
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

		u16SciTemp = 0x4E1A;
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

		u16SciTemp = 0;
		if(g_stCellInfoReport.u16Ichg > 0){ 			   //battery status
			u16SciTemp |= 1<<1; 	   //10
			u16SciTemp |= 0<<0;
		}else if(g_stCellInfoReport.u16IDischg > 0){
			u16SciTemp |= 1<<1; 	   //11
			u16SciTemp |= 1<<0;
		}else if(g_stCellInfoReport.u16IDischg == 0 && g_stCellInfoReport.u16Ichg == 0){
			u16SciTemp |= 0<<1; 	   //01
			u16SciTemp |= 1<<0;
		}

		u16SciTemp |= (1<<2);								//Error bit Enable flag 
		u16SciTemp |= ((g_stCellInfoReport.u16BalanceFlag1 > 0 ? 1 : 0)<<3);	//Cell balance status

		if(SleepElement.u8_ToSleepFlag == 1)			//��λ��1��5s�󣬽������ߣ����Ż�
			u16SciTemp |= (1<<4);							//Sleep status 
		else
			u16SciTemp |= (0<<4);

		if(SystemStatus.bits.b1Status_MOS_DSG)
			u16SciTemp |= (1<<5);							//Output Discharge status
		else
			u16SciTemp |= (0<<5);
		
		if(SystemStatus.bits.b1Status_MOS_CHG)
			u16SciTemp |= (1<<6);							//Output Charge status
		else
			u16SciTemp |= (0<<6);
		
		u16SciTemp |= (0<<7);								//Battery terminal status

		u16SciTemp |= 8<<0; 								//����
		u16SciTemp |= 9<<0;

		if(g_stCellInfoReport.u16Ichg > 0){ 				//battery status
			u16SciTemp |= 1<<11;		//10
			u16SciTemp |= 0<<10;
		}else if(g_stCellInfoReport.u16IDischg > 0){
			u16SciTemp |= 1<<11;		//11
			u16SciTemp |= 1<<10;
		}else if(g_stCellInfoReport.u16IDischg == 0 && g_stCellInfoReport.u16Ichg == 0){
			u16SciTemp |= 0<<11;		//01
			u16SciTemp |= 1<<10;
		}
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
			
		
		u16SciTemp = 0;
		u16SciTemp |= (g_stCellInfoReport.unMdlFault_Third.bits.b1IdischgOcp<<0);
		u16SciTemp |= 0<<1; 			   //SCD(Short Circuit Discharge) protection
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_Third.bits.b1BatOvp | g_stCellInfoReport.unMdlFault_Third.bits.b1CellOvp)<<2);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_Third.bits.b1BatUvp | g_stCellInfoReport.unMdlFault_Third.bits.b1CellUvp)<<3);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgOtp)<<4);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgOtp)<<5);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_Third.bits.b1CellDischgUtp)<<6);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_Third.bits.b1CellChgUtp)<<7);
		u16SciTemp |= (0<<8);				//soft start fail
		u16SciTemp |= (0<<9);				//Permanent Fault
		u16SciTemp |= (0<<10);				//Delta V Fail
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_Third.bits.b1IchgOcp)<<11);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_Third.bits.b1TmosOtp)<<12);				//MOS Over Temperature
		u16SciTemp |= (0<<13);				//Environment Over Temperature
		u16SciTemp |= (0<<14);				//Environment Under Temperature
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
		
		u16SciTemp = g_stCellInfoReport.SocElement.u16Soc;
		//u16SciTemp = 45;			//����
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
				
		u16SciTemp = g_stCellInfoReport.u16VCellTotle;
		//u16SciTemp = 24;			//����
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
				
		if(g_stCellInfoReport.u16Ichg > 0) {
			u16SciTemp = g_stCellInfoReport.u16Ichg;	// ����ܵ���
		}
		else {
			u16SciTemp = g_stCellInfoReport.u16IDischg;
		}
		//u16SciTemp = 10;			//����
		u16SciTemp = u16SciTemp * 10;
		if(g_stCellInfoReport.u16IDischg > 0)		//�ŵ磬A*100
		{
			u16SciTemp = (0x7fff - u16SciTemp + 1) | 0x8000;
		}
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
				
		i16Temp = ((INT16)g_stCellInfoReport.u16TempMax) / 10 - 40;
		t_u8BuffTemp[i++] = (i16Temp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = i16Temp & 0x00FF;

		//u16SciTemp = PRT_E2ROMParas.u16IchgOcp_First; 			//�ؼ����������ָ��
		u16SciTemp = InverterChgCurve()*10;
		//u16SciTemp = 5000;			//����
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
			
		u16SciTemp = g_stCellInfoReport.SocElement.u16CapacityFull; //��������Ah*100(SOC = 100%)
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;		

		u16SciTemp = g_stCellInfoReport.SocElement.u16CapacityFactory;	//��������Ah*100
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;		
			
		u16SciTemp = 0x0203;								//FW/YW
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
		
		u16SciTemp = g_stCellInfoReport.u16VCellDelta / 1000;
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

		u16SciTemp = g_stCellInfoReport.SocElement.u16Cycle_times;
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

		u16SciTemp = 0; 									//RSVD For Master Box
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

		u16SciTemp = g_stCellInfoReport.SocElement.u16Soh;	//���������ٷֱ�SOH
		u16SciTemp |= (1<<7);	//Bit 0~ Bit6 SOH Counters; Bit7:SOH Flag
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

		if(SeriesNum == 8) {
			u16SciTemp = 2920;								//CV  Voltage  List����ѹ��3.65*8=29.2V
		}
		else if(SeriesNum == 16) {
			u16SciTemp = 5840;
		}
		//u16SciTemp = 2840;			//����
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

		u16SciTemp = 0; 			//����
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_First.bits.b1CellOvp)<<0);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_First.bits.b1CellUvp)<<1);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_First.bits.b1BatOvp)<<2);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_First.bits.b1BatUvp)<<3);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_First.bits.b1IdischgOcp)<<4);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_First.bits.b1IchgOcp)<<5);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_First.bits.b1CellDischgOtp)<<6);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_First.bits.b1CellDischgUtp)<<7);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_First.bits.b1CellChgOtp)<<8);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_First.bits.b1CellChgUtp)<<9);
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_First.bits.b1TmosOtp)<<10);				//MOS Over Temperature
		u16SciTemp |= (0<<11);				//Environment Over Temperature
		u16SciTemp |= (0<<12);				//Environment Under Temperature
		u16SciTemp |= ((g_stCellInfoReport.unMdlFault_First.bits.b1BatUvp)<<13);				//ϵͳ��ѹ�ػ�ǰ�澯
		u16SciTemp |= (0<<14);				//�������
		u16SciTemp |= (0<<15);				//
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

		//u16SciTemp = PRT_E2ROMParas.u16IdsgOcp_First; 			//�ؼ����ŵ�����ָ��
		u16SciTemp = InverterDsgCurve()*10; 						//A*100 = 10mA
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

		u16SciTemp = 0; 											//Alpha Extended Error code
		t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
		t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
			
		for(j = 0; j < 12; j++){									//ʣ���û�õ�
			t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
			t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
		}
	//			u16ByteCount = 66;
	}
	else if(s->u16RdRegStartAddrActure >= 0x0001) {
		u16SciTemp = 0;
		for(j = 0; j < 15; j++){
			t_u8BuffTemp[i++] = (u16SciTemp>>8)& 0x00FF;
			t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
		}
	//			u16ByteCount = 30;
	}

}

void UartClient_AckDataDeal_0x02(struct RS485MSG *s,UINT8 t_u8BuffTemp[]) {
	t_u8BuffTemp[0] = SeriesNum;
	t_u8BuffTemp[1] = g_stCellInfoReport.SocElement.u16Soc;
}


//���ø�
void UartClient_DataDeal_0x01(struct RS485MSG *s) {
	#if 0
	UartClient_RxDataDeal_0x01(s, t_u8BuffTemp);
	#endif

	UINT16	t_u16Temp;

	t_u16Temp = s->u16Buffer[3] + (s->u16Buffer[2] << 8);
	s->u16RdRegStartAddrActure = t_u16Temp;
	if(t_u16Temp >= 0x0081 && t_u16Temp <= 0x0090) {	 //Box����ʱ�ϱ���2���ص����ѹ��Ϣ
		t_u16Temp -= 0x0081;
	}
	else if(t_u16Temp >= 0x0071 && t_u16Temp <= 0x0080) {	 //16����ش���ѹ
		t_u16Temp -= 0x0071;
	}
	else if(t_u16Temp == 0x0070) {//����ʶ��ͬһ��BMS�µĲ�ͬ��������Ϣ
		t_u16Temp -= 0x0070;
	}
	else if(t_u16Temp >= 0x0031 && t_u16Temp <= 0x006F) {//Box����ʱ�ϱ���2����Spec and Status Query��Ϣ
		t_u16Temp = 0x0031;
	}
	else if(t_u16Temp >= 0x0010 && t_u16Temp <= 0x0030) {//״̬��ѯ
		t_u16Temp -= 0x0010;
	}
	else if(t_u16Temp >= 0x0001 && t_u16Temp <= 0x000F) {//15��˵����ѯ
		t_u16Temp -= 0x0001;
	}
	else{
		s ->AckType = RS485_ACK_NEG;
		s ->ErrorType = RS485_ERROR_CMD_INVALID;		//����Ŵ���
	}
	s ->u16RdRegStartAddr = t_u16Temp;
	s ->u16RdRegByteNum = (s->u16Buffer[5] + (s->u16Buffer[4] << 8))<<1;
}


//���ø�
void UartClient_DataDeal_0x02(struct RS485MSG *s) {
	//UartClient_RxDataDeal_0x02(s, g_u8UartClientTxBuff);
	s ->AckType = RS485_ACK_NEG;
	s ->ErrorType = RS485_ERROR_CMD_INVALID;
}

void UartClient_ACK_0x01(struct RS485MSG *s) {
	UINT16 i;
	UINT16 u16SciTemp;
	
	if(s->AckType == RS485_ACK_POS) {
		UartClient_AckDataDeal_0x01(s, g_u8UartClientTxBuff);

		//ͷ�룬ǰ�����ֽڱ��ֲ���
		s->u16Buffer[0] = (s->u16Buffer[0] != 0)?RS485_SLAVE_ADDR:s->u16Buffer[0];
		s->u16Buffer[1] = s->enRs485CmdType;
		s->u16Buffer[2] = s->u16RdRegByteNum;
		//����
		for(i = 0; i < (s->u16RdRegByteNum); i++ ) {
			s->u16Buffer[i+3] = g_u8UartClientTxBuff[i + ((s->u16RdRegStartAddr)<<1)];
		}
		i =  s->u16RdRegByteNum + 3;
	}
	else {
		//s->csr = RS485_STA_TX_COMPLETE;
		//g_u8UartClientTxFinishFlag = TRUE;	//ֱ�ӽ���
		//s->AckLenth = 0;
		//��Ϊ���أ����ﲻ��Ҫ�������Ҫ�ش�����Ϣ����ο���λ����׼modbusЭ��
	}

	u16SciTemp = Sci_CRC16RTU((UINT8 * )s->u16Buffer, i);
	s->u16Buffer[i++] = u16SciTemp & 0x00FF;
	s->u16Buffer[i++] = u16SciTemp >> 8;
	s->AckLenth = i;
	
	s->ptr_no = 0;
	s->csr = RS485_STA_TX_COMPLETE; 
}


void UartClient_ACK_0x02(struct RS485MSG *s,UINT8 t_u8BuffTemp[]) {
	UINT8   i=0,j=0;	
	if(s->AckType == RS485_ACK_POS) {
		s->u16Buffer[i++] = UART_CLIENT_HEAD_ADDR;
		s->u16Buffer[i++] = s->enRs485CmdType + 0x20;		//�յ�A1,A2,����C1,C2
		while(j < 8) {
			s->u16Buffer[i++] = t_u8BuffTemp[j++];
		}
		//s->u16Buffer[i++] = s->u16Buffer[2] + s->u16Buffer[3] + s->u16Buffer[9] + 1;
		s->u16Buffer[i++] = CRC8((UINT8*)s->u16Buffer, 10, CRC_KEY);
		s->u16Buffer[i++] = UART_CLIENT_END_VALUE;
	}
	else {
		s->u16Buffer[i++] = UART_CLIENT_HEAD_ADDR;
		s->u16Buffer[i++] = s->enRs485CmdType + 0x20;		//�յ�A1,A2,����C1,C2
		//s->u16Buffer[i++] = 1;
		s->u16Buffer[i++] = CRC8((UINT8*)s->u16Buffer, 2, CRC_KEY);
		s->u16Buffer[i++] = UART_CLIENT_END_VALUE;
		++gu16_UartClientCommuErrCnt_SCI3;						//������
	}
	s->AckLenth = i;
	s->ptr_no = 0;
	s->csr = RS485_STA_TX_COMPLETE; 
}


#if (defined _CLIENT_SCI1)
void UartClient_FaultChk_SCI1(void) {
	UINT8 FaultCnt = 0;
	if(USART1->SR&0x08) {		//�����������RXNEIE��EIEʹ�ܲ����жϣ���
		USART1->SR &= ~0x08;
		FaultCnt++;
	}

	if(USART1->SR&0x04) {		//��⵽������Ĭ�Ͽ��������Ļ�CR3��ONEBIT��1������
								//USART_CR3��EIEʹ���ж�
		USART1->SR &= ~0x04;
		FaultCnt++;
	}

	if(USART1->SR&0x02) {		//֡����USART_CR3��EIEʹ���жϣ���
		USART1->SR &= ~0x02;	//���
		FaultCnt++;
	}

	if(USART1->SR&0x01) {		//��żУ������־ USART_CR1��PEIEʹ�ܸ��жϣ�����
		USART1->SR &= ~0x01;	//���
		FaultCnt++;
	}

	if(FaultCnt) {
		gu16_UartClientCommuErrCnt_SCI1++;
	}	
}


void UartClient_Rx_Deal_SCI1(struct RS485MSG *s) {
	USART1->CR1 &= ~(1<<5);						//��ֹEUSART�����ж�
	s->u16Buffer[s->ptr_no] = USART1->DR;
	//s->u16Buffer[s->ptr_no] = (UINT8)USART_ReceiveData9(USART2);	//����żУ�飬������ż��żУ��λ
																	//���������żУ����󣬻����жϺ���������
	if((s->ptr_no == 0) && (s->u16Buffer[0] != RS485_SLAVE_ADDR)&& (s->u16Buffer[0] != RS485_BROADCAST_ADDR)){
		s->ptr_no = 0;
		s->u16Buffer[0] = 0;
	}
	else {
		if(s->ptr_no == 1) {
			switch(s->u16Buffer[s->ptr_no]) {
				case RS485_CMD_READ_REGS:
					s->enRs485CmdType = RS485_CMD_READ_REGS;
					break;
				case RS485_CMD_WRITE_REGS:
					s->enRs485CmdType = RS485_CMD_WRITE_REGS;
					break;
				default:
					s->ptr_no = RS485_MAX_BUFFER_SIZE;
					s->u16Buffer[0] = 0;
					s->u16Buffer[1] = 0;
					break;
			}
		}
		else if(s->ptr_no > 2){
			switch(s->enRs485CmdType) {
				case RS485_CMD_READ_REGS: 
					if (s->ptr_no == 7) {		//receive complete
						s->csr = RS485_STA_RX_COMPLETE;
						USART1->CR1 &= ~(1<<2);	//��ֹ����
						USART1->CR1 &= ~(1<<5);	//��ֹEUSART�����ж�
					}
					break;
					
				case RS485_CMD_WRITE_REGS:
					if((s->ptr_no > 7)&&(s->ptr_no == (s->u16Buffer[6] + 8))) {		//�������������ı��
						s->csr = RS485_STA_RX_COMPLETE;
						USART1->CR1 &= ~(1<<2);	//��ֹ����
						USART1->CR1 &= ~(1<<5);	//��ֹEUSART�����ж�
					}
					break;
				default: {
					s->ptr_no = RS485_MAX_BUFFER_SIZE;
					s->u16Buffer[0] = 0;
					break;
				}
			}
		}
		
		s->ptr_no++;
		if(s->ptr_no >= RS485_MAX_BUFFER_SIZE) {
			s->ptr_no = 0;
			s->u16Buffer[0] = 0;
		}
	}
	
	USART1->CR1 |= (1<<5);	//ʹ��EUSART�����ж�
}


void UartClient_Tx_Deal_SCI1(struct RS485MSG *s ) {
	if(0 == gu8_UartClientTxEnable_SCI1) {
		return;
	}

	if(gu16_UartClientCommuErrCnt_SCI1) {	//������żУ�����Ҳ�ð�����ȫ�������꣬Ȼ�󲻻ظ�
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_UartClientTxFinishFlag_SCI1 = 1;
		gu8_UartClientTxEnable_SCI1 = 0;
		gu16_UartClientCommuErrCnt_SCI1 = 0;
		return;
	}
	
	while(!((USART1->SR)&(1<<7)));				// 1<<6 Ҳ����
	if( s->ptr_no < s -> AckLenth) {
        USART1->DR = s->u16Buffer[s->ptr_no];	//load data
        //USART_SendData9(USART1, Usart_9bitOddEvenData_Frame(s->u16Buffer[s->ptr_no], ODD));	//��֪���Ƿ������Ҫ�Լ����㣬������
        
		s->ptr_no++;
	}
	else {
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_UartClientTxFinishFlag_SCI1 = 1;
		gu8_UartClientTxEnable_SCI1 = 0;
	}
}


void InitSCI1_UartClient(void) {
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);		//PB6,PB7
	
	//Enable the USART1 Interrupt(ʹ��USART1�ж�)
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 


	//USART1_TX   PA9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //PA9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������������������ǿ�©���
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//USART1_RX	  PA10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//�������룬��RX
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	//���ڳ�ʼ��
    USART_InitStructure.USART_BaudRate = 9600;							//���ô��ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;			//��������λ���������żУ�飬���������λҪ��Ϊ9λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;				//����ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;					//����Ч��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//����������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//���ù���ģʽ
    USART_Init(USART1, &USART_InitStructure);

	USART1->CR3 |= 1<<0;	//EIE����֡�����жϣ�ͬʱ���������ж�

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);		   	//ʹ�ܽ����ж�
    USART_Cmd(USART1, ENABLE);								//ʹ�ܴ���1
    
    UartClient_DataInit(&g_UartClientMsgPtr_SCI1);	
}


void App_UartClient_SCI1(struct RS485MSG *s) {
	static UINT8 ResponseDelayFlag = 0;
	switch(s->csr) {
		//IDLE-����̬������50ms��ʹ�ܽ��գ�����㣩receive set
		case RS485_STA_IDLE: {
			break;
		}
		//receive complete, to deal the receive data
		case RS485_STA_RX_COMPLETE: {
			USART1->CR1 &= ~(1<<5);		//��ֹ�����ж�
			UartClient_CRC_Verify(s);
			if(s ->AckType == RS485_ACK_POS) {
				switch(s->enRs485CmdType) {
					case RS485_CMD_READ_REGS:
						UartClient_DataDeal_0x01(s);
						break;
					case RS485_CMD_WRITE_REGS:
						UartClient_DataDeal_0x02(s);
						break;
					default:
						s ->u16RdRegByteNum = 0;
						s ->AckType = RS485_ACK_NEG;
						s ->ErrorType = RS485_ERROR_NULL;
						break;
				}
			}
			s->csr = RS485_STA_RX_OK;	//receive the correct data, switch to transmit wait 50ms
			break;              		//��һ������
		}
		//receive ok, to transmit wait 50ms
		case RS485_STA_RX_OK: {
			if(ResponseDelayFlag >= 2) {		//�ӳ�20ms�ٻظ�
				ResponseDelayFlag = 0;

				switch(s->enRs485CmdType) {
					case RS485_CMD_READ_REGS:
						UartClient_ACK_0x01(s);
						MCUO_RJ45_LED = !MCUO_RJ45_LED;
						break;
					case RS485_CMD_WRITE_REGS:
	//					UartClient_ACK_0x02(s, g_u8UartClientTxBuff);

						s->csr = RS485_STA_TX_COMPLETE;
						gu8_UartClientTxFinishFlag_SCI1 = TRUE;	//ֱ�ӽ���

						MCUO_RJ45_LED = !MCUO_RJ45_LED;
						break;
					default:				//���defualt���üӴ������
						break;
				}
				USART1->CR1 |= (1<<3); 		//ʹ�ܷ���
				gu8_UartClientTxEnable_SCI1 = 1;
			}
			else if(g_st_SysTimeFlag.bits.b1Sys10msFlag1) {
				ResponseDelayFlag++;
			}
			
			if(RS485_ERROR_NULL == s ->ErrorType) {
				s->csr = RS485_STA_TX_COMPLETE;
				gu8_UartClientTxFinishFlag_SCI1 = TRUE;		//ֱ�ӽ���
				s->AckLenth = 0;
			}
		}
		//transmit complete, to switch receive wait 20ms
		case RS485_STA_TX_COMPLETE: {
			if(gu8_UartClientTxFinishFlag_SCI1) {
				s->csr = RS485_STA_IDLE;
				s->u16Buffer[0] = 0;
				s->u16Buffer[1] = 0;
				s->u16Buffer[2] = 0;
				s->u16Buffer[3] = 0;
				gu8_UartClientTxFinishFlag_SCI1 = 0;
				s->ptr_no = 0;
				USART1->CR1 |= (1<<2);	//ʹ�ܽ���
				USART1->CR1 |= (1<<5);	//ʹ��EUSART�����ж�
				//TXSTA1bits.TXEN = 0;    //��ֹ���ͣ����ܽ�ֹ������ͨ�ź����׶�
				gu8_UartClientTxEnable_SCI1 = 0;
			}
			break;
		}
		
		default: {
			s->csr = RS485_STA_IDLE;
			break;
		}
	}

	UartClient_Tx_Deal_SCI1(s);
	//UartClient_FaultChk();		//STMϵ�е�д����������ô�
}
#endif


#if (defined _CLIENT_SCI2)
void UartClient_FaultChk_SCI2(void) {
	UINT8 FaultCnt = 0;
	if(USART2->SR&0x08) {		//�����������RXNEIE��EIEʹ�ܲ����жϣ���
		USART2->SR &= ~0x08;
		FaultCnt++;
	}

	if(USART2->SR&0x04) {		//��⵽������Ĭ�Ͽ��������Ļ�CR3��ONEBIT��1������
								//USART_CR3��EIEʹ���ж�
		USART2->SR &= ~0x04;
		FaultCnt++;
	}

	if(USART2->SR&0x02) {		//֡����USART_CR3��EIEʹ���жϣ���
		USART2->SR &= ~0x02;	//���
		FaultCnt++;
	}

	if(USART2->SR&0x01) {		//��żУ������־ USART_CR1��PEIEʹ�ܸ��жϣ�����
		USART2->SR &= ~0x01;	//���
		FaultCnt++;
	}
	
	if(FaultCnt) {
		gu16_UartClientCommuErrCnt_SCI2++;
	}	

}


void UartClient_Rx_Deal_SCI2(struct RS485MSG *s) {
	USART2->CR1 &= ~(1<<5);						//��ֹEUSART�����ж�
	s->u16Buffer[s->ptr_no] = USART2->DR;
	//s->u16Buffer[s->ptr_no] = (UINT8)USART_ReceiveData9(USART2);	//����żУ�飬������ż��żУ��λ
																	//���������żУ����󣬻����жϺ���������
	if((s->ptr_no == 0) && (s->u16Buffer[0] != RS485_SLAVE_ADDR)&& (s->u16Buffer[0] != RS485_BROADCAST_ADDR)){
		s->ptr_no = 0;
		s->u16Buffer[0] = 0;
	}
	else {
		if(s->ptr_no == 1) {
			switch(s->u16Buffer[s->ptr_no]) {
				case RS485_CMD_READ_REGS:
					s->enRs485CmdType = RS485_CMD_READ_REGS;
					break;
				case RS485_CMD_WRITE_REGS:
					s->enRs485CmdType = RS485_CMD_WRITE_REGS;
					break;
				default:
					s->ptr_no = RS485_MAX_BUFFER_SIZE;
					s->u16Buffer[0] = 0;
					s->u16Buffer[1] = 0;
					break;
			}
		}
		else if(s->ptr_no > 2){
			switch(s->enRs485CmdType) {
				case RS485_CMD_READ_REGS: 
					if (s->ptr_no == 7) {		//receive complete
						s->csr = RS485_STA_RX_COMPLETE;
						USART2->CR1 &= ~(1<<2);	//��ֹ����
						USART2->CR1 &= ~(1<<5);	//��ֹEUSART�����ж�
					}
					break;
					
				case RS485_CMD_WRITE_REGS:
					if((s->ptr_no > 7)&&(s->ptr_no == (s->u16Buffer[6] + 8))) {		//�������������ı��
						s->csr = RS485_STA_RX_COMPLETE;
						USART2->CR1 &= ~(1<<2);	//��ֹ����
						USART2->CR1 &= ~(1<<5);	//��ֹEUSART�����ж�
					}
					break;
				default: {
					s->ptr_no = RS485_MAX_BUFFER_SIZE;
					s->u16Buffer[0] = 0;
					break;
				}
			}
		}
		
		s->ptr_no++;
		if(s->ptr_no >= RS485_MAX_BUFFER_SIZE) {
			s->ptr_no = 0;
			s->u16Buffer[0] = 0;
		}
	}
	
	USART2->CR1 |= (1<<5);	//ʹ��EUSART�����ж�
}


void UartClient_Tx_Deal_SCI2(struct RS485MSG *s ) {
	if(0 == gu8_UartClientTxEnable_SCI2) {
		return;
	}

	if(gu16_UartClientCommuErrCnt_SCI2) {	//������żУ�����Ҳ�ð�����ȫ�������꣬Ȼ�󲻻ظ�
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_UartClientTxFinishFlag_SCI2 = 1;
		gu8_UartClientTxEnable_SCI2 = 0;
		gu16_UartClientCommuErrCnt_SCI2 = 0;
		return;
	}
	
	while(!((USART2->SR)&(1<<7)));				// 1<<6 Ҳ����
	if( s->ptr_no < s->AckLenth) {
        USART2->DR = s->u16Buffer[s->ptr_no];	//load data
        //USART_SendData9(USART2, Usart_9bitOddEvenData_Frame(s->u16Buffer[s->ptr_no], ODD));	//��֪���Ƿ������Ҫ�Լ����㣬������
        
		s->ptr_no++;
	}
	else {
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_UartClientTxFinishFlag_SCI2 = 1;
		gu8_UartClientTxEnable_SCI2 = 0;
	}
}


void InitSCI2_UartClient(void) {
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//Enable the USART1 Interrupt(ʹ��USART1�ж�)
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 


	//USART2_TX   PA2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������������������ǿ�©���
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART2_RX	  PA3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//�������룬��RX
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//���ڳ�ʼ��
    USART_InitStructure.USART_BaudRate = 9600;							//���ô��ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;			//��������λ���������żУ�飬���������λҪ��Ϊ9λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;				//����ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;					//����Ч��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//����������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//���ù���ģʽ
    USART_Init(USART2, &USART_InitStructure);

	USART2->CR3 |= 1<<0;	//EIE����֡�����жϣ�ͬʱ���������ж�

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);		   	//ʹ�ܽ����ж�
    USART_Cmd(USART2, ENABLE);								//ʹ�ܴ���1
    
    UartClient_DataInit(&g_UartClientMsgPtr_SCI2);	

}


void App_UartClient_SCI2(struct RS485MSG *s) {
	static UINT8 ResponseDelayFlag = 0;
	switch(s->csr) {
		//IDLE-����̬������50ms��ʹ�ܽ��գ�����㣩receive set
		case RS485_STA_IDLE: {
			break;
		}
		//receive complete, to deal the receive data
		case RS485_STA_RX_COMPLETE: {
			USART2->CR1 &= ~(1<<5);		//��ֹ�����ж�
			UartClient_CRC_Verify(s);
			if(s ->AckType == RS485_ACK_POS) {
				switch(s->enRs485CmdType) {
					case RS485_CMD_READ_REGS:
						UartClient_DataDeal_0x01(s);
						break;
					case RS485_CMD_WRITE_REGS:
						UartClient_DataDeal_0x02(s);
						break;
					default:
						s ->u16RdRegByteNum = 0;
						s ->AckType = RS485_ACK_NEG;
						s ->ErrorType = RS485_ERROR_NULL;
						break;
				}
			}
			s->csr = RS485_STA_RX_OK;	//receive the correct data, switch to transmit wait 50ms
			break;              		//��һ������
		}
		//receive ok, to transmit wait 50ms
		case RS485_STA_RX_OK: {
			if(ResponseDelayFlag >= 2){		//�ӳ�20ms�ٻظ�
				ResponseDelayFlag = 0;

				switch(s->enRs485CmdType) {
					case RS485_CMD_READ_REGS:
						UartClient_ACK_0x01(s);
						//MCUO_RJ45_LED = !MCUO_RJ45_LED;
						break;
					case RS485_CMD_WRITE_REGS:
	//					UartClient_ACK_0x02(s, g_u8UartClientTxBuff);

						s->csr = RS485_STA_TX_COMPLETE;
						gu8_UartClientTxFinishFlag_SCI2 = TRUE;	//ֱ�ӽ���

						//MCUO_RJ45_LED = !MCUO_RJ45_LED;
						break;
					default:				//���defualt���üӴ������
						break;
				}
				USART2->CR1 |= (1<<3); 		//ʹ�ܷ���
				gu8_UartClientTxEnable_SCI2 = 1;
			}
			else if(g_st_SysTimeFlag.bits.b1Sys10msFlag1) {
				ResponseDelayFlag++;
			}
			
			if(RS485_ERROR_NULL == s ->ErrorType) {
				s->csr = RS485_STA_TX_COMPLETE;
				gu8_UartClientTxFinishFlag_SCI2 = TRUE;		//ֱ�ӽ���
				s->AckLenth = 0;
			}
		}
		//transmit complete, to switch receive wait 20ms
		case RS485_STA_TX_COMPLETE: {
			if(gu8_UartClientTxFinishFlag_SCI2) {
				s->csr = RS485_STA_IDLE;
				s->u16Buffer[0] = 0;
				s->u16Buffer[1] = 0;
				s->u16Buffer[2] = 0;
				s->u16Buffer[3] = 0;
				gu8_UartClientTxFinishFlag_SCI2 = 0;
				s->ptr_no = 0;
				USART2->CR1 |= (1<<2);	//ʹ�ܽ���
				USART2->CR1 |= (1<<5);	//ʹ��EUSART�����ж�
				//TXSTA1bits.TXEN = 0;    //��ֹ���ͣ����ܽ�ֹ������ͨ�ź����׶�
				gu8_UartClientTxEnable_SCI2 = 0;
			}
			break;
		}
		
		default: {
			s->csr = RS485_STA_IDLE;
			break;
		}
	}

	UartClient_Tx_Deal_SCI2(s);
	//UartClient_FaultChk();		//STMϵ�е�д����������ô�
}
#endif


#if (defined _CLIENT_SCI3)
void UartClient_FaultChk_SCI3(void) {
	UINT8 FaultCnt = 0;
	if(USART3->SR&0x08) {		//�����������RXNEIE��EIEʹ�ܲ����жϣ���
		USART3->SR &= ~0x08;
		FaultCnt++;
	}

	if(USART3->SR&0x04) {		//��⵽������Ĭ�Ͽ��������Ļ�CR3��ONEBIT��1������
								//USART_CR3��EIEʹ���ж�
		USART3->SR &= ~0x04;
		FaultCnt++;
	}

	if(USART3->SR&0x02) {		//֡����USART_CR3��EIEʹ���жϣ���
		USART3->SR &= ~0x02;	//���
		FaultCnt++;
	}

	if(USART3->SR&0x01) {		//��żУ������־ USART_CR1��PEIEʹ�ܸ��жϣ�����
		USART3->SR &= ~0x01;	//���
		FaultCnt++;
	}
	
	if(FaultCnt) {
		gu16_UartClientCommuErrCnt_SCI3++;
	}	

}


void UartClient_Rx_Deal_SCI3(struct RS485MSG *s) {
	USART3->CR1 &= ~(1<<5);						//��ֹEUSART�����ж�
	s->u16Buffer[s->ptr_no] = USART3->DR;
	//s->u16Buffer[s->ptr_no] = (UINT8)USART_ReceiveData9(USART3);	//����żУ�飬������ż��żУ��λ
																	//���������żУ����󣬻����жϺ���������
	if((s->ptr_no == 0) && (s->u16Buffer[0] != UART_CLIENT_HEAD_ADDR)) {
		s->ptr_no = 0;
		s->u16Buffer[0] = 0;
	}
	else {
		if(s->ptr_no == 1) {
			switch(s->u16Buffer[s->ptr_no]) {
				case UART_CLIENT_CMD_0x01:
					s->enRs485CmdType = (enum RS485_CMD_E)UART_CLIENT_CMD_0x01;
					break;
				case UART_CLIENT_CMD_0x02:
					s->enRs485CmdType = (enum RS485_CMD_E)UART_CLIENT_CMD_0x02;
					break;
				default:
					s->ptr_no = RS485_MAX_BUFFER_SIZE;
					s->u16Buffer[0] = 0;
					s->u16Buffer[1] = 0;
					break;
			}
		}
		else if(s->ptr_no >= 2) {
			switch(s->enRs485CmdType) {
				case UART_CLIENT_CMD_0x01: 
					if (s->ptr_no == 11) {		//receive complete
						s->csr = RS485_STA_RX_COMPLETE;
						USART3->CR1 &= ~(1<<2);	//��ֹ����
						USART3->CR1 &= ~(1<<5);	//��ֹEUSART�����ж�
					}
					break;
					
				case UART_CLIENT_CMD_0x02:
					if(s->ptr_no == 11) {		//�������������ı��
						s->csr = RS485_STA_RX_COMPLETE;
						USART3->CR1 &= ~(1<<2);	//��ֹ����
						USART3->CR1 &= ~(1<<5);	//��ֹEUSART�����ж�
					}
					break;
				default: {
					s->ptr_no = RS485_MAX_BUFFER_SIZE;
					s->u16Buffer[0] = 0;
					break;
				}
			}
		}
		
		s->ptr_no++;
		if(s->ptr_no >= RS485_MAX_BUFFER_SIZE) {
			s->ptr_no = 0;
			s->u16Buffer[0] = 0;
		}
	}
	
	USART3->CR2 |= (1<<5);	//ʹ��EUSART�����ж�
}


void UartClient_Tx_Deal_SCI3(struct RS485MSG *s ) {
	if(0 == gu8_UartClientTxEnable_SCI3) {
		return;
	}

	if(gu16_UartClientCommuErrCnt_SCI3) {	//������żУ�����Ҳ�ð�����ȫ�������꣬Ȼ�󲻻ظ�
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_UartClientTxFinishFlag_SCI3 = 1;
		gu8_UartClientTxEnable_SCI3 = 0;
		gu16_UartClientCommuErrCnt_SCI3 = 0;
		return;
	}
	
	while(!((USART3->SR)&(1<<7)));				// 1<<6 Ҳ����
	if( s->ptr_no < s -> AckLenth) {
        USART3->DR = s->u16Buffer[s->ptr_no];	//load data
        //USART_SendData9(USART3, Usart_9bitOddEvenData_Frame(s->u16Buffer[s->ptr_no], ODD));	//��֪���Ƿ������Ҫ�Լ����㣬������
        
		s->ptr_no++;
	}
	else {
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_UartClientTxFinishFlag_SCI3 = 1;
		gu8_UartClientTxEnable_SCI3 = 0;
	}
}


void InitSCI3_UartClient(void) {
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
	//Enable the USART3 Interrupt(ʹ��USART3�ж�)
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 


	//USART3_TX   PD8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������������������ǿ�©���
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//USART3_RX	  PD9��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//�������룬��RX
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//���ڳ�ʼ��
    USART_InitStructure.USART_BaudRate = 19200;							//���ô��ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;			//��������λ���������żУ�飬���������λҪ��Ϊ9λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;				//����ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;					//����Ч��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//����������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		//���ù���ģʽ
    USART_Init(USART3, &USART_InitStructure);

	USART3->CR3 |= 1<<0;	//EIE����֡�����жϣ�ͬʱ���������ж�

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);		   	//ʹ�ܽ����ж�
    USART_Cmd(USART3, ENABLE);								//ʹ�ܴ���3

    UartClient_DataInit(&g_UartClientMsgPtr_SCI3);	

}


void App_UartClient_SCI3(struct RS485MSG *s) {
	static UINT8 ResponseDelayFlag = 0;
	switch(s->csr) {
		//IDLE-����̬������50ms��ʹ�ܽ��գ�����㣩receive set
		case RS485_STA_IDLE: {
			break;
		}
		//receive complete, to deal the receive data
		case RS485_STA_RX_COMPLETE: {
			USART3->CR1 &= ~(1<<5);		//��ֹ�����ж�
			UartClient_CRC_Verify(s);
			if(s ->AckType == RS485_ACK_POS) {
				switch(s->enRs485CmdType) {
					case RS485_CMD_READ_REGS:
						UartClient_DataDeal_0x01(s);
						break;
					case RS485_CMD_WRITE_REGS:
						UartClient_DataDeal_0x02(s);
						break;
					default:
						s ->u16RdRegByteNum = 0;
						s ->AckType = RS485_ACK_NEG;
						s ->ErrorType = RS485_ERROR_NULL;
						break;
				}
			}
			s->csr = RS485_STA_RX_OK;	//receive the correct data, switch to transmit wait 50ms
			break;              		//��һ������
		}
		//receive ok, to transmit wait 50ms
		case RS485_STA_RX_OK: {
			if(ResponseDelayFlag >= 2){		//�ӳ�20ms�ٻظ�
				ResponseDelayFlag = 0;

				switch(s->enRs485CmdType) {
					case RS485_CMD_READ_REGS:
						UartClient_ACK_0x01(s);
						MCUO_RJ45_LED = !MCUO_RJ45_LED;
						break;
					case RS485_CMD_WRITE_REGS:
	//					UartClient_ACK_0x02(s, g_u8UartClientTxBuff);

						s->csr = RS485_STA_TX_COMPLETE;
						gu8_UartClientTxFinishFlag_SCI3 = TRUE;	//ֱ�ӽ���

						MCUO_RJ45_LED = !MCUO_RJ45_LED;
						break;
					default:				//���defualt���üӴ������
						break;
				}
				USART3->CR1 |= (1<<3); 		//ʹ�ܷ���
				gu8_UartClientTxEnable_SCI3 = 1;
			}
			else if(g_st_SysTimeFlag.bits.b1Sys10msFlag1) {
				ResponseDelayFlag++;
			}
			
			if(RS485_ERROR_NULL == s ->ErrorType) {
				s->csr = RS485_STA_TX_COMPLETE;
				gu8_UartClientTxFinishFlag_SCI3 = TRUE;		//ֱ�ӽ���
				s->AckLenth = 0;
			}
		}
		//transmit complete, to switch receive wait 20ms
		case RS485_STA_TX_COMPLETE: {
			if(gu8_UartClientTxFinishFlag_SCI3) {
				s->csr = RS485_STA_IDLE;
				s->u16Buffer[0] = 0;
				s->u16Buffer[1] = 0;
				s->u16Buffer[2] = 0;
				s->u16Buffer[3] = 0;
				gu8_UartClientTxFinishFlag_SCI3 = 0;
				s->ptr_no = 0;
				USART3->CR1 |= (1<<2);	//ʹ�ܽ���
				USART3->CR1 |= (1<<5);	//ʹ��EUSART�����ж�
				//TXSTA1bits.TXEN = 0;    //��ֹ���ͣ����ܽ�ֹ������ͨ�ź����׶�
				gu8_UartClientTxEnable_SCI3 = 0;
			}
			break;
		}
		
		default: {
			s->csr = RS485_STA_IDLE;
			break;
		}
	}

	UartClient_Tx_Deal_SCI3(s);
	//UartClient_FaultChk();		//STMϵ�е�д����������ô�
}
#endif


void InitUSART_UartClient(void) {
	#ifdef _CLIENT_SCI1
	InitSCI1_UartClient();
	#endif

	#ifdef _CLIENT_SCI2
	InitSCI2_UartClient();
	#endif

	#ifdef _CLIENT_SCI3
	InitSCI3_UartClient();
	#endif
}


void App_UartClient_Updata(void) {
	#ifdef _CLIENT_SCI1
	App_UartClient_SCI1(&g_UartClientMsgPtr_SCI1);
	#endif

	#ifdef _CLIENT_SCI2
	App_UartClient_SCI2(&g_UartClientMsgPtr_SCI2);
	#endif

	#ifdef _CLIENT_SCI3
	App_UartClient_SCI3(&g_UartClientMsgPtr_SCI3);
	#endif
}



#ifdef _SCI_COMBINE
void Sci_Rx_Deal_Client(struct RS485MSG *s) {
	//s->u16Buffer[s->ptr_no] = (UINT8)USART_ReceiveData9(USART1);	//����żУ�飬������ż��żУ��λ
																	//���������żУ����󣬻����жϺ���������
	if((s->ptr_no == 0) && (s->u16Buffer[0] != UART_CLIENT_HEAD_ADDR)) {	//��Ҫ������仰��ܰ�
		s->ptr_no = 0;
		s->u16Buffer[0] = 0;
	}
	else {
		if(s->ptr_no == 1) {
			switch(s->u16Buffer[s->ptr_no]) {
				case UART_CLIENT_CMD_0x01:
					s->enRs485CmdType = UART_CLIENT_CMD_0x01;
					break;
				case UART_CLIENT_CMD_0x02:
					s->enRs485CmdType = UART_CLIENT_CMD_0x02;
					break;
				default:
					s->ptr_no = 0;
					s->u16Buffer[0] = 0;
					s->u16Buffer[1] = 0;
					break;
			}
		}
		else if(s->ptr_no >= 2) {
			switch(s->enRs485CmdType) {
				case UART_CLIENT_CMD_0x01: 
					if (s->ptr_no == 11) {		//receive complete
						s->csr = RS485_STA_RX_COMPLETE;
						USART1->CR1 &= ~(1<<2);	//��ֹ����
						USART1->CR1 &= ~(1<<5);	//��ֹEUSART�����ж�
					}
					break;
					
				case UART_CLIENT_CMD_0x02:
					if(s->ptr_no == 11) {		//�������������ı��
						s->csr = RS485_STA_RX_COMPLETE;
						USART1->CR1 &= ~(1<<2);	//��ֹ����
						USART1->CR1 &= ~(1<<5);	//��ֹEUSART�����ж�
					}
					break;
				default: {
					s->ptr_no = 0;
					s->u16Buffer[0] = 0;
					break;
				}
			}
		}
		
		s->ptr_no++;
		if(s->ptr_no == RS485_MAX_BUFFER_SIZE) {
			s->ptr_no = 0;
			s->u16Buffer[0] = 0;
		}
	}
	
	USART1->CR1 |= (1<<5);	//ʹ��EUSART�����ж�
}
#endif

