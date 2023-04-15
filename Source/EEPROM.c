#include "main.h"

UINT32 u32E2P_Pro_VolCur_WriteFlag = 0;
UINT32 u32E2P_Pro_Temp_WriteFlag = 0;
UINT32 u32E2P_Pro_Other_WriteFlag = 0;
UINT32 u32E2P_RTC_Element_WriteFlag = 0;
UINT32 u32E2P_OtherElement1_WriteFlag = 0;
UINT32 u32E2P_HeatCool_WriteFlag = 0;

UINT8 u8E2P_SocTable_WriteFlag = 0;
UINT8 u8E2P_CopperLoss_WriteFlag = 0;
UINT8 u8E2P_KB_WriteFlag = 0;

UINT8 u8E2P_KB_WritePos = 0;

void InitData_E2prom(void);

//����IIC��ʼ�ź�
void IIC_Start_SEE(void) {
	SDA_OUT_SEE();     //sda�����
	IIC_SDA_SEE=1;
	IIC_SCL_SEE=1;
	__delay_us(DELAY_US_IIC_EEPROM);
 	IIC_SDA_SEE=0;//START:when CLK is high,DATA change form high to low 
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}


//����IICֹͣ�ź�
void IIC_Stop_SEE(void) {
	SDA_OUT_SEE();		//sda�����
	IIC_SCL_SEE=0;
	IIC_SDA_SEE=0;		//STOP:when CLK is high DATA change form low to high
 	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=1;
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SDA_SEE=1;		//����I2C���߽����ź�
	__delay_us(DELAY_US_IIC_EEPROM);
}


//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
UINT8 IIC_Wait_Ack_SEE(void) {
	UINT8 ucErrTime=0;
	SDA_IN_SEE();      //SDA����Ϊ����  
	//IIC_SDA_SEE=1;__delay_us(4);
	IIC_SCL_SEE=1;__delay_us(DELAY_US_IIC_EEPROM);
	while(READ_SDA_SEE)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop_SEE();
			return 1;
		}
	}
	IIC_SCL_SEE=0;//ʱ�����0
	__delay_us(DELAY_US_IIC_EEPROM);
	return 0;  
}


//����ACKӦ��
void IIC_Ack_SEE(void) {
	IIC_SCL_SEE=0;
	SDA_OUT_SEE();
	IIC_SDA_SEE=0;
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=1;
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=0;
}


//������ACKӦ��		    
void IIC_NAck_SEE(void) {
	IIC_SCL_SEE=0;
	SDA_OUT_SEE();
	IIC_SDA_SEE=1;
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=1;
	__delay_us(DELAY_US_IIC_EEPROM);
	IIC_SCL_SEE=0;
}


//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte_SEE(UINT8 txd) {                        
    UINT8 t;
	SDA_OUT_SEE();
    IIC_SCL_SEE=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA_SEE=1;
		else
			IIC_SDA_SEE=0;
		txd<<=1;
		__delay_us(DELAY_US_IIC_EEPROM);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL_SEE=1;
		__delay_us(DELAY_US_IIC_EEPROM); 
		IIC_SCL_SEE=0;	
		__delay_us(DELAY_US_IIC_EEPROM);
    }
}


//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
UINT8 IIC_Read_Byte_SEE(unsigned char ack) {
	unsigned char i,receive=0;
	SDA_IN_SEE();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL_SEE=0; 
        __delay_us(DELAY_US_IIC_EEPROM);
		IIC_SCL_SEE=1;
        receive<<=1;
        if(READ_SDA_SEE)receive++;   
		__delay_us(DELAY_US_IIC_EEPROM); 
    }					 
    if (!ack)
        IIC_NAck_SEE();//����nACK
    else
        IIC_Ack_SEE(); //����ACK   
    return receive;
}


//�����뷨ʧ��
UINT8 ReadEEPROM_Byte2(UINT16 addr, UINT8* data) {
	IIC_Start_SEE();
	IIC_Send_Byte_SEE(sEEAddress|I2C_RW_W);	//����д����
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;							//���Ƿ���ֵ�����⣬ֻ�����������
	}
	
	#ifndef AT24C02
	IIC_Send_Byte_SEE(addr>>8);				//���͸ߵ�ַ
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}
	#endif
	
	IIC_Send_Byte_SEE(addr%256);  			//���͵͵�ַ
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}

	IIC_Start_SEE();
	IIC_Send_Byte_SEE(sEEAddress|I2C_RW_R);	//�������ģʽ
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}

	*data=IIC_Read_Byte_SEE(0);
	IIC_Stop_SEE();							//����һ��ֹͣ����
	return 0;
}

UINT8 ReadEEPROM_Word_NoZone2(UINT16 addr, UINT16* data) {
	UINT8  tmp8a,tmp8b,result = 0;	
	result += ReadEEPROM_Byte2(addr, &tmp8a);	//��ȡ��λ��ַA��Ӧ������
	result += ReadEEPROM_Byte2(addr+1, &tmp8b);	//��ȡ��λ��ַA+1��Ӧ������
	*data = ((UINT16)tmp8b<<8) | tmp8a;		//���ݴ洢�����ͨѶʧ�ܵĻ������ص���0x0000
	return result;
}



