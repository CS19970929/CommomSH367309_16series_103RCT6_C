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
	��������:	��ȡָ����ַ�İ���(16λ����)
  	�������:	faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
	����ֵ: 	��Ӧ����
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
        MCU_RESET();			//reset������һ���жϳ�ʼ������
    }
	#endif
}


void APP_To_IAP_Jump(void) {
	if (((*(__IO uint32_t *)FLASH_ADDR_IAP_START) & 0x2FFE0000) == 0x20000000) {	//�ж�APP���Ƿ��д���

		JumpAddress = *(__IO uint32_t *)(FLASH_ADDR_IAP_START + 4);				//Jump to user application
		Jump_To_Application = (pFunction)JumpAddress;
		__set_MSP(*(__IO uint32_t *)FLASH_ADDR_IAP_START);							//Initialize user application's Stack Pointer
		Jump_To_Application();													//Jump to application
	}
}

void InitAreaSelect(void) {
	if(FlashReadOneHalfWord(FLASH_ADDR_UPDATE_FLAG) == FLASH_TO_IAP_VALUE) {
		APP_To_IAP_Jump();		//����ȥ���ܿ������жϻ��߳�ʼ����Ҳ������ĳ�ʼ�����ܷ�����
	}
}

