#ifndef __UART_PRINTF_H__
#define __UART_PRINTF_H__

#include "Mini58Series.h"
#if 0
#define DEBUG_MSG(...)   uart_printf(__VA_ARGS__) 
#else
#define DEBUG_MSG(...)
#endif

void errorHandle(void);
void UART0_Init(uint32_t u32baudrate);
void UART1_Init(uint32_t u32baudrate);

void uart_printf(UART_T* uart,char *fmt,...);
#endif
