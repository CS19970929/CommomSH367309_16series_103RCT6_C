#ifndef UART_CLIENT_H
#define UART_CLIENT_H

#define USART_TX_BUF_LEN 			300

#define	UART_CLIENT_HEAD_ADDR		((UINT8)0x5A)
#define	UART_CLIENT_END_VALUE		((UINT8)0xFB)

#define UART_CLIENT_CMD_0x01 		0xA1	//¿Í»§µÄ
#define UART_CLIENT_CMD_0x02 		0xA2


extern struct RS485MSG g_UartClientMsgPtr_SCI1;
extern struct RS485MSG g_UartClientMsgPtr_SCI2;
extern struct RS485MSG g_UartClientMsgPtr_SCI3;

void UartClient_FaultChk_SCI1(void);
void UartClient_Rx_Deal_SCI1(struct RS485MSG *s);
void UartClient_FaultChk_SCI2(void);
void UartClient_Rx_Deal_SCI2(struct RS485MSG *s);
void UartClient_FaultChk_SCI3(void);
void UartClient_Rx_Deal_SCI3(struct RS485MSG *s);

void InitUSART_UartClient(void);
void App_UartClient_Updata(void);


void Sci_Rx_Deal_Client(struct RS485MSG *s);

#endif	/* UART_CLIENT_H */
