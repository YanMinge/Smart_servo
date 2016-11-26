#ifndef __MENUMERICDISPLAY_H__
#define __MENUMERICDISPLAY_H__

#include "Mini58Series.h"
#include "mygpio.h"

/* positive shared*/
#define A_PIN       P5_3
#define B_PIN		P5_4
#define C_PIN       P0_6
#define D_PIN       P0_4
#define E_PIN       P0_1
#define F_PIN       P0_0
#define G_PIN       P0_7
#define DP_PIN      P0_5
#define A1_PIN      P3_0
#define A2_PIN      P3_1
#define A3_PIN      P3_4
#define A4_PIN		P3_5

void NumericDisplayBuf(uint8_t DispData[]);

//extern void NumericDisplayUint8(uint8_t value);
//extern void NumericDisplayUint16(uint16_t value);
//extern void NumericDisplayFloat(float value);
void NumericDisplayFloat(float value);
void NumericDispalyInit(void);
void refresh_display(void);
void update_display(void);

#endif
