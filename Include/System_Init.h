#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入


#define MCUO_DEBUG_LED1 	PBout(15)		//LED1

//电源模块
#define MCUO_PWSV_STB 		PCout(10)		//
#define MCUO_PWSV_LDO		PAout(7)		//
#define MCUO_PWSV_CTR		PCout(13)		//
#define MCUO_DRV_WLM_PW		PBout(0)		//
#define MCUO_PW_WK_EN		PCout(11)
//#define MCUO_SD_DRV_CHG 	PCout(11)		//驱动供电

//AFE模块
#define MCUO_AFE_SHIP 		PAout(6)		//AFE_SHIP
#define MCUO_AFE_MODE 		PCout(8)		//AFE_MODE
#define MCUO_AFE_VPRO 		PCout(7)		//AFE_VPRO
#define MCUO_AFE_CTLC 		PCout(4)		//控制驱动

//其它模块
#define MCUO_E2PR_WP		PBout(13)	//EEPROM写保护
#define MCUO_DRV_DET_CHG	PBout(3)	//
//#define MCUO_DRV_DET_LOAD	PAout(8)	//

#define MCUI_INT_WK_MCU		PAin(0)		//唤醒MCU
#define MCUI_INT_WK_CHG		PAin(15)	//充电唤醒
//#define MCUI_INT_WK_LOAD	PDin(2)		//负载唤醒
#define MCUI_CBC_DSG 		PBin(12)


//驱动模块
#define MCUO_MOS_PRE 		PCout(12)	//PC12，充电MOS

#define MCUO_RELAY_HEAT 	PBout(14)		//加热继电器
#define MCUO_RELAY_COOL 	PCout(6)		//冷凝继电器

#define MCUO_ENO_DO1		PCout(0)	//O口1
#define MCUO_ENO_DO2		PCout(1)	//O口2
#define MCUO_ENO_DO3		PCout(2)	//O口3
#define MCUO_ENO_DO4		PCout(3)	//O口4
#define MCUO_ENO_DO5		PBout(4)	//O口5
#define MCUO_ENO_DO6		PBout(1)	//O口6
#define MCUI_ENI_DI1		PBin(5)		//I口1


union SYS_TIME {			//TODO
    UINT16 all;
    struct StatusSysTimeFlagBit {
        UINT8 b1Sys10msFlag1        :1;
        UINT8 b1Sys10msFlag2        :1;
        UINT8 b1Sys10msFlag3        :1;
        UINT8 b1Sys10msFlag4        :1;
		
        UINT8 b1Sys10msFlag5        :1;
		//UINT8 b1Sys20msFlag        	:1;
		UINT8 b1Sys1msFlag        	:1;	
		UINT8 b1Sys50msFlag        	:1;
		UINT8 b1Sys100msFlag       	:1;

		UINT8 b1Sys200msFlag1       :1;
		UINT8 b1Sys200msFlag2       :1;
		UINT8 b1Sys200msFlag3       :1;
		UINT8 b1Sys200msFlag4       :1;
		
		UINT8 b1Sys200msFlag5       :1;
		UINT8 b1Sys1000msFlag1      :1;
		UINT8 b1Sys1000msFlag2      :1;
		UINT8 b1Sys1000msFlag3      :1;
     }bits;
};


struct CBC_ELEMENT {
	UINT8 u8CBC_CHG_ErrFlag;	//出现CBC保护标志位
	UINT8 u8CBC_CHG_Cnt;		//出现充电CBC的次数
	UINT8 u8CBC_DSG_ErrFlag;	//出现CBC保护标志位
	UINT8 u8CBC_DSG_Cnt;		//出现放电CBC的次数
};


void IWDG_Feed(void);
#define Feed_WatchDog IWDG_Feed()

extern volatile union SYS_TIME g_st_SysTimeFlag;
extern struct CBC_ELEMENT CBC_Element;
extern UINT8 gu8_200msAccClock_Flag;


void InitDelay(void);
void __delay_ms(UINT16 nms);
void __delay_us(UINT32 nus);
void InitTimer(void);
void InitIO(void);
//void InitKey(void);
void InitNVIC(void);
void Init_IWDG(void);
void App_CBC(void);
void App_SysTime(void);
void App_ChgDet_Status(void);


#endif	/* SYSTEM_INIT_H */