//˼��ʧ�ܣ��е������뷨
UINT8 ReadEEPROM_Word_WithZone2(UINT16 addr, UINT16* data) {
	UINT16 tmp16a,tmp16b,tmp16c;
	UINT16 addrB,addrC;
	UINT8 result1 = 0, result2 = 0;
	addrB = addr + BZONE;
	addrC = addr + CZONE;
	result1 += ReadEEPROM_Word_NoZone2(addr, &tmp16a);
	result1 += ReadEEPROM_Word_NoZone2(addrB, &tmp16b);
	result1 += ReadEEPROM_Word_NoZone2(addrC, &tmp16c);
	if(result1 != 0) {
		return ERROR_EEPROM_COM;
	}
	
	if (tmp16a == tmp16b) {                                 //a == b
        if (tmp16a != tmp16c) {                             //a != c
        	result1 += WriteEEPROM_Word_NoZone(addrC, tmp16a);
        }
		*data = tmp16a;
		++result2;
    }
    else {
        if (tmp16b == tmp16c) {                              //b==c  a != b
            result1 += WriteEEPROM_Word_NoZone(addr, tmp16b);
			*data = tmp16b;
			++result2;
        }	
        else {
            if (tmp16a == tmp16c) {                          //a == c, a != b
                result1 += WriteEEPROM_Word_NoZone(addrB, tmp16a);
				*data = tmp16a;
				++result2;
            }
            else {                                           //a != b, b != c, c != a
                result1 += WriteEEPROM_Word_NoZone(addr, tmp16a);
				result1 += WriteEEPROM_Word_NoZone(addrB, tmp16a);
				result1 += WriteEEPROM_Word_NoZone(addrC, tmp16a);
				*data = tmp16a;								//tmp16a,tmp16b,tmp16c����Ĭ��ֵ�����ص�һ��ֵ��
				++result2;
            }
        }
    }
	if(result1 != 0) {
		return ERROR_EEPROM_COM;
	}
	else if(result2 != 0) {
		return ERROR_EEPROM_COM;
	}
	return 0;
}



//����ά����Ա��ֹʹ���������
UINT8 WriteEEPROM_Byte(UINT16 addr, UINT8 val) {
	Feed_WatchDog;
	MCUO_E2PR_WP = 0;
	
    IIC_Start_SEE();  
	IIC_Send_Byte_SEE(sEEAddress|I2C_RW_W);	//����д����
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}
	
	#ifndef AT24C02
	IIC_Send_Byte_SEE(addr>>8);				//���͸ߵ�ַ
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}

	#endif
	
    IIC_Send_Byte_SEE(addr%256);   			//���͵͵�ַ
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}	 										  		   
	IIC_Send_Byte_SEE(val);     			//�����ֽ�
	if(1 == IIC_Wait_Ack_SEE()) {
		return 1;
	}		    	   
    IIC_Stop_SEE();							//����һ��ֹͣ����
	__delay_ms(5);	 						//EEPROM���ԣ���Ҫ5ms��֤д��
	
	MCUO_E2PR_WP = 1;
	Feed_WatchDog;
	return 0;
}


UINT8 ReadEEPROM_Byte(UINT16 addr) {
	UINT8 temp=0;
	Feed_WatchDog;
	IIC_Start_SEE();
	IIC_Send_Byte_SEE(sEEAddress|I2C_RW_W);	//����д����
	if(1 == IIC_Wait_Ack_SEE()) {
		System_ERROR_UserCallback(ERROR_EEPROM_COM);	//�������������һ��ŵ�����������������̷��
		return 0;							//���Ƿ���ֵ�����⣬ֻ�����������
	}

	
	#ifndef AT24C02
	IIC_Send_Byte_SEE(addr>>8);				//���͸ߵ�ַ
	if(1 == IIC_Wait_Ack_SEE()) {
		System_ERROR_UserCallback(ERROR_EEPROM_COM);
		return 0;
	}

	#endif
	
	IIC_Send_Byte_SEE(addr%256);  			//���͵͵�ַ
	if(1 == IIC_Wait_Ack_SEE()) {
		System_ERROR_UserCallback(ERROR_EEPROM_COM);
		return 0;
	}

	IIC_Start_SEE();
	IIC_Send_Byte_SEE(sEEAddress|I2C_RW_R);	//�������ģʽ
	if(1 == IIC_Wait_Ack_SEE()) {
		System_ERROR_UserCallback(ERROR_EEPROM_COM);
		return 0;
	}

	temp=IIC_Read_Byte_SEE(0);
	IIC_Stop_SEE();							//����һ��ֹͣ����
	Feed_WatchDog;
	return temp;
}


UINT16 ReadEEPROM_Word_NoZone(UINT16 addr) {
	UINT16 tmp16a;
	UINT8  tmp8a,tmp8b;	
	tmp8a = ReadEEPROM_Byte(addr);		                        //��ȡ��λ��ַA��Ӧ������
	tmp8b = ReadEEPROM_Byte(addr+1);	                        //��ȡ��λ��ַA+1��Ӧ������
	tmp16a = tmp8b;
	tmp16a = (tmp16a<<8) | tmp8a;	                        //���ݴ洢

	return tmp16a;
}


//��Ҫ����������˼��仰
UINT8 WriteEEPROM_Word_NoZone(UINT16            addr, UINT16 data) {
    UINT8  tmp8a, tmp8b, WriteCounter = 0, result = 0;
    UINT16 tmp_addr,tmp16;;

	tmp_addr = addr;						//��ֲ������仰
	WriteCounter = 0;
	do {
		result += WriteEEPROM_Byte(tmp_addr, data&0xff);		//���ݵĵ�8λд��EEPROM
		result += WriteEEPROM_Byte(tmp_addr+1, data>>8);		//���ݵĸ�8λд��EEPROM
		tmp8a = ReadEEPROM_Byte(tmp_addr);						//��ȡ�մ���EEPROM�ĵ�8λ����
		tmp8b = ReadEEPROM_Byte(tmp_addr+1);					//��ȡ�մ���EEPROM�ĸ�8λ����
		tmp16 = (tmp8b<<8) |tmp8a ; 						//�洢�����������ڱ���tmp16
		
		WriteCounter++;
		if(WriteCounter > 2 || result != 0) {				/*�ж�tmp16 != data�ļ���*/
			++result;										//Ҫ������ִ��д������λ
			break;
		}
	}while(tmp16 != data);
	return result;
}

