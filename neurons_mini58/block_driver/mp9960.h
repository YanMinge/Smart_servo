#ifndef __MP9960_H__
#define __MP9960_H__
#include "Mini58Series.h"
#include "spi.h"
#include "mygpio.h"

extern void mp9960_init(void);
extern int16_t mp9960_read_raw_angle(void);

#endif
