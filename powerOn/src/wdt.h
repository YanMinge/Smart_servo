
#ifndef _WDT_H_
#define _WDT_H_

#include "config.h"
//sfr WDT_CONTR = 0xC1; //Watch-Dog-Timer Control register
//                                      7     6     5      4       3      2   1   0     Reset Value
//                                  WDT_FLAG  -  EN_WDT CLR_WDT IDLE_WDT PS2 PS1 PS0    xx00,0000
#define D_WDT_FLAG			(1<<7)
#define D_EN_WDT			(1<<5)
#define D_CLR_WDT			(1<<4)	//auto clear
#define D_IDLE_WDT			(1<<3)	//WDT counter when Idle
#define D_WDT_SCALE_2		0
#define D_WDT_SCALE_4		1
#define D_WDT_SCALE_8		2		//T=12*32768*N/F_CPU=393216*N/F_CPU
#define D_WDT_SCALE_16		3
#define D_WDT_SCALE_32		4
#define D_WDT_SCALE_64		5
#define D_WDT_SCALE_128		6
#define D_WDT_SCALE_256		7

#define wdt_reset()  WDT_CONTR |= D_CLR_WDT;

 
#define wdt_enable(value)   WDT_CONTR = D_EN_WDT + D_CLR_WDT + D_IDLE_WDT + (value)      //Æô¶¯¿´ÃÅ¹·


#define wdt_disable() WDT_CONTR &= ~D_EN_WDT
 

#endif 
