#include "MeMPU6050.h"
#include "uart_printf.h"
#include <string.h>
#include <math.h>
#include "Interrupt.h"
#include "systime.h"
#include "i2c.h"


#define MPU6050_RA_PWR_MGMT_1      0x6b
#define IIC_MPU6050_ADDR           0x68
#define ACCEL_XOUT_H               0x3b
#define ACCEL_XOUT_L               0x3c
#define ACCEL_YOUT_H               0x3d
#define ACCEL_YOUT_L               0x3e
#define ACCEL_ZOUT_H               0x3f
#define ACCEL_ZOUT_L               0x40

#define TEMP_OUT_H                 0X41

#define GYRO_XOUT_H                0x43
#define GYRO_XOUT_L                0x44
#define GYRO_YOUT_H                0x45
#define GYRO_YOUT_L                0x46
#define GYRO_ZOUT_H                0x47
#define GYRO_ZOUT_L                0x48

#define GYRO_CONFIG                0x1b
#define ACCEL_CONFIG               0x1c

#define scale_2g_250d              0x00
#define scale_4g_500d	           0x08
#define scale_8g_1000d	           0x10
#define scale_16g_2000d	           0x18

typedef union
{
  float f_data;
  uint8_t data[4];
}unionfloat;

uint8_t i2c_tx_array[3];
uint8_t i2c_rx_array[14];
volatile uint8_t i2c_tx_data_len;
volatile uint8_t i2c_rx_data_len;
volatile uint8_t g_i2c_end_flag = 0;
volatile uint8_t g_device_addr = 0;

static double s_rotation_angle_x = 0;
static double s_rotation_angle_y = 0;
static double s_rotation_angle_z = 0;

void I2C_Init(I2C_T *i2c)
{
	if(i2c == I2C0)
	{
		SYS->P3_MFP &= ~(SYS_MFP_P34_Msk | SYS_MFP_P35_Msk); 	
		SYS->P3_MFP |= (SYS_MFP_P34_I2C0_SDA | SYS_MFP_P35_I2C0_SCL);    
	
		CLK_EnableModuleClock(I2C0_MODULE);
		I2C_Open(I2C0, 100000);

		/* Enable I2C interrupt */
		I2C_EnableInt(I2C0);
		NVIC_EnableIRQ(I2C0_IRQn);
	}
	else if(i2c == I2C1)
	{
		SYS->P2_MFP &= ~(SYS_MFP_P22_Msk | SYS_MFP_P23_Msk); 	
		SYS->P2_MFP |= (SYS_MFP_P22_I2C1_SCL | SYS_MFP_P23_I2C1_SDA);    
	
		CLK_EnableModuleClock(I2C1_MODULE);
		I2C_Open(I2C1, 100000);

		/* Enable I2C interrupt */
		I2C_EnableInt(I2C1);
		NVIC_EnableIRQ(I2C1_IRQn);
		
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Rx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterRx(uint32_t u32Status)
{
    static uint8_t rxcount = 0;
    if(u32Status == 0x08)                       /* START has been transmitted and prepare SLA+W */
    {
        I2C_SET_DATA(I2C0,(g_device_addr<<1) + 0);    /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    }
    else if(u32Status == 0x18)                  /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C0,i2c_tx_array[0]);
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    }
    else if(u32Status == 0x20)                  /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
    }
    else if(u32Status == 0x28)                  /* DATA has been transmitted and ACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_STA | I2C_SI); 
    }
    else if(u32Status == 0x10)                  /* Repeat START has been transmitted and prepare SLA+R */
    {
        I2C_SET_DATA(I2C0, (g_device_addr<<1) + 1); /* Write SLA+R to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    }
    else if(u32Status == 0x40)                  /* SLA+R has been transmitted and ACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);			
    }
	
    else if(u32Status == 0x58)                  /* DATA has been received and NACK has been returned */
    {
        i2c_rx_array[rxcount]  = (unsigned char) I2C_GET_DATA(I2C0);
        I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
        g_i2c_end_flag = 1;
        rxcount = 0;
    }
    else
    {
		I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Tx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterTx(uint32_t u32Status)
{
    static uint8_t txcount = 0;
    if(u32Status == 0x08)                       /* START has been transmitted */
    {
        I2C_SET_DATA(I2C0, g_device_addr<<1);      /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    }
    else if(u32Status == 0x18)                  /* SLA+W has been transmitted and ACK has been received */
    { 
        I2C_SET_DATA(I2C0, i2c_tx_array[txcount++]);
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);

    }
    else if(u32Status == 0x20)                  /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
    }
    else if(u32Status == 0x28)                  /* DATA has been transmitted and ACK has been received */
    {
        if(txcount != i2c_tx_data_len)
        {
            I2C_SET_DATA(I2C0, i2c_tx_array[txcount++]);
            I2C_SET_CONTROL_REG(I2C0, I2C_SI);
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
            txcount = 0;
            g_i2c_end_flag = 1;
        }
    }
    else
    {
		I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
    }
}

void I2C_Read_MPU6050(uint8_t data_addr,uint8_t* data)
{
    i2c_rx_data_len = 1;
    
    g_i2c_end_flag = 0;
    i2c_tx_array[0] = data_addr;
    /* I2C function to read data from slave */
    s_I2C0HandlerFn = (I2C_FUNC)I2C_MasterRx;
	I2C_SET_CONTROL_REG(I2C0, I2C_STA);
	
    /* Wait I2C Rx Finish */
    while(g_i2c_end_flag == 0)
	{
		if(whileCoutOver_us(2000))
		{
			return;
		}
	}
	clearWhileCount();
    memcpy(data,i2c_rx_array,1);
}


