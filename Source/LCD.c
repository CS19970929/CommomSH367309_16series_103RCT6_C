#include "main.h"

struct RS485MSG g_stCurrentMsgPtrLCD_SCI1;
UINT16 gu16_UsartCommuErrCntLCD_SCI1 = 0; // SCIͨ���쳣����
UINT8 gu8_UsartTxEnableLCD_SCI1 = 0;
UINT8 gu8_UsartTxFinishFlagLCD_SCI1 = 0;

struct RS485MSG g_stCurrentMsgPtrLCD_SCI2;
UINT16 gu16_UsartCommuErrCntLCD_SCI2 = 0; // SCIͨ���쳣����
UINT8 gu8_UsartTxEnableLCD_SCI2 = 0;
UINT8 gu8_UsartTxFinishFlagLCD_SCI2 = 0;

struct RS485MSG g_stCurrentMsgPtrLCD_SCI3;
UINT16 gu16_UsartCommuErrCntLCD_SCI3 = 0; // SCIͨ���쳣����
UINT8 gu8_UsartTxEnableLCD_SCI3 = 0;
UINT8 gu8_UsartTxFinishFlagLCD_SCI3 = 0;

UINT8 g_u8UsartTxBuff_LCD[LCD_TX_BUF_LEN];

void LCD_DataInit(struct RS485MSG *s)
{
	UINT16 i;

	s->ptr_no = 0;
	s->csr = RS485_STA_IDLE;
	s->enRs485CmdType = RS485_CMD_READ_REGS;
	for (i = 0; i < RS485_MAX_BUFFER_SIZE; i++)
	{
		s->u16Buffer[i] = 0;
	}
	for (i = 0; i < LCD_TX_BUF_LEN; i++)
	{
		g_u8UsartTxBuff_LCD[i] = 0;
	}
}

void Sci_Tx_Fun_LCD(struct RS485MSG *s, UINT8 t_u8BuffTemp[])
{
	UINT16 u16SciTemp;
	UINT16 i;

	i = 0;

	u16SciTemp = 1;
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = (g_stCellInfoReport.u16VCellTotle + 50) / 100;
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	if (g_stCellInfoReport.u16Ichg > 0)
	{
		u16SciTemp = (g_stCellInfoReport.u16Ichg + 5005) / 10;
	}
	else
	{
		u16SciTemp = (5000 - g_stCellInfoReport.u16IDischg) / 10;
	}
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = (g_stCellInfoReport.u16TempMax + 5) / 10;
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;

	u16SciTemp = g_stCellInfoReport.SocElement.u16Soc;
	t_u8BuffTemp[i++] = (u16SciTemp >> 8) & 0x00FF;
	t_u8BuffTemp[i++] = u16SciTemp & 0x00FF;
}

void LCD_verify(struct RS485MSG *s)
{
	UINT16 u16SciVerify;
	UINT16 t_u16FrameLenth;

	t_u16FrameLenth = s->ptr_no - 2;
	u16SciVerify = s->u16Buffer[t_u16FrameLenth] + (s->u16Buffer[t_u16FrameLenth + 1] << 8);
	if (u16SciVerify == Sci_CRC16RTU((UINT8 *)s->u16Buffer, t_u16FrameLenth))
	{
		s->AckType = RS485_ACK_POS;
	}
	else
	{
		s->u16RdRegByteNum = 0;
		s->AckType = RS485_ACK_NEG;
		s->ErrorType = RS485_ERROR_CRC_ERROR;
	}
}

void LCD_ReadRegs(struct RS485MSG *s)
{
	UINT16 t_u16Temp;

	t_u16Temp = s->u16Buffer[3] + (s->u16Buffer[2] << 8);
	s->u16RdRegStartAddrActure = t_u16Temp;
	if (t_u16Temp >= RS485_ADDR_LCD)
	{
		t_u16Temp -= RS485_ADDR_LCD;
	}
	s->u16RdRegStartAddr = t_u16Temp;
	s->u16RdRegByteNum = (s->u16Buffer[5] + (s->u16Buffer[4] << 8)) << 1;
}

