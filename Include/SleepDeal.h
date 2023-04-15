#ifndef SLEEPDEAL_H
#define SLEEPDEAL_H

#include "SleepFunction.h"



extern UINT8 RTC_ExtComCnt;

void InitData_SleepFunc(void);
void EnterSleepJudge(void);
void App_SleepDeal(void);

#endif	/* SLEEPDEAL_H */

