#ifndef __SYSTIME_H__
#define __SYSTIME_H__

#include "stm32f0xx.h"
extern volatile unsigned long system_time;

extern unsigned long millis(void);
extern void delay(unsigned long ms);
#endif //__SYSTIME_H__