/*
=================���½���ڶ���Ӧ�ý׶�=================
*/

UINT16 ReadEEPROM_Word_WithZone(UINT16 addr) {

	UINT16 tmp16a,tmp16b,tmp16c;
	UINT8  tmp8a,tmp8b;	
	UINT16 addrB,addrC;
	
	addrB = addr + BZONE;
	addrC = addr + CZONE;

	tmp8a = ReadEEPROM_Byte(addr);		                        //��ȡ��λ��ַA��Ӧ������
	tmp8b = ReadEEPROM_Byte(addr+1);	                        //��ȡ��λ��ַA+1��Ӧ������
	tmp16a = tmp8b;
	tmp16a = (tmp16a<<8) | tmp8a;	                        //���ݴ洢
	
	tmp8a = ReadEEPROM_Byte(addrB);                            //��ȡ��λ��ַB��Ӧ������
	tmp8b = ReadEEPROM_Byte(addrB+1);	                        //��ȡ��λ��ַB+1��Ӧ������
	tmp16b = tmp8b;
	tmp16b = (tmp16b<<8) | tmp8a;	                        //���ݴ洢
	
	tmp8a = ReadEEPROM_Byte(addrC);                            //��ȡ��λ��ַC��Ӧ������
	tmp8b = ReadEEPROM_Byte(addrC+1);	                        //��ȡ��λ��ַC+1��Ӧ������
	tmp16c = tmp8b;
	tmp16c = (tmp16c<<8) | tmp8a;	                        //���ݴ洢
	
	if (tmp16a == tmp16b) {                                 //a == b
        if (tmp16a != tmp16c) {                             //a != c      
            WriteEEPROM_Word_NoZone(addrC, tmp16a);
        }
        return tmp16a;
    }
    else {
        if (tmp16b == tmp16c) {                              //b==c  a != b
            WriteEEPROM_Word_NoZone(addr, tmp16b);
            return tmp16b;
        }	
        else {
            if (tmp16a == tmp16c) {                          //a == c, a != b
                WriteEEPROM_Word_NoZone(addrB, tmp16a);
                return tmp16a;
            }
            else {                                            //a != b, b != c, c != a
                WriteEEPROM_Word_NoZone(addr, tmp16a);
				WriteEEPROM_Word_NoZone(addrB, tmp16a);
				WriteEEPROM_Word_NoZone(addrC, tmp16a);
                return tmp16a;                        //tmp16a,tmp16b,tmp16c����Ĭ��ֵ�����ص�һ��ֵ��
            }		
        }
    }
}



//��Ҫ����������˼��仰
void WriteEEPROM_Word_WithZone(UINT16 addr, UINT16 data) {
	UINT8 result = 0;
	result += WriteEEPROM_Word_NoZone(addr, data);
	result += WriteEEPROM_Word_NoZone(addr + BZONE, data);
	result += WriteEEPROM_Word_NoZone(addr + CZONE, data);
	if(result != 0) {
		System_ERROR_UserCallback(ERROR_EEPROM_COM);
	}
}

