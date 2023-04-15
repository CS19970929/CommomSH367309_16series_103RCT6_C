#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

//系统错误控制类型
typedef enum {STARTUP_CONT = 0, STARTUP_OVER = !STARTUP_CONT} StartUp_Status;

enum SYSTEM_ERROR_COMMAND {
	ERROR_AFE1 = 1,			//为了方便使用函数返回值
	ERROR_AFE2,
	ERROR_CAN,
	ERROR_EEPROM_COM,
	ERROR_SPI,
	ERROR_UPPER,
	ERROR_CLIENT,
	ERROR_SCREEN,
	
	ERROR_WIFI,
	ERROR_BLUETOOTH,
	ERROR_APP,
	ERROR_CBC_CHG,
	ERROR_CBC_DSG,
	ERROR_EEPROM_STORE,
	ERROR_HSE,
	ERROR_LSE,
	ERROR_VDEATLE_OVER,
	
	ERROR_BALANCED,
	ERROR_ADC,
	ERROR_SOC_CAIL,
	ERROR_HEAT,
	ERROR_COOL,
	ERROR_TEMP_BREAK,
	ERROR_NUM = ERROR_TEMP_BREAK,


	ERROR_REMOVE_AFE1,
	ERROR_REMOVE_AFE2,
	ERROR_REMOVE_CAN,
	ERROR_REMOVE_EEPROM_COM,
	ERROR_REMOVE_SPI,
	ERROR_REMOVE_UPPER,
	ERROR_REMOVE_CLIENT,
	ERROR_REMOVE_SCREEN,
	
	ERROR_REMOVE_WIFI,
	ERROR_REMOVE_BLUETOOTH,
	ERROR_REMOVE_APP,
	ERROR_REMOVE_CBC_CHG,
	ERROR_REMOVE_CBC_DSG,
	ERROR_REMOVE_EEPROM_STORE,
	ERROR_REMOVE_HSE,
	ERROR_REMOVE_LSE,
	ERROR_REMOVE_VDEATLE_OVER,
	
	ERROR_REMOVE_BALANCED,
	ERROR_REMOVE_ADC,
	ERROR_REMOVE_HEAT,
	ERROR_REMOVE_COOL,
	ERROR_REMOVE_SOC_CAIL,
	ERROR_REMOVE_TEMP_BREAK,

	ERROR_STATUS_AFE1,
	ERROR_STATUS_AFE2,
	ERROR_STATUS_CAN,
	ERROR_STATUS_EEPROM_COM,
	ERROR_STATUS_SPI,
	ERROR_STATUS_UPPER,
	ERROR_STATUS_CLIENT,
	ERROR_STATUS_SCREEN,
	
	ERROR_STATUS_WIFI,
	ERROR_STATUS_BLUETOOTH,
	ERROR_STATUS_APP,
	ERROR_STATUS_CBC_CHG,
	ERROR_STATUS_CBC_DSG,
	ERROR_STATUS_EEPROM_STORE,
	ERROR_STATUS_HSE,
	ERROR_STATUS_LSE,
	ERROR_STATUS_VDEATLE_OVER,
	
	ERROR_STATUS_BALANCED,
	ERROR_STATUS_ADC,
	ERROR_STATUS_HEAT,
	ERROR_STATUS_COOL,
	ERROR_STATUS_SOC_CAIL,
	ERROR_STATUS_TEMP_BREAK,
};


struct SYSTEM_ERROR {	
	UINT8 u8ErrFlag_Com_AFE1;
	UINT8 u8ErrFlag_Com_AFE2;
	UINT8 u8ErrFlag_Com_Can;
	UINT8 u8ErrFlag_Com_EEPROM;
	
	UINT8 u8ErrFlag_Com_SPI;
	UINT8 u8ErrFlag_Com_Upper;
	UINT8 u8ErrFlag_Com_Client;
	UINT8 u8ErrFlag_Com_Screen;
	
