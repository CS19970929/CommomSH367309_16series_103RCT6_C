#ifndef LCD_H
#define LCD_H

#define	LCD_TX_BUF_LEN	50

#define RS485_ADDR_LCD       0xC000

extern struct RS485MSG g_stCurrentMsgPtrLCD_SCI1;
extern struct RS485MSG g_stCurrentMsgPtrLCD_SCI2;
extern struct RS485MSG g_stCurrentMsgPtrLCD_SCI3;


void LCD_FaultChk_SCI1(void);
void LCD_Rx_Deal_SCI1(struct RS485MSG *s);
void LCD_FaultChk_SCI2(void);
void LCD_Rx_Deal_SCI2(struct RS485MSG *s);
void LCD_FaultChk_SCI3(void);
void LCD_Rx_Deal_SCI3(struct RS485MSG *s);

void InitUSART_LCD(void);
void App_USART_LCD(void);

#endif	/* LCD_H */

