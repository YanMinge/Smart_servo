#include "Interrupt.h"
#include "protocol.h"
#include "systime.h"
#include <string.h>

volatile I2C_FUNC s_I2C0HandlerFn = NULL;
	
static void Uart0_Handle(void);
static void Uart1_Handle(void);

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt handler entry                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*  TMR0 IRQ                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief       Timer0 IRQ Handler
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Timer0 default IRQ, declared in startup_M051Series.s.
 */

void TMR0_IRQHandler(void)
{
  if(TIMER_GetIntFlag(TIMER0) == 1)
  {
    /* Clear Timer0 time-out interrupt flag */
    TIMER_ClearIntFlag(TIMER0);
    system_time++;
  }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  UART IRQ                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief       UART0 IRQ Handler
 *
 * @param       None
 *
 * @return      None
 *
 * @details     ISR to handle UART Channel 0 interrupt event
 */
void UART0_IRQHandler(void)
{
  Uart0_Handle();
}

/**
 * @brief       UART1 IRQ Handler
 *
 * @param       None
 *
 * @return      None
 *
 * @details     ISR to handle UART Channel 1 interrupt event
 */
void UART1_IRQHandler(void)
{
  Uart1_Handle();
}

/*---------------------------------------------------------------------------------------------------------*/
/*  GPIO IRQ                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
/* P2.3 IRQ Handler */
void GPIOP2P3P4_IRQHandler(void)
{
  /* To check if P2.3 interrupt occurred */
  if(GPIO_GET_INT_FLAG(P2, BIT3))
  {
    /* Clear P2.3 interrupt flag */
    GPIO_CLR_INT_FLAG(P2, BIT3);
  }
  else
  {
    /* Un-expected interrupt. Just clear all PORT2, PORT3 and PORT4 interrupts */
    P2->ISRC = P2->ISRC;
    P3->ISRC = P3->ISRC;
    P4->ISRC = P4->ISRC;
  }
}

/*---------------------------------------------------------------------------------------------------------*/
/* UART0 Callback function                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
static void Uart0_Handle(void)
{
  uint8_t inputData = 0xFF;
  uint32_t u32IntSts = UART0->ISR;
  UART_DISABLE_INT(UART0, UART_IER_RDA_IEN_Msk);
  NVIC_DisableIRQ(UART0_IRQn);
  if((command_mode == FIRMATA_DATA_MODE) && (u32IntSts & UART_ISR_RDA_INT_Msk))
  {
    /* Get all the input characters */
    while(UART_IS_RX_READY(UART0))
    {
      inputData = UART_READ(UART0);

      if(parsingSysex)
      {
        if (inputData == END_SYSEX)
        {
          //stop sysex byte
          parsingSysex = false;
          //fire off handler function
          processSysexMessage();
        }
        else
        {
          //normal data byte - add to buffer
          sysex.storedInputData[sysexBytesRead] = inputData;
          sysexBytesRead++;
          if(sysexBytesRead > DEFAULT_UART_BUF_SIZE-1)
          {
            parsingSysex = false;
            sysexBytesRead = 0;
          }
        }
      }
      else if(inputData == START_SYSEX)
      {
        parsingSysex = true;
        sysexBytesRead = 0;
      }
    }
  }
  else if((command_mode == G_CODE_MODE) && (u32IntSts & UART_ISR_RDA_INT_Msk))
  {
    /* Get all the input characters */
    while(UART_IS_RX_READY(UART0))
    {
      inputData = UART_READ(UART0);
	  
      if(parsingGcode)
      {
        if ((char)inputData == '\n')
        {
          //stop sysex byte
          parsingGcode = false;
          //fire off handler function
          parse_gcommand(sysex.gcode_buffer);
        }
        else
        {
          //normal data byte - add to buffer
          sysex.storedInputData[sysexBytesRead] = inputData;
          sysexBytesRead++;
          if(sysexBytesRead > DEFAULT_UART_BUF_SIZE-1)
          {
            parsingGcode = false;
            sysexBytesRead = 0;
          }
        }
      }
      else if(inputData == 'G')
      {
        parsingGcode = true;
        sysexBytesRead = 0;
      }
    }
  }
  UART_ENABLE_INT(UART0, UART_IER_RDA_IEN_Msk);
  NVIC_EnableIRQ(UART0_IRQn);
}

/*---------------------------------------------------------------------------------------------------------*/
/* UART1 Callback function                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
static void Uart1_Handle(void)
{
  uint8_t inputData = 0xFF;
  uint32_t u32IntSts = UART1->ISR;
  if(u32IntSts & UART_ISR_RDA_INT_Msk)
  {
    /* Get all the input characters */
    while(UART_IS_RX_READY(UART1))
    {
      inputData = UART_READ(UART1);
      /* Check if buffer full */
      if(ForwardBytesRead < UART1_REV_BUF_SIZE)
      {
        /* Enqueue the character */
        Uart1RecData[Uart1RevRtail] = inputData;
        Uart1RevRtail = (Uart1RevRtail == (UART1_REV_BUF_SIZE - 1)) ? 0 : (Uart1RevRtail + 1);
        ForwardBytesRead++;
      }
      else
      {
        ForwardBytesRead = 0;
        Uart1RevRtail = 0;
        Uart1Revhead = 0;
      }
    }
  }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_IRQHandler(void)
{
  uint32_t u32Status;

  u32Status = I2C_GetStatus(I2C0);

  if(I2C_GET_TIMEOUT_FLAG(I2C0))
  {
    /* Clear I2C0 Timeout Flag */
    I2C_ClearTimeoutFlag(I2C0);
  }
  else
  {
    if(s_I2C0HandlerFn != NULL)
    {
      s_I2C0HandlerFn(u32Status);
    }
  }
}
