#ifndef PUBFUNC_H
#define PUBFUNC_H


typedef enum {ODD = 0, EVEN = !ODD} Parity;

typedef	struct{
	UINT16	u16ChkVal;                      // 当前比较量大小
	UINT16	u16OPValB;                      // 比较大值
	UINT16	u16OPValS;                      // 比较小值
    UINT16   *i16ChkCnt;                     // 指向计时器地址
    UINT16  u16TimeCntB;                    // 超过大值时间
	UINT16  u16TimeCntS;                    // 超过小值时间
    UINT8	u8FlagLogic;                    // 正逻辑 1：u8Flag=s_u16ChkVal > s_u16OPValB ? 1 : 0 负逻辑 1：u8Flag=s_u16ChkVal > s_u16OPValB ? 0 : 1
	UINT8	u8FlagBit;                      // 判断结果
}SPUBOPUPCHK;


UINT16 Sci_CRC16RTU( UINT8 * pszBuf, UINT8 unLength);
UINT8 App_PubOPUPChk(SPUBOPUPCHK *t_sPubOPChk);
UINT16 GetEndValue(const UINT16 * ptbl,UINT16 tblsize,UINT16 dat);
unsigned char CRC8(unsigned char *ptr, unsigned char len,unsigned char key);
UINT32 ModulusSub(UINT32 Data1, UINT32 Data2);
void Delay_Base10us(int n);

void Delay1ms(UINT8 delaycnt);
void MemoryCopy(UINT8 *source, UINT8 *target, UINT8 length);
UINT16 U16_SwapEndian(UINT16 target);
UINT8 Monitor_TempBreak(UINT16* temp_AD);

#endif	/* PUBFUNC_H */

