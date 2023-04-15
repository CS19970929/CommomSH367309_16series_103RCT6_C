#include "main.h"

volatile union SYS_TIME g_st_SysTimeFlag;
struct CBC_ELEMENT CBC_Element;

UINT8 g_u81msCnt = 0;
UINT8 g_u810msClockCnt = 0;
UINT8 g_u81msClockCnt = 0;

UINT8 gu8_200msCnt = 0;
UINT8 gu8_200msAccClock_Flag = 0;

static UINT8 fac_us = 0; // us延时倍乘数
static UINT16 fac_ms = 0;

// 一般不需要另立函数的IO口处理
void InitIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // 使能IO复用功能模块时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // 使能GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // 使能GPIOC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); // 使能GPIOD时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); // 使能GPIOE时钟

	// PA6_MCUO_AFE_SHIP，PA7_PWSV_LDO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // IO口速度为2MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// PB0_DRV_WLM_PW，PB15_LED1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // IO口速度为2MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// PC8_MCUO_AFE_MODE，PC13_PWSV_CTR，PC10_PW_SPS_STB，PC7_MCUO_AFE_VPRO，PC11_MCUO_AFE_VPRO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_13 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // IO口速度为2MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// PC4_CTLC
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; // IO口速度为2MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// PA0_INT_WK_MCU
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// PB5_DI1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

// 使用LSI
void Init_IWDG(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); // 使能PWR外设时钟，待机模式，RTC，看门狗
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);		// 打开独立看门狗寄存器操作权限
	IWDG_SetPrescaler(IWDG_Prescaler_64);				// 预分频系数
	IWDG_SetReload(160);								// 设置重载计数值，k = Xms / (1 / (40KHz/64)) = X/64*40; 4096最高
						 // 800——1.28s，80——128ms
	IWDG_ReloadCounter();										// 喂狗
	IWDG_Enable();												// 使能IWDG
	DBGMCU->CR |= ((uint32_t)0x00000100); /* Debug IWDG Stop */ // STlink使用
	DBGMCU->CR |= ((uint32_t)0x00000200);						/* Debug WWDG Stop */
}

// 关于NVIC_PriorityGroupConfig这个函数
// https://blog.csdn.net/zhuminzeng/article/details/8880138
// 第0组：所有4位用于指定响应优先级
// 第1组：最高1位用于指定抢占式优先级，最低3位用于指定响应优先级
void InitNVIC(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 中断嵌套设计
}

// 非PCLK1(最大36MHz)，所以为72MHz
void InitTimer(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // 时钟3使能

	// 定时器TIM3初始化
	if (0 == System_ErrFlag.u8ErrFlag_HSE)
	{
		TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1; // 设置用来作为TIMx时钟频率除数的预分频值——计数分频
	}
	else
	{
		TIM_TimeBaseStructure.TIM_Prescaler = 8 - 1; // 设置用来作为TIMx时钟频率除数的预分频值——计数分频
	}
	TIM_TimeBaseStructure.TIM_Period = 499;						// 设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// 设置时钟分割:TDTS = Tck_tim——时钟分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
																// 我看了，向下计数是从自动装载值递减至0，向上计数是从0增加至装载值，也就是说在中断时间上没什么区别
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // 根据指定的参数初始化TIMx的时间基数单位
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);		// 清除更新中断请求位，据说这个能防止打开中断瞬间立刻进入中断函数
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);		// 使能指定的TIM3中断,允许更新中断

	/*	TIM3 中断嵌套设计*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // 从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM3, ENABLE); // 使能TIMx
}

/*
HCLK即SYSCLK分频得来（在此未分频），即HCLK==SystemCoreClock
SysTick时钟是HCLK8分频，即SysTick时钟频率==HCLK/8==SystemCoreClock/8
*/
void InitDelay(void)
{
	SysTick->CTRL &= ~(1 << 2);			// 1=内核时钟(FCLK)       0=外部时钟源(STCLK)
	fac_us = SystemCoreClock / 8000000; // SysTick时钟是SYSCLK 8分频，即SysTick时钟频率=SYSCLK/8，1us要计的个数为还得/1MHz
	fac_ms = (INT16)fac_us * 1000;		// 每个ms需要的systick时钟数
}

void __delay_us(UINT32 nus)
{
	UINT32 temp;
	SysTick->LOAD = nus * fac_us;			  // 时间加载
	SysTick->VAL = 0x00;					  // 清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // 开始倒数
	do
	{
		temp = SysTick->CTRL;
	} while ((temp & 0x01) && !(temp & (1 << 16))); // 等待时间到达
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;		// 关闭计数器
	SysTick->VAL = 0X00;							// 清空计数器
}