void LCD_ReadRegs_ACK(struct RS485MSG *s)
{
	UINT8 i;
	UINT16 u16SciTemp;
	if (s->AckType == RS485_ACK_POS)
	{
		if (s->u16RdRegStartAddrActure >= RS485_ADDR_LCD)
		{
			if (s->u16RdRegStartAddrActure >= RS485_ADDR_LCD)
			{
				Sci_Tx_Fun_LCD(s, g_u8UsartTxBuff_LCD);
			}

			// ͷ�룬ǰ�����ֽڱ��ֲ���
			s->u16Buffer[0] = (s->u16Buffer[0] != 0) ? RS485_SLAVE_ADDR : s->u16Buffer[0];
			s->u16Buffer[1] = s->enRs485CmdType;
			s->u16Buffer[2] = s->u16RdRegByteNum;
			// ����
			for (i = 0; i < (s->u16RdRegByteNum); i++)
			{
				s->u16Buffer[i + 3] = g_u8UsartTxBuff_LCD[i + ((s->u16RdRegStartAddr) << 1)];
			}
			i = s->u16RdRegByteNum + 3;
		}
	}
	else
	{
		i = 1;
		s->u16Buffer[i++] = s->enRs485CmdType | 0x80;
		s->u16Buffer[i++] = s->ErrorType;
	}
	u16SciTemp = Sci_CRC16RTU((UINT8 *)s->u16Buffer, i);
	s->u16Buffer[i++] = u16SciTemp & 0x00FF;
	s->u16Buffer[i++] = u16SciTemp >> 8;
	s->AckLenth = i;

	s->ptr_no = 0;
	s->csr = RS485_STA_TX_COMPLETE;
}

#if (defined _LCD_SCI1)
void LCD_FaultChk_SCI1(void)
{
	UINT8 FaultCnt = 0;
	if (USART1->SR & 0x08)
	{ // �����������RXNEIE��EIEʹ�ܲ����жϣ���
		USART1->SR &= ~0x08;
		FaultCnt++;
	}

	if (USART1->SR & 0x04)
	{ // ��⵽������Ĭ�Ͽ��������Ļ�CR3��ONEBIT��1������
	  // USART_CR3��EIEʹ���ж�
		USART1->SR &= ~0x04;
		FaultCnt++;
	}

	if (USART1->SR & 0x02)
	{						 // ֡����USART_CR3��EIEʹ���жϣ���
		USART1->SR &= ~0x02; // ���
		FaultCnt++;
	}

	if (USART1->SR & 0x01)
	{						 // ��żУ������־ USART_CR1��PEIEʹ�ܸ��жϣ�����
		USART1->SR &= ~0x01; // ���
		FaultCnt++;
	}

	if (FaultCnt)
	{
		gu16_UsartCommuErrCntLCD_SCI1++;
	}
}

