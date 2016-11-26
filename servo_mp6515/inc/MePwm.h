#ifndef __MEPWM_H__
#define __MEPWM_H__

#include "M051Series.h"
#include "mygpio.h"

void pwm_init(int pin,int fq);
void pwm_write(int pin, int value,int min,int max);
void pwm_canceled(int pin);

#endif
