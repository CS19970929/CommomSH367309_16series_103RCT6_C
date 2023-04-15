#include "main.h"

AFEDATA Registers_AFE1;
struct SH367309_Read SH367309_Read_AFE1;


#define LENGTH_TBLTEMP_AFE_10K    ((UINT16)56)
const UINT16 iSheldTemp_10K_AFE[LENGTH_TBLTEMP_AFE_10K] = {
	//AD(kΩ*100)		(Temp+40)*10
	11611	,	100 ,	//-30
	8935	,	150 ,	//-25
	6943	,	200 ,	//-20
	5442	,	250 ,	//-15
	4300	,	300 ,	//-10
	3422	,	350 ,	//-5
	2751	,	400 ,	//0
	2214	,	450 ,	//5
	1801	,	500 ,	//10
	1470	,	550 ,	//15
	1209	,	600 ,	//20
	1000	,	650 ,	//25
	831		,	700 ,	//30
	694		,	750 ,	//35
	583		,	800 ,	//40
	492		,	850 ,	//45
	416		,	900 ,	//50
	355		,	950 ,	//55
	303		,	1000,	//60
	260		,	1050,	//65
	224		,	1100,	//70
	193		,	1150,	//75
	167		,	1200,	//80
	146		,	1250,	//85
	127 	,	1300,	//90
	111 	,	1350,	//95
	98 		,	1400,	//100
	86 		,	1450,	//105
};


//这个code是啥
//const UINT8 code CRC8Table[]=
const UINT8 CRC8Table[]= {							//120424-1			CRC Table
	0x00,0x07,0x0E,0x09,0x1C,0x1B,0x12,0x15,0x38,0x3F,0x36,0x31,0x24,0x23,0x2A,0x2D,
	0x70,0x77,0x7E,0x79,0x6C,0x6B,0x62,0x65,0x48,0x4F,0x46,0x41,0x54,0x53,0x5A,0x5D,
	0xE0,0xE7,0xEE,0xE9,0xFC,0xFB,0xF2,0xF5,0xD8,0xDF,0xD6,0xD1,0xC4,0xC3,0xCA,0xCD,
	0x90,0x97,0x9E,0x99,0x8C,0x8B,0x82,0x85,0xA8,0xAF,0xA6,0xA1,0xB4,0xB3,0xBA,0xBD,
	0xC7,0xC0,0xC9,0xCE,0xDB,0xDC,0xD5,0xD2,0xFF,0xF8,0xF1,0xF6,0xE3,0xE4,0xED,0xEA,
	0xB7,0xB0,0xB9,0xBE,0xAB,0xAC,0xA5,0xA2,0x8F,0x88,0x81,0x86,0x93,0x94,0x9D,0x9A,
	0x27,0x20,0x29,0x2E,0x3B,0x3C,0x35,0x32,0x1F,0x18,0x11,0x16,0x03,0x04,0x0D,0x0A,
	0x57,0x50,0x59,0x5E,0x4B,0x4C,0x45,0x42,0x6F,0x68,0x61,0x66,0x73,0x74,0x7D,0x7A,
	0x89,0x8E,0x87,0x80,0x95,0x92,0x9B,0x9C,0xB1,0xB6,0xBF,0xB8,0xAD,0xAA,0xA3,0xA4,
	0xF9,0xFE,0xF7,0xF0,0xE5,0xE2,0xEB,0xEC,0xC1,0xC6,0xCF,0xC8,0xDD,0xDA,0xD3,0xD4,
	0x69,0x6E,0x67,0x60,0x75,0x72,0x7B,0x7C,0x51,0x56,0x5F,0x58,0x4D,0x4A,0x43,0x44,
	0x19,0x1E,0x17,0x10,0x05,0x02,0x0B,0x0C,0x21,0x26,0x2F,0x28,0x3D,0x3A,0x33,0x34,
	0x4E,0x49,0x40,0x47,0x52,0x55,0x5C,0x5B,0x76,0x71,0x78,0x7F,0x6A,0x6D,0x64,0x63,
	0x3E,0x39,0x30,0x37,0x22,0x25,0x2C,0x2B,0x06,0x01,0x08,0x0F,0x1A,0x1D,0x14,0x13,
	0xAE,0xA9,0xA0,0xA7,0xB2,0xB5,0xBC,0xBB,0x96,0x91,0x98,0x9F,0x8A,0x8D,0x84,0x83,
	0xDE,0xD9,0xD0,0xD7,0xC2,0xC5,0xCC,0xCB,0xE6,0xE1,0xE8,0xEF,0xFA,0xFD,0xF4,0xF3
};



