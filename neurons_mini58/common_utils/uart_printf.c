#include "uart_printf.h"
#include <stdio.h>
#include <stdarg.h>
#include "protocol.h"

void UART0_Init(uint32_t u32baudrate)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART0                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, u32baudrate);
}

void UART1_Init(uint32_t u32baudrate)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART1                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART1 module */
    SYS_ResetModule(UART1_RST);

    /* Configure UART1 and set UART1 Baudrate */
    UART_Open(UART1, u32baudrate);
}

void send_string(UART_T* uart,char *str)
{
    uint32_t u32IntSts = uart->INTSTS;
    while(*str)
    {
        UART_WRITE(uart,*str++);
        UART_WAIT_TX_EMPTY(uart);
    }
}

void uart_printf(UART_T* uart,char *fmt,...)
{
    va_list ap;
    char string[128];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    send_string(uart,string);
    va_end(ap);
}
