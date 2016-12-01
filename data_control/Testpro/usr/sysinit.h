#ifndef __SYSINIT_H__
#define __SYSINIT_H__
#include "stm32f0xx.h"
#include "mygpio.h"

//GPIO define
#define RUN_LED  PB2
#define GET_ON   PA15
#define SW_LED   PA8
#define SET_ON   PB5

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#if !defined(__bool_true_false_are_defined)
typedef unsigned char           boolean; //!< Boolean.
#define true  1
#define false 0
#endif

#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK           50000000
#endif /* __SYSINIT_H__ */

extern void RCC_Configuration(void);
extern void GPIO_Configuration(void);
extern void NVIC_Configuration(void);
extern void gpio_init(void);