// 注意nms的范围
// SysTick->LOAD为24位寄存器,所以,最大延时为:
// nms<=2^24*8*1000/SYSCLK = 2^24/fac_ms
// SYSCLK单位为Hz,nms单位为ms
// 对72M条件下,nms<=1864
// 48M为nms<=2^24/6000 = 2796
// 意思就是2^24次计数对应时间长度(基于HCLK/8   )
void __delay_ms(UINT16 nms)
{
	UINT32 temp;
	SysTick->LOAD = (UINT32)nms * fac_ms;	  // 时间加载(SysTick->LOAD为24bit)
	SysTick->VAL = 0x00;					  // 清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // 开始倒数
	do
	{
		temp = SysTick->CTRL;
	} while ((temp & 0x01) && !(temp & (1 << 16))); // 等待时间到达
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;		// 关闭计数器
	SysTick->VAL = 0X00;							// 清空计数器
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
	{ // 1ms定时标志
		s_u8Cnt1ms = g_u81msClockCnt;
		g_st_SysTimeFlag.bits.b1Sys1msFlag = 1;
	}

	g_st_SysTimeFlag.bits.b1Sys10msFlag1 = 0;
	g_st_SysTimeFlag.bits.b1Sys10msFlag2 = 0;
	g_st_SysTimeFlag.bits.b1Sys10msFlag3 = 0;
	g_st_SysTimeFlag.bits.b1Sys10msFlag4 = 0;
	g_st_SysTimeFlag.bits.b1Sys10msFlag5 = 0;
	if (s_u8Cnt10ms != g_u810msClockCnt)
	{ // 10ms定时标志
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
			s_u8Cnt200ms1++; // 本想用一个变量搞一个循环然后置位，发现有BUG，不行
			s_u8Cnt200ms2++; // 会持续进来一个10ms，必须改变标志位让其不再进来
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
		g_st_SysTimeFlag.bits.b1Sys50msFlag = 1; // 50ms定时标志
	}

	g_st_SysTimeFlag.bits.b1Sys100msFlag = 0;
	if (s_u8Cnt100ms >= 10)
	{
		s_u8Cnt100ms = 0;
		g_st_SysTimeFlag.bits.b1Sys100msFlag = 1; // 100ms定时标志
	}

	g_st_SysTimeFlag.bits.b1Sys200msFlag1 = 0;
	g_st_SysTimeFlag.bits.b1Sys200msFlag2 = 0;
	g_st_SysTimeFlag.bits.b1Sys200msFlag3 = 0;
	g_st_SysTimeFlag.bits.b1Sys200msFlag4 = 0;
	g_st_SysTimeFlag.bits.b1Sys200msFlag5 = 0;
	if (s_u8Cnt200ms1 >= 20)
	{
		s_u8Cnt200ms1 = 0;
		g_st_SysTimeFlag.bits.b1Sys200msFlag1 = 1; // 200ms定时标志
	}
	if (s_u8Cnt200ms2 >= 20)
	{
		s_u8Cnt200ms2 = 0;
		g_st_SysTimeFlag.bits.b1Sys200msFlag2 = 1; // 200ms定时标志
	}
	if (s_u8Cnt200ms3 >= 20)
	{
		s_u8Cnt200ms3 = 0;
		g_st_SysTimeFlag.bits.b1Sys200msFlag3 = 1; // 200ms定时标志
	}
	if (s_u8Cnt200ms4 >= 20)
	{
		s_u8Cnt200ms4 = 0;
		g_st_SysTimeFlag.bits.b1Sys200msFlag4 = 1; // 200ms定时标志
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
		g_st_SysTimeFlag.bits.b1Sys200msFlag5 = 1; // 200ms定时标志
	}

	g_st_SysTimeFlag.bits.b1Sys1000msFlag1 = 0;
	g_st_SysTimeFlag.bits.b1Sys1000msFlag2 = 0;
	g_st_SysTimeFlag.bits.b1Sys1000msFlag3 = 0;
	if (s_u8Cnt1000ms1 >= 100)
	{
		s_u8Cnt1000ms1 = 0;
		g_st_SysTimeFlag.bits.b1Sys1000msFlag1 = 1; // 1000ms定时标志
	}
	if (s_u8Cnt1000ms2 >= 100)
	{
		s_u8Cnt1000ms2 = 0;
		g_st_SysTimeFlag.bits.b1Sys1000msFlag2 = 1; // 1000ms定时标志
	}
	if (s_u8Cnt1000ms3 >= 100)
	{
		s_u8Cnt1000ms3 = 0;
		g_st_SysTimeFlag.bits.b1Sys1000msFlag3 = 1; // 1000ms定时标志
		// MCUO_DEBUG_LED2 = !MCUO_DEBUG_LED2;
	}
}

void IWDG_Feed(void)
{
	IWDG_ReloadCounter();
}

// 定时器3中断服务程序
void TIM3_IRQHandler(void)
{ // TIM3中断
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{												// 检查TIM3更新中断发生与否
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除TIMx更新中断标志
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
