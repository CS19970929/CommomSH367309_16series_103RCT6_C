#ifndef ADC_H
#define ADC_H

#define AD_Used_amount 		1


#define AD_CalNum			8		//������λ�ƴ���>>2������������Ҹ�
#define AD_CalNum_2			3		//2^3 = 8�������Ǹ�����2�Ķ��ٴη�������λ��

#define AD_CalNum_Cur		32		//������λ�ƴ���>>5������������Ҹ�
#define AD_CalNum_Cur_2		5		//2^5 = 32�������Ǹ�����2�Ķ��ٴη�������λ��

//AD��������ö��
enum tagInfoForADCArray {
    ADC_TEMP_MOS1,      // MOS1�¶�
	ADC_CUR_AMP,		//����������ѹ
    ADC_VBC,            // ĸ�ߵ�ѹ
    ADC_TEMP_EV1,       // �ⲿ�¶�

    ADC_TEMP_MOS2,		// MOS2�¶�
    AD_VREF_AD,			//̧����ѹ
	ADC_TEMP_EV2,		// �ⲿ�¶�
	ADC_TEMP_EV3,		// �ⲿ�¶�
    ADC_EXT_C1,         // ��1�ڵ�ص�ѹ
    ADC_EXT_C2,         // ��2�ڵ�ص�ѹ
	ADC_CURR,

	ADC_NUM		        // ADC number
};


#define LENGTH_TBLTEMP_PORT_10K    ((UINT16)56)
#define Vbc_scale 	  31	  		//��ѹ�ɱ���ֵ


extern INT32 g_i32ADCResult[ADC_NUM];             //ADC���ݻ���
//extern __IO UINT16 g_u16ADCValFilter[ADC_NUM];		//���λ�����ܸ�
extern UINT16 gu16_BusCurr_CHG;
extern UINT16 gu16_BusCurr_DSG;


void InitADC(void);
void App_AnlogCal(void);

#endif	/* ADC_H */

