#ifndef SOC_H
#define SOC_H

#include "SocEnhance.h"

#define SOC_TABLE_SIZE 		42
#define SOC_VOL_MIN   		((UINT16)0)
#define SOC_VOL_MAX   		((UINT16)5000)		//mV
#define SOC_VALUE_MIN   	((UINT16)0)
#define SOC_VALUE_MAX   	((UINT16)100)		//%


extern UINT16 SOC_Table_Set[SOC_TABLE_SIZE];
extern const UINT16 SOC_Table_Default[42];

void InitData_SOC(void);
void App_SOC(void);

#endif	/* SOC_H */