/*******************************************************************************
Function: Delay4us()
Description: 
Input:
Output:
Others:
*******************************************************************************/
void Delay4us(void) {
	UINT8 i,j;
	
	#if 1
	for(j=0; j<8; j++) {	//72MHz
		for(i=0; i<13; i++) {
			//system clock = 24MHz
		}
	}
	#endif

	#if 0
	//当为5时，6.7us
	for(i=0; i<5; i++) {
		//system clock = 8MHz
	}
	#endif
}


#ifdef _TWI_COM
/*******************************************************************************
Function: CRC8cal()
Description:  calculate CRC
Input: 	      *p--data to calculate
          Length--data length
Output: crc8
********************************************************************************/
UINT8 CRC8cal(UINT8 *p, UINT8 Length) {  		   //look-up table calculte CRC 
    UINT8 crc8 = 0;    
    
	for(; Length > 0; Length--) {    
        crc8 = CRC8Table[crc8^*p];    
	    p++;    
    } 
       
    return(crc8);       
}



void F_TWI_CLK_OUT(void) {

}


void F_TWI_CLK_IN(void) {

}

void F_TWI_CLK_HIGH(void) {
	//TWI_CLK_OUT();
	GPIO_SetBits(GPIOB , GPIO_Pin_6); 
}

void F_TWI_CLK_LOW(void) {
	//TWI_CLK_OUT();
	GPIO_ResetBits(GPIOB , GPIO_Pin_6);
}

void F_TWI_DAT_OUT(void) {

}

void F_TWI_DAT_IN(void) {

}

void F_TWI_DAT_HIGH(void) {
//	TWI_DAT_OUT();
	GPIO_SetBits(GPIOB , GPIO_Pin_7); 
}

void F_TWI_DAT_LOW(void) {
//	TWI_DAT_OUT();
	GPIO_ResetBits(GPIOB , GPIO_Pin_7); 
}

uint8_t F_TWI_RD_DAT(void) {
	uint8_t ii;
	
	ii = GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_7);	
	
	return ii;
}

uint8_t F_TWI_RD_CLK(void) {
	uint8_t ii;
	
	ii = GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_6);
	
	return ii;
}


/*******************************************************************************
Function:
1. TwiStart()		
2. TwiReStart()
3. TwiStop()
Input: 
Output:
********************************************************************************/
void TwiStart(void) {
	TWI_DAT_HIGH;
	TWI_CLK_HIGH;
	TWI_DAT_OUT;
	TWI_CLK_OUT;
	TWI_DAT_LOW;
	Delay4us();
	TWI_CLK_LOW;
}

void TwiReStart(void) {
	TWI_DAT_HIGH;
	TWI_CLK_HIGH;
	Delay4us();
	TWI_DAT_LOW;
	Delay4us();
	TWI_CLK_LOW;
}

void TwiStop(void) {
	TWI_DAT_OUT;
	TWI_DAT_LOW;
	Delay4us();
	TWI_CLK_HIGH;
	Delay4us();
	TWI_DAT_HIGH;
	Delay4us();
	TWI_DAT_IN;
	TWI_CLK_IN;
}

/*******************************************************************************
Function:TwiChkClkRelease()
Description: check clk release
Input: 
Output:
********************************************************************************/
UINT8 TwiChkClkRelease(void) {
	UINT16 TimeoutCnt=1000;				//If Clock is not released within 4ms, is considered overtime
	UINT8 result=0;

	TWI_CLK_IN;

	while(TimeoutCnt--) {
		Feed_WatchDog;
		Delay4us();
		if(TWI_RD_CLK) {
			result = 1;
			break;
		}
	}

	TWI_CLK_HIGH;
	TWI_CLK_OUT;
	
	return result;
}


