#ifndef __MECOOLLIGHT_H__
#define __MECOOLLIGHT_H__

#include "Mini58Series.h"
#include "mygpio.h"

#define COMMON_EN_PIN               P2_6
#define LIGHT1_EN_PIN               P0_5
#define LIGHT2_EN_PIN               P0_4               
#define LIGHT3_EN_PIN               P3_2
#define LIGHT4_EN_PIN               P2_5


void cool_light_set(uint8_t number, uint8_t on_off);

void cool_light_init(void);

#endif