void ReadEEPROM_ByteData_StartUp(void) {
	UINT16  i;
	UINT16  j;
	UINT16  t_u16RdTemp;
    INT16  t_i16RdTemp;
	UINT16  t_u16TempMax,t_u16TempMin;
	
	const struct PRT_E2ROM_PARAS PrtE2PARAS_Default 	= E2P_PROTECT_DEFAULT_PRT;
    const struct PRT_E2ROM_PARAS PrtE2paras_Min 		= E2P_PROTECT_MIN_PRT;
    const struct PRT_E2ROM_PARAS PrtE2paras_Max     	= E2P_PROTECT_MAX_PRT;
    const struct PRT_E2ROM_PARAS PrtE2paras_Pos 		= E2P_ADDR_E2POS_PROTECT;

	const struct RTC_ELEMENT RTC_element_Default	= RTC_element_default;
	const struct RTC_ELEMENT RTC_element_Min		= RTC_element_min;
	const struct RTC_ELEMENT RTC_element_Max		= RTC_element_max;
	const struct RTC_ELEMENT RTC_element_Pos		= E2P_ADDR_E2POS_RTC;

	const struct OTHER_ELEMENT OtherElement_to_default 	= OtherElement_default;
	const struct OTHER_ELEMENT OtherElement_to_Max 		= OtherElement_max;
    const struct OTHER_ELEMENT OtherElement_to_Min     	= OtherElement_min;
	const struct OTHER_ELEMENT OtherElement_to_Pos 		= E2P_ADDR_E2POS_OTHER_ELEMENT1;

	const struct HEAT_COOL_ELEMENT HeatCoolEle_Default 	= HeatCoolElement_Default;
	const struct HEAT_COOL_ELEMENT HeatCoolEle_Max 		= HeatCoolElement_Max;
    const struct HEAT_COOL_ELEMENT HeatCoolEle_Min     	= HeatCoolElement_Min;
	const struct HEAT_COOL_ELEMENT HeatCoolEle_Pos 		= E2P_ADDR_E2POS_HEAT_COOL;


    for(i = 0; i < E2P_PARA_NUM_PROTECT; ++i) {		//������
    	t_u16RdTemp = ReadEEPROM_Word_WithZone((UINT16)*(&PrtE2paras_Pos.u16VcellOvp_First+i));
		t_u16TempMax = (*(&PrtE2paras_Max.u16VcellOvp_First+i));
		t_u16TempMin =  (*(&PrtE2paras_Min.u16VcellOvp_First+i));
		if ((t_u16RdTemp >= t_u16TempMin)&&(t_u16RdTemp <= t_u16TempMax)) {
        	*(&PRT_E2ROMParas.u16VcellOvp_First+i) = t_u16RdTemp;
        }
        else {
        	*(&PRT_E2ROMParas.u16VcellOvp_First+i) = *(&PrtE2PARAS_Default.u16VcellOvp_First+i);
			if(0 == System_ErrFlag.u8ErrFlag_Com_EEPROM) {		//������ʵ��̫�ã���õİ취�ǰ�ReadEEPROM_Word_WithZone()���������������
				//g_st_SysStatusFlag.bits.b1EepromErr = 1;		//���¸�����һ��������������ʧ�ܸ��գ����ĺù���
				System_ERROR_UserCallback(ERROR_EEPROM_STORE);			//ֻҪȷ��ͨѶû���⣬�����������
			}
        }
	}

    for(i = 0; i < E2P_PARA_NUM_RTC; ++i) {		//RTC
    	t_u16RdTemp = ReadEEPROM_Word_WithZone((UINT16)*(&RTC_element_Pos.RTC_Time_Year+i));
		t_u16TempMax = (*(&RTC_element_Max.RTC_Time_Year+i));
		t_u16TempMin =  (*(&RTC_element_Min.RTC_Time_Year+i));
		if ((t_u16RdTemp >= t_u16TempMin)&&(t_u16RdTemp <= t_u16TempMax)) {
        	*(&RTC_time.RTC_Time_Year+i) = t_u16RdTemp;
        }
        else {
        	*(&RTC_time.RTC_Time_Year+i) = *(&RTC_element_Default.RTC_Time_Year+i);
			if(0 == System_ErrFlag.u8ErrFlag_Com_EEPROM) {
				//g_st_SysStatusFlag.bits.b1EepromErr = 1;
				System_ERROR_UserCallback(ERROR_EEPROM_STORE);
			}
        }
	}

	for(i = 0; i < E2P_PARA_NUM_CALIB_K; ++i) {		//Kֵ
    	t_u16RdTemp = ReadEEPROM_Word_WithZone(E2P_ADDR_START_CALIB_K + (i<<1));
		if ((t_u16RdTemp  >=  SYSKMIN)&&(t_u16RdTemp <= SYSKMAX)) {
        	g_u16CalibCoefK[i] = t_u16RdTemp;
        }
        else {
			if(0 == System_ErrFlag.u8ErrFlag_Com_EEPROM) {
				System_ERROR_UserCallback(ERROR_EEPROM_STORE);
			}
        	g_u16CalibCoefK[i] = SYSKDEFAULT;
        }

		t_i16RdTemp = ReadEEPROM_Word_WithZone(E2P_ADDR_START_CALIB_B + (i<<1));
		if ((t_i16RdTemp  >=  SYSBMIN)&&(t_i16RdTemp  <= SYSBMAX)) {
        	g_i16CalibCoefB[i] = t_i16RdTemp;		//Bֵ
        }
        else {
			if(0 == System_ErrFlag.u8ErrFlag_Com_EEPROM) {
				System_ERROR_UserCallback(ERROR_EEPROM_STORE);
			}			
        	g_i16CalibCoefB[i] = SYSBDEFAULT;
        }
	}

	for(i = 0; i < SOC_TABLE_SIZE; ++i) {		//SOC��
    	t_u16RdTemp = ReadEEPROM_Word_WithZone(E2P_ADDR_START_SOC_TABLE + (i<<1));
		if(i%2 == 0) {
			//if((t_u16RdTemp  >=  SOC_VOL_MIN)&&(t_u16RdTemp <= SOC_VOL_MAX)) {
			if(t_u16RdTemp <= SOC_VOL_MAX) {	//�����������޷�������Ϊ���棬��ɾ��
	        	SOC_Table_Set[i] = t_u16RdTemp;
	        }
	        else {
				if(0 == System_ErrFlag.u8ErrFlag_Com_EEPROM) {
					System_ERROR_UserCallback(ERROR_EEPROM_STORE);
				}
	        	SOC_Table_Set[i] = SOC_Table_Default[i];
	        }
		}
		else {
			//if((t_u16RdTemp  >=  SOC_VALUE_MIN)&&(t_u16RdTemp  <= SOC_VALUE_MAX)) {
			if(t_u16RdTemp  <= SOC_VALUE_MAX) {
	        	SOC_Table_Set[i] = t_u16RdTemp;
	        }
	        else {
				if(0 == System_ErrFlag.u8ErrFlag_Com_EEPROM) {
					System_ERROR_UserCallback(ERROR_EEPROM_STORE);
				}				
	        	SOC_Table_Set[i] = SOC_Table_Default[i];
	        }
		}
	}

	for(i = 0; i < E2P_PARA_NUM_COPPERLOSS; ++i) {		//ͭ�����ֵ
    	t_u16RdTemp = ReadEEPROM_Word_WithZone(E2P_ADDR_START_COPPERLOSS + (i<<1));
		//if ((t_u16RdTemp  >=  CopperLoss_Min)&&(t_u16RdTemp <= CopperLoss_Max)) {
		if(t_u16RdTemp <= CopperLoss_Max) {
        	CopperLoss[i] = t_u16RdTemp;
        }
        else {
			if(0 == System_ErrFlag.u8ErrFlag_Com_EEPROM) {
				System_ERROR_UserCallback(ERROR_EEPROM_STORE);
			}
        	CopperLoss[i] = CopperLoss_Default;
        }

		t_u16RdTemp = ReadEEPROM_Word_WithZone(E2P_ADDR_START_COPPERLOSS_NUM + (i<<1));
		//if ((t_u16RdTemp  >=  CopperLossNum_Min)&&(t_u16RdTemp  <= CopperLossNum_Max)) {
		if(t_u16RdTemp  <= CopperLossNum_Max) {
        	CopperLoss_Num[i] = t_u16RdTemp;			//ͭ����
        }
        else {
			if(0 == System_ErrFlag.u8ErrFlag_Com_EEPROM) {
				System_ERROR_UserCallback(ERROR_EEPROM_STORE);
			}
        	CopperLoss_Num[i] = CopperLossNum_Default;
        }
	}

	for(i = 0; i < Record_len; ++i) {
    	t_u16RdTemp = ReadEEPROM_Word_WithZone(E2P_ADDR_START_FR_FIRST + (i<<1));
		if (((t_u16RdTemp  >=  CellOvp_First)&&(t_u16RdTemp  <= CellSocUp_First))\
			|| t_u16RdTemp == 0) {
        	Fault_record_First[i]  =  t_u16RdTemp;		//һ������
        }
        else {
			if(0 == System_ErrFlag.u8ErrFlag_Com_EEPROM) {
				System_ERROR_UserCallback(ERROR_EEPROM_STORE);
			}
        	Fault_record_First[i]  =  0;
        }

		t_u16RdTemp = ReadEEPROM_Word_WithZone(E2P_ADDR_START_FR_SECOND + (i<<1));
		if (((t_u16RdTemp  >=  CellOvp_Second)&&(t_u16RdTemp  <= CellSocUp_Second))\
			|| t_u16RdTemp == 0) {
        	Fault_record_Second[i]  =  t_u16RdTemp;		//��������
        }
        else {
			if(0 == System_ErrFlag.u8ErrFlag_Com_EEPROM) {
				System_ERROR_UserCallback(ERROR_EEPROM_STORE);
			}			
        	Fault_record_Second[i]  =  0;
        }

		t_u16RdTemp = ReadEEPROM_Word_WithZone(E2P_ADDR_START_FR_THIRD + (i<<1));
		if (((t_u16RdTemp  >=  CellOvp_Third)&&(t_u16RdTemp  <= CellSocUp_Third))\
			|| t_u16RdTemp == 0) {
        	Fault_record_Third[i]  =  t_u16RdTemp;		//��������
        }
        else {
			if(0 == System_ErrFlag.u8ErrFlag_Com_EEPROM) {
				System_ERROR_UserCallback(ERROR_EEPROM_STORE);
			}
        	Fault_record_Third[i]  =  0;
        }
	}


	FaultPoint_First = ReadEEPROM_Word_WithZone(E2P_ADDR_E2POS_FR_TEMP_FIRST);	//����ָ��
	FaultPoint_Second = ReadEEPROM_Word_WithZone(E2P_ADDR_E2POS_FR_TEMP_SECOND);
	FaultPoint_Third = ReadEEPROM_Word_WithZone(E2P_ADDR_E2POS_FR_TEMP_THIRD);

	for(i = 0; i < Record_len; i++) {				//��������RTC
		for(j = 0; j < 6; ++j) {
			t_u16TempMax = (*(&RTC_element_Max.RTC_Time_Year + j));
			t_u16TempMin =  (*(&RTC_element_Min.RTC_Time_Year + j));
			t_u16RdTemp = ReadEEPROM_Word_WithZone(E2P_ADDR_START_FR_THIRD_RTC + ((i*6 + j)<<1));
			if ((t_u16RdTemp >= t_u16TempMin)&&(t_u16RdTemp <= t_u16TempMax)) {
	        	RTC_Fault_record_Third[i][j] = t_u16RdTemp;
	        }
	        else {
				if(0 == System_ErrFlag.u8ErrFlag_Com_EEPROM) {
					System_ERROR_UserCallback(ERROR_EEPROM_STORE);
				}
	        	RTC_Fault_record_Third[i][j] = *(&RTC_element_Default.RTC_Time_Year+j);	
	        }
		}
	}

    for(i = 0; i < E2P_PARA_NUM_OTHER_ELEMENT1; ++i) {		//Other_CanAdd
    	t_u16RdTemp = ReadEEPROM_Word_WithZone((UINT16)*(&OtherElement_to_Pos.u16Balance_OpenVoltage+i));
		t_u16TempMax = (*(&OtherElement_to_Max.u16Balance_OpenVoltage+i));
		t_u16TempMin =  (*(&OtherElement_to_Min.u16Balance_OpenVoltage+i));
		if ((t_u16RdTemp >= t_u16TempMin) && (t_u16RdTemp <= t_u16TempMax)) {
        	*(&OtherElement.u16Balance_OpenVoltage+i) = t_u16RdTemp;
        }
        else {
        	*(&OtherElement.u16Balance_OpenVoltage+i) = *(&OtherElement_to_default.u16Balance_OpenVoltage+i);
			if(0 == System_ErrFlag.u8ErrFlag_Com_EEPROM) {
				//g_st_SysStatusFlag.bits.b1EepromErr = 1;
				System_ERROR_UserCallback(ERROR_EEPROM_STORE);
			}
        }
	}

    for(i = 0; i < E2P_PARA_NUM_HEAT_COOL; ++i) {		//HeatCool_element
    	t_u16RdTemp = ReadEEPROM_Word_WithZone((UINT16)*(&HeatCoolEle_Pos.u16Heat_OpenTemp+i));
		t_u16TempMax = (*(&HeatCoolEle_Max.u16Heat_OpenTemp+i));
		t_u16TempMin =  (*(&HeatCoolEle_Min.u16Heat_OpenTemp+i));
		if ((t_u16RdTemp >= t_u16TempMin) && (t_u16RdTemp <= t_u16TempMax)) {
        	*(&Heat_Cool_Element.u16Heat_OpenTemp+i) = t_u16RdTemp;
        }
        else {
        	*(&Heat_Cool_Element.u16Heat_OpenTemp+i) = *(&HeatCoolEle_Default.u16Heat_OpenTemp+i);
			if(0 == System_ErrFlag.u8ErrFlag_Com_EEPROM) {
				//g_st_SysStatusFlag.bits.b1EepromErr = 1;
				System_ERROR_UserCallback(ERROR_EEPROM_STORE);
			}
        }
	}

	ReadEEPROM_AFE_Parameters();
	ReadEEPROM_EventRecord_Parameters();
}


