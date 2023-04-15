#ifndef I2C_AFE1_H
#define I2C_AFE1_H

//#define AFE_ID				0x34
#define AFE_ID				0x34

#define E2PROM_ID			0xA0

//Define MTP register addr
#define MTP_OTC             0x11
#define MTP_OTCR            0x12
#define MTP_UTC             0x13
#define MTP_UTCR            0x14
#define MTP_OTD             0x15
#define MTP_OTDR            0x16
#define MTP_UTD             0x17
#define MTP_UTDR            0x18
#define MTP_TR              0x19

#define MTP_CONF			0x40
#define MTP_BALANCEH		0x41
#define MTP_BALANCEL		0x42
#define MTP_BSTATUS1		0x43
#define MTP_BSTATUS2		0x44
#define MTP_BSTATUS3		0x45
#define MTP_TEMP1			0x46
#define MTP_TEMP2			0x48
#define MTP_TEMP3			0x4A
#define MTP_CUR				0x4C
#define MTP_CELL1			0x4E
#define MTP_CELL2			0x50
#define MTP_CELL3			0x52
#define MTP_CELL4			0x54
#define MTP_CELL5			0x56
#define MTP_CELL6			0x58
#define MTP_CELL7			0x5A
#define MTP_CELL8			0x5C
#define MTP_CELL9			0x5E
#define MTP_CELL10			0x60
#define MTP_CELL11			0x62
#define MTP_CELL12			0x64
#define MTP_CELL13			0x66
#define MTP_CELL14			0x68
#define MTP_CELL15			0x6A
#define MTP_CELL16			0x6C
#define MTP_ADC2			0x6E
#define MTP_BFLAG1			0x70
#define MTP_BFLAG2			0x71
#define MTP_RSTSTAT			0x72

typedef struct _AFEDATA_{
	UINT16 Temp1;		//回来之后V*100
	UINT16 Temp2;
	UINT16 Temp3;
	INT16 Cur1;			//实时电流值，是Vadc
	UINT16 Cell[16];
	INT16 Cadc;			//更精准的库仑计，用这个
}AFEDATA;


struct SH367309_Read {			/* AD Read	*/
	UINT16		u16VCell[16];   // mv
	UINT16		u16TempBat[3];					
	UINT32		u32VBat;       	// mv
	UINT16      u16Current;     // mA
};


#define _TWI_COM

#if 1
#define TWI_CLK_OUT		{GPIOB->CRH&=0xFFFFFFF0;GPIOB->CRH|=(UINT32)3<<(0<<2);}
#define TWI_CLK_IN      {GPIOB->CRH&=0xFFFFFFF0;GPIOB->CRH|=(UINT32)8<<(0<<2);}
#define TWI_CLK_HIGH	(PBout(8) = 1)
#define TWI_CLK_LOW	    (PBout(8) = 0)

#define TWI_DAT_OUT		{GPIOB->CRH&=0xFFFFFF0F;GPIOB->CRH|=(UINT32)3<<(1<<2);}
#define TWI_DAT_IN      {GPIOB->CRH&=0xFFFFFF0F;GPIOB->CRH|=(UINT32)8<<(1<<2);}
#define TWI_DAT_HIGH	(PBout(9) = 1)
#define TWI_DAT_LOW	    (PBout(9) = 0)

//#define TWI_RD_CLK		(uint16_t)(GPIOB->IDR&GPIO_Pin_8)  //输入SDA 
//#define TWI_RD_DAT		(uint16_t)(GPIOB->IDR&GPIO_Pin_9)  //输入SDA 
//#define TWI_RD_CLK		(PBin(8))  //输入SDA
//#define TWI_RD_DAT		(PBin(9))  //输入SDA
#define TWI_RD_CLK		(PBin(8))  //输入SDA
#define TWI_RD_DAT		(PBin(9))  //输入SDA
#endif

//这个和上面那个二选一
#if 0
#define TWI_CLK_OUT		F_TWI_CLK_OUT()
#define TWI_CLK_IN      F_TWI_CLK_IN()
#define TWI_CLK_HIGH	F_TWI_CLK_HIGH()
#define TWI_CLK_LOW	    F_TWI_CLK_LOW()

#define TWI_DAT_OUT		F_TWI_DAT_OUT()
#define TWI_DAT_IN      F_TWI_DAT_IN()
#define TWI_DAT_HIGH	F_TWI_DAT_HIGH()
#define TWI_DAT_LOW	    F_TWI_DAT_LOW()

#define TWI_RD_CLK		F_TWI_RD_CLK()
#define TWI_RD_DAT		F_TWI_RD_DAT()
#endif


extern struct SH367309_Read SH367309_Read_AFE1;
extern AFEDATA Registers_AFE1;


UINT8 MTPWrite(UINT8 WrAddr, UINT8 Length, UINT8 *WrBuf);
UINT8 MTPRead(UINT8 RdAddr, UINT8 Length, UINT8 *RdBuf);
UINT8 MTPWriteROM(UINT8 WrAddr, UINT8 Length, UINT8 *WrBuf);
void InitAFE1_Sleep(UINT8 mode);
void InitAFE1(void);
UINT8 UpdateVoltageFromBqMaximo(void);

#endif	/* I2C_AFE1_H */

