#include "main.h"


//ͨ��whileѭ����ÿִ��һ�Σ�v��1����Ŀ�ͻ����1�����v��1����ĿΪ��������parity=true������parity=false��
Parity OddEven_Check(UINT8 v) {
	Parity parity = EVEN;  //��ʼ�жϱ��
	while (v) {
	  parity = (Parity)(!parity);
	  v = v & (v - 1);
	}
	return parity;
}

//Ӳ���Դ���żУ�飬����Ҫ��������ˣ�ģ�⴮�ڿ���
UINT16 Usart_9bitOddEvenData_Frame(UINT8 data, Parity Parity_type) {
	UINT16 result;
	switch(Parity_type) {
		case ODD:
			if(OddEven_Check(data) == ODD) {
				result = ((UINT16)data)|((UINT16)0<<8);
			}
			else {
				result = ((UINT16)data)|((UINT16)1<<8);
			}
			break;
		case EVEN:
			if(OddEven_Check(data) == EVEN) {
				result = ((UINT16)data)|((UINT16)0<<8);
			}
			else {
				result = ((UINT16)data)|((UINT16)1<<8);
			}
			break;
		default:
			break;
	}
	return result;
}


/*=================================================================
* FUNCTION: GetEndValue
* PURPOSE : ���
* INPUT:    UINT16
*
* RETURN:   UINT16
*
* CALLS:    void
*
* CALLED BY:Adc_AnlogCal()
*
*=================================================================*/
UINT16 GetEndValue(const UINT16 * ptbl,UINT16 tblsize,UINT16 dat)                           
{
    UINT16 i, t_linenum ;
    UINT32 x1 = 0, y1 = 0, x2 = 1, y2 = 1;
    const UINT16 * p ;
    UINT16 t_tmp16a, t_tmp16b;
    INT32 t_tmp32a, t_tmp32b;
    UINT32 k, b;
    INT32 ret;
    p = ptbl;

    t_linenum = tblsize - 1;
    for (i = 0; i < tblsize - 2; i = i + 2)
    {
        t_tmp16a = p[i];
        t_tmp16b = p[i + 2];

        if (((dat >= t_tmp16a) && (dat <= t_tmp16b))
         || ((dat <= t_tmp16a) && (dat >= t_tmp16b))
           )
        {
            x1 = t_tmp16a;
            x2 = t_tmp16b;			
            y1 = p[i + 1];
            y2 = p[i + 3];			
            break;
        }
    }

    if (i >= t_linenum - 1)
    {
        p = ptbl;
        t_tmp16a = p[0];			
        t_tmp16b = p[tblsize - 2];		

        if (t_tmp16a <= t_tmp16b)
        {
            if (dat >= t_tmp16b)
            {
                t_tmp16a = p[tblsize - 1];
            }
            else
            {
                t_tmp16a = p[1];
            }
        }
        else
        {
            if (dat >= t_tmp16a)
            {
                t_tmp16a = p[1];
            }
            else
            {
                t_tmp16a =  p[tblsize - 1];
            }
        }
        return t_tmp16a;
    }
    else
    {
        if (x2 < x1)
        {
            ret = x2;
            x2  = x1;
            x1  = ret;
            ret = y2;
            y2  = y1;
            y1  = ret;
        }

        if (y2 >= y1)
        {
            t_tmp32a = y1 * x2;
            t_tmp32b = y2 * x1;
            ret = dat;
            k = y2 - y1;
            ret = ret * k;				
            if (t_tmp32a >= t_tmp32b)
            {
                b = t_tmp32a - t_tmp32b;
                ret = ret + b;
            }
            else
            {
                b = t_tmp32b - t_tmp32a;
                ret = ret  - b;
            }
            ret = ret / (x2 - x1);
        }
        else
        {
            t_tmp32a = y1 * x2;
            t_tmp32b = y2 * x1;
            ret = dat;
            k = y1 - y2;
            ret = ret * k;
            b = t_tmp32a - t_tmp32b;
            ret = b - ret;		
            ret = ret / (x2 - x1);			
        }
        return (ret & 0xffff);
    }
}


