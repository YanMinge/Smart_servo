#ifndef __MYGPIO_H__
#define __MYGPIO_H__
#include "stm32f0xx.h"
#include "sysinit.h"
#include <stdbool.h>

#define HIGH 1
#define LOW  0

#define PIN_Mode(port_m,pin_n,pin_mode)     GPIO_SetMode(P##port_m,pin_n,pin_mode)

enum{
  P0_0   =   0x00,
  P0_1   =   0x01,
  P0_2   =   0x02,
  P0_3   =   0x03,
  P0_4   =   0x04,
  P0_5   =   0x05,
  P0_6   =   0x06,
  P0_7   =   0x07,

  P1_0   =   0x10,
  P1_1   =   0x11,
  P1_2   =   0x12,
  P1_3   =   0x13,
  P1_4   =   0x14,
  P1_5   =   0x15,
  P1_6   =   0x16,
  P1_7   =   0x17,

  P2_0   =   0x20,
  P2_1   =   0x21,
  P2_2   =   0x22,
  P2_3   =   0x23,
  P2_4   =   0x24,
  P2_5   =   0x25,
  P2_6   =   0x26,
  P2_7   =   0x27,

  P3_0   =   0x30,
  P3_1   =   0x31,
  P3_2   =   0x32,
  P3_3   =   0x33,
  P3_4   =   0x34,
  P3_5   =   0x35,
  P3_6   =   0x36,
  P3_7   =   0x37,

  P4_0   =   0x40,
  P4_1   =   0x41,
  P4_2   =   0x42,
  P4_3   =   0x43,
  P4_4   =   0x44,
  P4_5   =   0x45,
  P4_6   =   0x46,
  P4_7   =   0x47
};

long map(long x, long in_min, long in_max, long out_min, long out_max);
bool is_pin_servo(int pin);
bool is_pin_pwm(int pin);
uint32_t* Pin2Addr(uint8_t pin);
void pinMode(uint8_t pin,uint8_t mode);
void digitalWrite(uint8_t pin,int val);
int digitalRead(uint8_t pin);        
int analogRead(uint8_t pin);
// analogWrite(uint8_t pin, int val);
#endif