void LCD_Rx_Deal_SCI1(struct RS485MSG *s)
{
	USART1->CR1 &= ~(1 << 5);
	s->u16Buffer[s->ptr_no] = USART1->DR; // ��RXFIFO �ж�ȡ���յ�������
	if ((s->ptr_no == 0) && (s->u16Buffer[0] != RS485_SLAVE_ADDR) && (s->u16Buffer[0] != RS485_BROADCAST_ADDR))
	{
		s->ptr_no = 0;
		s->u16Buffer[0] = 0;
	}
	else
	{
		if (s->ptr_no == 1)
		{
			switch (s->u16Buffer[s->ptr_no])
			{
			case RS485_CMD_READ_REGS:
				s->enRs485CmdType = RS485_CMD_READ_REGS;
				break;
			case RS485_CMD_WRITE_REG:
				s->enRs485CmdType = RS485_CMD_WRITE_REG;
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
		else if (s->ptr_no > 2)
		{
			switch (s->enRs485CmdType)
			{
			case RS485_CMD_READ_REGS:
			case RS485_CMD_WRITE_REG:
				if (s->ptr_no == 7)
				{ // receive complete
					s->csr = RS485_STA_RX_COMPLETE;
					// RCSTA1bits.CREN = 0;  //��ֹ����
					// RC1IE = 0;			// ��ֹEUSART2 �����ж�
					USART1->CR1 &= ~(1 << 2);
					USART1->CR1 &= ~(1 << 5);
				}
				break;

			case RS485_CMD_WRITE_REGS:
				if ((s->ptr_no >= 7) && (s->ptr_no == (s->u16Buffer[6] + 8)))
				{
					s->csr = RS485_STA_RX_COMPLETE;
					USART1->CR1 &= ~(1 << 2);
					USART1->CR1 &= ~(1 << 5);
				}
				break;

			default:
				s->ptr_no = RS485_MAX_BUFFER_SIZE;
				s->u16Buffer[0] = 0;
				break;
			}
		}

		s->ptr_no++;
		if (s->ptr_no >= RS485_MAX_BUFFER_SIZE)
		{
			s->ptr_no = 0;
			s->u16Buffer[0] = 0;
		}
	}
	USART1->CR1 |= (1 << 5);
}

void LCD_Tx_Deal_SCI1(struct RS485MSG *s)
{
	if (0 == gu8_UsartTxEnableLCD_SCI1)
	{
		return;
	}

	if (gu16_UsartCommuErrCntLCD_SCI1)
	{ // ���ִ���Ҳ�ð�����ȫ�������꣬Ȼ�󲻻ظ�
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_UsartTxFinishFlagLCD_SCI1 = 1;
		gu8_UsartTxEnableLCD_SCI1 = 0;
		gu16_UsartCommuErrCntLCD_SCI1 = 0;
		return;
	}

	while (!((USART1->SR) & (1 << 7)))
		; // 1<<6 Ҳ����
	if (s->ptr_no < s->AckLenth)
	{
		USART1->DR = s->u16Buffer[s->ptr_no]; // load data
		s->ptr_no++;
	}
	else
	{
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_UsartTxFinishFlagLCD_SCI1 = 1;
		gu8_UsartTxEnableLCD_SCI1 = 0;
	}
}

// ���ڳ�ʼ������
void InitSCI1_LCD(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE); // PB6,PB7

	// Enable the USART1 Interrupt(ʹ��USART1�ж�)
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // ��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // �����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// USART1_TX   PA9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; // PA9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // ��������������������ǿ�©���
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// USART1_RX	  PA10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;			  // PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // �������룬��RX
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// ���ڳ�ʼ��
	USART_InitStructure.USART_BaudRate = 19200;										// ���ô��ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// ��������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// ����ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����Ч��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ����������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// ���ù���ģʽ
	USART_Init(USART1, &USART_InitStructure);

	USART1->CR3 |= 1 << 0; // EIE����֡�����жϣ�ͬʱ���������ж�

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // ʹ�ܽ����ж�
	USART_Cmd(USART1, ENABLE);					   // ʹ�ܴ���1

	LCD_DataInit(&g_stCurrentMsgPtrLCD_SCI1);
}

void App_LCD_SCI1(struct RS485MSG *s)
{
	switch (s->csr)
	{
	// IDLE-����̬������50ms��ʹ�ܽ��գ�����㣩receive set
	case RS485_STA_IDLE:
		break;

	case RS485_STA_RX_COMPLETE:
		USART1->CR1 &= ~(1 << 5); // ��ֹ�����ж�
		LCD_verify(s);
		if (s->AckType == RS485_ACK_POS)
		{
			switch (s->enRs485CmdType)
			{
			case RS485_CMD_READ_REGS:
				LCD_ReadRegs(s);
				break;
			case RS485_CMD_WRITE_REG:
				break;
			case RS485_CMD_WRITE_REGS:
				break;
			default:
				s->u16RdRegByteNum = 0;
				s->AckType = RS485_ACK_NEG;
				s->ErrorType = RS485_ERROR_NULL;
				break;
			}
		}
		s->csr = RS485_STA_RX_OK; // receive the correct data, switch to transmit wait 50ms
		break;					  // ��һ������

	case RS485_STA_RX_OK:
		switch (s->enRs485CmdType)
		{
		case RS485_CMD_READ_REGS:
			LCD_ReadRegs_ACK(s);
			break;
		case RS485_CMD_WRITE_REG:
		case RS485_CMD_WRITE_REGS:
			break;
		default:
			break;
		}
		USART1->CR1 |= (1 << 3); // ʹ�ܷ���
		gu8_UsartTxEnableLCD_SCI1 = 1;
		// break;						//����ֱ�ӷ��ͣ���break;
	// transmit complete, to switch receive wait 20ms
	case RS485_STA_TX_COMPLETE:
		if (gu8_UsartTxFinishFlagLCD_SCI1)
		{
			s->csr = RS485_STA_IDLE;
			s->u16Buffer[0] = 0;
			s->u16Buffer[1] = 0;
			s->u16Buffer[2] = 0;
			s->u16Buffer[3] = 0;
			gu8_UsartTxFinishFlagLCD_SCI1 = 0;
			s->ptr_no = 0;
			USART1->CR1 |= (1 << 2);
			USART1->CR1 |= (1 << 5);
			gu8_UsartTxEnableLCD_SCI1 = 0;
		}
		break;

	default:
		s->csr = RS485_STA_IDLE;
		break;
	}
	LCD_Tx_Deal_SCI1(s);
}
#endif

#if (defined _LCD_SCI2)
void LCD_FaultChk_SCI2(void)
{
	UINT8 FaultCnt = 0;
	if (USART2->SR & 0x08)
	{ // �����������RXNEIE��EIEʹ�ܲ����жϣ���
		USART2->SR &= ~0x08;
		FaultCnt++;
	}

	if (USART2->SR & 0x04)
	{ // ��⵽������Ĭ�Ͽ��������Ļ�CR3��ONEBIT��1������
	  // USART_CR3��EIEʹ���ж�
		USART2->SR &= ~0x04;
		FaultCnt++;
	}

	if (USART2->SR & 0x02)
	{						 // ֡����USART_CR3��EIEʹ���жϣ���
		USART2->SR &= ~0x02; // ���
		FaultCnt++;
	}

	if (USART2->SR & 0x01)
	{						 // ��żУ������־ USART_CR1��PEIEʹ�ܸ��жϣ�����
		USART2->SR &= ~0x01; // ���
		FaultCnt++;
	}

	if (FaultCnt)
	{
		gu16_UsartCommuErrCntLCD_SCI2++;
	}
}

void LCD_Rx_Deal_SCI2(struct RS485MSG *s)
{
	USART2->CR1 &= ~(1 << 5);
	s->u16Buffer[s->ptr_no] = USART2->DR; // ��RXFIFO �ж�ȡ���յ�������
	if ((s->ptr_no == 0) && (s->u16Buffer[0] != RS485_SLAVE_ADDR) && (s->u16Buffer[0] != RS485_BROADCAST_ADDR))
	{
		s->ptr_no = 0;
		s->u16Buffer[0] = 0;
	}
	else
	{
		if (s->ptr_no == 1)
		{
			switch (s->u16Buffer[s->ptr_no])
			{
			case RS485_CMD_READ_REGS:
				s->enRs485CmdType = RS485_CMD_READ_REGS;
				break;
			case RS485_CMD_WRITE_REG:
				s->enRs485CmdType = RS485_CMD_WRITE_REG;
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
		else if (s->ptr_no > 2)
		{
			switch (s->enRs485CmdType)
			{
			case RS485_CMD_READ_REGS:
			case RS485_CMD_WRITE_REG:
				if (s->ptr_no == 7)
				{ // receive complete
					s->csr = RS485_STA_RX_COMPLETE;
					// RCSTA1bits.CREN = 0;  //��ֹ����
					// RC1IE = 0;			// ��ֹEUSART2 �����ж�
					USART2->CR1 &= ~(1 << 2);
					USART2->CR1 &= ~(1 << 5);
				}
				break;

			case RS485_CMD_WRITE_REGS:
				if ((s->ptr_no >= 7) && (s->ptr_no == (s->u16Buffer[6] + 8)))
				{
					s->csr = RS485_STA_RX_COMPLETE;
					USART2->CR1 &= ~(1 << 2);
					USART2->CR1 &= ~(1 << 5);
				}
				break;

			default:
				s->ptr_no = RS485_MAX_BUFFER_SIZE;
				s->u16Buffer[0] = 0;
				break;
			}
		}

		s->ptr_no++;
		if (s->ptr_no >= RS485_MAX_BUFFER_SIZE)
		{
			s->ptr_no = 0;
			s->u16Buffer[0] = 0;
		}
	}
	USART2->CR1 |= (1 << 5);
}

void LCD_Tx_Deal_SCI2(struct RS485MSG *s)
{
	if (0 == gu8_UsartTxEnableLCD_SCI2)
	{
		return;
	}

	if (gu16_UsartCommuErrCntLCD_SCI2)
	{ // ���ִ���Ҳ�ð�����ȫ�������꣬Ȼ�󲻻ظ�
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_UsartTxFinishFlagLCD_SCI2 = 1;
		gu8_UsartTxEnableLCD_SCI2 = 0;
		gu16_UsartCommuErrCntLCD_SCI2 = 0;
		return;
	}

	while (!((USART2->SR) & (1 << 7)))
		; // 1<<6 Ҳ����
	if (s->ptr_no < s->AckLenth)
	{
		USART2->DR = s->u16Buffer[s->ptr_no]; // load data
		s->ptr_no++;
	}
	else
	{
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_UsartTxFinishFlagLCD_SCI2 = 1;
		gu8_UsartTxEnableLCD_SCI2 = 0;
	}
}

// ���ڳ�ʼ������
void InitSCI2_LCD(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// Enable the USART1 Interrupt(ʹ��USART1�ж�)
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // ��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // �����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// USART2_TX   PA2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // ��������������������ǿ�©���
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// USART2_RX	  PA3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // �������룬��RX
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// ���ڳ�ʼ��
	USART_InitStructure.USART_BaudRate = 19200;										// ���ô��ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// ��������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// ����ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����Ч��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ����������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// ���ù���ģʽ
	USART_Init(USART2, &USART_InitStructure);

	USART2->CR3 |= 1 << 0; // EIE����֡�����жϣ�ͬʱ���������ж�

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // ʹ�ܽ����ж�
	USART_Cmd(USART2, ENABLE);					   // ʹ�ܴ���2

	LCD_DataInit(&g_stCurrentMsgPtrLCD_SCI1);
}

void App_LCD_SCI2(struct RS485MSG *s)
{
	switch (s->csr)
	{
	// IDLE-����̬������50ms��ʹ�ܽ��գ�����㣩receive set
	case RS485_STA_IDLE:
		break;

	case RS485_STA_RX_COMPLETE:
		USART2->CR1 &= ~(1 << 5); // ��ֹ�����ж�
		LCD_verify(s);
		if (s->AckType == RS485_ACK_POS)
		{
			switch (s->enRs485CmdType)
			{
			case RS485_CMD_READ_REGS:
				LCD_ReadRegs(s);
				break;
			case RS485_CMD_WRITE_REG:
				break;
			case RS485_CMD_WRITE_REGS:
				break;
			default:
				s->u16RdRegByteNum = 0;
				s->AckType = RS485_ACK_NEG;
				s->ErrorType = RS485_ERROR_NULL;
				break;
			}
		}
		s->csr = RS485_STA_RX_OK; // receive the correct data, switch to transmit wait 50ms
		break;					  // ��һ������

	case RS485_STA_RX_OK:
		switch (s->enRs485CmdType)
		{
		case RS485_CMD_READ_REGS:
			LCD_ReadRegs_ACK(s);
			break;
		case RS485_CMD_WRITE_REG:
		case RS485_CMD_WRITE_REGS:
			break;
		default:
			break;
		}
		USART2->CR1 |= (1 << 3); // ʹ�ܷ���
		gu8_UsartTxEnableLCD_SCI2 = 1;
		// break;						//����ֱ�ӷ��ͣ���break;
	// transmit complete, to switch receive wait 20ms
	case RS485_STA_TX_COMPLETE:
		if (gu8_UsartTxFinishFlagLCD_SCI2)
		{
			s->csr = RS485_STA_IDLE;
			s->u16Buffer[0] = 0;
			s->u16Buffer[1] = 0;
			s->u16Buffer[2] = 0;
			s->u16Buffer[3] = 0;
			gu8_UsartTxFinishFlagLCD_SCI2 = 0;
			s->ptr_no = 0;
			USART2->CR1 |= (1 << 2);
			USART2->CR1 |= (1 << 5);
			gu8_UsartTxEnableLCD_SCI2 = 0;
		}
		break;

	default:
		s->csr = RS485_STA_IDLE;
		break;
	}
	LCD_Tx_Deal_SCI2(s);
}
#endif

#if (defined _LCD_SCI3)
void LCD_FaultChk_SCI3(void)
{
	UINT8 FaultCnt = 0;
	if (USART3->SR & 0x08)
	{ // �����������RXNEIE��EIEʹ�ܲ����жϣ���
		USART3->SR &= ~0x08;
		FaultCnt++;
	}

	if (USART3->SR & 0x04)
	{ // ��⵽������Ĭ�Ͽ��������Ļ�CR3��ONEBIT��1������
	  // USART_CR3��EIEʹ���ж�
		USART3->SR &= ~0x04;
		FaultCnt++;
	}

	if (USART3->SR & 0x02)
	{						 // ֡����USART_CR3��EIEʹ���жϣ���
		USART3->SR &= ~0x02; // ���
		FaultCnt++;
	}

	if (USART3->SR & 0x01)
	{						 // ��żУ������־ USART_CR1��PEIEʹ�ܸ��жϣ�����
		USART3->SR &= ~0x01; // ���
		FaultCnt++;
	}

	if (FaultCnt)
	{
		gu16_UsartCommuErrCntLCD_SCI3++;
	}
}

// ���������ݽ��룬�����ж��е���
/*=================================================================
 * FUNCTION: Sci2_Rx_Deal
 * PURPOSE : �������ݽ��ս���
 * INPUT:    void
 *
 * RETURN:   void
 *
 * CALLS:    void
 *
 * CALLED BY:ISR()
 *
 *=================================================================*/
void LCD_Rx_Deal_SCI3(struct RS485MSG *s)
{
	USART3->CR1 &= ~(1 << 5);
	s->u16Buffer[s->ptr_no] = USART3->DR; // ��RXFIFO �ж�ȡ���յ�������
	if ((s->ptr_no == 0) && (s->u16Buffer[0] != RS485_SLAVE_ADDR) && (s->u16Buffer[0] != RS485_BROADCAST_ADDR))
	{
		s->ptr_no = 0;
		s->u16Buffer[0] = 0;
	}
	else
	{
		if (s->ptr_no == 1)
		{
			switch (s->u16Buffer[s->ptr_no])
			{
			case RS485_CMD_READ_REGS:
				s->enRs485CmdType = RS485_CMD_READ_REGS;
				// s->ptr_no++;
				break;
			case RS485_CMD_WRITE_REG:
				s->enRs485CmdType = RS485_CMD_WRITE_REG;
				// s->ptr_no++;
				break;
			case RS485_CMD_WRITE_REGS:
				s->enRs485CmdType = RS485_CMD_WRITE_REGS;
				// s->ptr_no++;
				break;
			default:
				s->ptr_no = RS485_MAX_BUFFER_SIZE;
				s->u16Buffer[0] = 0;
				s->u16Buffer[1] = 0;
				break;
			}
		}
		else if (s->ptr_no > 2)
		{
			switch (s->enRs485CmdType)
			{
			case RS485_CMD_READ_REGS:
			case RS485_CMD_WRITE_REG:
				if (s->ptr_no == 7)
				{ // receive complete
					s->csr = RS485_STA_RX_COMPLETE;
					// RCSTA1bits.CREN = 0;  //��ֹ����
					// RC1IE = 0;			// ��ֹEUSART2 �����ж�
					USART3->CR1 &= ~(1 << 2);
					USART3->CR1 &= ~(1 << 5);
				}
				break;

			case RS485_CMD_WRITE_REGS:
				if ((s->ptr_no >= 7) && (s->ptr_no == (s->u16Buffer[6] + 8)))
				{
					s->csr = RS485_STA_RX_COMPLETE;
					USART3->CR1 &= ~(1 << 2);
					USART3->CR1 &= ~(1 << 5);
				}
				break;

			default:
				s->ptr_no = RS485_MAX_BUFFER_SIZE;
				s->u16Buffer[0] = 0;
				break;
			}
		}

		s->ptr_no++;
		if (s->ptr_no >= RS485_MAX_BUFFER_SIZE)
		{
			s->ptr_no = 0;
			s->u16Buffer[0] = 0;
		}
	}
	USART3->CR1 |= (1 << 5);
}

void LCD_Tx_Deal_SCI3(struct RS485MSG *s)
{
	if (0 == gu8_UsartTxEnableLCD_SCI3)
	{
		return;
	}

	if (gu16_UsartCommuErrCntLCD_SCI3)
	{ // ���ִ���Ҳ�ð�����ȫ�������꣬Ȼ�󲻻ظ�
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_UsartTxFinishFlagLCD_SCI3 = 1;
		gu8_UsartTxEnableLCD_SCI3 = 0;
		gu16_UsartCommuErrCntLCD_SCI3 = 0;
		return;
	}

	while (!((USART3->SR) & (1 << 7)))
		; // 1<<6 Ҳ����
	if (s->ptr_no < s->AckLenth)
	{
		USART3->DR = s->u16Buffer[s->ptr_no]; // load data
		s->ptr_no++;
	}
	else
	{
		s->ptr_no = 0;
		s->csr = RS485_STA_TX_COMPLETE;
		gu8_UsartTxFinishFlagLCD_SCI3 = 1;
		gu8_UsartTxEnableLCD_SCI3 = 0;
	}
}

// ���ڳ�ʼ������
void InitSCI3_LCD(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
	// Enable the USART3 Interrupt(ʹ��USART3�ж�)
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // ��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // �����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// USART3_TX   PD8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // ��������������������ǿ�©���
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// USART3_RX	  PD9��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // �������룬��RX
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// ���ڳ�ʼ��
	USART_InitStructure.USART_BaudRate = 19200;										// ���ô��ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// ��������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// ����ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����Ч��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ����������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// ���ù���ģʽ
	USART_Init(USART3, &USART_InitStructure);

	USART3->CR3 |= 1 << 0; // EIE����֡�����жϣ�ͬʱ���������ж�

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // ʹ�ܽ����ж�
	USART_Cmd(USART3, ENABLE);					   // ʹ�ܴ���1

	LCD_DataInit(&g_stCurrentMsgPtrLCD_SCI3);
}

void App_LCD_SCI3(struct RS485MSG *s)
{
	switch (s->csr)
	{
	// IDLE-����̬������50ms��ʹ�ܽ��գ�����㣩receive set
	case RS485_STA_IDLE:
		break;

	case RS485_STA_RX_COMPLETE:
		USART3->CR1 &= ~(1 << 5); // ��ֹ�����ж�
		LCD_verify(s);
		if (s->AckType == RS485_ACK_POS)
		{
			switch (s->enRs485CmdType)
			{
			case RS485_CMD_READ_REGS:
				LCD_ReadRegs(s);
				break;
			case RS485_CMD_WRITE_REG:
				break;
			case RS485_CMD_WRITE_REGS:
				break;
			default:
				s->u16RdRegByteNum = 0;
				s->AckType = RS485_ACK_NEG;
				s->ErrorType = RS485_ERROR_NULL;
				break;
			}
		}
		s->csr = RS485_STA_RX_OK; // receive the correct data, switch to transmit wait 50ms
		break;					  // ��һ������

	case RS485_STA_RX_OK:
		switch (s->enRs485CmdType)
		{
		case RS485_CMD_READ_REGS:
			LCD_ReadRegs_ACK(s);
			break;
		case RS485_CMD_WRITE_REG:
		case RS485_CMD_WRITE_REGS:
			break;
		default:
			break;
		}
		USART3->CR1 |= (1 << 3); // ʹ�ܷ���
		gu8_UsartTxEnableLCD_SCI3 = 1;
		// break;						//����ֱ�ӷ��ͣ���break;
	// transmit complete, to switch receive wait 20ms
	case RS485_STA_TX_COMPLETE:
		if (gu8_UsartTxFinishFlagLCD_SCI3)
		{
			s->csr = RS485_STA_IDLE;
			s->u16Buffer[0] = 0;
			s->u16Buffer[1] = 0;
			s->u16Buffer[2] = 0;
			s->u16Buffer[3] = 0;
			gu8_UsartTxFinishFlagLCD_SCI3 = 0;
			s->ptr_no = 0;
			USART3->CR1 |= (1 << 2);
			USART3->CR1 |= (1 << 5);
			gu8_UsartTxEnableLCD_SCI3 = 0;
		}
		break;

	default:
		s->csr = RS485_STA_IDLE;
		break;
	}
	LCD_Tx_Deal_SCI3(s);
}
#endif

void InitUSART_LCD(void)
{
#ifdef _LCD_SCI1
	InitSCI1_LCD();
#endif

#ifdef _LCD_SCI12
	InitSCI2_LCD();
#endif

#ifdef _LCD_SCI3
	InitSCI3_LCD();
#endif
}

void App_USART_LCD(void)
{
#ifdef _LCD_SCI1
	App_LCD_SCI1(&g_stCurrentMsgPtrLCD_SCI1);
#endif

#ifdef _LCD_SCI12
	App_LCD_SCI2(&g_stCurrentMsgPtrLCD_SCI2);
#endif

#ifdef _LCD_SCI3
	App_LCD_SCI3(&g_stCurrentMsgPtrLCD_SCI3);
#endif
}
