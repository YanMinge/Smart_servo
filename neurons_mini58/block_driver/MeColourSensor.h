#ifndef __MECOLOURSENSOR_H__
#define __MECOLOURSENSOR_H__
#include "Mini58Series.h"

#define COLOUR_SENSOR_MIN  0
#define COLOUR_SENSOR_MAX  255

struct colour_sensor_data
{
	uint8_t r_value;
	uint8_t g_value;
	uint8_t b_value;
};

void colour_sensor_init(void);

struct colour_sensor_data colour_sensor_read(void);

#endif
