#ifndef __MENUMERICDISPLAY_H__
#define __MENUMERICDISPLAY_H__

#include "Mini58Series.h"
#include "mygpio.h"

/* positive shared*/
#define A_PIN       P3_2
#define B_PIN       P5_4
#define C_PIN       P0_6
#define D_PIN       P0_4
#define E_PIN       P0_1
#define F_PIN       P1_4
#define G_PIN       P3_6
#define DP_PIN      P0_5
#define A1_PIN      P3_0
#define A2_PIN      P3_1
#define A3_PIN      P3_4
#define A4_PIN		P3_5

void NumericDisplayBuf(uint8_t DispData[]);

void NumericDisplayFloat(float value);
void NumericDisplayStr(char * str);
void NumericDispalyInit(void);
void refresh_display(void);
void update_display(void);

#endif
