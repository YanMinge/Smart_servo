#ifndef __METEMPERATURE_H__
#define __METEMPERATURE_H__
#include "systime.h"
#include "sysinit.h"

#define DS18B20_PIN  P0_1

extern void sendChangeCmd(void);
extern void DS18B20_Init(void);
extern boolean DS18B20_ReadTemp(float * temperature);

#endif