/*******************************************************************************
Function: TwiSendData()
Description: i2c send data(one byte)
Input: 		Data: data to send
			ClkFlg: 1 means need to check clk release	
Output:	RdData: the data received
********************************************************************************/
UINT8 TwiSendData(UINT8 Data, UINT8 ClkFlg) {
	UINT8 i;
	UINT8 result=0;

//1. After sending the Start signal, there is no need to detect Clock is released, And sending the first bit
	if(Data&0x80) {
		TWI_DAT_HIGH;
	}
	else {
		TWI_DAT_LOW;
	}
	
	if(ClkFlg == 1) {
		Delay4us();
		if(TwiChkClkRelease()) {
			TWI_CLK_LOW;
		}
		else {
			return result;
		}
	}
	else {
		Delay4us();
		TWI_CLK_HIGH;
		Delay4us();
		TWI_CLK_LOW;
	}
	
//2. Send the remaining seven bit
	Data = Data<<1;
	for(i=0; i<7; i++) {
		if(Data&0x80) {
			TWI_DAT_HIGH;
		}
		else {
			TWI_DAT_LOW;
		}
		Data = Data<<1;
		Delay4us();
		TWI_CLK_HIGH;
		Delay4us();
		TWI_CLK_LOW;
	}
	TWI_DAT_IN;
	Delay4us();
	
	for(i=0; i<10; i++) {
		if(TWI_RD_DAT == 0) {
			result = 1;
			break;
		}
	}
	TWI_CLK_HIGH;

	Delay4us();
	TWI_DAT_LOW;
	TWI_DAT_OUT;
	TWI_CLK_LOW;
	Delay4us();
	
	return result;
}

/*******************************************************************************
Function: TwiGetData()
Description: i2c get data(one byte)
Input: 		AckFlg: 0 means no need to send ack
Output:	RdData: the data received
********************************************************************************/
UINT8 TwiGetData(UINT8 AckFlg) {
	UINT8 i, RdData=0;

	TWI_DAT_IN;
	Delay4us();

	for(i=0; i<8; i++) {
		TWI_CLK_HIGH;
		Delay4us();
		if(TWI_RD_DAT) {
			RdData |= (1<<(7-i));
		}
		TWI_CLK_LOW;
		Delay4us();
	}

	TWI_DAT_OUT;
	if(AckFlg) {
		TWI_DAT_LOW;
	}
	else {
		TWI_DAT_HIGH;
	}
	TWI_CLK_HIGH;
	Delay4us();
	TWI_CLK_LOW;
	Delay4us();
	
	return RdData;
}


/*******************************************************************************
Function: TwiWrite()
Description:  write multi bytes
Input: SlaveID--Slave Address
          WrAddr--register addr
		  Length--write data length
          *WrBuf--data buffer 
Output: result:1--OK
               0--Error
********************************************************************************/
UINT8 TwiWrite(UINT8 SlaveID, UINT16 WrAddr, UINT8 Length, UINT8 *WrBuf) {
	UINT8 i;
    UINT8 TempBuf[4];
	UINT8 result = 0;

    TempBuf[0] = SlaveID;
    TempBuf[1] = (UINT8)WrAddr;
    TempBuf[2] = *WrBuf;
    TempBuf[3] = CRC8cal(TempBuf, 3);
	
	if(Length > 0) {
		TwiStart();
		
		if(!TwiSendData(SlaveID, 1)) {		//Send Slave ID
			goto WrErr;
		}
		
		if(TwiSendData(WrAddr, 0)) {				//Send Write Address(Low 8bit)
			result = 1;
			for(i=0; i<Length; i++) {
				if(TwiSendData(*WrBuf, 0)) {		//Send Write Data
					WrBuf++;
				}
				else {
					result = 0;
					break;
				}
			}
			if(!TwiSendData(TempBuf[3], 0)) {			//write CRC
				result = 0;
			}
		}
WrErr:
		TwiStop();
	}
	
	return result;
}


