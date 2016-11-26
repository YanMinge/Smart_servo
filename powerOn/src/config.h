#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "STC15F2K60S2.h"
#include "stdint.h"

#define F_CPU 11059200


#define interrupts() EA = 1
#define noInterrupts() EA = 0

#define HIGH 1
#define LOW  0


#define true 1
#define false 0
	
typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

#endif