	UINT8 u8ErrFlag_Com_Wifi;
	UINT8 u8ErrFlag_Com_BlueTooth;
	UINT8 u8ErrFlag_Com_App;
	UINT8 u8ErrFlag_CBC_CHG;
	
	UINT8 u8ErrFlag_Store_EEPROM;
	UINT8 u8ErrFlag_HSE;
	UINT8 u8ErrFlag_LSE;
	UINT8 u8ErrFlag_Vdelta_OVER;
	
	UINT8 u8ErrFlag_Balanced;
	UINT8 u8ErrFlag_ADC;
	UINT8 u8ErrFlag_Heat;
	UINT8 u8ErrFlag_Cool;
	
	UINT8 u8ErrFlag_CBC_DSG;
	UINT8 u8ErrFlag_SOC_Cail;
	UINT8 u8ErrFlag_TempBreak;
	UINT8 u8Res6;
};


//系统开机时序控制类型
enum SYSTEM_FUNC_STARTUP_COMMAND {
	SYSTEM_FUNC_STARTUP_SOC = 1,			//为了方便使用函数返回值
	SYSTEM_FUNC_STARTUP_BALANCE,
	SYSTEM_FUNC_STARTUP_PROTECT,
	SYSTEM_FUNC_STARTUP_MOS,
	SYSTEM_FUNC_STARTUP_RELAY,
	SYSTEM_FUNC_STARTUP_ADC,
	SYSTEM_FUNC_STARTUP_CAN,
	SYSTEM_FUNC_STARTUP_HEAT,	
	SYSTEM_FUNC_STARTUP_COOL,
	SYSTEM_FUNC_STARTUP_NUM = SYSTEM_FUNC_STARTUP_COOL,
};


union System_Function_StartUp {				//功能初始化统一管理
    UINT32 all;
    struct System_Func_StartUp_Flag {
		UINT8 b1StartUpFlag_SOC		:1;		//SOC初始化，目前用于开路电压和安时积分，阻抗跟踪不会用到，因为后面会把文件隔离
		UINT8 b1StartUpFlag_Balance	:1;		//均衡初始化
		UINT8 b1StartUpFlag_Protect :1;		//保护功能初始化
		UINT8 b1StartUpFlag_MOS     :1;		//MOS管初始化
		
		UINT8 b1StartUpFlag_Relay   :1;		//继电器管初始化
		UINT8 b1StartUpFlag_ADC     :1;		//ADC初始化
		UINT8 b1StartUpFlag_CAN		:1;		//Can初始化
		UINT8 b1StartUpFlag_Cool	:1;		//冷凝器开机自检

		UINT8 b1StartUpFlag_Heat	:1;		//散热器器开机自检
		UINT8 b1StartUpFlag_AFE1	:1;		//AFE1		//由于AFE在上电唤醒后便初始化完，所以这个不需要，不然太复杂了
		UINT8 b1StartUpFlag_AFE2	:1;		//AFE2
		UINT8 b1StartUpFlag_BlueT	:1;		//蓝牙开机自检
		
		UINT8 bRcved7				:1;		//res
		UINT8 bRcved8				:1;		//res
		UINT8 bRcved9				:1;		//res
		UINT8 bRcved10				:1;		//res

		UINT8 bRcved11				:8;		//res
		UINT8 bRcved12				:8;		//res
     }bits;
};


//系统状态跟踪类型
union System_Status {				//TODO，均衡，Heat，Cool，没搞
    UINT32 all;
    struct System_Status_Flag {
		UINT8 b1StartUpBMS			:1;		//BMS第一次开机标志位，初始为1，确定能打开管子视为系统初始化完毕				//第二个8位
		UINT8 b1Status_MOS_PRE      :1;		//预充MOS管功能状态
		UINT8 b1Status_MOS_CHG      :1;		//充电MOS管功能状态
		UINT8 b1Status_MOS_DSG      :1;		//放电MOS管功能状态

