#ifndef __SYSTTIME_H__
#define __SYSTTIME_H__

#include "M051Series.h"

void errorHandle(void);
void UART0_Init(uint32_t u32baudrate);
void UART1_Init(uint32_t u32baudrate);

void uart_printf(UART_T* uart,char *fmt,...);
#endif
