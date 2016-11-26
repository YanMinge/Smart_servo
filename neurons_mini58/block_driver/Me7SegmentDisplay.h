#ifndef __ME7SEGMENTDISPLAY_H__
#define __ME7SEGMENTDISPLAY_H__

#include "Mini58Series.h"
#include "mygpio.h"

//pinMode(P0_5,GPIO_PMD_OUTPUT);   //dp
//pinMode(P2_5,GPIO_PMD_OUTPUT);   //g
//pinMode(P2_3,GPIO_PMD_OUTPUT);   //f
//pinMode(P2_4,GPIO_PMD_OUTPUT);   //e
//pinMode(P0_6,GPIO_PMD_OUTPUT);   //d
//pinMode(P1_4,GPIO_PMD_OUTPUT);   //c
//pinMode(P1_5,GPIO_PMD_OUTPUT);   //b
//pinMode(P2_6,GPIO_PMD_OUTPUT);   //a
extern void seg_display(uint8_t value);

#endif
