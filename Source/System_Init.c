#include "main.h"

volatile union SYS_TIME g_st_SysTimeFlag;
struct CBC_ELEMENT CBC_Element;

UINT8 g_u81msCnt = 0;
UINT8 g_u810msClockCnt = 0;
UINT8 g_u81msClockCnt = 0;

UINT8 gu8_200msCnt = 0;
UINT8 gu8_200msAccClock_Flag = 0;

static UINT8 fac_us = 0; // us��ʱ������
static UINT16 fac_ms = 0;

// һ�㲻��Ҫ����������IO�ڴ���
void InitIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // ʹ��IO���ù���ģ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // ʹ��GPIOBʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // ʹ��GPIOCʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); // ʹ��GPIODʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); // ʹ��GPIOEʱ��

	// PA6_MCUO_AFE_SHIP��PA7_PWSV_LDO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // IO���ٶ�Ϊ2MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// PB0_DRV_WLM_PW��PB15_LED1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // IO���ٶ�Ϊ2MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// PC8_MCUO_AFE_MODE��PC13_PWSV_CTR��PC10_PW_SPS_STB��PC7_MCUO_AFE_VPRO��PC11_MCUO_AFE_VPRO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_13 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // IO���ٶ�Ϊ2MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// PC4_CTLC
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // IO���ٶ�Ϊ2MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// PA0_INT_WK_MCU
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // ��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// PB5_DI1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

// ʹ��LSI
void Init_IWDG(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); // ʹ��PWR����ʱ�ӣ�����ģʽ��RTC�����Ź�
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);		// �򿪶������Ź��Ĵ�������Ȩ��
	IWDG_SetPrescaler(IWDG_Prescaler_64);				// Ԥ��Ƶϵ��
	IWDG_SetReload(160);								// �������ؼ���ֵ��k = Xms / (1 / (40KHz/64)) = X/64*40; 4096���
						 // 800����1.28s��80����128ms
	IWDG_ReloadCounter();										// ι��
	IWDG_Enable();												// ʹ��IWDG
	DBGMCU->CR |= ((uint32_t)0x00000100); /* Debug IWDG Stop */ // STlinkʹ��
	DBGMCU->CR |= ((uint32_t)0x00000200);						/* Debug WWDG Stop */
}

// ����NVIC_PriorityGroupConfig�������
// https://blog.csdn.net/zhuminzeng/article/details/8880138
// ��0�飺����4λ����ָ����Ӧ���ȼ�
// ��1�飺���1λ����ָ����ռʽ���ȼ������3λ����ָ����Ӧ���ȼ�
void InitNVIC(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // �ж�Ƕ�����
}

// ��PCLK1(���36MHz)������Ϊ72MHz
void InitTimer(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // ʱ��3ʹ��

	// ��ʱ��TIM3��ʼ��
	if (0 == System_ErrFlag.u8ErrFlag_HSE)
	{
		TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; // ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ����������Ƶ
	}
	else
	{
		TIM_TimeBaseStructure.TIM_Prescaler = 8 - 1; // ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ����������Ƶ
	}
	TIM_TimeBaseStructure.TIM_Period = 499;						// ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// ����ʱ�ӷָ�:TDTS = Tck_tim����ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ
																// �ҿ��ˣ����¼����Ǵ��Զ�װ��ֵ�ݼ���0�����ϼ����Ǵ�0������װ��ֵ��Ҳ����˵���ж�ʱ����ûʲô����
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // ����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);		// ��������ж�����λ����˵����ܷ�ֹ���ж�˲�����̽����жϺ���
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);		// ʹ��ָ����TIM3�ж�,��������ж�

	/*	TIM3 �ж�Ƕ�����*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // ��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // �����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM3, ENABLE); // ʹ��TIMx
}

/*
HCLK��SYSCLK��Ƶ�������ڴ�δ��Ƶ������HCLK==SystemCoreClock
SysTickʱ����HCLK8��Ƶ����SysTickʱ��Ƶ��==HCLK/8==SystemCoreClock/8
*/
void InitDelay(void)
{
	SysTick->CTRL &= ~(1 << 2);			// 1=�ں�ʱ��(FCLK)       0=�ⲿʱ��Դ(STCLK)
	fac_us = SystemCoreClock / 8000000; // SysTickʱ����SYSCLK 8��Ƶ����SysTickʱ��Ƶ��=SYSCLK/8��1usҪ�Ƶĸ���Ϊ����/1MHz
	fac_ms = (INT16)fac_us * 1000;		// ÿ��ms��Ҫ��systickʱ����
}