//Sci���������
void EEPROM_ResetData_AllToDefault(void) {
	const struct PRT_E2ROM_PARAS PrtE2PARAS_Default = E2P_PROTECT_DEFAULT_PRT;
	const struct RTC_ELEMENT RTC_element_Default = RTC_element_default;
	const struct OTHER_ELEMENT OtherElement_Default = OtherElement_default;
	const struct HEAT_COOL_ELEMENT HeatCoolEle_Default 	= HeatCoolElement_Default;
	UINT8 i;

	//KB
	for(i = 0;i < KB_NUM; ++i) {
		g_u16CalibCoefK[i] = SYSKDEFAULT;
	   	g_i16CalibCoefB[i] = SYSBDEFAULT;
	}
	u8E2P_KB_WriteFlag = KB_NUM;
	u8E2P_KB_WritePos = 0;

	//Protect
	for(i = 0;i < E2P_PARA_NUM_PROTECT; ++i) {
		*(&PRT_E2ROMParas.u16VcellOvp_First+i) = *(&PrtE2PARAS_Default.u16VcellOvp_First+i);
	}
    u32E2P_Pro_VolCur_WriteFlag = E2P_PARA_ALL_VOLCUR_PROTECT;	
	u32E2P_Pro_Temp_WriteFlag = E2P_PARA_ALL_TEM_PROTECT;
	u32E2P_Pro_Other_WriteFlag = E2P_PARA_ALL_OTHER_PROTECT;

	//SocTable
	for(i = 0;i < E2P_PARA_NUM_SOC_TABLE; ++i) {
		SOC_Table_Set[i] = SOC_Table_Default[i];
	}
	u8E2P_SocTable_WriteFlag = E2P_PARA_NUM_SOC_TABLE;

	//CopperLoss
	for(i = 0; i < E2P_PARA_NUM_COPPERLOSS; ++i) {
		CopperLoss[i] = CopperLoss_Default;
		CopperLoss_Num[i] = CopperLossNum_Default;
	}
	u8E2P_CopperLoss_WriteFlag = E2P_PARA_NUM_COPPERLOSS;

	//RTC_element
	for(i = 0; i < E2P_PARA_NUM_RTC; ++i) {
		*(&RTC_time.RTC_Time_Year+i) = *(&RTC_element_Default.RTC_Time_Year+i);
	}
	u32E2P_RTC_Element_WriteFlag = E2P_PARA_ALL_RTC_ELEMENT;

	//Other_CanAdd_element
	for(i = 0; i < E2P_PARA_NUM_OTHER_ELEMENT1; ++i) {
		*(&OtherElement.u16Balance_OpenVoltage+i) = *(&OtherElement_Default.u16Balance_OpenVoltage+i);
	}
	u32E2P_OtherElement1_WriteFlag = E2P_PARA_ALL_OTHER_ELEMENT1;

	//HeatCool_element
	for(i = 0; i < E2P_PARA_NUM_HEAT_COOL; ++i) {
		*(&Heat_Cool_Element.u16Heat_OpenTemp+i) = *(&HeatCoolEle_Default.u16Heat_OpenTemp+i);
	}
	u32E2P_HeatCool_WriteFlag = E2P_PARA_ALL_HEAT_COOL_ELE;
}


