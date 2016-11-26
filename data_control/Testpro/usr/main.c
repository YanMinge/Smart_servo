/*************************************************************************
* File Name          :main.c
* Author             : Mark Yan
* Updated            : Mark Yan
* Version            : V21.01.001
* Date               : 11/25/2016
* Description        : Firmware for Makeblock smart servo data board.  
* License            : CC-BY-SA 3.0
* Copyright (C) 2013 - 2016 Maker Works Technology Co., Ltd. All right reserved.
* http://www.makeblock.cc/
**************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "systime.h"
#include "sysinit.h"
#include "mygpio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Buffer used for transmission */
uint8_t aTxBuffer[BUFFER_SIZE] = "hello World!\r\n";
uint8_t aRxBuffer[USART_MAX_INDEX][BUFFER_SIZE];


/* Buffer used for reception */
USART_InitTypeDef USART_InitStructure;
__IO uint8_t TxCounter = 0;
extern __IO uint8_t ReceiveState;
USART_TypeDef* UsartInstance[USART_MAX_INDEX] = {USART1, USART2, USART3, USART4, USART5};
uint8_t UsartIndex = 0;
/* Private function prototypes -----------------------------------------------*/
static void RCC_Configuration(void);
static void GPIO_Configuration(void);
static void NVIC_Configuration(void);
bool blink_flag = false;

void led_gpio_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Configure PC10 and PC11 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  uint32_t delaytime;
  /*!< At this stage the microcontroller clock setting is already configured, 
  this is done through SystemInit() function which is called from startup
  file (startup_stm32f0xx.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32f0xx.c file
  */ 
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* error */ 
    while (1);
  }
  /* Configure clock GPIO, USARTs */
  RCC_Configuration();
  
  /* Configure GPIO Pin Tx/Rx for Usart communication */
  GPIO_Configuration();
  
  /* Configure NVIC */
  NVIC_Configuration();

  led_gpio_init();

//	usart2_Init(115200);
  
  /* 8xUSARTs configuration --------------------------------------------------*/
  /* 8xUSARTs  configured as follow:
  - BaudRate = 115200 baud  
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  /* Prepare all uart to receive a data packet */
  for(UsartIndex = 0; UsartIndex < USART_MAX_INDEX; UsartIndex++)
  {
    USART_Init(UsartInstance[UsartIndex], &USART_InitStructure);
    
    /* Enable 8xUSARTs Receive interrupts */
    USART_ITConfig(UsartInstance[UsartIndex], USART_IT_RXNE, ENABLE);
    
    /* Enable the 8xUSARTs */
    USART_Cmd(UsartInstance[UsartIndex], ENABLE);
  }
	
	delaytime = millis();

  /* Infinite Loop */
  while(1)
  {
    /* Set aRxBuffer to 0 */
    memset(aRxBuffer, 0x0, BUFFER_SIZE * USART_MAX_INDEX);
    
    /* When data has been transferred start the next transfer */
    for(UsartIndex = 0; UsartIndex < USART_MAX_INDEX; UsartIndex++)
    { 
       uart_printf(UsartInstance[UsartIndex],"hello world %d!\r\n",millis());
    }

    /* Insert delay */
		if(millis() - delaytime > 500)
    {
      delaytime = millis();
		}
    blink_flag = !blink_flag;
    if(blink_flag == false)
    {
      GPIO_SetBits(GPIOB, GPIO_Pin_2);
    }
    else
    {
      GPIO_ResetBits(GPIOB, GPIO_Pin_2);
    }
  }
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
static void RCC_Configuration(void)
{   
  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB, ENABLE);
  
  /* Enable 8xUSARTs Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_USART3|RCC_APB1Periph_USART4|RCC_APB1Periph_USART5, ENABLE);  
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
static void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* USART1 Pins configuration ************************************************/
  /* Connect pin to Periph */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
  
  /* Configure pins as AF pushpull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //GPIO_PuPd_NOPULL
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

  /* USART2 Pins configuration ************************************************/  
  /* Connect pin to Periph */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
  
  /* Configure pins as AF pushpull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* USART3 Pins configuration  ***********************************************/  
  /* Connect pin to Periph */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_4);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_4);
  
  /* Configure pins as AF pushpull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
  
  /* USART4 Pins configuration  ***********************************************/  
  /* Connect pin to Periph */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_4);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_4);
 
  /* Configure pins as AF pushpull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 

  /* USART5 Pins configuration*************************************************/  
  /* Connect pin to Periph */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_4);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_4);
  
  /* Configure pins as AF pushpull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* USART1 IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* USART2 IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* USART3_8 IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