/*=================================================================
* FUNCTION: App_PubOPUPChk
* PURPOSE : ��Ƿ�жϴ���
* INPUT:    *t_sPubOPChk���ж������߼�����ǰ�ж�ֵ���Ƚ�ֵ���� *(t_sPubOPChk->i16ChkCnt)����ʱ�ۼ�����ַ
*           t_sPubOPChk->u16TimeCntB����ֵʱ���ж�ֵ t_sPubOPChk->u16TimeCntS��Сֵʱ���ж�ֵ
* RETURN:   0����ֵ�쳣 1���������
*
* CALLS:    void
*
* CALLED BY:
*
*=================================================================*/
UINT8 App_PubOPUPChk(SPUBOPUPCHK *t_sPubOPChk)
{
	if((t_sPubOPChk->u8FlagLogic > 1)
	|| (t_sPubOPChk->u16OPValB < t_sPubOPChk->u16OPValS))
	{
		return(0);                                                              // ��ֵ�쳣����0
	}
    
	if(t_sPubOPChk->u8FlagBit == (1 - t_sPubOPChk->u8FlagLogic))                       
	{
		if(t_sPubOPChk->u16ChkVal >= t_sPubOPChk->u16OPValB)                    // ��ǰ�ж�ֵ������ֵ
		{
			if((++(*(t_sPubOPChk->i16ChkCnt))) >= t_sPubOPChk->u16TimeCntB)     // ��ǰ�ж�ֵ������ֵt_sPubOPChk->u16TimeCntBʱ���ñ�־λΪu8FlagLogic
			{
				(*(t_sPubOPChk->i16ChkCnt)) = 0;
				t_sPubOPChk->u8FlagBit = t_sPubOPChk->u8FlagLogic;
			}			
		}
		else                                                                    // ���ڴ�ֵ���ʱ���ݼ�
		{
			if((*(t_sPubOPChk->i16ChkCnt)) > 0)
			{
				(*(t_sPubOPChk->i16ChkCnt))--;
			}
		}		
	}
	else
	{
		if(t_sPubOPChk->u16ChkVal <= t_sPubOPChk->u16OPValS)                    // ��ǰ�ж�ֵ����Сֵ
		{
			if((++(*(t_sPubOPChk->i16ChkCnt))) >= t_sPubOPChk->u16TimeCntS)
			{
				(*(t_sPubOPChk->i16ChkCnt)) = 0;
				t_sPubOPChk->u8FlagBit = 1 - t_sPubOPChk->u8FlagLogic;          // ��ǰ�ж�ֵ����Сֵt_sPubOPChk->u16TimeCntSʱ���ñ�־λΪ��u8FlagLogic
			}			
		}
		else                                                                    // ����Сֵ���ʱ���ݼ�
		{
			if((*(t_sPubOPChk->i16ChkCnt)) > 0)
			{
				(*(t_sPubOPChk->i16ChkCnt))--;
			}
		}
	}
    
	return(1);                                                                  // ������ɷ���1
}


UINT16 Sci_CRC16RTU( UINT8 * pszBuf, UINT8 unLength)
{
	UINT16 CRCC=0XFFFF;
	UINT32 CRC_count;

	for(CRC_count=0;CRC_count<unLength;CRC_count++)
	{
		int i;

		CRCC=CRCC^*(pszBuf+CRC_count);

		for(i=0;i<8;i++)
		{
			if(CRCC&1)
			{
				CRCC>>=1;
				CRCC^=0xA001;
			}
			else
			{ 
                CRCC>>=1;
			}
				
		}
	}

	return CRCC;
}

unsigned char CRC8(unsigned char *ptr, unsigned char len,unsigned char key)
{
	unsigned char i;
	unsigned char crc=0;
	while(len--!=0)
	{
		for(i=0x80; i!=0; i/=2)
		{
			if((crc & 0x80) != 0)
			{
				crc *= 2;
				crc ^= key;
			}
			else
				crc *= 2;

			if((*ptr & i)!=0)
				crc ^= key;
		}
		ptr++;
	}
	return(crc);
}