//��ʷ������¼reset
void EEPROM_ResetData_OtherToDefault(void) {
	UINT8 i,j;
	const struct RTC_ELEMENT RTC_element_Default = RTC_element_default;
	
	for(i = 0; i < Record_len; ++i) {
		Fault_record_First[i] = 0;
		Fault_record_Second[i] = 0;
		Fault_record_Third[i] = 0;
		for(j = 0; j < 6; ++j) {
			RTC_Fault_record_Third[i][j] = *(&RTC_element_Default.RTC_Time_Year+j);
		}
	}
	FaultPoint_First = 0;
	FaultPoint_Second = 0;
	FaultPoint_Third = 0;

	for(i = 0; i < Record_len; ++i) {
		WriteEEPROM_Word_WithZone(E2P_ADDR_START_FR_FIRST + (i<<1),Fault_record_First[i]);
		WriteEEPROM_Word_WithZone(E2P_ADDR_START_FR_SECOND + (i<<1),Fault_record_Second[i]);
		WriteEEPROM_Word_WithZone(E2P_ADDR_START_FR_THIRD + (i<<1),Fault_record_Third[i]);
		for(j = 0; j < 6; ++j) {
			WriteEEPROM_Word_WithZone(E2P_ADDR_START_FR_THIRD_RTC + ((i*6+j)<<1), RTC_Fault_record_Third[i][j]);
		}
	}
	WriteEEPROM_Word_WithZone(E2P_ADDR_E2POS_FR_TEMP_FIRST, FaultPoint_First);
	WriteEEPROM_Word_WithZone(E2P_ADDR_E2POS_FR_TEMP_SECOND, FaultPoint_Second);
	WriteEEPROM_Word_WithZone(E2P_ADDR_E2POS_FR_TEMP_THIRD, FaultPoint_Third);

	EEPROM_ResetData_AFE_ParametersToDefault();
	EEPROM_ResetData_EventRecord_ToDefault();

	SystemMonitorResetData_EEPROM();					//ϵͳ����ѡȡ��־λ�洢
	WriteEEPROM_Word_NoZone(EEPROM_ADDR_PASS, EEPROM_VALUE_BEGIN_FLAG);			//��һ���ϵ��ʼ�����
}