/*******************************************************************************
Function: TwiRead()
Description:  read multi bytes
Input: SlaveID--Slave Address
          RdAddr--register addr
		  Length--read data length
          *RdBuf--data buffer
Output: result:1--OK
               0--Error
Others:
********************************************************************************/
UINT8 TwiRead(UINT8 SlaveID, UINT16 RdAddr, UINT8 Length, UINT8 *RdBuf) {
	UINT8 i;
	UINT8 result=0;
    UINT8 TempBuf[46];
    UINT8 RdCrc=0;

    TempBuf[0] = SlaveID;
    TempBuf[1] = (UINT8)RdAddr;
    TempBuf[2] = Length;
    TempBuf[3] = SlaveID | 0x01;
	
	if(Length > 0) {
		TwiStart();
		
		if(!TwiSendData(SlaveID, 1)) {			//Send Slave ID
			goto RdErr;
		}
		
		if(!TwiSendData(RdAddr, 0)) {			//Send Read Address(Low 8bit)
			goto RdErr;
		}

		if(!TwiSendData(Length, 0)) {
			goto RdErr;
		}

		TwiReStart();
		
		if(TwiSendData(SlaveID|0x1, 0))	 {		//Send Slave ID
			result = 1;
			for(i=0; i<Length+1; i++) {
        		if(i == Length) {
                    RdCrc = TwiGetData(0);      //Get Data
        		}
        		else {
                    TempBuf[4+i] = TwiGetData(1);     //Get Data
        		}
			}

            if(RdCrc != CRC8cal(TempBuf, 4+Length)) {
                result = 0;    
            }
            else {
                for(i=0; i<Length; i++) {
                    *RdBuf = TempBuf[4+i];
                    RdBuf++;
					//下面的问题在于，如果传进来的数值不是16位，是8位，又有问题。
					//还是外部自己写一个大小端转换函数自己看情况是否处理
                    #if 0
                    //问题在于030的小端存储
					if(i == Length - 1 && Length%2) {
						*(RdBuf) = TempBuf[4+i];		//当取奇数个数据，最后一个，没形成对的那个孤零零的数据
					}
					else {
	                    if(i%2) {
							*(--RdBuf) = TempBuf[4+i];
							RdBuf += 2;
	                    }
						else {
							*(++RdBuf) = TempBuf[4+i];
						}
					}
					#endif
                }
            }
		}
	
RdErr:
		TwiStop();
	}
	
	return result;
}


/*******************************************************************************
Function: TwiWrite()
Description:  write multi bytes
Input: SlaveID--Slave Address
          WrAddr--register addr
		  Length--write data length
          *WrBuf--data buffer 
Output: result:1--OK
               0--Error
********************************************************************************/
UINT8 TwiWrite_old(UINT8 SlaveID, UINT16 WrAddr, UINT8 Length, UINT8 *WrBuf) {
	UINT8 i;
    UINT8 TempBuf[4];
	UINT8 result = 0;

    TempBuf[0] = SlaveID;
    TempBuf[1] = (UINT8)WrAddr;
    TempBuf[2] = *WrBuf;
    TempBuf[3] = CRC8cal(TempBuf, 3);
	
	if(Length > 0) {
		TwiStart();
		
		if(!TwiSendData(SlaveID, 1)) {		//Send Slave ID
			goto WrErr;
		}
		
		if(SlaveID == E2PROM_ID) {
			if(!TwiSendData(WrAddr>>8, 0)) {	//Send Write Address(High 8bit) For EEPROM
				goto WrErr;
			}
		}
		if(TwiSendData(WrAddr, 0)) {				//Send Write Address(Low 8bit)
			result = 1;
			for(i=0; i<Length; i++) {
				if(TwiSendData(*WrBuf, 0)) {		//Send Write Data
					WrBuf++;
				}
				else {
					result = 0;
					break;
				}
			}

            if(SlaveID == AFE_ID) {
                if(!TwiSendData(TempBuf[3], 0)) {           //write CRC
                    result = 0;
                }
            }
		}
WrErr:
		TwiStop();
	}
	
	return result;
}

/*******************************************************************************
Function: TwiRead()
Description:  read multi bytes
Input: SlaveID--Slave Address
          RdAddr--register addr
		  Length--read data length
          *RdBuf--data buffer
Output: result:1--OK
               0--Error
Others:
********************************************************************************/
UINT8 TwiRead_old(UINT8 SlaveID, UINT16 RdAddr, UINT8 Length, UINT8 *RdBuf) {
	UINT8 i;
	UINT8 result=0;
    UINT8 TempBuf[46];
    UINT8 RdCrc=0;

    TempBuf[0] = SlaveID;
    TempBuf[1] = (UINT8)RdAddr;
    TempBuf[2] = Length;
    TempBuf[3] = SlaveID | 0x01;
	
	if(Length > 0) {
		TwiStart();
		
		if(!TwiSendData(SlaveID, 1)) {			//Send Slave ID
			goto RdErr;
		}
		
		if(SlaveID == E2PROM_ID) {
			if(!TwiSendData(RdAddr>>8, 0)) {	//Send Read Address(High 8bit) For EEPROM
				goto RdErr;
			}
		}
		if(!TwiSendData(RdAddr, 0)) {			//Send Read Address(Low 8bit)
			goto RdErr;
		}

        if(SlaveID==AFE_ID) {         //CRC
            if(!TwiSendData(Length, 0)) {
                goto RdErr;
            }
        }
		
		TwiReStart();
		
		if(TwiSendData(SlaveID|0x1, 0))	 {		//Send Slave ID
			result = 1;
            if(SlaveID==AFE_ID) {
    			for(i=0; i<Length+1; i++) {
            		if(i == Length) {
                        RdCrc = TwiGetData(0);      //Get Data
            		}
            		else {
                        TempBuf[4+i] = TwiGetData(1);     //Get Data
            		}
    			}

                if(RdCrc != CRC8cal(TempBuf, 4+Length)) {
                    result = 0;    
                }
                else {
                    for(i=0; i<Length; i++) {
                        *RdBuf = TempBuf[4+i];
                        RdBuf++;   
                    }
                }
            }
            else {
                for(i=0; i<Length; i++) {
        			*RdBuf = TwiGetData(Length-i-1);//Get Data
                    RdBuf++;
                }
            }
		}
	
RdErr:
		TwiStop();
	}
	
	return result;
}

