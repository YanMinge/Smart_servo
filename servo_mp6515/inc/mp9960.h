#ifndef __MP9960_H__
#define __MP9960_H__
#include "M051Series.h"
#include "spi.h"
;
extern void mp9960_init(void);
extern int16_t mp9960_read_raw_angle(void);

#endif
