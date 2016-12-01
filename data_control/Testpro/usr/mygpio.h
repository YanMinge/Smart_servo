#ifndef __MYGPIO_H__
#define __MYGPIO_H__
#include "stm32f0xx.h"
#include "sysinit.h"
#include <stdbool.h>

#define HIGH 1
#define LOW  0

#define GPIO_PIN(n)     GPIO_Pin_##n

enum{
  INPUT           = 0,
  OUTPUT          = 1,
  INPUT_PULLUP    = 2,
  INPUT_PULLDOWN  = 3,
  ANALOG          = 4,
};

enum{
  PA0   =   0x00,
  PA1   =   0x01,
  PA2   =   0x02,
  PA3   =   0x03,
  PA4   =   0x04,
  PA5   =   0x05,
  PA6   =   0x06,
  PA7   =   0x07,
  PA8   =   0x08,
  PA9   =   0x09,
  PA10   =   0x0a,
  PA11   =   0x0b,
  PA12   =   0x0c,
  PA13   =   0x0d,
  PA14   =   0x0e,
  PA15   =   0x0f,

  PB0   =   0x10,
  PB1   =   0x11,
  PB2   =   0x12,
  PB3   =   0x13,
  PB4   =   0x14,
  PB5   =   0x15,
  PB6   =   0x16,
  PB7   =   0x17,
  PB8   =   0x18,
  PB9   =   0x19,
  PB10   =   0x1a,
  PB11   =   0x1b,
  PB12   =   0x1c,
  PB13   =   0x1d,
  PB14   =   0x1e,
  PB15   =   0x1f,
};

long map(long x, long in_min, long in_max, long out_min, long out_max);
void pinMode(uint8_t pin,uint8_t mode);
void digitalWrite(uint8_t pin,int val);
int digitalRead(uint8_t pin);        
int analogRead(uint8_t pin);
// analogWrite(uint8_t pin, int val);
#endif
