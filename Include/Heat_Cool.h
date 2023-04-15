#ifndef HEAT_COOL_H
#define HEAT_COOL_H

enum HEAT_COOL_CTRL_STATUS {
	ST_HEAT_DET_SELF,
	ST_HEAT_DET_NORMAL,
	ST_HEAT_CONT,
	ST_HEAT_DET_ERR,
	ST_HEAT_ERR_RECOVER_DEAL,

	ST_COOL_DET_SELF,
	ST_COOL_DET_NORMAL,
	ST_COOL_CONT,
	ST_COOL_DET_ERR,
	ST_COOL_ERR_RECOVER_DEAL,
};


union HEAT_COOL_FAULT_FLAG {
    UINT16 all;
    struct HEAT_COOL_Fault_Flag {
		UINT8 HeatErr_OpenCir     	:1;
		UINT8 HeatErr_OnTime_Max 	:1;
		UINT8 HeatErr_ShortCir     	:1;
		UINT8 HeatErr_Normal    	:1;
		UINT8 Rcv1					:4;
		
		UINT8 CoolErr_OpenCir     	:1;
		UINT8 CoolErr_OnTime_Max 	:1;
		UINT8 CoolErr_ShortCir     	:1;
		UINT8 CoolErr_Normal    	:1;
		UINT8 Rcv2					:4;
     }bits;	
};


struct HEAT_COOL_ELEMENT {
	//UINT16 u16Heat_OpenTemp;		//(T+40)*10
	//UINT16 u16Heat_DSG_Mid;		//(T+40)*10
	//UINT16 u16Heat_DSG_Low;		//(T+40)*10
	//UINT16 u16Heat_CHG_High;		//(T+40)*10
	
	UINT16 u16Heat_OpenTemp;		//(T+40)*10
	UINT16 u16Heat_CloseTemp;		//(T+40)*10
	UINT16 u16Heat_OpenCur;			//A*10
	UINT16 u16Cool_OpenTemp;		//(T+40)*10
	UINT16 u16Cool_CloseTemp;		//(T+40)*10
	UINT16 u16Heat_Res3;			//
	UINT16 u16Heat_Res4;			//
	UINT16 u16Heat_Res5;			//
	UINT16 u16Heat_Res6;			//
	UINT16 u16Heat_Res7;			//
	UINT16 u16Heat_Res8;			//
	UINT16 u16Heat_Res9;			//
	UINT16 u16Heat_Res10;			//


	UINT16 u16Cool_Res1;			//
	UINT16 u16Cool_Res2;			//
	UINT16 u16Cool_Res3;			//
	UINT16 u16Cool_Res4;			//
	UINT16 u16Cool_Res5;			//
	UINT16 u16Cool_Res6;			//
	UINT16 u16Cool_Res7;			//
	UINT16 u16Cool_Res8;			//
	UINT16 u16Cool_Res9;			//
	UINT16 u16Cool_Res10;			//
	UINT16 u16Cool_Res11;			//
};


#define HeatCoolElement_Min		{0,0,0,0,0,0,0,0,0,\
								 0,0,0,0,\
								 0,0,0,0,0,0,0,\
								 0,0,0,0}

#define HeatCoolElement_Default {400,500,20,1100,1000,\
								 0,0,0,0,0,0,0,0,\
								 0,0,0,0,0,0,0,\
								 0,0,0,0}

#define HeatCoolElement_Max 	{2000,2000,2000,2000,2000,\
								 65000,65000,65000,65000,65000,65000,65000,65000,\
								 65000,65000,65000,65000,65000,65000,65000,\
								 65000,65000,65000,65000}


extern struct HEAT_COOL_ELEMENT Heat_Cool_Element;
//extern enum HEAT_COOL_CTRL_STATUS HeatCtrl_Command;
//extern enum HEAT_COOL_CTRL_STATUS CoolCtrl_Command;
extern union HEAT_COOL_FAULT_FLAG Heat_Cool_FaultFlag;


void InitHeat_Cool(void);
void App_Heat_Cool_Ctrl(void);

#endif	/* HEAT_COOL_H */

