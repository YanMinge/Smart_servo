#ifndef __MELEDMATRIX8_16_H__
#define __MELEDMATRIX8_16_H__
#include "Mini58Series.h"
#include "mygpio.h"

#define SCK_PIN  P2_2
#define SDA_PIN	 P2_3
#define STRING_DISPLAY_BUFFER_SIZE 48

void led_matrix8_16_init(void);

void MeLEDMatrix_DrawTime(uint8_t hour, uint8_t minute, boolean point_flag);

void MeLEDMatrix_DrawStr(int16_t X_position, int8_t Y_position, const char *str);

void MeLEDMatrix_DrawBitMap(uint8_t matrix[]);

extern char i8_Str_Display_Buffer[STRING_DISPLAY_BUFFER_SIZE];

#endif
