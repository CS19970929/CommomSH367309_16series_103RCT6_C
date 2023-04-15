#ifndef ADC_H
#define ADC_H

#define AD_Used_amount 		1


#define AD_CalNum			8		//后面是位移处理>>2，所以这个别乱改
#define AD_CalNum_2			3		//2^3 = 8，上面那个数是2的多少次方，用于位移

#define AD_CalNum_Cur		32		//后面是位移处理>>5，所以这个别乱改
#define AD_CalNum_Cur_2		5		//2^5 = 32，上面那个数是2的多少次方，用于位移

//AD采样变量枚举
enum tagInfoForADCArray {
    ADC_TEMP_MOS1,      // MOS1温度
	ADC_CUR_AMP,		//电流采样电压
    ADC_VBC,            // 母线电压
    ADC_TEMP_EV1,       // 外部温度

    ADC_TEMP_MOS2,		// MOS2温度
    AD_VREF_AD,			//抬升电压
	ADC_TEMP_EV2,		// 外部温度
	ADC_TEMP_EV3,		// 外部温度
    ADC_EXT_C1,         // 第1节电池电压
    ADC_EXT_C2,         // 第2节电池电压
	ADC_CURR,

	ADC_NUM		        // ADC number
};


#define LENGTH_TBLTEMP_PORT_10K    ((UINT16)56)
#define Vbc_scale 	  31	  		//总压采比例值


extern INT32 g_i32ADCResult[ADC_NUM];             //ADC数据缓存
//extern __IO UINT16 g_u16ADCValFilter[ADC_NUM];		//这个位数不能改
extern UINT16 gu16_BusCurr_CHG;
extern UINT16 gu16_BusCurr_DSG;


void InitADC(void);
void App_AnlogCal(void);

#endif	/* ADC_H */