void __delay_us(UINT32 nus)
{
	UINT32 temp;
	SysTick->LOAD = nus * fac_us;			  // ʱ�����
	SysTick->VAL = 0x00;					  // ��ռ�����
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // ��ʼ����
	do
	{
		temp = SysTick->CTRL;
	} while ((temp & 0x01) && !(temp & (1 << 16))); // �ȴ�ʱ�䵽��
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;		// �رռ�����
	SysTick->VAL = 0X00;							// ��ռ�����
}

// ע��nms�ķ�Χ
// SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
// nms<=2^24*8*1000/SYSCLK = 2^24/fac_ms
// SYSCLK��λΪHz,nms��λΪms
// ��72M������,nms<=1864
// 48MΪnms<=2^24/6000 = 2796
// ��˼����2^24�μ�����Ӧʱ�䳤��(����HCLK/8   )
void __delay_ms(UINT16 nms)
{
	UINT32 temp;
	SysTick->LOAD = (UINT32)nms * fac_ms;	  // ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL = 0x00;					  // ��ռ�����
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // ��ʼ����
	do
	{
		temp = SysTick->CTRL;
	} while ((temp & 0x01) && !(temp & (1 << 16))); // �ȴ�ʱ�䵽��
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;		// �رռ�����
	SysTick->VAL = 0X00;							// ��ռ�����
}

