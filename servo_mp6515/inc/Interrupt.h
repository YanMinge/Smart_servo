#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "M051Series.h"

typedef void (*I2C_FUNC)(uint32_t u32Status);
extern volatile I2C_FUNC s_I2C0HandlerFn;

void TMR0_IRQHandler(void);
void UART0_IRQHandler(void);
void UART1_IRQHandler(void);
void GPIOP2P3P4_IRQHandler(void);
void I2C_IRQHandler(void);
#endif

