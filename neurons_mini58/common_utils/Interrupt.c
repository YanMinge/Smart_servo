#include "Interrupt.h"
#include "protocol.h"

volatile boolean g_int_flag = 0;

I2C_FUNC s_I2C0HandlerFn = NULL;
volatile I2C_FUNC s_I2C1HandlerFn = NULL;

/*---------------------------------------------------------------------------------------------------------*/
/* Interrupt handler entry                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*  TMR0 IRQ                                                                                               */
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
    uint8_t inputData = 0xFF;
    uint32_t u32IntSts = UART0->INTSTS;

    if(u32IntSts & UART_INTSTS_RDAINT_Msk)
    {
        /* Get all the input characters */
        while(UART_IS_RX_READY(UART0))
        {
			
            inputData = UART_READ(UART0);
            uint16_t BufferStoredData = Uart0RecvBufferPushBytes - Uart0RecvBufferPopBytes;
            if(BufferStoredData < UART0_REV_BUF_SIZE)
            {
                /* Enqueue the character */
                Uart0RecData[Uart0RevRtail] = inputData;
                Uart0RevRtail = (Uart0RevRtail == (UART0_REV_BUF_SIZE - 1)) ? 0 : (Uart0RevRtail + 1);
                Uart0RecvBufferPushBytes++;
            }	
        }
		
    }
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
    uint8_t inputData = 0xFF;
    uint32_t u32IntSts = UART1->INTSTS;
    if(u32IntSts & UART_INTSTS_RDAINT_Msk)
    {
        /* Get all the input characters */
        while(UART_IS_RX_READY(UART1))
        {
            inputData = UART_READ(UART1);
            /* Check if buffer full */
			uint16_t BufferStoredData = Uart1RecvBufferPushBytes - Uart1RecvBufferPopBytes;
            if(BufferStoredData < UART1_REV_BUF_SIZE)
            {
                /* Enqueue the character */
                Uart1RecData[Uart1RevRtail] = inputData;
                Uart1RevRtail = (Uart1RevRtail == (UART1_REV_BUF_SIZE - 1)) ? 0 : (Uart1RevRtail + 1);
				Uart1RecvBufferPushBytes++;
            }
        }
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C0_IRQHandler(void)
{
    uint32_t u32Status;

    u32Status = I2C_GET_STATUS(I2C0);

    if(I2C_GET_TIMEOUT_FLAG(I2C0))
    {
        /* Clear I2C0 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C0);
    }
    else
    {
        if(s_I2C0HandlerFn != NULL)
            s_I2C0HandlerFn(u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C1 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C1_IRQHandler(void)
{
    uint32_t u32Status;

    u32Status = I2C_GET_STATUS(I2C1);

    if(I2C_GET_TIMEOUT_FLAG(I2C1))
    {
        /* Clear I2C0 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C1);
    }
    else
    {
        if(s_I2C1HandlerFn != NULL)
            s_I2C1HandlerFn(u32Status);
    }
}
