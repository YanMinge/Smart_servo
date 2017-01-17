#ifndef __MERGBLED_H__
#define __MERGBLED_H__

#include "Mini58Series.h"
#include "mygpio.h"

#define LED_PIN P0_4

void rgb_led_set(uint8_t r_value, uint8_t g_value, uint8_t b_value);

void rgb_led_init(void);

#endif
