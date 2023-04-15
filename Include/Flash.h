#ifndef FLASH_H
#define FLASH_H

//系统存储器：出厂由ST在这个区域内部预置了一段BootLoader，也就是ISP程序，这是一块ROM
//小容量产品主存储块1-32KB，	 每页1KB。系统存储器2KB。
//中容量产品主存储块64-128KB，	 每页1KB。系统存储器2KB。
//大容量产品主存储块256KB以上，  每页2KB。系统存储器2KB。
//互联型产品主存储块256KB以上，  每页2KB。系统存储器18KB。
#define FLASH_ADDR_IAP_START 			0x08000000
#define FLASH_ADDR_APP_START 			0x08004800			//256-18-2-2=234K，这个地方出了一次问题，修改后大于6K，侧面反映编译出来的Zi-data也是flash的东西

#define FLASH_ADDR_SH367309_VALUE 		0x0803E000			//休眠带电关键参数(电流偏置值)，1K
#define FLASH_ADDR_SH367309_FLAG 		0x0803E800			//休眠是否带电标志位，1K
#define FLASH_ADDR_UPDATE_FLAG 			0x0803F000			//IAP=18K
#define FLASH_ADDR_SLEEP_FLAG           0x0803F800			//休眠关键指令，2K

#define FLASH_309_RTC_RTC_VALUE			((UINT16)0x1222)	//RTC休眠，RTC唤醒，只有RTC休眠，才有可能RTC唤醒。
#define FLASH_309_RTC_NORMAL_VALUE		((UINT16)0x2333)	//RTC休眠，别的形式唤醒(通讯，按钮等等)
#define FLASH_309_NORMAL_NORMAL_VALUE	((UINT16)0xFFFF)	//普通休眠，别的形式唤醒(通讯，按钮等等)

#define FLASH_TO_IAP_VALUE				((UINT16)0x00AB)
#define FLASH_TO_APP_VALUE				((UINT16)0xFFFF)

#define FLASH_NORMAL_SLEEP_VALUE    	((UINT16)0x1234)
#define FLASH_DEEP_SLEEP_VALUE    		((UINT16)0x1235)
#define FLASH_HICCUP_SLEEP_VALUE    	((UINT16)0x1236)
#define FLASH_SLEEP_RESET_VALUE    		((UINT16)0xFFFF)


FLASH_Status FlashWriteOneHalfWord(uint32_t StartAddr,uint16_t Buffer);
UINT16 FlashReadOneHalfWord(UINT32 faddr);
void FlashTest(void);

void App_FlashUpdate(void);
void APP_To_IAP_Jump(void);

#endif	/* FLASH_H */

