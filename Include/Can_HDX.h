#ifndef CAN_HDX_H
#define CAN_HDX_H


//发送ID
#define CANID_TX_Test      	((UINT32)0x001)

#define CANID_CHECK_0x00   ((UINT16)0x00)
#define CANID_CHECK_0x01   ((UINT16)0x01)
#define CANID_CHECK_0x02   ((UINT16)0x02)
#define CANID_CHECK_0x03   ((UINT16)0x03)
#define CANID_CHECK_0x04   ((UINT16)0x04)
#define CANID_CHECK_0x05   ((UINT16)0x05)
#define CANID_CHECK_0x06   ((UINT16)0x06)
#define CANID_CHECK_0x07   ((UINT16)0x07)
#define CANID_CHECK_0x08   ((UINT16)0x08)
#define CANID_CHECK_0x09   ((UINT16)0x09)
#define CANID_CHECK_0x0A   ((UINT16)0x0A)
#define CANID_CHECK_0x0B   ((UINT16)0x0B)
#define CANID_CHECK_0x0C   ((UINT16)0x0C)
#define CANID_CHECK_0x0D   ((UINT16)0x0D)
#define CANID_CHECK_0x0E   ((UINT16)0x0E)
#define CANID_CHECK_0x0F   ((UINT16)0x0F)
#define CANID_CHECK_0x10   ((UINT16)0x10)
#define CANID_CHECK_0x11   ((UINT16)0x11)



#define CAN_ADRESS_STD_ID				0x00
#define CANID_RX_COMMON_MSG_FILTER   	((UINT16)0x0000|((UINT16)CAN_ADRESS_STD_ID<<7))
//#define CANID_RX_COMMON_MSG_FILTER   	((UINT16)0x0000)

//单机版，11位的标准帧，高位的4位必须匹配为0bxxxx x000 0xxx xxxx
#define CANID_RX_COMMON_MSG_MASK      	((UINT16)0x0780)


union CanTxType_Status {
    UINT32   all;
    struct CanTxType_StatusBit {
	    UINT8 b1CanTx_Test     	:1;
		UINT8 b1CanTx_0x00		:1;
		UINT8 b1CanTx_0x01		:1;
    	UINT8 b1CanTx_0x02		:1;
		
        UINT8 b1CanTx_0x03     	:1;
        UINT8 b1CanTx_0x04     	:1;
        UINT8 b1CanTx_0x05     	:1;
        UINT8 b1CanTx_0x06     	:1;
		
        UINT8 b1CanTx_0x07     	:1;
        UINT8 b1CanTx_0x08    	:1;
        UINT8 b1CanTx_0x09    	:1;
        UINT8 b1CanTx_0x0A    	:1;
		
        UINT8 b1CanTx_0x0B    	:1;
        UINT8 b1CanTx_0x0C     	:1;
        UINT8 b1CanTx_0x0D    	:1;
        UINT8 b1CanTx_0x0E    	:1;

        UINT8 b1CanTx_0x0F    	:1;
        UINT8 b1CanTx_0x10     	:1;
        UINT8 b1CanTx_0x11    	:1;
        UINT8 b1CanTx_0x12    	:1;
		
        UINT8 b1CanTx_Res1      :4;
		UINT8 b1CanTx_Res2      :8;
     }bits;	
};


union Can_Status {
    UINT8   all;
    struct Can_StatusBit {
		UINT8 b1Can_BusOFF			:1;
		UINT8 b1Can_Received 		:1;
		UINT8 b1Can_Send            :1;
		UINT8 b1Can_Fault           :1;

		UINT8 b1Can_BusOFF_TestSd	:1;
		UINT8 b1Rcved				:3;
     }bits;
};


union MDLREPORTFAULT_REG {
    UINT16 all;
    struct MDLREPORTCHGFAULT_BITS {
		UINT8 b1CellOvp 		:1; 	//
		UINT8 b1CellUvp			:1; 	//
		UINT8 b1BatOvp			:1; 	//
		UINT8 b1BatUvp			:1; 	//
		
		UINT8 b1CellChgOtp		:1; 	//
		UINT8 b1CellChgUtp		:1; 	//
		UINT8 b1CellDischgOtp	:1; 	//
		UINT8 b1CellDischgUtp	:1; 	//
		
		UINT8 b1IchgOcp 		:1; 	//
		UINT8 b1IdischgOcp		:1; 	//
		UINT8 b1CBC_Err			:1; 	//
		UINT8 b1AFE_Err			:1; 	//
		
		UINT8 b1Soft_Lock_MOS	:1; 	//
		UINT8 b1VcellDeltaBig 	:1; 	//
		UINT8 b1SocLow 			:1; 	//
		UINT8 b1Charger_Online	:1; 	//
     }bits;
};


union WARNING_REG {
    UINT16 all;
    struct WARNING_BITS {
		UINT8 b1CellOvp 		:1; 	//
		UINT8 b1CellUvp			:1; 	//
		UINT8 b1BatOvp			:1; 	//
		UINT8 b1BatUvp			:1; 	//
		
		UINT8 b1CellChgOtp		:1; 	//
		UINT8 b1CellChgUtp		:1; 	//
		UINT8 b1CellDischgOtp	:1; 	//
		UINT8 b1CellDischgUtp	:1; 	//
		
		UINT8 b1IchgOcp 		:1; 	//
		UINT8 b1IdischgOcp		:1; 	//
		UINT8 b1Rec1			:1; 	//
		UINT8 b1VcellDeltaBig	:1; 	//
		
		UINT8 b1TempDeltaBig	:1; 	//
		UINT8 b1SocLow 			:1; 	//
		UINT8 b1TmosOtp 		:1; 	//
		UINT8 b1Rec2			:1; 	//
     }bits;
};


union SYS_LOSE_REG {
    UINT16 all;
    struct SYS_LOSE_BITS {
		UINT8 b1CellOvp_Err 	:1; 	//
		UINT8 b1CellUvp_Err		:1; 	//
		UINT8 b1MOS_Err			:1; 	//
		UINT8 b1Relay_Err		:1; 	//
		
		UINT8 b1AFE_Err			:1; 	//
		UINT8 b1Sys_Err			:1; 	//
		UINT8 b1Lifetime_Err	:1; 	//
		UINT8 b1Rec1			:1; 	//
		
		UINT8 b1Rec2			:8; 	//
     }bits;
};


union MOS_RELAY_REG {
    UINT16 all;
    struct MOS_RELAY_BITS {
		UINT8 b1Status_MOS_CHG      :1;		//充电MOS管功能状态
		UINT8 b1Status_MOS_DSG      :1;		//放电MOS管功能状态
		UINT8 b1Status_MOS_PRE      :1;		//预充MOS管功能状态
		UINT8 b1Status_Relay_CHG    :1;		//分口充电继电器功能状态
		
		UINT8 b1Status_Relay_DSG    :1;		//分口放电继电器功能状态
		UINT8 b1Status_Relay_PRE    :1;		//预充继电器功能状态
		UINT8 b1Status_Heat			:1; 	//
		UINT8 b1Status_Cool			:1; 	//
		
		UINT8 b1Status_Relay_MAIN   :1;		//同口主继电器功能状态
		UINT8 b1Status_Res   		:7;		//同口主继电器功能状态
     }bits;
};


void InitCan(void);
void App_Can(void);
void App_CanTest(void);

#endif