//�����ֵ
UINT32 ModulusSub(UINT32 Data1, UINT32 Data2) {
	return (UINT32)(Data1 > Data2 ? Data1 - Data2: Data2 - Data1);
}


//��10usΪʱ���������ʱ
//1ms������� 10%���ڣ�������ƫС��10us���9.4us��
void Delay_Base10us(int n) {
    unsigned char a,b;
	while(n--) {
		for(b=3;b>0;b--)
			for(a=22;a>0;a--);
	}	
}


//�����ʱ����
void Delay1ms(UINT8 delaycnt) {
	UINT8 i,k;
	UINT16 j;

	#if 0
	for(i=0; i<delaycnt; i++) {
		for(j=0; j<1670; j++) {
			//system clock = 24MHz
		}
	}
	#endif
	
	for(i=0; i<delaycnt; i++) {
		for(k=0; k < 9; k++) {		//9������72MHz
			for(j=0; j < 560; j++) {
				//system clock = 8MHz
			}
		}
	}
}


/*******************************************************************************
Function: MemoryCopy()
Description:
Input:	source--ԴMemoryָ��
		target---Ŀ��Memoryָ��
		length---��Ҫ���������ݳ���(Byres)
Output: 
Others:
*******************************************************************************/
void MemoryCopy(UINT8 *source, UINT8 *target, UINT8 length) {
	UINT8 i;
	for(i=0; i<length; i++) {
		*target = *source;
		target++;
		source++;
	}
}


//�ܲ���дһ��������˳���ԭֵ�ı��أ������ǵ�����ֵ
UINT16* U16_SwapEndian_Adress(UINT16* target) {
	#if 0
	UINT8* temp1,temp2;
	UINT8 value1;
	
	*temp1 = (UINT8*)target;
	*temp2 = ++(*temp1);
	value1 = **temp1;
	**temp1 = **temp2;
	**temp2 = value1;
	#endif
	return target;
}


//��С��ת������
UINT16 U16_SwapEndian(UINT16 target) {
	return (((uint16_t)target&0xFF00)>>8) | (((uint16_t)target&0x00FF)<<8);
}


//1:���¶ȶ��ߣ�0������
//�����⣬�Ӻ��¶��ߣ�����BMS������������
//����Զ���ԭ���ܣ�����Ҫ����
UINT8 Monitor_TempBreak(UINT16* temp_AD) {
	static UINT8 su8_Recover_Cnt = 0;
	static UINT8 su8_StartUp_Flag = 0;
	static UINT8 su8_Delay_Cnt = 0;
	UINT8 result = 0;

	switch(su8_StartUp_Flag) {
		case 0:		//�տ����������жϣ���Ϊ��ѯAFE�����Ѿ����ָ�����õ����ݣ���ʱ�жϱ�Ϊ��
			if(++su8_Delay_Cnt >= 20) {
				su8_Delay_Cnt = 0;
				su8_StartUp_Flag = 1;
			}
			break;
	
		case 1:
			if(*temp_AD < 110) {
				++result;
				*temp_AD = 110;		//������-29���϶ȡ��Է���λ����ʾNA��Ϊû����
				System_ERROR_UserCallback(ERROR_TEMP_BREAK);
				su8_Recover_Cnt = 0;
			}
			else {
				if(System_ERROR_UserCallback(ERROR_STATUS_TEMP_BREAK)) {
					if(++su8_Recover_Cnt >= 50) {		//�ж�50���Զ���ԭ��ԼΪ200*50=10s
						su8_Recover_Cnt = 0;
						System_ERROR_UserCallback(ERROR_REMOVE_TEMP_BREAK);
					}
				}
			}
			break;
			
		default:
			su8_StartUp_Flag = 0;
			break;
	}
	
	return result;
}

