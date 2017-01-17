#include "MeMPU6050.h"
#include "uart_printf.h"
#include <string.h>
#include <math.h>
#include "Interrupt.h"
#include "systime.h"
#include "i2c.h"


#define MPU6050_RA_PWR_MGMT_1      0x6b
#define IIC_MPU6050_ADDR           0x68
#define MPU6050_DATA_START_ADDR    0x3b
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

#define FILTER_LENGTH              10
#define IMU_KP                     2.0f     					
#define IMU_KI                     0.01f

volatile uint8_t s_i2c_tx_array[3];
volatile uint8_t s_i2c_rx_array[32];
volatile uint8_t i2c_tx_data_len;
volatile uint8_t i2c_rx_data_len;
volatile uint8_t g_i2c_end_flag = 0;
volatile uint8_t g_device_addr = 0;

static double s_rotation_angle_x = 0;
static double s_rotation_angle_y = 0;
static double s_rotation_angle_z = 0;
static struct mpu6050_raw_data s_filter_buff[FILTER_LENGTH] = {0};
static struct mpu6050_raw_data s_data_after_filter;


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
        I2C_SET_DATA(I2C0,s_i2c_tx_array[0]);
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
        I2C_SET_CONTROL_REG(I2C0, I2C_SI|I2C_AA);			
    }
    else if(u32Status == 0x50)
    {
        if(rxcount != i2c_rx_data_len)
        {
            s_i2c_rx_array[rxcount] = I2C_GET_DATA(I2C0);
            rxcount++;
            I2C_SET_CONTROL_REG(I2C0, I2C_SI|I2C_AA);
        }
        else
        {
            s_i2c_rx_array[rxcount] = I2C_GET_DATA(I2C0);
            I2C_SET_CONTROL_REG(I2C0, I2C_SI);
            g_i2c_end_flag = 1;
            rxcount = 0;
        }
    }
    else if(u32Status == 0x58)                  /* DATA has been received and NACK has been returned */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);    
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
        I2C_SET_DATA(I2C0, s_i2c_tx_array[txcount++]);
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
            I2C_SET_DATA(I2C0, s_i2c_tx_array[txcount++]);
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

void I2C_Read(uint8_t data_addr,uint8_t* data, uint8_t length)
{
    i2c_rx_data_len = length;
    
    g_i2c_end_flag = 0;
    s_i2c_tx_array[0] = data_addr;
    /* I2C function to read data from slave */
    s_I2C0HandlerFn = (I2C_FUNC)I2C_MasterRx;
    I2C_SET_CONTROL_REG(I2C0, I2C_STA);
	
    uint32_t timeout= millis()+300;
    /* Wait I2C Rx Finish */
    while(g_i2c_end_flag == 0)
    {
        if(millis() > timeout)
        {
            return;
        }
    }
    memcpy(data, (void*)s_i2c_rx_array, i2c_rx_data_len);
}

void I2C_Read_MPU6050(uint8_t data_addr,uint8_t* data)
{
    i2c_rx_data_len = 1;
    
    g_i2c_end_flag = 0;
    s_i2c_tx_array[0] = data_addr;
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
    memcpy(data,(void*)s_i2c_rx_array,1);
}
void I2C_Write(uint8_t* data, uint8_t len)
{
    memcpy((void*)s_i2c_tx_array, data, len);
    i2c_tx_data_len = len;
    g_i2c_end_flag = 0;

    /* I2C function to write data to slave */
    s_I2C0HandlerFn = (I2C_FUNC)I2C_MasterTx;
    /* I2C as master sends START signal */
    I2C_SET_CONTROL_REG(I2C0, I2C_STA);
    /* Wait I2C Tx Finish */
    uint32_t timeout= millis() + 2;
    while(g_i2c_end_flag == 0)
    {
        if(millis() > timeout)
	{
            return;
        }
    }
}

