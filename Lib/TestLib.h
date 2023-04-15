#ifndef TESTLIB_H
#define TESTLIB_H

#include "stm32f10x.h"

//事实证明，define编译后，新project调用修改define的东西无效果
//#define GPIOx GPIOB
//#define GPIO_Pin_x GPIO_Pin_4

extern int I2CWriteBlockWithCRC(unsigned char I2CSlaveAddress, unsigned char StartAddress, unsigned char *Buffer, unsigned char Length);

UINT16 GetEndValue2(const UINT16 * ptbl,UINT16 tblsize,UINT16 dat);
void GPIOtest(GPIO_TypeDef * GPIOx, UINT16 GPIO_Pin_x);




#endif	/* TESTLIB_H */