void App_SysTime(void)
{
	static UINT8 s_u8Cnt1ms = 0;

	static UINT8 s_u8Cnt10ms = 0;
	// static UINT8 s_u8Cnt20ms = 0;
	static UINT8 s_u8Cnt50ms = 0;
	static UINT8 s_u8Cnt100ms = 0;

	static UINT8 s_u8Cnt200ms1 = 0;
	static UINT8 s_u8Cnt200ms2 = 4;
	static UINT8 s_u8Cnt200ms3 = 8;
	static UINT8 s_u8Cnt200ms4 = 12;
	static UINT8 s_u8Cnt200ms5 = 16;

	static UINT8 s_u8Cnt1000ms1 = 0;
	static UINT8 s_u8Cnt1000ms2 = 33;
	static UINT8 s_u8Cnt1000ms3 = 66;

	g_st_SysTimeFlag.bits.b1Sys1msFlag = 0;
	if (s_u8Cnt1ms != g_u81msClockCnt)
	{ // 1ms��ʱ��־
		s_u8Cnt1ms = g_u81msClockCnt;
		g_st_SysTimeFlag.bits.b1Sys1msFlag = 1;
	}

	g_st_SysTimeFlag.bits.b1Sys10msFlag1 = 0;
	g_st_SysTimeFlag.bits.b1Sys10msFlag2 = 0;
	g_st_SysTimeFlag.bits.b1Sys10msFlag3 = 0;
	g_st_SysTimeFlag.bits.b1Sys10msFlag4 = 0;
	g_st_SysTimeFlag.bits.b1Sys10msFlag5 = 0;
	if (s_u8Cnt10ms != g_u810msClockCnt)
	{ // 10ms��ʱ��־
		s_u8Cnt10ms = g_u810msClockCnt;
		switch (g_u810msClockCnt)
		{
		case 0:
			g_st_SysTimeFlag.bits.b1Sys10msFlag1 = 1;
			// MCUO_DEBUG_LED2 = !MCUO_DEBUG_LED2;
			break;

		case 1:
			s_u8Cnt50ms++;
			g_st_SysTimeFlag.bits.b1Sys10msFlag2 = 1;
			break;

		case 2:
			s_u8Cnt100ms++;
			g_st_SysTimeFlag.bits.b1Sys10msFlag3 = 1;
			break;

		case 3:
			s_u8Cnt200ms1++; // ������һ��������һ��ѭ��Ȼ����λ��������BUG������
			s_u8Cnt200ms2++; // ���������һ��10ms������ı��־λ���䲻�ٽ���
			s_u8Cnt200ms3++;
			s_u8Cnt200ms4++;
			s_u8Cnt200ms5++;
			g_st_SysTimeFlag.bits.b1Sys10msFlag4 = 1;
			break;

		case 4:
			s_u8Cnt1000ms1++;
			s_u8Cnt1000ms2++;
			s_u8Cnt1000ms3++;
			g_st_SysTimeFlag.bits.b1Sys10msFlag5 = 1;
			break;

		default:
			break;
		}
	}

	g_st_SysTimeFlag.bits.b1Sys50msFlag = 0;
	if (s_u8Cnt50ms >= 5)
	{
		s_u8Cnt50ms = 0;
		g_st_SysTimeFlag.bits.b1Sys50msFlag = 1; // 50ms��ʱ��־
	}

	g_st_SysTimeFlag.bits.b1Sys100msFlag = 0;
	if (s_u8Cnt100ms >= 10)
	{
		s_u8Cnt100ms = 0;
		g_st_SysTimeFlag.bits.b1Sys100msFlag = 1; // 100ms��ʱ��־
	}

	g_st_SysTimeFlag.bits.b1Sys200msFlag1 = 0;
	g_st_SysTimeFlag.bits.b1Sys200msFlag2 = 0;
	g_st_SysTimeFlag.bits.b1Sys200msFlag3 = 0;
	g_st_SysTimeFlag.bits.b1Sys200msFlag4 = 0;
	g_st_SysTimeFlag.bits.b1Sys200msFlag5 = 0;
	if (s_u8Cnt200ms1 >= 20)
	{
		s_u8Cnt200ms1 = 0;
		g_st_SysTimeFlag.bits.b1Sys200msFlag1 = 1; // 200ms��ʱ��־
	}
	if (s_u8Cnt200ms2 >= 20)
	{
		s_u8Cnt200ms2 = 0;
		g_st_SysTimeFlag.bits.b1Sys200msFlag2 = 1; // 200ms��ʱ��־
	}
	if (s_u8Cnt200ms3 >= 20)
	{
		s_u8Cnt200ms3 = 0;
		g_st_SysTimeFlag.bits.b1Sys200msFlag3 = 1; // 200ms��ʱ��־
	}
	if (s_u8Cnt200ms4 >= 20)
	{
		s_u8Cnt200ms4 = 0;
		g_st_SysTimeFlag.bits.b1Sys200msFlag4 = 1; // 200ms��ʱ��־
#if 0
		if(!MCUI_SLEEP_DET) {
			MCUO_DEBUG_LED1 = !MCUO_DEBUG_LED1;
		}
#endif
		MCUO_DEBUG_LED1 = !MCUO_DEBUG_LED1;
	}
	if (s_u8Cnt200ms5 >= 20)
	{
		s_u8Cnt200ms5 = 0;
		g_st_SysTimeFlag.bits.b1Sys200msFlag5 = 1; // 200ms��ʱ��־
	}

	g_st_SysTimeFlag.bits.b1Sys1000msFlag1 = 0;
	g_st_SysTimeFlag.bits.b1Sys1000msFlag2 = 0;
	g_st_SysTimeFlag.bits.b1Sys1000msFlag3 = 0;
	if (s_u8Cnt1000ms1 >= 100)
	{
		s_u8Cnt1000ms1 = 0;
		g_st_SysTimeFlag.bits.b1Sys1000msFlag1 = 1; // 1000ms��ʱ��־
	}
	if (s_u8Cnt1000ms2 >= 100)
	{
		s_u8Cnt1000ms2 = 0;
		g_st_SysTimeFlag.bits.b1Sys1000msFlag2 = 1; // 1000ms��ʱ��־
	}
	if (s_u8Cnt1000ms3 >= 100)
	{
		s_u8Cnt1000ms3 = 0;
		g_st_SysTimeFlag.bits.b1Sys1000msFlag3 = 1; // 1000ms��ʱ��־
		// MCUO_DEBUG_LED2 = !MCUO_DEBUG_LED2;
	}
}

void IWDG_Feed(void)
{
	IWDG_ReloadCounter();
}

// ��ʱ��3�жϷ������
void TIM3_IRQHandler(void)
{ // TIM3�ж�
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{												// ���TIM3�����жϷ������
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // ���TIMx�����жϱ�־
		if ((++g_u81msCnt) >= 2)
		{ // 1ms
			g_u81msCnt = 0;
			g_u81msClockCnt++;
			gu8_200msCnt++;

			if (g_u81msClockCnt >= 2)
			{ // 2ms
				g_u81msClockCnt = 0;
				g_u810msClockCnt++;
				if (g_u810msClockCnt >= 5)
				{ // 10ms
					g_u810msClockCnt = 0;
				}
			}

			if (gu8_200msCnt >= 200)
			{
				gu8_200msCnt = 0;
				gu8_200msAccClock_Flag = 1;
			}
		}
	}
}
