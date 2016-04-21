#ifndef __SYSTIME_H__
#define __SYSTIME_H__

#include "M051Series.h"
extern volatile unsigned long system_time;

extern unsigned long millis(void);
extern void delayMicroseconds(uint32_t us);
extern void delay(unsigned long ms);
#endif //__SYSTIME_H__

