/**
  ******************************************************************************
  * @file    USART/USART_8xUsartsOneBoard/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    24-July-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "uart_printf.h"


/** @addtogroup STM32F0xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_8xUsartsOneBoard
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint8_t RxCounter= 0, ReceiveState = 0;
volatile unsigned long system_time = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
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
  * @brief  This function handles PendSVC exception.
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
  system_time++;
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
  volatile uint8_t inputData = 0xFF;
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    inputData = USART_ReceiveData(USART1);

    if(Uart1RevBytes < BUFFER_SIZE)
    {
      /* Enqueue the character */
      Uart1RecData[Uart1RevRtail] = inputData;
      Uart1RevRtail = (Uart1RevRtail == (uint16_t)(BUFFER_SIZE - 1)) ? 0 : (Uart1RevRtail + 1);
      Uart1RevBytes++;
    }
    else
    { 
      Uart1RevBytes = 0;
      Uart1RevRtail = 0;
      Uart1RevHead = 0;
    }
  }
}

/**
  * @brief  This function handles USART2 global interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
  volatile uint8_t inputData = 0xFF;
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    inputData = USART_ReceiveData(USART2);

    if(Uart2RevBytes < BUFFER_SIZE)
    {
      /* Enqueue the character */
      Uart2RecData[Uart2RevRtail] = inputData;
      Uart2RevRtail = (Uart2RevRtail == (uint16_t)(BUFFER_SIZE - 1)) ? 0 : (Uart2RevRtail + 1);
      Uart2RevBytes++;
    }
    else
    { 
      Uart2RevBytes = 0;
      Uart2RevRtail = 0;
      Uart2RevHead = 0;
    }
  }
}

/**
  * @brief  This function handles USART3-6 global interrupt request.
  * @param  None
  * @retval None
  */
void USART3_6_IRQHandler(void)
{
  volatile uint8_t inputData = 0xFF;
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    inputData = USART_ReceiveData(USART3);

    if(Uart3RevBytes < BUFFER_SIZE)
    {
      /* Enqueue the character */
      Uart3RecData[Uart3RevRtail] = inputData;
      Uart3RevRtail = (Uart3RevRtail == (uint16_t)(BUFFER_SIZE - 1)) ? 0 : (Uart3RevRtail + 1);
      Uart3RevBytes++;
    }
    else
    { 
      Uart3RevBytes = 0;
      Uart3RevRtail = 0;
      Uart3RevHead = 0;
    }
  }

  if(USART_GetITStatus(USART4, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    inputData = USART_ReceiveData(USART4);

    if(Uart4RevBytes < BUFFER_SIZE)
    {
      /* Enqueue the character */
      Uart4RecData[Uart4RevRtail] = inputData;
      Uart4RevRtail = (Uart4RevRtail == (uint16_t)(BUFFER_SIZE - 1)) ? 0 : (Uart4RevRtail + 1);
      Uart4RevBytes++;
    }
    else
    { 
      Uart4RevBytes = 0;
      Uart4RevRtail = 0;
      Uart4RevHead = 0;
    }
  }

  if(USART_GetITStatus(USART5, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    inputData = USART_ReceiveData(USART5);

    if(Uart5RevBytes < BUFFER_SIZE)
    {
      /* Enqueue the character */
      Uart5RecData[Uart5RevRtail] = inputData;
      Uart5RevRtail = (Uart5RevRtail == (uint16_t)(BUFFER_SIZE - 1)) ? 0 : (Uart5RevRtail + 1);
      Uart5RevBytes++;
    }
    else
    { 
      Uart5RevBytes = 0;
      Uart5RevRtail = 0;
      Uart5RevHead = 0;
    }
  }
}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
