#ifndef __METEMPERATURE_H__
#define __METEMPERATURE_H__
#include "systime.h"

#define DS18B20_PIN  P0_1

extern void sendChangeCmd(void);
extern void DS18B20_Init(void);
extern float DS18B20_ReadTemp(void);

#endif