void I2C_Write_MPU6050(uint8_t address,uint8_t data)
{
    g_device_addr = IIC_MPU6050_ADDR;
    s_i2c_tx_array[0] = address;
    s_i2c_tx_array[1] = data;
    i2c_tx_data_len = 2;
    g_i2c_end_flag = 0;

    /* I2C function to write data to slave */
    s_I2C0HandlerFn = (I2C_FUNC)I2C_MasterTx;
    /* I2C as master sends START signal */
    I2C_SET_CONTROL_REG(I2C0, I2C_STA);	
	

    /* Wait I2C Tx Finish */
    uint32_t timeout= millis() + 2;
    while(g_i2c_end_flag == 0)
    {
        if(millis() > timeout)
        {
            return;
	}
    }
}
static void filter_mpu6050(struct mpu6050_raw_data data)
{
    static uint8_t s_filter_cnt	=	0;
    uint8_t i;
  
    int32_t acc_x_sum, acc_y_sum, acc_z_sum;
    int32_t gyro_x_sum, gyro_y_sum, gyro_z_sum;
    acc_x_sum = 0;
    acc_y_sum = 0;
    acc_z_sum = 0;
    gyro_x_sum = 0;
    gyro_y_sum = 0;
    gyro_z_sum = 0;
    s_filter_buff[s_filter_cnt].acc_x_raw = data.acc_x_raw;
    s_filter_buff[s_filter_cnt].acc_y_raw = data.acc_y_raw;
    s_filter_buff[s_filter_cnt].acc_z_raw = data.acc_z_raw;
    s_filter_buff[s_filter_cnt].gyro_x_raw = data.gyro_x_raw;
    s_filter_buff[s_filter_cnt].gyro_y_raw = data.gyro_y_raw;
    s_filter_buff[s_filter_cnt].gyro_z_raw = data.gyro_z_raw;
	
    s_filter_cnt++;
	
    for(i = 0; i < FILTER_LENGTH; i++)
    {
        acc_x_sum += s_filter_buff[i].acc_x_raw;
	acc_y_sum += s_filter_buff[i].acc_y_raw;
	acc_z_sum += s_filter_buff[i].acc_z_raw;
        gyro_x_sum += s_filter_buff[i].gyro_x_raw;
        gyro_y_sum += s_filter_buff[i].gyro_y_raw;
        gyro_z_sum += s_filter_buff[i].gyro_z_raw;
    }
  
    s_data_after_filter.acc_x_raw = acc_x_sum/FILTER_LENGTH;
    s_data_after_filter.acc_y_raw = acc_y_sum/FILTER_LENGTH;
    s_data_after_filter.acc_z_raw = acc_z_sum/FILTER_LENGTH;
    s_data_after_filter.gyro_x_raw = gyro_x_sum/FILTER_LENGTH;
    s_data_after_filter.gyro_y_raw = gyro_y_sum/FILTER_LENGTH;
    s_data_after_filter.gyro_z_raw = gyro_z_sum/FILTER_LENGTH;
    if( s_filter_cnt == FILTER_LENGTH )
    {
        s_filter_cnt = 0;
    }
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
    
    struct mpu6050_raw_data read_data;
    uint8_t mpu6050_data[14];
    int16_t x_accel_raw, y_accel_raw, z_accel_raw, x_gyro_raw, y_gyro_raw, z_gyro_raw;
    I2C_Read(MPU6050_DATA_START_ADDR, mpu6050_data, sizeof(mpu6050_data));
    
    x_accel_raw = mpu6050_data[0];
    x_accel_raw = (x_accel_raw << 8)| mpu6050_data[1];
    y_accel_raw = mpu6050_data[2];
    y_accel_raw = (y_accel_raw << 8)| mpu6050_data[3];
    z_accel_raw = mpu6050_data[4];
    z_accel_raw = (z_accel_raw << 8)| mpu6050_data[5];
    x_gyro_raw = mpu6050_data[8];
    x_gyro_raw = (x_gyro_raw << 8) | mpu6050_data[9];
    y_gyro_raw = mpu6050_data[10];
    y_gyro_raw = (y_gyro_raw << 8) | mpu6050_data[11];
    z_gyro_raw = mpu6050_data[12];
    z_gyro_raw = (z_gyro_raw << 8) | mpu6050_data[13];
    
    read_data.acc_x_raw = x_accel_raw;
    read_data.acc_y_raw = y_accel_raw;
    read_data.acc_z_raw = z_accel_raw;
    read_data.gyro_x_raw = x_gyro_raw;
    read_data.gyro_y_raw = y_gyro_raw;
    read_data.gyro_z_raw = z_gyro_raw;
    
    filter_mpu6050(read_data);
    value->x_accel_value = 4*((float)(s_data_after_filter.acc_x_raw))/32768;
    value->y_accel_value = 4*((float)(s_data_after_filter.acc_y_raw))/32768;
    value->z_accel_value = 4*((float)(s_data_after_filter.acc_z_raw))/32768;
    
    value->x_gyro_value=500*((float)(s_data_after_filter.gyro_x_raw))/32768;
    value->y_gyro_value=500*((float)(s_data_after_filter.gyro_y_raw))/32768;
    value->z_gyro_value=500*((float)(s_data_after_filter.gyro_z_raw))/32768; 
	
    float ax, ay;
    ax = atan2(value->x_accel_value, sqrt( pow(value->y_accel_value, 2) + pow(value->z_accel_value, 2) ) ) * 180 / PI;
    ay = atan2(value->y_accel_value, sqrt( pow(value->x_accel_value, 2) + pow(value->z_accel_value, 2) ) ) * 180 / PI;
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
	
    value->x_angle = (int16_t)s_rotation_angle_x;
    value->y_angle = (int16_t)s_rotation_angle_y;
    value->z_angle = (int16_t)s_rotation_angle_z;
    if((value->x_accel_value > 1.5)||(value->y_accel_value > 1.5)||(value->z_accel_value > 1.5))
    {
        value->shake_flag = 1;
    }
    else
    {
        value->shake_flag = 0;
    }
    //uart_printf(UART0, "data->x_angle:%d, data->y_angle:%d, data->z_angle:%d\n", (int16_t)s_rotation_angle_x, (int16_t)s_rotation_angle_y, (int16_t)s_rotation_angle_z);
    //uart_printf(UART0, "data->x:%d, data->y:%d, data->z:%d\n", (int16_t)value->x_accel_value, (int16_t)value->y_accel_value, (int16_t)value->z_accel_value);
    //uart_printf(UART0, "value->shake_flag:%d\n", value->shake_flag );
}

