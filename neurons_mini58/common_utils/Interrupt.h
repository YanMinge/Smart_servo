#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "Mini58Series.h"
#include "sysinit.h"
;
extern volatile unsigned long system_time;
extern volatile boolean g_int_flag;

typedef void (*I2C_FUNC)(uint32_t u32Status);
extern I2C_FUNC s_I2C0HandlerFn;
extern volatile I2C_FUNC s_I2C1HandlerFn;

void TMR0_IRQHandler(void);
void UART0_IRQHandler(void);
void UART1_IRQHandler(void);
void I2C1_IRQHandler(void);
void GPIOP0P1_IRQHandler(void);
void EINT1_IRQHandler(void);

#endif

