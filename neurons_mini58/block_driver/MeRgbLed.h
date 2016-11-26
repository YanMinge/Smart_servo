#ifndef __MERGBLED_H__
#define __MERGBLED_H__

#include "Mini58Series.h"
#include "mygpio.h"

#define R_PIN    P2_2
#define G_PIN	 P2_3
#define B_PIN 	 P2_4

void rgb_led_set(uint8_t r_value, uint8_t g_value, uint8_t b_value);

void rgb_led_init(void);

#endif
