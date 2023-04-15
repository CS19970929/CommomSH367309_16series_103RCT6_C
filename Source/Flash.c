#include "main.h"

typedef void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

FLASH_Status FlashWriteOneHalfWord(uint32_t StartAddr,uint16_t Buffer) {
	FLASH_Status result;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	while(FLASH_ErasePage(StartAddr) != FLASH_COMPLETE);
	result = FLASH_ProgramHalfWord(StartAddr,Buffer);
	FLASH_Lock();
	return result;
}

/*
	函数功能:	读取指定地址的半字(16位数据)
  	输入参数:	faddr:读地址(此地址必须为2的倍数!!)
	返回值: 	对应数据
*/
UINT16 FlashReadOneHalfWord(UINT32 faddr) {
	return *(vu16*)faddr; 
}


void FlashTest(void) {
	/*
	if(FLASH_COMPLETE != FlashWriteOneHalfWord(FLASH_ADDR_UPDATE_FLAG - 1024, FLASH_TO_APP_VALUE -1)) {
		Flash_Faultcnt++;
	}
	
	g_stCellInfoReport.u16VCell[2] = FlashReadOneHalfWord(FLASH_ADDR_UPDATE_FLAG - 1024);
	*/
	g_stCellInfoReport.u16VCell[2] = FlashReadOneHalfWord(FLASH_ADDR_UPDATE_FLAG);	
}


void App_FlashUpdate(void) {
	#ifdef _IAP
    if(1 == u8FlashUpdateFlag) {
        __delay_ms(10);
		u8FlashUpdateFlag = 0;
		__disable_fault_irq();
        MCU_RESET();			//reset避免了一切中断初始化问题
    }
	#endif
}


void APP_To_IAP_Jump(void) {
	if (((*(__IO uint32_t *)FLASH_ADDR_IAP_START) & 0x2FFE0000) == 0x20000000) {	//判断APP区是否有代码

		JumpAddress = *(__IO uint32_t *)(FLASH_ADDR_IAP_START + 4);				//Jump to user application
		Jump_To_Application = (pFunction)JumpAddress;
		__set_MSP(*(__IO uint32_t *)FLASH_ADDR_IAP_START);							//Initialize user application's Stack Pointer
		Jump_To_Application();													//Jump to application
	}
}

void InitAreaSelect(void) {
	if(FlashReadOneHalfWord(FLASH_ADDR_UPDATE_FLAG) == FLASH_TO_IAP_VALUE) {
		APP_To_IAP_Jump();		//跳回去不能开各种中断或者初始化，也即下面的初始化不能放上来
	}
}