void I2C_Write_MPU6050(uint8_t address,uint8_t data)
{
    g_device_addr = IIC_MPU6050_ADDR;
    i2c_tx_array[0] = address;
    i2c_tx_array[1] = data;
    i2c_tx_data_len = 2;
    g_i2c_end_flag = 0;

    /* I2C function to write data to slave */
    s_I2C0HandlerFn = (I2C_FUNC)I2C_MasterTx;
    /* I2C as master sends START signal */
    I2C_SET_CONTROL_REG(I2C0, I2C_STA);	
	

    /* Wait I2C Tx Finish */
    while(g_i2c_end_flag == 0)
	{
		if(whileCoutOver_us(2000))
		{
			return;
		}
	}
	clearWhileCount();
}

void mpu6050_init(void)
{
    I2C_Init(I2C0);
	delayMicroseconds(5000);
    I2C_Write_MPU6050(MPU6050_RA_PWR_MGMT_1,0x00); // wake up.
    I2C_Write_MPU6050(GYRO_CONFIG,scale_4g_500d);
    I2C_Write_MPU6050(ACCEL_CONFIG,scale_4g_500d);
}

void ReadMpu6050(struct mpu6050_data * value)
{
	static uint32_t s_last_time = 0;
	uint32_t now_time = 0;
	double dt;
	
	uint8_t x_accel_h, x_accel_l, y_accel_h, y_accel_l, z_accel_h, z_accel_l, \
			x_gyro_h, x_gyro_l, y_gyro_h, y_gyro_l, z_gyro_h, z_gyro_l;
	
	// acceleration.
    I2C_Read_MPU6050(ACCEL_XOUT_H, &x_accel_h);
	I2C_Read_MPU6050(ACCEL_XOUT_L, &x_accel_l);
	I2C_Read_MPU6050(ACCEL_YOUT_H, &y_accel_h);
	I2C_Read_MPU6050(ACCEL_YOUT_L, &y_accel_l);
	I2C_Read_MPU6050(ACCEL_ZOUT_H, &z_accel_h);
	I2C_Read_MPU6050(ACCEL_ZOUT_L, &z_accel_l);
	
	// gyro.
	I2C_Read_MPU6050(GYRO_XOUT_H,  &x_gyro_h);
	I2C_Read_MPU6050(GYRO_XOUT_L,  &x_gyro_l);
	I2C_Read_MPU6050(GYRO_YOUT_H,  &y_gyro_h);
	I2C_Read_MPU6050(GYRO_YOUT_L,  &y_gyro_l);
	I2C_Read_MPU6050(GYRO_ZOUT_H,  &z_gyro_h);
	I2C_Read_MPU6050(GYRO_ZOUT_L,  &z_gyro_l);
	
	
	value->x_accel_value=((float)(short)((x_accel_h<<8)|x_accel_l))/32768*4;
	value->y_accel_value=((float)(short)((y_accel_h<<8)|y_accel_l))/32768*4;
	value->z_accel_value=((float)(short)((z_accel_h<<8)|z_accel_l))/32768*4;
    value->x_gyro_value=((float)(short)((x_gyro_h<<8)|x_gyro_l))/32768*500;
    value->y_gyro_value=((float)(short)((y_gyro_h<<8)|y_gyro_l))/32768*500;
    value->z_gyro_value=((float)(short)((z_gyro_h<<8)|z_gyro_l))/32768*500; 
	 
	float ax, ay, az;
	ax = atan2(value->x_accel_value, sqrt( pow(value->y_accel_value, 2) + pow(value->z_accel_value, 2) ) ) * 180 / 3.1415926;
	ay = atan2(value->y_accel_value, sqrt( pow(value->x_accel_value, 2) + pow(value->z_accel_value, 2) ) ) * 180 / 3.1415926;
	now_time = millis();
	dt = (double)(now_time - s_last_time) / 1000;
	s_last_time = now_time;
	if(value->z_accel_value > 0)
	{
		s_rotation_angle_x = s_rotation_angle_x - (value->y_gyro_value) * dt;
		s_rotation_angle_y = s_rotation_angle_y + (value->x_gyro_value) * dt;
	}
	else
	{
		s_rotation_angle_x = s_rotation_angle_x + (value->y_gyro_value) * dt;
		s_rotation_angle_y = s_rotation_angle_y - (value->x_gyro_value) * dt;
	}
	s_rotation_angle_z += value->z_gyro_value * dt;
	s_rotation_angle_z = s_rotation_angle_z - 360 * floor(s_rotation_angle_z / 360);
	if(s_rotation_angle_z > 180)
	{
		s_rotation_angle_z = s_rotation_angle_z - 360;
	}
	
	s_rotation_angle_y = 0.98 * s_rotation_angle_y + 0.02 * ay;
	s_rotation_angle_x = 0.98 * s_rotation_angle_x + 0.02 * ax;
	
	value->x_angle = (int8_t)s_rotation_angle_x;
	value->y_angle = (int8_t)s_rotation_angle_y;
	value->z_angle = (int8_t)s_rotation_angle_z;
	
}

