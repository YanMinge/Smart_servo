#ifndef __SYSTTIME_H__
#define __SYSTTIME_H__

#include "config.h"
extern void uart_init();
extern void uart_write(uint8_t dat);
extern void send_string(char *str);
#endif