#endif


//RAM类型寄存器，用这个函数写，大于0x40的类型
//Output: result:1--OK，0--Error
//这个返回值就不改了，和目前体系相反，别的函数返回来就行
UINT8 MTPWrite(UINT8 WrAddr, UINT8 Length, UINT8 *WrBuf) {
	UINT8 result;
	UINT8 i;
	Feed_WatchDog;

    for(i=0; i<Length; i++) {
		result = TwiWrite(AFE_ID, WrAddr, 1, WrBuf);
		if(!result) {
			Delay1ms(1);
            result = TwiWrite(AFE_ID, WrAddr, 1, WrBuf);
            if(!result) {
                break;
            }
		}
		WrAddr++;
		WrBuf++;
		Delay1ms(1);
 	}

	if(!result) {
		System_ERROR_UserCallback(ERROR_AFE1);
	}
	
    return result;  
}


//0x00----0x19，写EEPROM寄存器用这个函数
//Output: result:1--OK，0--Error
//这个返回值就不改了，和目前体系相反，别的函数返回来就行
UINT8 MTPWriteROM(UINT8 WrAddr, UINT8 Length, UINT8 *WrBuf) {
	UINT8 result;
	UINT8 i;

	for(i=0; i<Length; i++) {
		Feed_WatchDog;
		result = TwiWrite(AFE_ID, WrAddr, 1, WrBuf);
		if(!result) {
			Delay1ms(40);
			result = TwiWrite(AFE_ID, WrAddr, 1, WrBuf);
			if(!result) {
				break;
			}
		}
		WrAddr++;
		WrBuf++;
		Delay1ms(40);
	}
	
	if(!result) {
		System_ERROR_UserCallback(ERROR_AFE1);
	}

    return result;  
}


//Output: result:1--OK，0--Error
//这个返回值就不改了，和目前体系相反，别的函数返回来就行
UINT8 MTPRead(UINT8 RdAddr, UINT8 Length, UINT8 *RdBuf) {
	UINT8 result = 1;
	
	Feed_WatchDog;

	/*
    if(System_ErrFlag.u8ErrFlag_Com_AFE1) {
        result = 0;
    }
    else {
		result = TwiRead(AFE_ID, RdAddr, Length, RdBuf);
		if(!result) {
			result = TwiRead(AFE_ID, RdAddr, Length, RdBuf);
		}
    }
    */
    
	result = TwiRead(AFE_ID, RdAddr, Length, RdBuf);
	if(!result) {
		result = TwiRead(AFE_ID, RdAddr, Length, RdBuf);
	}

	if(!result) {
		System_ERROR_UserCallback(ERROR_AFE1);
	}
	return result;
}


//0为sleep模式，1为idle模式
void InitAFE1_Sleep(UINT8 mode) {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOB, GPIO_Pin_8|GPIO_Pin_9); 	//输出高

	AFE_IsReady();
	//因为已经初始化过了，休眠的时候调用，不需要再次初始化参数
	//这玩意会复位模拟前端，导致MOS开关打开关闭反复
	//SH367309_UpdataAfeConfig();
	
	if(mode) {
		SH367309_Enable_AFE_Wdt_Cadc_Drivers();
	}
}