//Sci������У���ΪSTM8��Ե�ʣ�����ȫ����ͨѶ���Ƴ���д
void WriteEEPROM_ByteData_Circle(void) {
	UINT8 i = 0;
	UINT8 u8temp;
	const struct PRT_E2ROM_PARAS PrtE2paras_Pos = E2P_ADDR_E2POS_PROTECT;
	const struct OTHER_ELEMENT OtherCanAdd_Pos = E2P_ADDR_E2POS_OTHER_ELEMENT1;
	const struct RTC_ELEMENT RTC_Element_Pos = E2P_ADDR_E2POS_RTC;
	const struct HEAT_COOL_ELEMENT HeatCoolEle_Pos = E2P_ADDR_E2POS_HEAT_COOL;

	if(u8E2P_KB_WriteFlag) {		//����KBֵ�������ȿ�ȫ��дһ�飬Ҳ���Ե���д����һ��KBֵ
		WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_K + (u8E2P_KB_WritePos<<1)), g_u16CalibCoefK[u8E2P_KB_WritePos]);
		WriteEEPROM_Word_WithZone((E2P_ADDR_START_CALIB_B + (u8E2P_KB_WritePos<<1)), g_i16CalibCoefB[u8E2P_KB_WritePos]);
		++u8E2P_KB_WritePos;		//���u8E2P_KB_WriteFlag=0����Pos�����Ҳû�ã���ĵط����޸�KBֵ�Ļ��������߱���ͬʱ������
		--u8E2P_KB_WriteFlag;
	}
	else if(u32E2P_Pro_VolCur_WriteFlag & E2P_PARA_ALL_VOLCUR_PROTECT) {
		while(i < E2P_PARA_ALL_VOLCUR_PROTECT) {
			if((u32E2P_Pro_VolCur_WriteFlag>>i)&1) {
				WriteEEPROM_Word_WithZone((UINT16)*(&PrtE2paras_Pos.u16VcellOvp_First+i),
					*(&PRT_E2ROMParas.u16VcellOvp_First+i));
				u32E2P_Pro_VolCur_WriteFlag -= ((long)1<<i);	   //��λ��������һ����һ����
				break;
			}
			i++;
		}
	}
	else if(u32E2P_Pro_Temp_WriteFlag & E2P_PARA_ALL_TEM_PROTECT) {
		while(i < E2P_PARA_ALL_TEM_PROTECT) {
			if((u32E2P_Pro_Temp_WriteFlag>>i)&1) {
				WriteEEPROM_Word_WithZone((UINT16)*(&PrtE2paras_Pos.u16TChgOTp_First+i),
					*(&PRT_E2ROMParas.u16TChgOTp_First+i));
				u32E2P_Pro_Temp_WriteFlag -= ((long)1<<i);
				break;
			}
			i++;
		}
	}
	else if(u32E2P_Pro_Other_WriteFlag & E2P_PARA_ALL_OTHER_PROTECT) {
		while(i < E2P_PARA_ALL_OTHER_PROTECT) {
			if((u32E2P_Pro_Other_WriteFlag>>i)&1) {
				WriteEEPROM_Word_WithZone((UINT16)*(&PrtE2paras_Pos.u16VdeltaOvp_First+i),
					*(&PRT_E2ROMParas.u16VdeltaOvp_First+i));
				u32E2P_Pro_Other_WriteFlag -= ((long)1<<i);
				break;
			}
			i++;
		}
	}
	else if(u32E2P_OtherElement1_WriteFlag & E2P_PARA_ALL_OTHER_ELEMENT1) {
		while(i < E2P_PARA_ALL_OTHER_ELEMENT1) {
			if((u32E2P_OtherElement1_WriteFlag>>i)&1) {
				WriteEEPROM_Word_WithZone((UINT16)*(&OtherCanAdd_Pos.u16Balance_OpenVoltage+i),
					*(&OtherElement.u16Balance_OpenVoltage+i));
				u32E2P_OtherElement1_WriteFlag -= ((long)1<<i);
				break;
			}
			i++;
		}
	}
	else if(u32E2P_RTC_Element_WriteFlag) {
		while(i < E2P_PARA_ALL_RTC_ELEMENT) {
			if((u32E2P_RTC_Element_WriteFlag>>i)&1) {
				WriteEEPROM_Word_WithZone((UINT16)*(&RTC_Element_Pos.RTC_Time_Year+i),
					*(&RTC_time.RTC_Time_Year+i));
				u32E2P_RTC_Element_WriteFlag -= ((long)1<<i);
				break;
			}
			i++;
		}
	}
	else if(u8E2P_SocTable_WriteFlag) {
		u8temp = E2P_PARA_NUM_SOC_TABLE - u8E2P_SocTable_WriteFlag;
		WriteEEPROM_Word_WithZone(E2P_ADDR_START_SOC_TABLE + (u8temp<<1), SOC_Table_Set[u8temp]);
		u8E2P_SocTable_WriteFlag--;
	}
	else if(u8E2P_CopperLoss_WriteFlag) {
		u8temp = E2P_PARA_NUM_COPPERLOSS - u8E2P_CopperLoss_WriteFlag;
		WriteEEPROM_Word_WithZone(E2P_ADDR_START_COPPERLOSS + (u8temp<<1), CopperLoss[u8temp]);
		WriteEEPROM_Word_WithZone(E2P_ADDR_START_COPPERLOSS_NUM + (u8temp<<1), CopperLoss_Num[u8temp]);
		u8E2P_CopperLoss_WriteFlag--;
	}
	else if(u32E2P_HeatCool_WriteFlag) {
		for(i = 0; i < E2P_PARA_NUM_HEAT_COOL; ++i) {
			if((u32E2P_HeatCool_WriteFlag>>i)&1) {
				WriteEEPROM_Word_WithZone((UINT16)*(&HeatCoolEle_Pos.u16Heat_OpenTemp+i),*(&Heat_Cool_Element.u16Heat_OpenTemp+i));
				u32E2P_HeatCool_WriteFlag -= ((long)1<<i);
				break;
			}
		}
	}
	else if(gu8_Reset_EventRecord) {
		u8temp = 100 - gu8_Reset_EventRecord;
		WriteEEPROM_Word_WithZone(E2P_ADDR_START_EVENT_RECORD + (u8temp<<1), 0);
		gu8_Reset_EventRecord--;
		if(gu8_Reset_EventRecord == 1) {
			WriteEEPROM_Word_WithZone(E2P_ADDR_E2POS_EVENT_POINT, 0);
		}
	}
}