		UINT8 b1Status_Relay_PRE    :1;		//预充继电器功能状态
		UINT8 b1Status_Relay_CHG    :1;		//分口充电继电器功能状态
		UINT8 b1Status_Relay_DSG    :1;		//分口放电继电器功能状态
		UINT8 b1Status_Relay_MAIN   :1;		//同口主继电器功能状态

		UINT8 b1Status_Heat         :1;		//加热功能状态					//第一个8位
		UINT8 b1Status_Cool         :1;		//制冷功能状态
		UINT8 b1Status_AFE1         :1;		//AFE1状态
		UINT8 b1Status_AFE2	        :1;		//AFE2状态

		UINT8 b1Status_Balance		:1;		//均衡功能状态
		UINT8 b1Status_ToSleep		:1;		//进入休眠命令状态
		UINT8 b1Status_BnCloseIO	:1;		//均衡控制MOS关闭标志位
		UINT8 b1Status_HeatCloseIO	:1;		//加热时关闭MosRelay
		
		UINT8 b1Status_SysLimits	:1;		//res						//第四个8位
		UINT8 b1Status_ChgCloseIO	:1;		//电枪在线强制关闭驱动，限流模块强制关闭驱动
		UINT8 b1Status_DriverExtCtrl:1;		//驱动转为外部控制，特殊需求可用，慎用
		UINT8 bRcved6				:1;		//res

		UINT8 b4Status_ProjectVer	:4;		//记录一些项目信息，目前休眠带电为1，别的为0

		UINT8 bRcved11				:8;		//res						//第三个8位
     }bits;
};


//系统功能控制类型
union System_OnOFF_Function {				//TODO
    UINT32 all;
    struct System_OnOFF_Ctrl {
		UINT8 b1OnOFF_Balance		:1;		//均衡功能
		UINT8 b1OnOFF_BMS_Source 	:1;		//BMS辅源功能
		UINT8 b1OnOFF_MOS_Relay     :1;		//核心MOS和接触器功能，充放电用，若要单纯控制IO，则把相关功能关闭，通过Switch控制便可
											//原来是分开的，但是后面发现每个用到的地方都要选通，干脆就合并成一个不需要选通了。
		UINT8 b1OnOFF_Relay_Rec     :1;		//继电器功能
		
		UINT8 b1OnOFF_SOC_Fixed     :1;		//Soc固定功能
		UINT8 b1OnOFF_Heat          :1;		//加热功能
		UINT8 b1OnOFF_Cool          :1;		//制冷功能
		UINT8 b1OnOFF_AFE1         	:1;		//AFE1状态

		UINT8 b1OnOFF_AFE2	        :1;		//AFE2状态
		UINT8 b1OnOFF_Sleep			:1;		//休眠功能
		UINT8 b1OnOFF_SOC_Zero		:1;		//休眠功能
		UINT8 bRcved5				:1;		//
		
		UINT8 bRcved1				:4;		//res

		UINT8 bRcved2				:8;		//res
		UINT8 bRcved3				:8;		//res
     }bits;
};


extern volatile struct SYSTEM_ERROR System_ErrFlag;
extern volatile union System_Function_StartUp System_Func_StartUp;
extern volatile union System_OnOFF_Function System_OnOFF_Func_StartUpRec;

extern volatile union System_OnOFF_Function System_OnOFF_Func;
extern volatile union System_Status SystemStatus;

void InitSystemMonitorData_EEPROM(void);
void SystemMonitorResetData_EEPROM(void);

UINT8 System_ERROR_UserCallback(enum SYSTEM_ERROR_COMMAND errorCode);
StartUp_Status System_FUNC_StartUp(enum SYSTEM_FUNC_STARTUP_COMMAND SystemFunction);

#endif	/* SYSTEM_MONITOR_H */