/*******************************************************************************
Function:InitAFE() 
Description:  check SH367309 is ready, and initialization MTP Buffer
Input:	NULL 	
Output: NULL
Others:
*******************************************************************************/
void InitAFE1(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB , &GPIO_InitStructure);	
	GPIO_SetBits(GPIOB, GPIO_Pin_8|GPIO_Pin_9); 	//输出高

	//预充MOS，模拟前端驱动的补充
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	


	#ifdef _SLEEP_WITH_CURRENT
	if(FLASH_309_RTC_RTC_VALUE == FlashReadOneHalfWord(FLASH_ADDR_SH367309_FLAG)\
		|| FLASH_309_RTC_NORMAL_VALUE == FlashReadOneHalfWord(FLASH_ADDR_SH367309_FLAG)) {
		AFE_PARAM_WRITE_Flag = 0;		//如果是RTC起来的，则不需要进入烧写模式，烧写模式会短暂关闭MOS，这个岂不是可以直接控制MOS关掉了嘛？
	}
	#endif	

	__delay_ms(100);
	AFE_IsReady();
	SH367309_UpdataAfeConfig();
	SH367309_Enable_AFE_Wdt_Cadc_Drivers();
}


/*调试心得
1，I2C没有上拉，导致AFE发送数据波形异常，上不到3.3V
2，波形正常后，发现读回来的数据有问题
3，第一时间应该是观察存储数据的数组的实际值和自带上位机读取的值作对比，而不是通过分析波形，再逻辑分析仪解析波形，这样太慢了。
4，果然还是030小端存储导致的数据相反错误
*/

/*
1，&Registers_AFE1--为结构体的首地址，不过该地址类型为结构体类型(实际上也是结构体第一个字节的地址)
2，(UINT8*)&Registers_AFE1--把该结构体类型的地址转化为UINT8类型的地址(实际上数值没变，也是结构体第一个字节的地址，类型变了而已)
3，(UINT8)&Registers_AFE1--把该地址值(32位数据)强制转化为8位数据，例如0x2000 0A17转为0x17
*/
UINT8 UpdateVoltageFromBqMaximo(void) {
	UINT8 i,result = 0;
	UINT32 u32temp = 0;
	
	if(MTPRead(MTP_TEMP1, sizeof(Registers_AFE1), (UINT8*)&Registers_AFE1)) {		//demo代码返回1为OK，
		for(i = 0; i < SeriesNum; i++) {
			SH367309_Read_AFE1.u16VCell[i] = ((UINT32)U16_SwapEndian(Registers_AFE1.Cell[i])*5>>5);		////Vcell*5/32
		}


		u32temp = ((UINT32)SH367309_Reg_Store.TR_ResRef*U16_SwapEndian(Registers_AFE1.Temp1))/(32769 - U16_SwapEndian(Registers_AFE1.Temp1));
		UPDNLMT16(u32temp, 65535, 0);
		SH367309_Read_AFE1.u16TempBat[0] = GetEndValue(iSheldTemp_10K_AFE, (UINT16)LENGTH_TBLTEMP_AFE_10K, u32temp);
		u32temp = ((UINT32)SH367309_Reg_Store.TR_ResRef*U16_SwapEndian(Registers_AFE1.Temp2))/(32769 - U16_SwapEndian(Registers_AFE1.Temp2));
		UPDNLMT16(u32temp, 65535, 0);
		SH367309_Read_AFE1.u16TempBat[1] = GetEndValue(iSheldTemp_10K_AFE, (UINT16)LENGTH_TBLTEMP_AFE_10K, u32temp);
		u32temp = ((UINT32)SH367309_Reg_Store.TR_ResRef*U16_SwapEndian(Registers_AFE1.Temp3))/(32769 - U16_SwapEndian(Registers_AFE1.Temp3));
		UPDNLMT16(u32temp, 65535, 0);
		SH367309_Read_AFE1.u16TempBat[2] = GetEndValue(iSheldTemp_10K_AFE, (UINT16)LENGTH_TBLTEMP_AFE_10K, u32temp);

		//电流要不要加滤波1s除以4，demo是这样的，现在先观察一下
		//SH367309_Read_AFE1.i16Current = (UINT16)((UINT32)U16_SwapEndian(Registers_AFE1.Cadc)*200/(21470*RSENSE));		//TODO
		SH367309_Read_AFE1.u16Current = U16_SwapEndian(Registers_AFE1.Cadc);
	}
	else {
		result = 1;
	}
	return result;
}