//��ʼ��IIC
void InitE2PROM(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	//PB3_I2C_SCL_eeprom��PB5_I2C_SDA_eeprom
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//PB3ΪJTAG��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOB, GPIO_Pin_10|GPIO_Pin_11); 	//�����

	//E2PR_WP
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOB, GPIO_Pin_13); 	//�����	

	InitData_E2prom();
}


void InitData_E2prom(void) {
    if(EEPROM_VALUE_BEGIN_FLAG == ReadEEPROM_Word_NoZone(EEPROM_ADDR_PASS)) {//�ڶ����ϵ�ͻ�ִ�����
    	ReadEEPROM_ByteData_StartUp();
	}
	else {     								//��һ���ϵ磬��������
    	EEPROM_ResetData_AllToDefault();
    	while(u8E2P_KB_WriteFlag\
		|| u32E2P_Pro_VolCur_WriteFlag || u32E2P_Pro_Temp_WriteFlag || u32E2P_Pro_Other_WriteFlag\
		|| u8E2P_SocTable_WriteFlag|| u8E2P_CopperLoss_WriteFlag || u32E2P_RTC_Element_WriteFlag\
		|| u32E2P_OtherElement1_WriteFlag || u32E2P_HeatCool_WriteFlag) {	//0x2000,0x2100,0x2200,0x2300
    		WriteEEPROM_ByteData_Circle();
    	}
		EEPROM_ResetData_OtherToDefault();	//��E2P_BEGIN_FLAGд��ͷ��ַ��
											//����б����ӣ��������������д��Ŀǰ���˱�����¼��ʼ��
		WriteProID_Default();
	}
}


void App_E2promDeal(void) {
    if(u8E2P_KB_WriteFlag\
		|| u32E2P_Pro_VolCur_WriteFlag || u32E2P_Pro_Temp_WriteFlag || u32E2P_Pro_Other_WriteFlag\
		|| u8E2P_SocTable_WriteFlag|| u8E2P_CopperLoss_WriteFlag || u32E2P_RTC_Element_WriteFlag\
		|| u32E2P_OtherElement1_WriteFlag || u32E2P_HeatCool_WriteFlag) {	//0x2000,0x2100,0x2200,0x2300
		WriteEEPROM_ByteData_Circle();
    }

	if(gu8_Reset_EventRecord) {			//�����������
		WriteEEPROM_ByteData_Circle();
	}
}


//�����ҳ���������BC��д����ȥ������0xFF
void EEPROM_test(void) {
	#if 0
	if(0 == g_st_SysTimeFlag.bits.b1Sys1000msFlag2) {
		return;
	}
	#endif
	
	#if 1
	WriteEEPROM_Word_NoZone(0x20, EEPROM_VALUE_FLASHUPDATE);
	g_stCellInfoReport.u16VCell[30] = ReadEEPROM_Word_NoZone(0x20);

	WriteEEPROM_Word_NoZone(0x22, EEPROM_VALUE_FLASHUPDATE_RESET);
	g_stCellInfoReport.u16VCell[31] = ReadEEPROM_Word_NoZone(0x22)&0x000F;
	//g_stCellInfoReport.u16VCell[31] = 111;
	#endif

	#if 0
	WriteEEPROM_Byte(0x20, 0x11);
	g_stCellInfoReport.u16VCell[5] = ReadEEPROM_Byte(0x20);

	WriteEEPROM_Byte(0x22, 0x12);
	g_stCellInfoReport.u16VCell[6] = ReadEEPROM_Byte(0x22);
	#endif
}

