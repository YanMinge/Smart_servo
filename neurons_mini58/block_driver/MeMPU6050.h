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
	int16_t x_angle;
	int16_t y_angle;
	int16_t z_angle;
    int8_t shake_flag;
};

struct mpu6050_raw_data
{
    int16_t acc_x_raw;
    int16_t acc_y_raw;
    int16_t acc_z_raw;
    int16_t gyro_x_raw;
    int16_t gyro_y_raw;
    int16_t gyro_z_raw;
};

void mpu6050_init(void);

void ReadMpu6050(struct mpu6050_data * value);

#endif
