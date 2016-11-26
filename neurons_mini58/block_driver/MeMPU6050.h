#ifndef __MEMPU6050_H__
#define __MEMPU6050_H__
#include "Mini58Series.h"


struct mpu6050_data
{
	float x_accel_value;
	float y_accel_value;
	float z_accel_value;
	float x_gyro_value;
	float y_gyro_value;
	float z_gyro_value;
	int8_t x_angle;
	int8_t y_angle;
	int8_t z_angle;
};

void mpu6050_init(void);

void ReadMpu6050(struct mpu6050_data * value);

#endif
