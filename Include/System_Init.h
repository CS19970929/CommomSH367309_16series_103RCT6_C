#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
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

//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //���
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //����

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //���
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //����

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //���
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //����

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //���
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //����

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //���
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //���
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //���
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����


#define MCUO_DEBUG_LED1 	PBout(15)		//LED1

//��Դģ��
#define MCUO_PWSV_STB 		PCout(10)		//
#define MCUO_PWSV_LDO		PAout(7)		//
#define MCUO_PWSV_CTR		PCout(13)		//
#define MCUO_DRV_WLM_PW		PBout(0)		//
#define MCUO_PW_WK_EN		PCout(11)
//#define MCUO_SD_DRV_CHG 	PCout(11)		//��������

//AFEģ��
#define MCUO_AFE_SHIP 		PAout(6)		//AFE_SHIP
#define MCUO_AFE_MODE 		PCout(8)		//AFE_MODE
#define MCUO_AFE_VPRO 		PCout(7)		//AFE_VPRO
#define MCUO_AFE_CTLC 		PCout(4)		//��������

//����ģ��
#define MCUO_E2PR_WP		PBout(13)	//EEPROMд����
#define MCUO_DRV_DET_CHG	PBout(3)	//
//#define MCUO_DRV_DET_LOAD	PAout(8)	//

#define MCUI_INT_WK_MCU		PAin(0)		//����MCU
#define MCUI_INT_WK_CHG		PAin(15)	//��绽��
//#define MCUI_INT_WK_LOAD	PDin(2)		//���ػ���
#define MCUI_CBC_DSG 		PBin(12)


//����ģ��
#define MCUO_MOS_PRE 		PCout(12)	//PC12�����MOS

#define MCUO_RELAY_HEAT 	PBout(14)		//���ȼ̵���
#define MCUO_RELAY_COOL 	PCout(6)		//�����̵���

#define MCUO_ENO_DO1		PCout(0)	//O��1
#define MCUO_ENO_DO2		PCout(1)	//O��2
#define MCUO_ENO_DO3		PCout(2)	//O��3
#define MCUO_ENO_DO4		PCout(3)	//O��4
#define MCUO_ENO_DO5		PBout(4)	//O��5
#define MCUO_ENO_DO6		PBout(1)	//O��6
#define MCUI_ENI_DI1		PBin(5)		//I��1


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
	UINT8 u8CBC_CHG_ErrFlag;	//����CBC������־λ
	UINT8 u8CBC_CHG_Cnt;		//���ֳ��CBC�Ĵ���
	UINT8 u8CBC_DSG_ErrFlag;	//����CBC������־λ
	UINT8 u8CBC_DSG_Cnt;		//���ַŵ�CBC�Ĵ���
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

