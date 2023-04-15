#ifndef CELL_BALANCE_H
#define CELL_BALANCE_H

//二选一
#define _BALANCE_AFE
//#define _BALANCE_AD

#define CELL_NUMS_MAX              16  //max cell numbers 

#define ODD_SELECT			(UINT16)0x5555
#define EVEN_SELECT			(UINT16)0xAAAA

#define	TIME_200MS_1S		5
#define	TIME_200MS_2S		10
#define	TIME_200MS_5S		25
#define	TIME_200MS_10S		50

#define TIME_1000MS_1S      1
#define TIME_1000MS_2S      2
#define TIME_1000MS_5S      5
#define TIME_1000MS_10S     10
//#define fuck     10

#define Balance_OpenT_MOS     ((UINT16)0)
#define Balance_OpenT_ODD     ((UINT16)7)
#define Balance_OpenT_EVEN    ((UINT16)5)


//Balance state
enum BALANCE_STATE_E {
	BALANCE_ST_INIT = 0,
	BALANCE_ST_MONITOR,
	BALANCE_ST_ODD_ON,
	BALANCE_ST_EVEN_ON,
	BALANCE_ST_OFF,
};

//Single cell Balance status
enum CELL_BALANCE_STATUS_E {
    CELL_BALANCE_STATUS_OFF = 0,       // cell balance off
	CELL_BALANCE_STATUS_ON_VOLT,       // cell balance on because of cell volt > on volt
	CELL_BALANCE_STATUS_ON_VOLT_DELTA  // cell balance on because of cell delta volt > on volt delta
};

//Single cell Balance status
enum CELL_BALANCE_FLAG_UPPER {
    CELL_BALANCE_COLSE = 0,       	// cell balance off
    CELL_BALANCE_ON_BEGIN,      	// cell balance ODD on
	CELL_BALANCE_ON_ODD,      	 	// cell balance ODD on
	CELL_BALANCE_ON_EVEN  			// cell balance EVEN on
};

extern UINT16 g_u16CBnFLAG_ToUpper;    	//上传到上位机的
extern UINT8  g_u8CBn_StatusFlag;		//用于控制Mos或者Relay
extern UINT8 g_u8CBn_AFECloseFlag;


void CellBalance_DataInit(void);
void App_CellBalance(void);
void CellBalanceTest(void);


#endif	/* CELL_BALANCE_H */

