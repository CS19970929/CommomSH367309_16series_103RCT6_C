/**
  ******************************************************************************
  * @file    EXTI/EXTI_Config/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "main.h"			//在it的头文件可能会导致别的地方也能调用main的东西，不符合安全规范

//#include "stm32_eval.h"

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup EXTI_Config
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}


__asm void wait()
{
      BX lr
}


/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  	wait();
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval Nonehttps://www.cingta.com/static/image/20190816/bb94f7d7c1f94c4daf38e66b8aaa80cd.png
  */
void EXTI0_IRQHandler(void) {
	if(EXTI_GetITStatus(EXTI_Line0) != RESET) {
		EXTI_ClearITPendingBit(EXTI_Line0);
		ChargerLoad_Func.bits.b1ON_Charger_AllSeries = 1;
	}
}


void EXTI15_10_IRQHandler(void) {
	if(EXTI_GetITStatus(EXTI_Line12) != RESET) {		
		EXTI_ClearITPendingBit(EXTI_Line12);
	}
}


/**
  * @brief  This function handles External lines 9 to 5 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void) {
	if(EXTI_GetITStatus(EXTI_Line6) != RESET) {
		EXTI_ClearITPendingBit(EXTI_Line6);
	}

	if(EXTI_GetITStatus(EXTI_Line5) != RESET) {
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
	
	if(EXTI_GetITStatus(EXTI_Line7) != RESET) {
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
	if(EXTI_GetITStatus(EXTI_Line5) != RESET) {
		EXTI_ClearITPendingBit(EXTI_Line5);
	}

	if(EXTI_GetITStatus(EXTI_Line9) != RESET) {
		EXTI_ClearITPendingBit(EXTI_Line9);
	}	
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */

//以下的是非共有的，如果多处使用到，则移动到it.c文件
void USART1_IRQHandler(void) {
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		RTC_ExtComCnt++;
 		
		#ifdef _COMMOM_UPPER_SCI1
		Sci1_CommonUpper_FaultChk();
		Sci1_CommonUpper_Rx_Deal(&g_stCurrentMsgPtr_SCI1);
		#endif
		
		#ifdef _CLIENT_SCI1
		UartClient_FaultChk_SCI1();
		UartClient_Rx_Deal_SCI1(&g_UartClientMsgPtr_SCI1);
		//RTC_ExtComCnt++;
		#endif
		
		#ifdef _LCD_SCI1
		LCD_FaultChk_SCI1();
		LCD_Rx_Deal_SCI1(&g_stCurrentMsgPtrLCD_SCI1);
		#endif
	}
}


void USART2_IRQHandler(void) {
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		RTC_ExtComCnt++;
		
		#ifdef _COMMOM_UPPER_SCI2
		Sci2_CommonUpper_FaultChk();
		Sci2_CommonUpper_Rx_Deal(&g_stCurrentMsgPtr_SCI2);
		#endif
		
		#ifdef _CLIENT_SCI2
		UartClient_FaultChk_SCI2();
		UartClient_Rx_Deal_SCI2(&g_UartClientMsgPtr_SCI2);
		#endif
		
		#ifdef _LCD_SCI2
		LCD_FaultChk_SCI2();
		LCD_Rx_Deal_SCI2(&g_stCurrentMsgPtrLCD_SCI2);
		#endif
	}
}


void USART3_IRQHandler(void) {
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
		RTC_ExtComCnt++;

		#ifdef _COMMOM_UPPER_SCI3
		Sci3_CommonUpper_FaultChk();
		Sci3_CommonUpper_Rx_Deal(&g_stCurrentMsgPtr_SCI3);
		#endif
		
		#ifdef _CLIENT_SCI3
		UartClient_FaultChk_SCI3();
		UartClient_Rx_Deal_SCI3(&g_UartClientMsgPtr_SCI3);
		#endif
		
		#ifdef _LCD_SCI3
		LCD_FaultChk_SCI3();
		LCD_Rx_Deal_SCI3(&g_stCurrentMsgPtrLCD_SCI3);
		#endif
	}
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
